#!/bin/bash
#
# run-generated-code-benchmark.sh: Aldor build and development support.
#
# This file is part of Aldor.
#
# Aldor is licensed under the Apache License, Version 2.0.
#
#
# See legal/LICENSE in the Aldor distribution for details.
#
# Copyright (C) 2026 Stephen M. Watt.
#
set -euo pipefail

usage() {
    cat <<USAGE
Usage: $0 SUPPORT_ROOT BASELINE_COMPILER CURRENT_COMPILER [OUTDIR]

Compare execution speed of generated Aldor code for a deterministic version
of the Algebra sm_dmp0 test.  Normally both compilers use the same support
root.  When comparing across an intentional generated-code ABI change, the
optional BENCH_*_SUPPORT_ROOT variables provide matched generated libraries
from the same sources while retaining a common runtime/toolchain.  The
benchmark translation unit is compiled with Aldor -O -Qall.  A gcc wrapper
appends -O3 so the generated C is compiled at effective -O3 (Aldor itself
does not accept -O3).

Environment:
  BENCH_RUNS       measured paired runs (default 7)
  BENCH_WARMUPS    warm-up runs per executable (default 2)
  BENCH_COMPILE_RUNS measured paired compiler runs (default 5)
  BENCH_CPU        CPU number to pin to (default first allowed CPU)
  REAL_CC          backend C compiler (default /usr/bin/gcc)
  BENCH_BASELINE_SUPPORT_ROOT
                   optional baseline generated-library root; defaults to
                   SUPPORT_ROOT
  BENCH_CURRENT_SUPPORT_ROOT
                   optional current generated-library root; defaults to
                   SUPPORT_ROOT
USAGE
}

if [ $# -lt 3 ] || [ $# -gt 4 ]; then
    usage >&2
    exit 2
fi

support_root=$(cd "$1" && pwd)
baseline_support_root=$(cd "${BENCH_BASELINE_SUPPORT_ROOT:-$support_root}" && pwd)
current_support_root=$(cd "${BENCH_CURRENT_SUPPORT_ROOT:-$support_root}" && pwd)
baseline_compiler=$(cd "$(dirname "$2")" && pwd)/$(basename "$2")
current_compiler=$(cd "$(dirname "$3")" && pwd)/$(basename "$3")
outdir=${4:-benchmark-out}
runs=${BENCH_RUNS:-7}
warmups=${BENCH_WARMUPS:-2}
compile_runs=${BENCH_COMPILE_RUNS:-5}
real_cc=${REAL_CC:-/usr/bin/gcc}
script_dir=$(cd "$(dirname "$0")" && pwd)
source_file="$script_dir/sm_dmp0-benchmark.as"

mkdir -p "$outdir" "$outdir/wrap" "$outdir/baseline" "$outdir/current"
outdir=$(cd "$outdir" && pwd)

cat > "$outdir/wrap/gcc" <<'WRAP'
#!/bin/sh
printf '%s\n' "$*" >> "${BENCH_CC_LOG:?}"
exec "${REAL_CC:?}" "$@" -O3
WRAP
chmod +x "$outdir/wrap/gcc"

# Time compiler+generated-C compilation separately from execution.  Each pair
# alternates order to reduce drift bias, and both compilers use the same support
# root and GCC -O3 wrapper as the executable benchmark below.
printf 'pair\tversion\twall_seconds\tuser_seconds\tsys_seconds\n' > \
    "$outdir/compile-timings.tsv"
for ((pair=0; pair<compile_runs; pair++)); do
    if (( pair % 2 == 0 )); then
        order=(baseline current)
    else
        order=(current baseline)
    fi
    for label in "${order[@]}"; do
        if [ "$label" = baseline ]; then
            compiler="$baseline_compiler"
            run_support_root="$baseline_support_root"
        else
            compiler="$current_compiler"
            run_support_root="$current_support_root"
        fi
        dir="$outdir/compile-$label-$pair"
        rm -rf "$dir"
        mkdir -p "$dir"
        cp "$source_file" "$dir/benchmark.as"
        : > "$dir/cc.log"
        (
            cd "$dir"
            /usr/bin/time -f '%e\t%U\t%S' -o time.txt \
                env ALDORROOT="$run_support_root" \
                    BENCH_CC_LOG="$dir/cc.log" \
                    REAL_CC="$real_cc" \
                    PATH="$outdir/wrap:$run_support_root/bin:/usr/bin:/bin" \
                    "$compiler" -Fx -Y"$run_support_root/lib" \
                        -Lalgebra -Laldor -O -Qall benchmark.as \
                        > compile.log 2>&1
        )
        read -r wall user sys < <(tr '\t' ' ' < "$dir/time.txt")
        printf '%s\t%s\t%s\t%s\t%s\n' \
            "$pair" "$label" "$wall" "$user" "$sys" \
            >> "$outdir/compile-timings.tsv"
    done
done

python3 - "$outdir" <<'PY'
import csv
import statistics
import sys
from pathlib import Path

out = Path(sys.argv[1])
with (out / "compile-timings.tsv").open(newline="") as f:
    raw = list(csv.DictReader(f, delimiter="\t"))
rows = [(int(r["pair"]), r["version"], float(r["wall_seconds"]),
         float(r["user_seconds"]) + float(r["sys_seconds"])) for r in raw]
with (out / "compile-summary.txt").open("w") as f:
    print(f"Measured paired compile runs: {len(set(r[0] for r in rows))}", file=f)
    for metric, pos in (("compile cpu", 3), ("compile wall", 2)):
        med = {}
        for label in ("baseline", "current"):
            vals = [r[pos] for r in rows if r[1] == label]
            med[label] = statistics.median(vals)
        by_pair = {}
        for r in rows:
            by_pair.setdefault(r[0], {})[r[1]] = r[pos]
        pair_ratios = [v["current"] / v["baseline"]
                       for v in by_pair.values() if len(v) == 2]
        paired = statistics.median(pair_ratios)
        print(f"{metric} baseline median: {med['baseline']:.6f} s", file=f)
        print(f"{metric} current median:  {med['current']:.6f} s", file=f)
        print(f"{metric} median paired ratio: {paired:.6f}", file=f)
        print(f"{metric} paired change: {(paired - 1.0) * 100.0:+.3f}%", file=f)
PY

build_one() {
    local label=$1 compiler=$2 run_support_root=$3 dir="$outdir/$1"
    cp "$source_file" "$dir/benchmark.as"
    : > "$dir/cc.log"
    (
        cd "$dir"
        ALDORROOT="$run_support_root" \
        BENCH_CC_LOG="$dir/cc.log" \
        REAL_CC="$real_cc" \
        PATH="$outdir/wrap:$run_support_root/bin:/usr/bin:/bin" \
        "$compiler" -Fx -Y"$run_support_root/lib" \
            -Lalgebra -Laldor -O -Qall benchmark.as \
            > compile.log 2>&1
        ./benchmark > output.txt
        sha256sum output.txt > output.sha256
    )
}

build_one baseline "$baseline_compiler" "$baseline_support_root"
build_one current  "$current_compiler" "$current_support_root"

if ! cmp -s "$outdir/baseline/output.txt" "$outdir/current/output.txt"; then
    echo "ERROR: benchmark outputs differ" >&2
    exit 1
fi

cpu=${BENCH_CPU:-}
if [ -z "$cpu" ]; then
    cpu=$(python3 - <<'PY'
import os
print(min(os.sched_getaffinity(0)))
PY
)
fi

python3 - "$outdir" "$cpu" "$warmups" "$runs" <<'PY'
import csv
import os
import resource
import statistics
import subprocess
import sys
import time
from pathlib import Path

out = Path(sys.argv[1])
cpu = sys.argv[2]
warmups = int(sys.argv[3])
runs = int(sys.argv[4])
cmd = {
    "baseline": ["taskset", "-c", cpu, str(out / "baseline" / "benchmark")],
    "current":  ["taskset", "-c", cpu, str(out / "current" / "benchmark")],
}

for _ in range(warmups):
    for label in ("baseline", "current"):
        subprocess.run(cmd[label], stdout=subprocess.DEVNULL, check=True)

rows = []
for pair in range(runs):
    order = ("baseline", "current") if pair % 2 == 0 else \
            ("current", "baseline")
    for label in order:
        r0 = resource.getrusage(resource.RUSAGE_CHILDREN)
        t0 = time.perf_counter()
        subprocess.run(cmd[label], stdout=subprocess.DEVNULL, check=True)
        wall = time.perf_counter() - t0
        r1 = resource.getrusage(resource.RUSAGE_CHILDREN)
        cpu_time = ((r1.ru_utime - r0.ru_utime) +
                    (r1.ru_stime - r0.ru_stime))
        rows.append((pair, label, wall, cpu_time))

with (out / "timings.tsv").open("w", newline="") as f:
    w = csv.writer(f, delimiter="\t")
    w.writerow(("pair", "version", "wall_seconds", "cpu_seconds"))
    w.writerows(rows)

summary = {}
for metric, pos in (("wall", 2), ("cpu", 3)):
    med = {}
    for label in ("baseline", "current"):
        vals = [row[pos] for row in rows if row[1] == label]
        med[label] = statistics.median(vals)
    ratio = med["current"] / med["baseline"]
    by_pair = {}
    for row in rows:
        by_pair.setdefault(row[0], {})[row[1]] = row[pos]
    pair_ratios = [v["current"] / v["baseline"]
                   for v in by_pair.values() if len(v) == 2]
    paired = statistics.median(pair_ratios)
    summary[metric] = (med["baseline"], med["current"], ratio, paired)

with (out / "summary.txt").open("w") as f:
    cs = out / "compile-summary.txt"
    if cs.exists():
        print(cs.read_text().rstrip(), file=f)
        print(file=f)
    print(f"CPU affinity: {cpu}", file=f)
    print(f"Warmups: {warmups}", file=f)
    print(f"Measured paired runs: {runs}", file=f)
    for metric in ("cpu", "wall"):
        base, cur, ratio, paired = summary[metric]
        print(f"{metric} baseline median: {base:.6f} s", file=f)
        print(f"{metric} current median:  {cur:.6f} s", file=f)
        print(f"{metric} ratio of medians: {ratio:.6f}", file=f)
        print(f"{metric} median paired ratio: {paired:.6f}", file=f)
        print(f"{metric} paired change: {(paired - 1.0) * 100.0:+.3f}%", file=f)

print((out / "summary.txt").read_text(), end="")
PY

#!/bin/bash
#
# generated-c-compare.sh: Aldor build and development support.
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
set -u

LABEL="${1:?label required}"
ROOT_A="${2:?first ALDORROOT required}"
ROOT_B="${3:?second ALDORROOT required}"
SRCROOT="${4:?source root required}"
OUTROOT="${5:?output root required}"

mkdir -p "$OUTROOT"
RESULTS="$OUTROOT/results.tsv"
FINGERPRINT="$OUTROOT/inputs.sha256"

sha256_file() {
        if command -v sha256sum >/dev/null 2>&1; then
                sha256sum "$1" | awk '{print $1}'
        elif command -v shasum >/dev/null 2>&1; then
                shasum -a 256 "$1" | awk '{print $1}'
        elif command -v openssl >/dev/null 2>&1; then
                openssl dgst -sha256 "$1" | awk '{print $NF}'
        else
                echo "no SHA-256 utility available" >&2
                return 1
        fi
}

sha256_files() {
        if command -v sha256sum >/dev/null 2>&1; then
                sha256sum "$@" | sha256sum | awk '{print $1}'
        elif command -v shasum >/dev/null 2>&1; then
                shasum -a 256 "$@" | shasum -a 256 | awk '{print $1}'
        elif command -v openssl >/dev/null 2>&1; then
                for f in "$@"; do
                        openssl dgst -sha256 "$f"
                done | openssl dgst -sha256 | awk '{print $NF}'
        else
                echo "no SHA-256 utility available" >&2
                return 1
        fi
}

root_fingerprint() {
        local root="$1"
        local inputs=(
                "$root/bin/aldor"
                "$root/lib/libaldor.al"
                "$root/lib/libalgebra.al"
                "$root/lib/libalgebra-gmp.al"
        )
        local f
        for f in "${inputs[@]}"; do
                if [ ! -f "$f" ]; then
                        echo "missing comparison input: $f" >&2
                        return 1
                fi
        done
        sha256_files "${inputs[@]}"
}

FP_A=$(root_fingerprint "$ROOT_A") || exit 1
FP_B=$(root_fingerprint "$ROOT_B") || exit 1
CURRENT_FP="$FP_A $FP_B"

if [ -f "$RESULTS" ] && [ ! -f "$FINGERPRINT" ]; then
        rm -f "$RESULTS"
        rm -rf "$OUTROOT"/work-*
elif [ -f "$FINGERPRINT" ] && \
     [ "$(cat "$FINGERPRINT")" != "$CURRENT_FP" ]; then
        rm -f "$RESULTS"
        rm -rf "$OUTROOT"/work-*
fi
printf '%s\n' "$CURRENT_FP" > "$FINGERPRINT"

if [ ! -f "$RESULTS" ]; then
        printf 'label\tbackend\ttest\trc_a\trc_b\tsha_a\tsha_b\tidentical\n' \
                > "$RESULTS"
fi

EXTRACT="$ROOT_A/bin/extract"
[ -x "$EXTRACT" ] || EXTRACT="$ROOT_A/toolbin/extract"
if [ ! -x "$EXTRACT" ]; then
        echo "missing installed extract tool" >&2
        exit 1
fi

SRCDIR="$SRCROOT/lib/algebra/src"
files=()
while IFS= read -r f; do
        if grep -q ALDORTEST "$f"; then
                files[${#files[@]}]="$f"
        fi
done < <(find "$SRCDIR" -name '*.as' -print | sort)

already_done() {
        awk -F '\t' -v b="$1" -v t="$2" \
                'NR > 1 && $2 == b && $3 == t { found=1 } END { exit !found }' \
                "$RESULTS"
}

compile_one() {
        local root="$1" backend="$2" t="$3" tag="$4"
        rm -f "$t.test.c"
        if [ "$backend" = std ]; then
                ALDORROOT="$root" "$root/bin/aldor" -Fc -y"$root/lib" \
                        -lalgebra -laldor -q1 -qinline-all "$t.test.as" \
                        >"compile-$tag.out" 2>&1
        else
                ALDORROOT="$root" "$root/bin/aldor" -Fc -y"$root/lib" \
                        -lalgebra-gmp -laldor -q1 -qinline-all \
                        -cruntime=foam-gmp,gmp -dGMP "$t.test.as" \
                        >"compile-$tag.out" 2>&1
        fi
        local rc=$?
        if [ "$rc" -eq 0 ] && [ -s "$t.test.c" ]; then
                mv "$t.test.c" "$tag.c"
        fi
        return "$rc"
}

for backend in std gmp; do
        for f in "${files[@]}"; do
                t=$(basename "$f" .as)
                if already_done "$backend" "$t"; then
                        printf '%s %-3s %-18s resumed\n' \
                                "$LABEL" "$backend" "$t"
                        continue
                fi

                w="$OUTROOT/work-$backend-$t"
                rm -rf "$w"
                mkdir -p "$w"
                cd "$w"
                "$EXTRACT" -mALDORTEST -o "$t.test.as" "$f" \
                        >extract.log 2>&1

                compile_one "$ROOT_A" "$backend" "$t" a
                rc_a=$?
                compile_one "$ROOT_B" "$backend" "$t" b
                rc_b=$?

                sha_a=NA
                sha_b=NA
                identical=NA
                [ -s a.c ] && sha_a=$(sha256_file a.c)
                [ -s b.c ] && sha_b=$(sha256_file b.c)
                if [ "$rc_a" -eq "$rc_b" ]; then
                        if [ "$rc_a" -ne 0 ]; then
                                identical=YES
                        elif [ "$sha_a" = "$sha_b" ]; then
                                identical=YES
                        else
                                identical=NO
                        fi
                else
                        identical=NO
                fi

                printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
                        "$LABEL" "$backend" "$t" "$rc_a" "$rc_b" \
                        "$sha_a" "$sha_b" "$identical" >> "$RESULTS"
                printf '%s %-3s %-18s rc=%s/%s identical=%s\n' \
                        "$LABEL" "$backend" "$t" "$rc_a" "$rc_b" \
                        "$identical"
        done
done

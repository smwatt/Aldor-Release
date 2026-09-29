#!/usr/bin/env python3
"""Guard Aldor's remaining raw va_arg ABI sites.

The dangerous class is a caller passing one promoted type through `...` while
its callee retrieves another type with va_arg.  This gate deliberately keeps a
small, reviewed inventory of every raw va_arg extraction in aldor/src.  A new
site, a removed site, or a changed retrieval type therefore requires an
explicit review rather than silently entering the portability build.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "aldor" / "src"

EXPECTED = {
    "absyn.cpp": ["Symbol", "Doc", "MutString", "AbSyn"],
    "fint.cpp": ["DataObj"],
    "foam_c.cpp": ["FiSInt"],
    "format.cpp": [
        "int", "int", "short *", "long *", "int *", "char *", "void *",
        "long", "int", "unsigned long", "unsigned int", "LongDouble", "double",
    ],
    "genfoam.cpp": ["Foam", "Foam", "Foam"],
    "sexpr.cpp": ["SExpr", "SExpr"],
    "sto_btree.c0": ["int", "FILE *", "CBool"],
    "strops.cpp": ["String", "String", "String", "String"],
    "tconst.cpp": ["TForm"],
    "tform.cpp": ["TForm", "TForm", "TForm", "TForm", "TForm"],
}

VA_ARG_RE = re.compile(r"va_arg\s*\(\s*[^,]+,\s*([^\)]+)\)")

def norm(t: str) -> str:
    return re.sub(r"\s+", " ", t.strip()).replace(" *", " *")

found = {}
for path in sorted(SRC.iterdir()):
    if not path.is_file() or path.suffix not in {".cpp", ".c", ".c0", ".h"}:
        continue
    vals = [norm(v) for v in VA_ARG_RE.findall(path.read_text(errors="ignore"))]
    if vals:
        found[path.name] = vals

ok = True
if found != EXPECTED:
    ok = False
    print("FAIL: raw va_arg inventory changed", file=sys.stderr)
    for name in sorted(set(found) | set(EXPECTED)):
        if found.get(name) != EXPECTED.get(name):
            print(f"  {name}: expected {EXPECTED.get(name)!r}", file=sys.stderr)
            print(f"       found {found.get(name)!r}", file=sys.stderr)

checks = [
    (SRC / "axlcomp.cpp", r"stoCtl\s*\(\s*StoCtl_GcFile\s*,\s*\(\s*int\s*\)",
     "StoCtl_GcFile must not receive an int through varargs"),
    (SRC / "sto_btree.c0", r"StoFiFun\s*\)\s*\([^\n]*\.\.\.",
     "Store tracer must not restore a variadic erased closure prototype"),
    (SRC / "ccode.cpp", r"\bccoNew\s*\([^;]*\.\.\.",
     "CCode must not restore its mixed raw-varargs constructor"),
    (SRC / "ccode.h", r'strPrintf\s*\(\s*"%ldL"\s*,\s*num\s*\)',
     "ccoIntOf must convert its argument to long before %ld varargs formatting"),
]
for path, pattern, message in checks:
    text = path.read_text(errors="ignore")
    if re.search(pattern, text):
        ok = False
        print(f"FAIL: {message}: {path.relative_to(ROOT)}", file=sys.stderr)

if ok:
    print("PASS: raw va_arg ABI inventory and forbidden-pattern checks")
    for name, vals in EXPECTED.items():
        print(f"  {name}: {', '.join(vals)}")
    sys.exit(0)
sys.exit(1)

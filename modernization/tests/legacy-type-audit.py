#!/usr/bin/env python3
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
"""Audit removed C/C++ aliases without confusing comments/string literals.

The Aldor language itself legitimately contains names such as Bool and Pointer;
this audit is deliberately restricted to C/C++ source tokens.
"""
from pathlib import Path
import re, sys

ROOT = Path(__file__).resolve().parents[1]
SEARCH_ROOTS = [
    ROOT / "aldor" / "src",
    ROOT / "aldor" / "subcmd",
    ROOT / "aldor" / "contrib",
    ROOT / "utils",
    ROOT / "misc",
]
EXTS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hpp"}
REMOVED = ("Bool", "Pointer")
LEXICAL_NOISE = re.compile(
    r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\])*\"|'(?:\\.|[^'\\])*'",
    re.S,
)

def strip_noise(text: str) -> str:
    def repl(match: re.Match[str]) -> str:
        return "".join("\n" if ch == "\n" else " " for ch in match.group())
    return LEXICAL_NOISE.sub(repl, text)

bad = []
for base in SEARCH_ROOTS:
    if not base.exists():
        continue
    for path in base.rglob("*"):
        if not path.is_file() or path.suffix not in EXTS:
            continue
        text = path.read_text(errors="ignore")
        code = strip_noise(text)
        for name in REMOVED:
            for hit in re.finditer(r"\b" + name + r"\b", code):
                line = code.count("\n", 0, hit.start()) + 1
                bad.append((path.relative_to(ROOT), line, name))

if bad:
    for path, line, name in bad:
        print(f"{path}:{line}: residual removed token {name}")
    sys.exit(1)
print("legacy-type-audit: PASS (no C/C++ code tokens Bool or Pointer)")

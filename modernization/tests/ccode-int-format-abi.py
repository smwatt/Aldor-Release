#!/usr/bin/env python3
"""Guard the CCode integer-literal formatter against varargs ABI mismatch."""
from pathlib import Path
import re, sys
root = Path(__file__).resolve().parents[2]
hdr = (root / 'aldor/src/ccode.h').read_text()
pat = re.compile(r'#define\s+ccoIntOf\(num\)\s+ccoIntVal\(Symbol::intern\(strPrintf\("%ldL",\s*\(long\)\(num\)\)\)\)')
if not pat.search(hdr):
    print('FAIL: ccoIntOf must pass a long to %ldL', file=sys.stderr)
    sys.exit(1)
print('PASS: ccoIntOf %ldL argument is explicitly long')

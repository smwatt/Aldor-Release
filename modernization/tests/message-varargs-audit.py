#!/usr/bin/env python3
"""Check message-catalog varargs arity against the selected message formats.

The C/C++ compiler cannot format-check comsg*/msgFPrintf calls because the
format string is selected indirectly by a numeric message id.  This source
check resolves the generated comsgdb table, finds active calls, and verifies
that each call supplies exactly the number of arguments consumed by its
message format (including `*` width/precision arguments).
"""
from pathlib import Path
import ast
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "aldor" / "src"

# (message-argument index, first variadic-argument index), zero based.
FUNS = {
    "comsgFatal": (1, 2), "comsgError": (1, 2),
    "comsgWarning": (1, 2), "comsgRemark": (1, 2),
    "comsgNError": (1, 2), "comsgNWarning": (1, 2),
    "comsgNRemark": (1, 2), "comsgNote": (1, 2),
    "comsgWarnPos": (1, 2), "comsgFPrintf": (1, 2),
    "bloopMsgFPrintf": (1, 2), "msgFPrintf": (2, 3),
}


def generated_formats():
    text = (SRC / "comsgdb.c").read_text(errors="ignore")
    arrays = {}
    for m in re.finditer(
        r"static char\s+(msg_comsgdb_\d+_\d+)\[\]\s*=\s*\{(.*?)\n\};",
        text, re.S):
        chars = []
        for tok in re.findall(r"'(?:\\.|[^'\\])+'|\b\d+\b", m.group(2)):
            if tok.startswith("'"):
                chars.append(ast.literal_eval(tok))
            else:
                n = int(tok)
                if n == 0:
                    break
                chars.append(chr(n))
        arrays[m.group(1)] = "".join(chars)

    fmts = {}
    entry = re.compile(
        r'\{\s*1,\s*\d+,\s*"(ALDOR_[^"]+)",\s*\n?\s*'
        r'((?:"(?:[^"\\]|\\.)*")|(?:msg_comsgdb_\d+_\d+))\s*\}', re.S)
    for m in entry.finditer(text):
        value = m.group(2)
        if value.startswith('"'):
            fmts[m.group(1)] = ast.literal_eval(value)
        else:
            fmts[m.group(1)] = arrays[value]
    return fmts


def mask_comments_and_literals(s):
    """Preserve positions, but hide comments/string contents for call search."""
    a = list(s)
    i, state = 0, "code"
    while i < len(s):
        c = s[i]
        n = s[i + 1] if i + 1 < len(s) else ""
        if state == "code":
            if c == '"': state = "str"
            elif c == "'": state = "chr"
            elif c == '/' and n == '/':
                state = "line"; a[i] = a[i + 1] = ' '; i += 1
            elif c == '/' and n == '*':
                state = "block"; a[i] = a[i + 1] = ' '; i += 1
        elif state == "str":
            if c == '\\':
                a[i] = ' '
                if i + 1 < len(a): a[i + 1] = ' '
                i += 1
            elif c == '"': state = "code"
            else: a[i] = ' '
        elif state == "chr":
            if c == '\\':
                a[i] = ' '
                if i + 1 < len(a): a[i + 1] = ' '
                i += 1
            elif c == "'": state = "code"
            else: a[i] = ' '
        elif state == "line":
            if c == '\n': state = "code"
            else: a[i] = ' '
        elif state == "block":
            if c == '*' and n == '/':
                a[i] = a[i + 1] = ' '; state = "code"; i += 1
            else: a[i] = ' '
        i += 1
    return "".join(a)


def close_paren(s, opening):
    depth, i, state = 1, opening + 1, "code"
    while i < len(s):
        c = s[i]
        n = s[i + 1] if i + 1 < len(s) else ""
        if state == "code":
            if c == '"': state = "str"
            elif c == "'": state = "chr"
            elif c == '/' and n == '/': state = "line"; i += 1
            elif c == '/' and n == '*': state = "block"; i += 1
            elif c == '(': depth += 1
            elif c == ')':
                depth -= 1
                if depth == 0: return i
        elif state == "str":
            if c == '\\': i += 1
            elif c == '"': state = "code"
        elif state == "chr":
            if c == '\\': i += 1
            elif c == "'": state = "code"
        elif state == "line":
            if c == '\n': state = "code"
        elif state == "block":
            if c == '*' and n == '/': state = "code"; i += 1
        i += 1
    return None


def split_args(s):
    result, start, stack, i, state = [], 0, [], 0, "code"
    closes = {')': '(', ']': '[', '}': '{'}
    while i < len(s):
        c = s[i]
        n = s[i + 1] if i + 1 < len(s) else ""
        if state == "code":
            if c == '"': state = "str"
            elif c == "'": state = "chr"
            elif c == '/' and n == '/': state = "line"; i += 1
            elif c == '/' and n == '*': state = "block"; i += 1
            elif c in '([{': stack.append(c)
            elif c in closes:
                if stack: stack.pop()
            elif c == ',' and not stack:
                result.append(s[start:i].strip()); start = i + 1
        elif state == "str":
            if c == '\\': i += 1
            elif c == '"': state = "code"
        elif state == "chr":
            if c == '\\': i += 1
            elif c == "'": state = "code"
        elif state == "line":
            if c == '\n': state = "code"
        elif state == "block":
            if c == '*' and n == '/': state = "code"; i += 1
        i += 1
    result.append(s[start:].strip())
    return result


def format_arg_count(fmt):
    count, i = 0, 0
    while i < len(fmt):
        if fmt[i] != '%': i += 1; continue
        i += 1
        if i < len(fmt) and fmt[i] == '%': i += 1; continue
        while i < len(fmt) and fmt[i] in '-+ #0': i += 1
        if i < len(fmt) and fmt[i] == '*': count += 1; i += 1
        else:
            while i < len(fmt) and fmt[i].isdigit(): i += 1
        if i < len(fmt) and fmt[i] == '.':
            i += 1
            if i < len(fmt) and fmt[i] == '*': count += 1; i += 1
            else:
                while i < len(fmt) and fmt[i].isdigit(): i += 1
        for lm in ('hh', 'll', 'h', 'l', 'L', 'z', 'j', 't'):
            if fmt.startswith(lm, i): i += len(lm); break
        if i < len(fmt):
            if fmt[i] != '%': count += 1
            i += 1
    return count

fmts = generated_formats()
errors = []
calls = 0
for path in sorted(list(SRC.glob('*.cpp')) + list(SRC.glob('*.c'))):
    text = path.read_text(errors="ignore")
    mask = mask_comments_and_literals(text)
    for fn, (msg_index, vararg_index) in FUNS.items():
        for m in re.finditer(r'\b' + re.escape(fn) + r'\s*\(', mask):
            op = mask.find('(', m.start())
            cp = close_paren(text, op)
            if cp is None: continue
            args = split_args(text[op + 1:cp])
            if len(args) <= msg_index: continue
            msg = args[msg_index].strip()
            if not msg.startswith('ALDOR_'): continue
            calls += 1
            line = text.count('\n', 0, m.start()) + 1
            if msg not in fmts:
                errors.append(f"{path.name}:{line}: unknown message format {msg}")
                continue
            need = format_arg_count(fmts[msg])
            got = len(args[vararg_index:])
            if need != got:
                errors.append(
                    f"{path.name}:{line}: {fn} {msg} consumes {need} "
                    f"format arguments but call supplies {got}")

if errors:
    print("FAIL: message-catalog varargs audit", file=sys.stderr)
    for error in errors:
        print("  " + error, file=sys.stderr)
    sys.exit(1)

print(f"PASS: message-catalog varargs arity ({calls} active calls, {len(fmts)} formats)")

#!/bin/bash
#
# check-package-continuity.sh: Aldor build and development support.
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
set -eu

usage() {
        echo "usage: $0 PREVIOUS_ROOT CANDIDATE_ROOT [APPROVED_REMOVALS|-] [RELOCATIONS]" >&2
        exit 2
}

[ "$#" -ge 2 ] && [ "$#" -le 4 ] || usage
prev=$1
cand=$2
approved=${3:-}
relocations=${4:-}
[ "$approved" = - ] && approved=

[ -d "$prev" ] || { echo "missing previous package root: $prev" >&2; exit 2; }
[ -d "$cand" ] || { echo "missing candidate package root: $cand" >&2; exit 2; }

if [ ! -d "$cand/.git" ]; then
        echo "package continuity failure: top-level .git is missing" >&2
        exit 1
fi
if ! git -C "$cand" rev-parse --git-dir >/dev/null 2>&1; then
        echo "package continuity failure: top-level .git is not a Git repository" >&2
        exit 1
fi
if ! git -C "$cand" fsck --full >/dev/null 2>&1; then
        echo "package continuity failure: git fsck failed" >&2
        exit 1
fi
if [ -e "$cand/distro/.git" ]; then
        echo "package continuity failure: distro/.git should not exist" >&2
        exit 1
fi

work=$(mktemp -d "${TMPDIR:-/tmp}/aldor-package-continuity.XXXXXX")
trap 'rm -rf "$work"' EXIT HUP INT TERM

list_payload() {
        root=$1
        (
                cd "$root"
                find . \( -path './.git' -o -path './.git/*' -o \
                           -path './distro/.git' -o -path './distro/.git/*' \) -prune -o \
                       \( -type f -o -type l \) -print
        ) | sed 's#^\./##' | LC_ALL=C sort -u
}

list_payload "$prev" > "$work/previous"
list_payload "$cand" > "$work/candidate"
comm -23 "$work/previous" "$work/candidate" > "$work/removed"

 : > "$work/approved"
if [ -n "$approved" ]; then
        [ -f "$approved" ] || {
                echo "approved-removals file does not exist: $approved" >&2
                exit 2
        }
        sed -e '/^[[:space:]]*#/d' -e '/^[[:space:]]*$/d' "$approved" >> "$work/approved"
fi

if [ -n "$relocations" ]; then
        [ -f "$relocations" ] || {
                echo "relocations file does not exist: $relocations" >&2
                exit 2
        }
        tab=$(printf '\t')
        while IFS="$tab" read -r oldpath newpath rest; do
                [ -n "$oldpath" ] || continue
                [ "$oldpath" = original_path ] && continue
                [ -e "$prev/$oldpath" ] || {
                        echo "relocation source missing from previous package: $oldpath" >&2
                        exit 1
                }
                [ -e "$cand/$newpath" ] || {
                        echo "relocation destination missing from candidate package: $newpath" >&2
                        exit 1
                }
                printf '%s\n' "$oldpath" >> "$work/approved"
        done < "$relocations"
fi

LC_ALL=C sort -u "$work/approved" -o "$work/approved"
comm -23 "$work/removed" "$work/approved" > "$work/unapproved"

if [ -s "$work/unapproved" ]; then
        echo "package continuity failure: unapproved paths were removed:" >&2
        sed 's/^/  /' "$work/unapproved" >&2
        exit 1
fi

echo "package continuity PASS"

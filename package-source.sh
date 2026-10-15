#!/bin/bash
# Reproducible standalone source export with embedded Git provenance.
set -euo pipefail

script_ref=${BASH_SOURCE[0]}
case "$script_ref" in
    */*) script_parent=${script_ref%/*} ;;
    *)   script_parent=. ;;
esac
here=$(cd -P -- "$script_parent" && pwd -P)
top=$(cd -P -- "$here/.." && pwd -P)
ref=${1:-HEAD}
expected=$(git -C "$top" rev-parse --verify "$ref^{commit}") || exit 2
version=$(git -C "$top" show "$ref:distro/aldor/src/version.h" |
    sed -n \
    's/^#define[[:space:]][[:space:]]*ALDOR_VERSION_STRING[[:space:]][[:space:]]*"\([^"]*\)".*/\1/p' |
    head -1)
[ -n "$version" ] || { echo 'Cannot determine Aldor version from source revision' >&2; exit 2; }
safe_ref=$(printf '%s' "$ref" | tr -c 'a-zA-Z0-9._-' '_')
out=${2:-"$top/delivery/aldor-$version-$safe_ref-source.tgz"}

case "$out" in
    */*) mkdir -p "${out%/*}" ;;
esac
tmp="$out.tmp.$$"
trap 'rm -f "$tmp"' EXIT
prefix="aldor-$version/"
git -C "$top" archive --format=tar --prefix="$prefix" "$ref" | gzip -n > "$tmp"
actual=$(gzip -cd "$tmp" | tar -xOf - "${prefix}distro/SOURCE-REVISION")
if [ "$actual" != "$expected" ]; then
    echo "Source revision mismatch: expected $expected, got $actual" >&2
    exit 2
fi
mv "$tmp" "$out"
echo "Source archive: $out"
echo "Git commit:     $actual"

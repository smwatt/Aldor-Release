#!/bin/bash
# Source revision shared by the build/test log driver and the release driver.
aldorSourceRevision() {
    local root=${1:?source root required} head ref git_top stamp
    git_top=$(git -C "$root" rev-parse --show-toplevel 2>/dev/null || true)
    if [ -n "$git_top" ] &&
       [ "$(cd -P -- "$git_top" 2>/dev/null && pwd -P)" = "$root" ]; then
        head=$(git -C "$root" rev-parse HEAD 2>/dev/null || true)
        case "$head" in
            *[!0-9a-fA-F]*|'') ;;
            *)
                case "${#head}" in
                    40|64) printf '%s\n' "$head"; return 0 ;;
                esac ;;
        esac
    fi
    # A complete ordinary checkout can be identified without git on PATH.
    if [ -r "$root/.git/HEAD" ]; then
        head=$(cat "$root/.git/HEAD" 2>/dev/null || true)
        case "$head" in
            'ref: '*)
                ref=${head#ref: }
                if [ -r "$root/.git/$ref" ]; then
                    head=$(cat "$root/.git/$ref" 2>/dev/null || true)
                elif [ -r "$root/.git/packed-refs" ]; then
                    head=$(awk -v r="$ref" '$2 == r { print $1; exit }' \
                        "$root/.git/packed-refs")
                else
                    head=
                fi ;;
        esac
        case "$head" in
            *[!0-9a-fA-F]*|'') ;;
            *)
                case "${#head}" in
                    40|64) printf '%s\n' "$head"; return 0 ;;
                esac ;;
        esac
    fi
    # A Git-exported source has a substituted SHA. A plain copy retains the
    # literal $Format template, which is deliberately NOT a valid revision.
    stamp="$root/distro/SOURCE-REVISION"
    if [ -r "$stamp" ]; then
        IFS= read -r head < "$stamp" || true
        case "$head" in
            *[!0-9a-fA-F]*|'') ;;
            *)
                case "${#head}" in
                    40|64) printf '%s\n' "$head"; return 0 ;;
                esac ;;
        esac
    fi
    printf 'unknown\n'
}

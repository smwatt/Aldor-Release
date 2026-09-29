#!/bin/bash
#
# make-distro-tgz.sh: Aldor build and development support.
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

here=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
distro=$(cd -P -- "$here/../.." && pwd -P)
out=${1:-"$(cd -P -- "$distro/.." && pwd -P)/distro.tgz"}

bash "$here/check-standalone-distro.sh"
parent=$(cd -P -- "$distro/.." && pwd -P)
base=${distro##*/}
(
    cd "$parent"
    COPYFILE_DISABLE=1 tar -czf "$out" --exclude='.DS_Store' --exclude='._*' "$base"
)
printf 'Wrote %s\n' "$out"

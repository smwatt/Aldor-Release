#!/bin/bash

if [ -z "$ALDORTMP" ] ; then export ALDORTMP=/tmp ; fi

export TestOut="$ALDORROOT/testout/aldor"

mkdir -p "$TestOut"

doCleanup() {
    # if [ -d "$TestTmp" ] ; then 
    #     rm -rf "$TestTmp"
    # fi
    :
}

doTest() { (
    export PATH="$ALDORROOT/toolbin:$ALDORROOT/bin:$PATH"
    export TestDir="`pwd`"

    local TMP_OK="$TestOut/tout-ok.log"
    local TMP_DIFF="$TestOut/tout-diff.log"

    echo "" > "$TMP_OK"
    echo "" > "$TMP_DIFF"

    echo "============ OK Self-Tests  ============"
    2>&1 doTestAldor -diff   "${OK_FILES[@]}"   | tee -a "$TMP_OK"
    echo "============ DIFF Self-Tests  ============"
    2>&1 doTestAldor -nodiff "${DIFF_FILES[@]}" | tee -a "$TMP_DIFF"

    echo "============ Logs ============"
    echo -n "ok   results:" ; analyzeLog "$TMP_OK"
    echo -n "diff results:" ; analyzeLog "$TMP_DIFF"

    local badOk badDiff
    badOk=$(grep -Ec '^>> .*(DIFFERENT|ERROR)$' "$TMP_OK" || true)
    badDiff=$(grep -Ec '^>> ' "$TMP_DIFF" || true)
    doCleanup
    [ "$badOk" -eq 0 ] && [ "$badDiff" -eq 0 ]
) }


# Lists of tests,

OK_FILES=(
    show-t.sh buffer-t.sh cport-t.sh dnf-t.sh priq-t.sh string-t.sh
    table-t.sh util-t.sh xfloat-t.sh fluid-t.sh
    bigint-t.sh bitv-t.sh btree-t.sh file-t.sh float-t.sh fname-t.sh
    format-t.sh link-t.sh list-t.sh opsys-t.sh stor1a-t.sh stor1b-t.sh
    stor1c-t.sh stor1d-t.sh store2-t.sh store3-t.sh symbol-t.sh
    ccode-t.sh msg-t.sh
)
DIFF_FILES=(
)

# This line must come last.
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

#!/bin/bash

#
# Extract tests embedded in the ../src files and run against 3 library versions:
# the regular library, the gmp library and the debug library.
#

if [ -z "$ALGEBRAROOT"  ] ; then ALGEBRAROOT="$ALDORROOT" ; fi
PATH="$ALDORROOT/bin:$ALDORROOT/toolbin:$PATH"

#-Clib=gmp
opts=(-fx "-y${ALDORROOT}/lib" "-y${ALGEBRAROOT}/lib")

optionsStd=(-lalgebra     -laldor -q1 -qinline-all)
optionsGmp=(-lalgebra-gmp -laldor -q1 -qinline-all -cruntime=foam-gmp,gmp -dGMP)
optionsDebug=(-lalgebrad -laldord -q1 -dDEBUG)

aldorStd()   { aldor "${opts[@]}" "${optionsStd[@]}"   "$1" ; }
aldorDebug() { aldor "${opts[@]}" "${optionsDebug[@]}" "$1" ; }
aldorGmp()   { aldor "${opts[@]}" "${optionsGmp[@]}"   "$1" ; }
aldorCant()  { echo "echo 'No can do.'" > "$1.test" ; chmod +x "$1.test" ; }

# Test output comparison is shared with the modernization matrix.
# testcmp canonicalizes CRLF to LF only; all other output is significant.

PATH="$ALDORROOT/bin:$ALDORROOT/toolbin:$PATH"

# Run a test with a particular compiler configuration and compare output.
testWith() {
    local compiler="$1"
    local fbase="$2"

    echon "Testing $compiler $bname.test.as ... "
    TEST_STATUS=ERROR_COMPILE
    TEST_NEW="$TestOut/$bname.$compiler.out"
    TEST_REF="$TestDir/testout/$bname.out"

    rm -f "./$bname.test"
    if ! "$compiler" "$fbase.test.as"; then
        echo "ERROR (compile)!"
        return 1
    fi
    if [ ! -x "./$bname.test" ]; then
        echo "ERROR (no executable)!"
        return 1
    fi

    TEST_STATUS=ERROR_RUN
    (
        trap 'if [[ $? -eq 139 ]] ; then echo "Seg Fault" ; fi ' EXIT
        ./"$bname.test"
        rc=$?
        rm -f ./"$bname.test"
        exit $rc
    ) > "$TEST_NEW" 2>&1
    local run_rc=$?
    if [ $run_rc -ne 0 ]; then
        echo "ERROR (run=$run_rc)!"
        echo "--- execution output ($TEST_NEW) ---"
        cat "$TEST_NEW" || true
        return 1
    fi

    if testcmp "$TEST_REF" "$TEST_NEW"; then
        TEST_STATUS=OK
        echo "OK! "
        return 0
    fi

    TEST_STATUS=DIFFERENT
    echo "DIFFERENT! "
    echo "--- reference: $TEST_REF"
    echo "+++ actual:    $TEST_NEW"
    diff -u --strip-trailing-cr "$TEST_REF" "$TEST_NEW" || true
    return 1
}

# Each generated test is a separate process, so run every test under every
# supported library variant.  Historical GMP skips are no longer needed.
canTest() { true; }
    
#
# Run the tests -- called by doMain below.
#
doTest() {
    local  SrcDir="`pwd`/../src"
    export TestDir="`pwd`"

    export TestOut="$ALDORROOT/testout/lib/algebra"
    mkdir -p "$TestOut"
    cd "$TestOut"

    local RESULTS_TSV="$TestOut/results.tsv"
    printf 'suite\tvariant\ttest\tstatus\treference\tactual\n' > "$RESULTS_TSV"

    local totalFailures=0
    local comp
    for comp in aldorStd aldorDebug aldorGmp

    do
        local failCount=0
        local totalCount=0

        local f
        while IFS= read -r f
        do
            local bname=`basename "$f" .as`
            extract -mALDORTEST -o"$bname".test.as "$f"

            if canTest "$comp" "$f" ; then
                if ! testWith "$comp" "$bname" ; then
                    failCount=$(($failCount + 1))
                    totalFailures=$(($totalFailures + 1))
                fi
            else
                if ! testWith aldorCant "$bname" ; then
                    failCount=$(($failCount + 1))
                    totalFailures=$(($totalFailures + 1))
                fi
            fi
            printf 'algebra\t%s\t%s\t%s\t%s\t%s\n' \
                "$comp" "$bname" "$TEST_STATUS" "$TEST_REF" "$TEST_NEW" \
                >> "$RESULTS_TSV"
            totalCount=$(($totalCount + 1))

            # Clean up
            if [ -z "$KeepFiles" ] ; then
                rm -f "$bname.test.as" "$bname.test"
            fi
        done < <(find "$SrcDir" -type f -name '*.as' \
            -exec grep -l ALDORTEST {} +)

        local msg
        if [ $failCount = 0 ] ; then
            msg="$comp: PASSED $totalCount tests"
        else
            msg="$comp: FAILED $failCount of $totalCount tests"
        fi

        echo   "************************************************************"
        echo   "***                                                      ***"
        printf "***  %-50s  ***\n" "$msg"
        echo   "***                                                      ***"
        echo   "************************************************************"
        echo   ""

    done
    cd "$TestDir"
    [ $totalFailures -eq 0 ]
    # if [ -z "$KeepFiles" ] ; then
    #     rm -rf "$TestTmp"
    # else
    #     echo "Keeping files int $TestTmp"
    # fi
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

#!/bin/bash

#
# Extract tests embedded in the ../src files and run them with both 
# the regular aldor library and the debug aldor library.
#

PATH="$ALDORROOT/bin:$ALDORROOT/toolbin:$PATH"

opts=(-Fx -Y"$ALDORROOT/lib" -Clib=gmp -Q1)

regularAldor() { aldor "${opts[@]}" -laldor  -Q inline-all "$1" ; }
debugAldor()   { aldor "${opts[@]}" -laldord -D DEBUG      "$1" ; }

testWith() {
    local compiler="$1"
    local fbase="$2"

    TEST_STATUS=ERROR_COMPILE
    TEST_NEW="$TestOut/$bname.$compiler.out"
    TEST_REF="$TestDir/testout/$bname.out"

    # Compile. Remove a stale executable first so a failed compile cannot
    # accidentally run an executable left by an earlier test.
    rm -f "./$bname.test"
    if ! "$compiler" "$TestOut/$fbase.test.as"; then
        echo "ERROR (compile)!"
        return 1
    fi
    if [ ! -x "./$bname.test" ]; then
        echo "ERROR (no executable)!"
        return 1
    fi

    local testargs=()
    if [ "$bname" = "sal_cmdline" ] ; then testargs=(-v -b -Lfoo -abar) ; fi

    TEST_STATUS=ERROR_RUN
    if ! ./"$bname.test" "${testargs[@]}" > "$TEST_NEW" 2>&1; then
        echo "ERROR (run)!"
        echo "--- execution output ($TEST_NEW) ---"
        cat "$TEST_NEW" || true
        return 1
    fi

    if cmp -s "$TEST_NEW" "$TEST_REF"; then
        TEST_STATUS=OK
        echo "OK!"
        return 0
    fi

    TEST_STATUS=DIFFERENT
    echo "DIFFERENT!"
    echo "--- reference: $TEST_REF"
    echo "+++ actual:    $TEST_NEW"
    diff -u "$TEST_REF" "$TEST_NEW" || true
    return 1
}

doTest() { (
    local  SrcDir="`pwd`/../src"
    export TestDir="`pwd`"

    export TestOut="$ALDORROOT/testout/lib/aldor"
    mkdir -p "$TestOut"
    cd "$TestOut"

    local RESULTS_TSV="$TestOut/results.tsv"
    printf 'suite\tvariant\ttest\tstatus\treference\tactual\n' > "$RESULTS_TSV"

    local failCount=0 totalCount=0

    local compiler
    for compiler in regularAldor debugAldor ; do
        local f
        while IFS= read -r f
        do
            local bname=`basename "$f" .as`
            if [ ! -e "$bname.test.as" ] ; then
                extract -mALDORTEST -o"$bname.test.as" "$f"
            fi

            printf "Testing %-12s %-12s ... " "$compiler" "$bname"

            if ! testWith "$compiler" "$bname" ; then
                ((failCount = failCount + 1))
            fi
            printf 'libaldor\t%s\t%s\t%s\t%s\t%s\n' \
                "$compiler" "$bname" "$TEST_STATUS" "$TEST_REF" "$TEST_NEW" \
                >> "$RESULTS_TSV"
            ((totalCount = totalCount + 1))
        done < <(find "$SrcDir" -type f -name '*.as' \
            -exec grep -l ALDORTEST {} +)
    done

    local msg
    if [ $failCount = 0 ] ; then
        msg="PASSED $totalCount tests"
    else
        msg="FAILED $failCount of $totalCount tests"
    fi
        
    echo   "*******************************************************"
    echo   "***                                                 ***"
    printf "***  libaldor: %-35s  ***\n"  "$msg"
    echo   "***                                                 ***"
    echo   "*******************************************************"

    cd "$TestDir"
    [ $failCount -eq 0 ]
    # if [ ! "$KeepFiles" ] ; then
    #     rm -rf "$TestOut"
    # else
    #     echo "Keeping files in $TestOut"
    # fi
) }

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

#!/bin/bash

if [ -z "$ALDORTMP" ] ; then export ALDORTMP=/tmp ; fi

export TestOut="$ALDORROOT/testout/lib/axllib"
mkdir -p "$TestOut"

doCleanup() {
    # if [ -d "$TestTmp" ] ; then 
    #     rm -rf "$TestTmp"
    # fi
    echo No cleanup needed.
}

doTestHelp() {
    cat <<EndOfHelp
Run the tests individually or in groups.

    doTest [ -help | -debug | -only kind | -mode mode | -diff | -nodiff ] file*

    -help   Show this help then exit

    -dryrun Output the commands that would be invoked, but do not run them.
            This option should come first.

    -debug  Turn on debugging.

    -only   Run all of the tests of a given kind, where kind is one of
               all, script, gen, phase, comp, run, int, errs
            Default is all.

    -mode  Run onl ytests with expected behaviour, where behaviour is one of
               all, ok, fix, diff, err
            Default is all.

    -diff | -nodiff
          Override default diff behaviour (-diff for ok, -nodiff, otherwise)

    If no files are given, then pre-determined lists of files are used.
EndOfHelp
}

doTest() { (
    local kind="all"
    local mode="all"
    local flags=""
    local diffArg="-default_diff"
    export dryrun=""

    while [ $# -gt 0 ] ; do
        case "$1" in
        -help)   doTestHelp                  ; return  ;;
        -dryrun) dryrun="yes"                ; shift   ;;
        -debug)  flags="$flags -debug -keep" ; shift   ;;
        -only)   kind=$2                     ; shift 2 ;;
        -mode)   mode=$2                     ; shift 2 ;;
        -diff | -nodiff) diffArg="$1"        ; shift   ;;
        *)       break
        esac
    done

    export PATH="$ALDORROOT/toolbin:$ALDORROOT/bin:$PATH"
    export TestDir="`pwd`"

    mkdir -p "$TestOut"

    local TMP_OK="$TestOut/tout-ok.log"
    local TMP_FIX="$TestOut/tout-fix.log"   # Temporarily broken -- fix priority
    local TMP_DIFF="$TestOut/tout-diff.log"
    local TMP_ERR="$TestOut/tout-err.log"
    local TMP_ALL="$TestOut/tout-all.log"

    echo "" > "$TMP_OK"
    echo "" > "$TMP_FIX"
    echo "" > "$TMP_DIFF"
    echo "" > "$TMP_ERR"
    echo "" > "$TMP_ALL"

    if [ $# -gt 0 ] ; then
        doTestIf $diffArg $kind all    $mode all  "$TMP_ALL"  $@
    else
        if [ "${ALDOR_RUNTIME_BLOCKED:-0}" != 1 ]; then
            doTestIf $diffArg $kind script $mode ok   "$TMP_OK"   ${ScriptOk[@]}
            doTestIf $diffArg $kind script $mode fix  "$TMP_FIX"  ${ScriptFix[@]}
            doTestIf $diffArg $kind script $mode diff "$TMP_DIFF" ${ScriptDiff[@]}
            doTestIf $diffArg $kind script $mode err  "$TMP_ERR"  ${ScriptErr[@]}
        fi

        doTestIf $diffArg $kind phase  $mode ok   "$TMP_OK"   ${PhaseOk[@]}
        doTestIf $diffArg $kind phase  $mode fix  "$TMP_FIX"  ${PhaseFix[@]}
        doTestIf $diffArg $kind phase  $mode diff "$TMP_DIFF" ${PhaseDiff[@]}
        doTestIf $diffArg $kind phase  $mode err  "$TMP_ERR"  ${PhaseErr[@]}

        doTestIf $diffArg $kind gen    $mode ok   "$TMP_OK"   ${GenOk[@]}
        doTestIf $diffArg $kind gen    $mode fix  "$TMP_FIX"  ${GenFix[@]}
        doTestIf $diffArg $kind gen    $mode diff "$TMP_DIFF" ${GenDiff[@]}
        doTestIf $diffArg $kind gen    $mode err  "$TMP_ERR"  ${GenErr[@]}

        doTestIf $diffArg $kind comp   $mode ok   "$TMP_OK"   ${CompOk[@]}
        doTestIf $diffArg $kind comp   $mode fix  "$TMP_FIX"  ${CompFix[@]}
        doTestIf $diffArg $kind comp   $mode diff "$TMP_DIFF" ${CompDiff[@]}
        doTestIf $diffArg $kind comp   $mode err  "$TMP_ERR"  ${CompErr[@]}

        if [ "${ALDOR_RUNTIME_BLOCKED:-0}" != 1 ]; then
            doTestIf $diffArg $kind int    $mode ok   "$TMP_OK"   ${IntOk[@]}
            doTestIf $diffArg $kind int    $mode fix  "$TMP_FIX"  ${IntFix[@]}
            doTestIf $diffArg $kind int    $mode diff "$TMP_DIFF" ${IntDiff[@]}
            doTestIf $diffArg $kind int    $mode err  "$TMP_ERR"  ${IntErr[@]}

            doTestIf $diffArg $kind run    $mode ok   "$TMP_OK"   ${RunOk[@]}
            doTestIf $diffArg $kind run    $mode fix  "$TMP_FIX"  ${RunFix[@]}
            doTestIf $diffArg $kind run    $mode diff "$TMP_DIFF" ${RunDiff[@]}
            doTestIf $diffArg $kind run    $mode err  "$TMP_ERR"  ${RunErr[@]}
        fi

        doTestIf $diffArg $kind errs   $mode ok   "$TMP_OK"   ${ErrOk[@]}
        doTestIf $diffArg $kind errs   $mode fix  "$TMP_FIX"  ${ErrFix[@]}
        doTestIf $diffArg $kind errs   $mode diff "$TMP_DIFF" ${ErrDiff[@]}
        doTestIf $diffArg $kind errs   $mode err  "$TMP_ERR"  ${ErrErr[@]}
    fi
    cat "$TMP_OK" "$TMP_FIX" "$TMP_DIFF" "$TMP_ERR" >> "$TMP_ALL"

    echo "============ Logs ============"
    doAnalyzeIf $mode ok   "$TMP_OK"
    doAnalyzeIf $mode fix  "$TMP_FIX"
    doAnalyzeIf $mode diff "$TMP_DIFF"
    doAnalyzeIf $mode err  "$TMP_ERR"
    doAnalyzeIf all   all  "$TMP_ALL"

    echo "============ Not Even Tried ============"
    grep -e "-->" ${ScriptDont[@]} ${PhaseDont[@]} ${GenDont[@]} \
                  ${CompDont[@]} ${IntDont[@]} ${RunDont[@]} ${ErrDont[@]} \
                  ${OtherDont[@]} || true

    # Historical diff/err classifications are diagnostic debt.  The release
    # gate is driven by deviations in groups expected to be correct and by
    # entries explicitly marked FIX-priority.
    local overall=0 nbad
    if isWanted "$mode" ok ; then
        nbad=`countTestLines '^>> .*(DIFFERENT.*|ERROR)$' "$TMP_OK"`
        [ "$nbad" -eq 0 ] || overall=1
    fi
    if isWanted "$mode" fix ; then
        nbad=`countTestLines '^>> .*(DIFFERENT.*|ERROR)$' "$TMP_FIX"`
        [ "$nbad" -eq 0 ] || overall=1
    fi
    if isWanted "$mode" diff ; then
        nbad=`countTestLines '^>> .*ERROR$' "$TMP_DIFF"`
        [ "$nbad" -eq 0 ] || overall=1
    fi
    if isWanted "$mode" err ; then
        nbad=`countTestLines '^>> .*DIFFERENT' "$TMP_ERR"`
        [ "$nbad" -eq 0 ] || overall=1
    fi

    # Write a compact machine-readable census alongside the full human log.
    # Full diff bodies remain in tout-*.log and the top-level test.log.
    local RESULTS_TSV="$TestOut/results.tsv"
    printf 'suite\tclass\ttest\tkind\tstatus\n' > "$RESULTS_TSV"
    local cls lf line name kind status rest
    for cls in ok fix diff err ; do
        case "$cls" in
            ok)   lf="$TMP_OK" ;;
            fix)  lf="$TMP_FIX" ;;
            diff) lf="$TMP_DIFF" ;;
            err)  lf="$TMP_ERR" ;;
        esac
        while IFS= read -r line ; do
            case "$line" in
                '>> '*)
                    rest=${line#>> }
                    name=${rest%%: (*}
                    rest=${rest#*: (}
                    kind=${rest%%) *}
                    case "$line" in
                        *' DIFFERENT'*) status=DIFFERENT ;;
                        *' ERROR')       status=ERROR ;;
                        *' OK')          status=OK ;;
                        *)                 status=UNKNOWN ;;
                    esac
                    printf 'axllib\t%s\t%s\t%s\t%s\n' \
                        "$cls" "$name" "$kind" "$status" >> "$RESULTS_TSV"
                    ;;
            esac
        done < "$lf"
    done

    doCleanup
    return $overall
) }


# Run tests if wanted
#
# doTestIf  user-given-kind  wanted-kind \
#                 user-given-mode  wanted-mode \
#                 logFile \
#                 file ...
#
# If no files are given, then all files of the wanted kind and mode are used.

doTestIf() {
    local diffArg="$1"
    local userKind="$2" wantKind="$3" userMode="$4" wantMode="$5"
    local logFile="$6" 
    shift 6

    local wantArg=""
    if [ wantKind != all ] ; then wantArg="-only $wantKind" ; fi

    # Convert diffArg to testaldor argument.
    case $diffArg in
    -diff )   diffArg="" ;;
    -nodiff ) diffArg="-nodiff" ;;
    * )       diffArg="" ;;
    esac

    # Ensure each file is given only once.
    local flist=`echo "$* " | tr ' '  '\n' | sort | uniq`

    if isWanted $userKind $wantKind  $userMode $wantMode ; then
        echo "============ $wantKind $wantMode ============"
        2>&1 doTestAldor $diffArg $wantArg $flist | tee -a "$logFile"
    fi
}

# If this mode was wanted, then analyze the corresponding log.
doAnalyzeIf() {
    local userMode="$1"  wantMode="$2" logName="$3"
    if isWanted $userMode $wantMode ; then
        printf "%-25s" "$wantMode test results:"
        analyzeLog "$logName"
    fi
}

# Test whether keywords match. Run as
#
# isWanted val1 kw1  [val2 kw2 [val3 kw3 [ ... ]]]
#
# Success if   val_i = kw_i   or   val_i = all   or   kw_i = all   for all i.
isWanted() {
    while [ $# -gt 0 ] ; do
        if [ "$1" != "$2"   -a   "$1" != all   -a   "$2" != all ] ; then
            false ; return
        fi
        shift 2
    done
    true
}


# The rest of the file contains lists of tests,
# and the trigger line which must be last.

ScriptOk=(
    ar0.sh ar1.sh ar2.sh ar6.sh bug1213.sh
    bug1226.sh bug1275.sh cmdline0.sh eager0.sh emit0.sh
    errorax.sh exptoc2.sh exptoc.sh fopt-mc.sh fopt-ai.sh
    intcat.sh interact.sh pack0.sh rawrec5.sh respfile.sh
    segext0.sh segext1.sh select3.sh session3.sh session6.sh
    t1070.sh t1075.sh t1157.sh forn8.sh forn9.sh
    forn10.sh genc0.sh fopt-ax.sh arrepl0.sh arrepl1.sh
    ar3.sh ar4.sh ar5.sh emit1.sh fopt.sh
    msg.sh multi.sh session1.sh session4.sh session5.sh
    t1107.sh fopt-fm.sh supcat.sh recov1.sh
)
ScriptFix=(

)
ScriptDiff=(

)
ScriptErr=(

)
ScriptDont=(
    session2.sh
)


PhaseOk=(
    include0.as include1.as include3.as scan0.as scan1.as
    scan2.as scan3.as scan4.as linear0.as linear1.as
    linear2.as linear3.as linear4.as parse0.as parse1.as
    parse2.as abnorm0.as abnorm1.as abnorm2.as macex0.as macex1.as macex2.as
    macex3.as macex4.as macex5.as macex6.as abcheck0.as abcheck1.as abcheck2.as
    abcheck3.as doc0.as qimport.as scope5.as scope7.as scope9.as scope10.as
    tinfer1.as
)
PhaseFix=(

)
PhaseDiff=(

)
PhaseErr=(

)
PhaseDont=(

)


GenOk=(
    big.as builtin1.as default9.as domain1.as enum0.as
    exptoc2.as fact.as flow0.as forn0.as forn1.as
    forn2.as forn3.as forn4.as funvar.as genc1.as
    genc2.as hash0.as hilbert0.as hilbert.as if2.as
    if3.as ifcross0.as lit1.as loop0.as loop1.as
    matops.as ratio0.as record0.as scope3.as triv1.as
    triv3.as triv4.as triv5.as triv6.as tuple2.as
    where0.as bug1200.as t619.as t934.as t947.as
    t1024.as t1142.as doc0.as gen0.as forn6.as
    forn7.as forn7a.as lit0.as forn5o.as forn6o.as
    l00p.as mandel_t.as pack3.as t1085.as t1088.as
    triv3ai.as gen1.as forn5.as domain0.as mandel.as
    pack1.as pack2.as union0.as union1.as srcpos.as
    pack0.as gen2.as gen3.as gen4.as
)
GenFix=(

)
GenDiff=(

)
GenErr=(

)
GenDont=(

)

CompOk=(
    1test.as array0.as array1.as axlist0.as basic.as
    big.as cascade0.as cascade1.as cascade2.as cascade3.as
    cascade4.as catdef0.as catdef1.as catdef2.as coerce0.as
    coerce1.as coerce2.as coerce3.as coerce4.as coerce5.as
    collect0.as collect1.as complex.as convert0.as defarg0.as
    defarg1.as defarg2.as defarg4.as defarg6.as defarg7.as
    defarg8.as defarg9.as defarg10.as default0.as default1.as
    default2.as default3.as default4.as default5.as depend0.as
    depend2.as depend3.as depend4.as depend5.as dnames.as
    domain2.as domain3.as embed.as embed1.as enum0.as
    enum1.as exit0.as exit2.as exn1.as exn2.as
    exn3.as fact.as fix1.as fluid0.as for0.as
    forn0.as forn7a.as forn7.as funlist1.as gc0.as
    genops.as gfGener1.as gfGener2.as goto0.as grok.as
    hash0.as hide0.as hilbert0.as hilbert.as if0.as
    if2.as if3.as if4.as if5.as impl.as
    inline0.as inline1.as inline2.as intfact.as ladder0.as
    lit1.as loop2.as mandel.as none0.as none1.as
    opt0.as opt2.as opt3.as ovload0.as ovload1.as
    ovload2.as parse4.as qimport.as qual0.as qual1.as
    qual2.as rawrec2.as rawrec3.as reclist0.as record0.as
    scobind4.as scope0.as scope1.as scope2.as scope7.as
    scope8.as scope9.as scope11.as segext5.as segext6.as
    segext7.as segment0.as select0.as select1.as self0.as
    setBANG.as slist.as supcat2.as supcat3.as swap1.as
    swap.as test1.as tinfer0.as topsort0.as topsort1.as
    tree1.as tree2.as triv0.as triv1.as triv3.as
    triv4.as triv5.as triv6.as try0.as tuple0.as
    tuple1.as tuple2.as tuple3.as tuple4.as type0.as
    type1.as type2.as type4.as type7.as type8.as
    union2.as vector0.as vector1.as vector2.as where0.as
    where1.as where2.as t619.as t715.as t897.as
    t934.as t944.as t947.as t958.as t966.as
    t973.as t1024.as t1053.as t1059.as t1099.as
    t1105.as t1106.as domain1.as inline3.as intcat0.as
    t1009.as t1093.as fix0.as where3.as hilbert1.as
    gbtest1.as default8.as triv3ai.as mandel_t.as limits.as
    t1094.as t1085.as builtin1.as domain0.as emerge0.as
    f11.as f21.as forn3.as has1.as inline4.as
    lit0.as object1.as rawrec1.as rawrec4.as tinfer2.as
    union0.as union1.as
)
CompFix=(

)
CompDiff=(

)
CompErr=(

)
CompDont=(

)


IntOk=(
    collect0.as default0.as ddata.as depend9.as exn1.as
    exn2.as exn3.as intbug1.as mandel.as oslowtst.as
    tree.as try0.as type0.as type4.as bug1164.as
    bug1235.as bug1237.as bug1238.as bug1242.as bug1247.as
    t1064.as bug1272.as t1029.as t1145.as t1149.as
    gbtest1.as bug954.as t1028.as opt1.as
)
IntFix=(

)
IntDiff=(

)
IntErr=(

)
IntDont=(

)

RunOk=(
    triv0.as triv1.as triv3.as triv4.as triv5.as
    triv6.as 1test.as bigmand.as binadd.as builtin0.as
    builtin1.as cascade0.as cascade1.as cascade2.as cascade3.as
    catdef3.as collect0.as condapply2.as const0.as const1.as
    const2.as const3.as const4.as const5.as ddata.as
    defarg0.as defarg1.as defarg2.as defarg4.as defarg6.as
    defarg8.as defarg9.as defarg10.as default0.as default1.as
    default2.as default3.as default4.as default5.as defgroup1.as
    defgroup3.as depend6.as depend7.as depend8.as df1.as
    domain1.as domain2.as domain3.as embed1.as emerge0.as
    enum0.as enum1.as exit2.as exn1.as exn2.as
    exn3.as extend0.as extend1.as f11.as f21.as
    float0.as float1.as float3.as flow0.as fluid0.as
    format1.as format2.as forn1.as funvar.as gc0.as
    genops.as goto0.as grok.as has1.as hash0.as
    imod0.as impl.as inline0.as inline1.as inline2.as
    inline4.as inline5.as intbug1.as iroots.as loop0.as
    loop1.as loop2.as loop3.as mandel.as none2.as
    opt2.as opt3.as oslowtst.as pack1.as pack2.as
    qual0.as qual2.as ratio0.as rawrec1.as rawrec2.as
    rawrec3.as rawrec4.as record0.as record1.as ref0.as
    scan6.as scobind1.as scobind6.as scope11.as scope3.as
    select0.as select1.as setBANG.as string1.as test1.as
    tree.as tree1.as try0.as tuple0.as tuple1.as
    tuple2.as tuple3.as tuple4.as type0.as type1.as
    type4.as type8.as union0.as union1.as union2.as
    where0.as bug969.as bug1127.as bug1170.as bug1172.as
    bug1193.as bug1237.as t897.as t944.as t950.as
    t958.as t986.as t1029.as t1053.as t715.as
    t1099.as t1106.as t1145.as t1149.as t1059.as
    l00p.as mandel_t.as triv3ai.as t1064.as recfix0.as
    cascade4.as cycle0.as defgroup0.as dnames.as fix0.as
    funlist1.as gbtest1.as gfGener1.as gfGener2.as has2.as
    hilbert1.as hilbert.as inline3.as opt0.as qimport.as
    reclist1.as swap1.as swap.as table1.as tree2.as
    type2.as type5.as type6.as type7.as where1.as
    where2.as where3.as bug1165.as t1009.as float4.as limits.as
    intfact.as t1025.as t1028.as exn4.as exn5.as
    exn6.as float2.as halt0.as halt1.as missing1.as
    object0.as bug1212.as default8.as pack3.as
)
RunFix=(
)
RunDiff=(

)
RunErr=(

)
RunDont=(

)


ErrOk=(
    include2.as abcheck1.as abcheck3.as apply0.as array0.as
    assign0.as assign1.as assign2.as assign3.as assign4.as
    coerce.as collect1.as condapply1.as const6.as const7.as
    const8.as ddata.as defarg3.as defarg5.as defarg7.as
    default6.as default7.as defgroup1.as defgroup2.as depend1.as
    error0.as error1.as exit0.as exit1.as exit2.as
    export1.as export2.as export3.as funct1.as funct2.as
    funct3.as funct4.as funct5.as funct6.as funct7.as
    funct8.as hide0.as if0.as if1.as junk0.as
    libdup0.as libdup1.as linear5.as pretend1.as macex2.as
    macex6.as macex7.as missing0.as none0.as ovload0.as
    ovload1.as parse1.as parse3.as parse4.as print.as
    qimport.as qual1.as record2.as record.as scan5.as
    scan6.as scobind0.as scobind1.as scobind2.as scobind3.as
    scobind5.as scope12.as select2.as tinfer0.as tinfer1.as
    tuple0.as tuple1.as type3.as unbal.as union2.as
    bug852.as bug941.as bug1144.as bug1154.as bug1189.as
    bug1192.as bug1210.as bug1238.as bug1242.as bug1246.as
    bug1265.as bug1278.as t672.as t1094.as t1097.as
    t1158.as t1166.as abcheck0.as t936.as t1125.as
)
ErrFix=(

)
ErrDiff=(

)
ErrDont=(

)



Helps=(
    arrepl1a.as arrepl1b.as arrepl1c.as
    arrepla.as arreplb.as arreplc.as
    expfact.c expquo.as exptoc2.as exptoc.as
    forn5_c.c
    intcat0.as intcat1.as
    packdefs.as packfns.as
    segext0.as segext1.as segext2.as segext3.as segext4.as supcat0.as supcat1.as
)

OtherDont=(
    db.as extend2.as forn8.as mylist.as numeral0.as numeral1.as numeral2.as
    rawcomplex.as rawrec5.as
    #
    t967.as t1070a.as t1070b.as t1075a.as
    t1075b.as t1075c.as t1107a.as t1107b.as
    t1157m.as t1157s.as
)


# This line must come last.
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

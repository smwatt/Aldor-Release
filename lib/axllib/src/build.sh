#!/bin/bash

doBuild() {
    #
    # Install include files.
    #
    cpCmd axllib.as "$ALDORROOT/include"


    #
    # Install library.
    #
    local aldorOpts="-Fo -Fao"
    aldorOpts="$aldorOpts -Q3 -Qinline-all -Wcheck -Qno-del-assert"
    aldorOpts="$aldorOpts -M no-ALDOR_W_OverRideLibraryFile"

    for b in \
        lang machine basic axlcat tuple gener ref except boolean segment \
        sinteger byte hinteger bpower integer pointer char list langx \
        uarray pkarray parray array string sfloat dfloat imod \
        \
        format complex ratio efuns partial sort oslow fname file opsys \
        textwrit textread rtexns fmtout table fprint pfloat float ieeectl \
        object fstring debug
    do
        local extraOpts
        case $b in
        lang)    extraOpts="-M no-ALDOR_W_WillObsolete" ;;
        machine) extraOpts="-laxllib" ;;
        basic)   extraOpts="-laxllib -Q inline-limit:18" ;;
        axlcat)  extraOpts="-laxllib" ;;
        except)  extraOpts="-Mno-ALDOR_W_GenDomFunNotConst" ;;
        fprint)  extraOpts="-Q inline-limit:2.4" ;;
        float)   extraOpts="-Q inline-limit:2.4" ;;
        pfloat)  extraOpts="-Q inline-limit:2.4" ;;

        # FIXME: Should not need Rep in "extend" definitions.
        string)  extraOpts="-Mno-ALDOR_E_GenImpNoRep" ;;
        dfloat)  extraOpts="-Mno-ALDOR_E_GenImpNoRep" ;;
        *)       extraOpts="" ;;
        esac

        compileAldorIntoLib $b libaxllib $aldorOpts $extraOpts
    done
    ( cd "$ALDORROOT/lib" ; ranlibCmd libaxllib.$ALDOR_LIBEXT )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

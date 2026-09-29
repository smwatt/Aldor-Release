#!/bin/bash

srcBases=( prime polycat poly random ibits lmdict spf
           dirprod gb nni vector quanc8 matrix matopdom )

opts=(-Fao -Fo
      -I"$INCPATH" -Y"$LIBPATH"   -laxllib
      -Q3 -Qinline-all  -W check  # -Wgc #-cargs=-g
      -M no-ALDOR_W_OverRideLibraryFile)

doBuild() {
    #
    # Install includes
    #
    cpCmd axldem.as "$INCPATH/axldem.as"

    #
    # Build library
    #
    local b
    for b in "${srcBases[@]}" ; do
        local extraOpts

        case $b in
        prime)    extraOpts="-Q inline-limit=2" ;;
        poly)     extraOpts="-laxldem" ;;
        dirprod)  extraOpts="-laxldem" ;;
        gb)       extraOpts="-laxldem -Q inline-limit=5" ;;
        *)        extraOpts="" ;;
        esac

        compileAldorIntoLib $b libaxldem "${opts[@]}" $extraOpts
    done

    ( cd "$LIBPATH" ; ranlibCmd libaxldem.$ALDOR_LIBEXT )

    #
    # Install samples
    #
    local SampleDir="$ALDORROOT/share/samples/libaxldem"
    mkdir -p "$SampleDir"

    for b in "${srcBases[@]}" ; do
        cpCmd $b.as "$SampleDir/$b.as"
    done
}


source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

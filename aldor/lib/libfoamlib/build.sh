#!/bin/bash
    
# This ets some the variables IncSrcs $AXL0Srcs $AXL1Srcs used below.
source OkFiles.sh >/dev/null

doBuild() {
	export PATH="$ALDORROOT/bin:$PATH"

    #
    # Set up directory variables
    #
    local SampleDir=$ALDORROOT/share/samples/libfoamlib
    local AldorSrc=`( cd ../../src ; pwd )`

    #
	# Ensure directories exist
    #
	mkdir -p "$ALDORROOT/include"
	mkdir -p "$ALDORROOT/lib"
	mkdir -p "$ALDORROOT/bin"
	mkdir -p "$SampleDir"

    #
	# Install include files
    #
    echo "Installing libfoamlib includes..."
	cpCmd foamlib.as             "$ALDORROOT/include"

    # FIXME: Should clean so no .h needed other than foam_c.h
	cpCmd "$AldorSrc/foam_c.h"   "$ALDORROOT/include"
	cpCmd "$AldorSrc/foam_c0.h"  "$ALDORROOT/include"
	cpCmd "$AldorSrc/optcfg.h"   "$ALDORROOT/include"
	cpCmd "$AldorSrc/foamopt.h"  "$ALDORROOT/include"
	cpCmd "$AldorSrc/cconfig.h"  "$ALDORROOT/include"
	cpCmd "$AldorSrc/platform.h" "$ALDORROOT/include"

    #
	# Do work to (re)build library
    #
    echo "Building libfoamlib..."
    ( cd "$LIBPATH" ; rmCmd libfoamlib.$ALDOR_LIBEXT libfoamlib.al )

    local f
    for f in $AXL0Srcs $AXL1Srcs; do
        local fb=`basename $f .as`
        local stdargs="-Fao -Fo -Zdb -Wcheck -Q3 -Qinline-all"

        local xtrargs="-lfoamlib"
        if [ $fb == lang  ] ; then xtrargs="-M no-ALDOR_W_WillObsolete"   ; fi
        if [ $fb == basic ] ; then xtrargs="-lfoamlib -Q inline-limit:18" ; fi

        compileAldorIntoLib $fb libfoamlib $stdargs $xtrargs

    done

    # Finish library
    ( cd "$LIBPATH" ; ranlibCmd libfoamlib.$ALDOR_LIBEXT )

	# Samples
	echo "Copying samples..."
    for f in $IncSrcs $AXL0Srcs $AXL1Srcs ; do
        cpCmd $f "$SampleDir/$f"
    done
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"


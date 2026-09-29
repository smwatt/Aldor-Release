#!/bin/bash

INTERPSYS="$AXIOM/bin/AXIOMsys"

SampleFiles="axlit.as axiom.as"
AxiomFiles="`cat ax.lst`"

#
# base clique -- needs a Axiom lisp system to generate
#
mkax() {
	if [ ! -d ap ] ; then mkdir ap ; fi
	${INTERPSYS} < mkax.lsp
#	if [ -d nap ] ; then cp nap/*.ap ap; fi
}


doBuild() {
    local SrcDir=`pwd`

    #
    # Install include files.
    #
	echo "Installing include files"
    cpCmd axiom.as "$INCPATH"
	cpCmd ax0.as   "$INCPATH"


    #
    # Build ax0 library.
    #
    # The library is built from some .as files and some .ap declarations.

    (
        cd "$LIBPATH"
        rmCmd libax0.al
        uniar x libaxllib.al lang.ao
        arDataCmd libax0.al lang.ao
        aldor -Flsp lang.ao
        rmCmd lang.ao
    )

    # Args that we always want, including -Fao.  CompileCmd is an array so
    # paths containing spaces remain single arguments.
    local -a args=(
        -Fao
        "-I$SrcDir" "-I$INCPATH" "-Y$LIBPATH"
        -M2 -Mno-macText
        -Mno-ALDOR_W_OverRideLibraryFile -Mno-ALDOR_W_WillObsolete
        -lAxiomLib=ax0
        -Q3 -Qinline-all -Wcheck
    )

    CompileCmd=(aldor "${args[@]}")
    SrcSuffix=as

    # Prime the pump with some declarations.
    compileIntoLib attrib   libax0  -Fasy -Flsp -Wname=axiom -Wnhash 
    compileIntoLib stub     libax0  
    compileIntoLib minimach libax0  -Fasy -Flsp

    # Make the Axiom library interface.
    (
        cd ap
        export SrcSuffix=ap

        local f
        for f in $AxiomFiles ; do
            compileIntoLib $f libax0 -Fao
        done
    )

    # Add some extensions.
    compileIntoLib axlit    libax0  -Fasy -Flsp
    compileIntoLib axextend libax0  -Fasy -Flsp -X

    # Put all the Lisp/Axiom stuff in one place.
    export LISPPATH="$LIBPATH/lisp"
    mkdir -p "$LISPPATH"

    cpCmd "$SrcDir/axext_l.lsp" "$LISPPATH"

    local f
    for f in lang attrib stub minimach axlit axextend
    do
        if [ -f "$f.lsp" ] ; then mvCmd $f.lsp "$LISPPATH" ; fi
        if [ -f "$f.asy" ] ; then mvCmd $f.asy "$LISPPATH" ; fi
    done
    
    #
    # Install samples.
    #
    mkdir -p "$ALDORROOT/share/samples/libax0"

    local f
    for  f in $SampleFiles ; do
        cpCmd $f "$ALDORROOT/share/samples/libax0"
    done
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

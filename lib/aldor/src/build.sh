#!/bin/bash

doBuildVariant() {
    # A variant rebuild must never inherit later members from an older
    # completed archive.  Such stale members can change name resolution while
    # early modules are being rebuilt.  Start both archives clean.
    createEmptyArchive "$ALDORROOT/lib/$1.al" || return $?
    createEmptyArchive "$ALDORROOT/lib/$1.$ALDOR_LIBEXT"  || return $?

    # The empty archives must exist so early compiles can resolve -l$1 while
    # the library is being assembled incrementally.

	(cd lang ;      bash build.sh build "$@" ) || return $?
	(cd base ;      bash build.sh build "$@" ) || return $?
	(cd arith ;     bash build.sh build "$@" ) || return $?
	(cd datastruc ; bash build.sh build "$@" ) || return $?
	(cd gmp ;       bash build.sh build "$@" ) || return $?
	(cd util ;      bash build.sh build "$@" ) || return $?
}


aOpts=(-Mno-mactext -Mno-ALDOR_W_CantUseArchive -M2
       -I "$ALDORROOT/include" -Y "$ALDORROOT/lib" -Fao -Fo)
cOts=(-I "$ALDORROOT/include" -c -O)

doBuild() { (
    SrcDir=`pwd`

    echo "************* Building Release Library *************"
    doBuildVariant libaldor  "${aOpts[@]}" -laldor  -Q5 -Qinline-all -Csmax=0

    echo "************* Building Debug Library *************"
    doBuildVariant libaldord "${aOpts[@]}" -laldord -Q1 -DDEBUG 

    echo "************* Building Object Libraries *************"
    # doBuildObjectLib libaldor "${aOpts[@]}"  -Fo  -Q5 -Qinline-all -Csmax=0
    compileCIntoLib  "$SrcDir/util/sal_util"  libaldor "${cOpts[@]}"  
    ( cd "$LIBPATH" ; ranlibCmd libaldor.$ALDOR_LIBEXT )

    # doBuildObjectLib libaldord "${aOpts[@]}" -Fo  -Q1 -DDEBUG
    compileCIntoLib  "$SrcDir/util/sal_util"  libaldord "${cOpts[@]}"  
    ( cd "$LIBPATH" ; ranlibCmd libaldord.$ALDOR_LIBEXT )
    
    echo "************* Building Interactive Loop *************"
    cd "$LIBPATH"
    aldor "${aOpts[@]}" -I"$SrcDir" -Fao -Fo -Q5 \
        "$SrcDir/aldor_gloop.as"
    aldor "${aOpts[@]}" -I"$SrcDir" -Fao -Fo -Q1 -DDEBUG \
        "$SrcDir/aldor_gloopd.as"
) }

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

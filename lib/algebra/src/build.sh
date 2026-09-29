#!/bin/bash

doBuildVariant() {
    # A variant rebuild must never inherit later members from an older
    # completed archive.  Such stale members can change name resolution while
    # early modules are being rebuilt.  Start both archives clean.
    createEmptyArchive "$ALDORROOT/lib/$1.al" || return $?
    createEmptyArchive "$ALDORROOT/lib/$1.$ALDOR_LIBEXT"  || return $?

    # The empty archives must exist so early compiles can resolve the library
    # while it is being assembled incrementally.

    # Basic stuff
    (cd util ;                  bash build.sh      build "$@" ) || return $?
    (cd extree ;                bash build.sh      build "$@" ) || return $?
    (cd numbers ;               bash build.sh      build "$@" ) || return $?
    (cd categories ;            bash build.sh      build "$@" ) || return $?
    (cd basic ;                 bash build.sh      build "$@" ) || return $?
    (cd multpoly/multpolydata ; bash build.sh      build "$@" ) || return $?

    # Matrices and univariate polynomials have interleaved stages.
    (cd mat ;                   bash buildPart1.sh build "$@" )
    (cd univpolycat ;           bash build.sh      build "$@" ) || return $?
    (cd univpoly ;              bash buildPart1.sh build "$@" )
    (cd mat ;                   bash buildPart2.sh build "$@" )
    (cd univpoly ;              bash buildPart2.sh build "$@" )
    (cd fraction ;              bash build.sh      build "$@" ) || return $?
    (cd mat ;                   bash buildPart3.sh build "$@" )
    (cd univpoly ;              bash buildPart3.sh build "$@" )

    # Constructions using univariate polynomials.
    (cd series ;                bash build.sh      build "$@" ) || return $?
    (cd algext ;                bash build.sh      build "$@" ) || return $?
    (cd polyfactorp ;           bash build.sh      build "$@" ) || return $?
    (cd ffield ;                bash build.sh      build "$@" ) || return $?
    (cd polyfactor0 ;           bash build.sh      build "$@" ) || return $?

    # Multivariate polynomials
    (cd multpoly ;              bash build.sh      build "$@" ) || return $?
}

opts=(-Fao -Fo -Mno-mactext -Mno-ALDOR_W_CantUseArchive -M2 
      -I "$ALDORROOT/include" -Y "$ALDORROOT/lib" )

doBuild() {
    echo "************* Building Release Library *************"
    doBuildVariant libalgebra     "${opts[@]}" -Q5 -Qinline-all \
        -laldor -lalgebra

    echo "************* Building GMP-Based Library *************"
    doBuildVariant libalgebra-gmp "${opts[@]}" -Q5 -Qinline-all -dGMP \
        -laldor -lalgebra-gmp 

    echo "************* Building Debug Library *************"
    doBuildVariant libalgebrad    "${opts[@]}" -Q1 -dDEBUG \
        -laldord -lalgebrad

    #echo "************* Building Object Libraries *************"
    #doBuildObjectLib libalgebra      -Q5 -Qinline-all
    #doBuildObjectLib libalgebra-gmp  -Q5 -Qinline-all -dGMP
    #doBuildObjectLib libalgebrad     -Q1 -dDEBUG
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

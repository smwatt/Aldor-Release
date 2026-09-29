#!/bin/bash

doBuild() {
    if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi
    
    # Tools to that allow tool building to work.  Must be first & order matters.

    install  toolbin ensureExecutable # Code sign if necessary
    install  toolbin testcmp

    # Tools delivered to end-user [bin directory]

    installc bin msgcat         # Construct X/Open message catalog+.c/.h files
    install  bin mklib.sh
    install  bin aldorbug           # Mail an Aldor bug report
    installc bin extract flags.c    # Extracts tests from source files
    installc bin aldoc2html flags.c # Take aldoc comments and create html file
    
    # Other tools [toolbin directory]
    installc toolbin echon
    installc toolbin txt2txt    # Convert between various os file formats.
    install  toolbin axiomxl
    install  toolbin dosify
    install  toolbin macify
    install  toolbin unixify
    
    installc toolbin skimenum cenum.c # Extract enum values from C source file.
    installc toolbin exclude    # Undo cpp includes.
    installc toolbin strarray   # Construct .c/.h files for array of file lines.
    installc toolbin dirname    # Directory of file name, for portable scripts.
    installc toolbin atinlay    # Translate a file to pig latin.
    installc toolbin memlay -I../../src # Explore machine's memory layout
    installc toolbin dosfile    # Convert between dos and unix file formats.
    install  toolbin undent     # Convert to std indentation -- user touches up.
    install  toolbin doaldor    # Helper script for compiling aldor files.
    install  toolbin dolatex    # Platform customization for "latex".
    install  toolbin doar       # Platform customization for native "ar".
    install  toolbin doranlib   # Platform customization for "ranlib".
    install  toolbin buildarg   # buildarg
    install  toolbin docc       # Platform customization for "cc".
    install  toolbin dog++      # Platform customization for "g++".
    install  toolbin lnorcp     # Symbolic link if can, copy if can't.
    install  toolbin mksrctex   # Latex axiomxl compiler source with index.
    install  toolbin cprt.awk   # called by mksrctex
    installc toolbin cwords cscan.c # Extract identifiers from a C source file.
    install  toolbin getbug     # Request a reported bug
    install  toolbin fixbug     # Save a bug fix
    install  toolbin fixreply   # Report bug fix to bug reporter
    install  toolbin mkasys     # dump .asy files from all .ao's in libaxllib.al
    
    # zacc: Parser generator with parameterized rules. Based on Yacc.
    (
        Src=`pwd`
        if [ -n "$ALDORTMP" ] ; then Tmp="$ALDORTMP" ; else Tmp=/tmp ; fi
        ZaccTmp="$Tmp/ZaccTmp.$$"
        mkdir -p "$ZaccTmp"
        cd "$ZaccTmp"

        if command -v flex >/dev/null 2>&1 ; then
            lexCmd "$Src/zaccscan.l"
        else
            cpCmd "$Src/preserve-lex.yy.c" lex.yy.c
        fi
        if [ ! -f lex.yy.c ] ; then
            cpCmd "$Src/preserve-lex.yy.c" lex.yy.c
        fi

        if command -v yacc >/dev/null 2>&1 ; then
            yaccCmd -d "$Src/zaccgram.y"
        fi
        if [ ! -f y.tab.h ] || [ ! -f y.tab.c ] ; then
            cpCmd "$Src/preserve-y.tab.h" zaccgram.h
            cpCmd "$Src/preserve-y.tab.c" y.tab.c
        else
            mvCmd y.tab.h zaccgram.h
        fi
        installc toolbin "$Src/zacc" -I "$ZaccTmp" -I "$Src" \
            lex.yy.c y.tab.c "$Src/cenum.c"
        rmCmd lex.yy.c y.tab.c zaccgram.h
        cd "$Src"
        rm -rf "$ZaccTmp"
        unset Src Tmp ZaccTmp
    )
    

}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

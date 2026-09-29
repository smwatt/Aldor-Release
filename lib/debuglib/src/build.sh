#!/bin/bash

#
# Debugger library.
#

# Compile with one of 
#   -DUseAxllib
#   -DUseBasicmath 
#   -DUseAldorlib
# Never compile this debugging library with -Wdebugger!

opts=(
    "-I$INCPATH" "-Y$LIBPATH" -D UseAldorlib
    -Qno-del-assert -Qno-cc
    -Csmax=0 -Cargs=-g
    -Mno-mactext
    -Mno-ALDOR_W_CantUseArchive -Mno-ALDOR_W_OverRideLibraryFile
    -Mno-ALDOR_W_FunnyJuxta -Mno-ALDOR_W_GenDomFunNotConst
)


# Level N sources depend on sources in level N-1 and below.
# Level 0: dbg_empty    -- Seed for library.
# Level 1: -- If needed, defns for uniform view of library (Axllib Basicmath...)
# Level 2: all the rest -- Utility domains providing the base of the library.
# Level 3: dbg_istack dbg_context dbg_file_tbl -- Calling contexts.
# Level 4: dbg_state    -- The internal debugger state uses levels 1 and 2.
# Level 5: dbg_ui_break -- Breakpoint handling relies on the debugger state.
# Level 6: dbg_ui       -- Debugger user interface (UI) uses all levels.
# Level 7: dbg_core     -- Top-level domain that is exported to clients.

SrcBases=(
    dbg_empty dbg_ltools dbg_exts dbg_gener dbg_var dbg_fmt dbg_file_idx
    dbg_bpoint dbg_help dbg_utils dbg_istack dbg_context dbg_file_tbl
    dbg_state dbg_ui_break dbg_ui dbg_core
)

doBuild() {
    #
    # Install include files.
    #
    cpCmd debuglib.as "$INCPATH/debuglib.as"

    #
    # Build library.
    #
    local b
    for b in "${SrcBases[@]}" ; do
        compileAldorIntoLib "$b" libdebuglib -Fao -Fo "${opts[@]}"
    done

    ( cd "$LIBPATH" ; ranlibCmd "libdebuglib.$ALDOR_LIBEXT" )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

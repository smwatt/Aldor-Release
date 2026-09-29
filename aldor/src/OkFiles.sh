#
# This file is to be included in a shell script.
#

# Miscellaneous
MiscFiles=""

# Level 4: Compiler Top Level
MainCpp="main.cpp main_t.cpp winmain.cpp"

TopH="axl.h axltop.h cxxexcept.h"
TopCpp="axlcomp.cpp cmdline.cpp"

# Level 3: Compiler Phases and Subordinates
PhaseY="axl.y"
PhaseH="axlphase.h gf_util.h"
PhaseCpp="
        phase.cpp bloop.cpp
        include.cpp scan.cpp syscmd.cpp linear.cpp parseby.cpp macex.cpp
        abnorm.cpp abcheck.cpp abuse.cpp scobind.cpp
        ti_decl.cpp
        tinfer.cpp ti_bup.cpp ti_tdn.cpp ti_sef.cpp terror.cpp
        genfoam.cpp gf_fortran.cpp gf_add.cpp gf_gener.cpp gf_imps.cpp
        gf_excpt.cpp gf_reference.cpp gf_implicit.cpp
        gf_prog.cpp gf_rtime.cpp gf_seq.cpp opttools.cpp
        optfoam.cpp of_util.cpp  of_inlin.cpp of_cfold.cpp of_hfold.cpp
        of_emerg.cpp of_env.cpp of_cprop.cpp of_jflow.cpp of_peep.cpp
        of_deadv.cpp of_comex.cpp of_loops.cpp of_retyp.cpp of_deada.cpp
        of_rrfmt.cpp of_killp.cpp
        emit.cpp ccomp.cpp usedef.cpp flatten.cpp
        inlutil.cpp genc.cpp genlisp.cpp fortran.cpp gencpp.cpp
"
# Level 2: Compiler Structures
ObjectsCpp="
        axlobs.cpp srcline.cpp token.cpp doc.cpp absyn.cpp absub.cpp ablogic.cpp
        fint.cpp simpl.cpp compcfg.cpp depdag.cpp
        sefo.cpp syme.cpp freevar.cpp tform.cpp tfsat.cpp tposs.cpp tconst.cpp
        tqual.cpp stab.cpp lib.cpp archive.cpp alar.cpp foam.cpp flog.cpp dflow.cpp spesym.cpp
        abpretty.cpp version.cpp comsg.cpp loops.cpp sexpr.cpp
"

# Level 1: General Library -- NB: Should split foam files
# Compiler-only implementations use truthful .cpp suffixes.
GeneralCpp="
        test.cpp bitv.cpp dnf.cpp msg.cpp path.cpp srcpos.cpp symbol.cpp
        ccode.cpp priq.cpp termtype.cpp textansi.cpp texthp.cpp textcolour.cpp
        file.cpp fname.cpp strops.cpp debug.cpp buffer.cpp list.cpp cfgfile.cpp
        format.cpp util.cpp foamopt.cpp compopt.cpp dword.cpp xfloat.cpp
        timer.cpp output.cpp table.cpp btree.cpp bigint.cpp
        sto_debug.cpp memclim.cpp store.cpp
        foam_c.cpp foam_i.cpp foam_cfp.cpp
"
GeneralC=""
GeneralH="axlgen.h axlgen0.h editlevels.h optcfg.h foam_c0.h"
GeneralT="
        store1_t.c store2_t.c store3_t.c format_t.c util_t.c
        bigint_t.c bitv_t.c btree_t.c
        strops_t.c table_t.c
        ccode_t.c xfloat_t.c link_t.c float_t.c
"

# Level 0: Portability
PortH="axlport.h platform.h cconfig.h fenvport.h"
StdcH="
        "
PortCpp="cport.cpp opsys.cpp"
PortC=""
OsInc="os_macosx_vm.c0 os_unix.c0 os_windows.c0
        sto_btree.c0 sto_layer.c0 sto_once.c0 sto_malloc.c0 sto_boehm.c0"
PortT="cport_t.c opsys_t.c"

PortA="" # any_as.s for Sun only

#
# Generated files -- keep for platforms with weak tool chains
#
MadeY="axl_y.yt axl_y.sed"
MadeC="comsgdb.c axl_y.c"
MadeH="comsgdb.h"
MadeFiles="$MadeY $MadeC $MadeH"

# Configuration files
ConfFiles="aldor.conf sample.terminfo"


#
# File collections
#

LayoutCpp="layout.cpp"
CompCpp="$TopCpp $PhaseCpp $ObjectsCpp $GeneralCpp $PortCpp $LayoutCpp"
ObjectsSharedC=""
CompSharedC="$ObjectsSharedC $GeneralC $PortC"
CompSources="$CompCpp $CompSharedC"
CompH="`echo $TopCpp $PhaseCpp $ObjectsCpp $GeneralCpp | sed -e 's/\.cpp/.h/g'` `echo $CompSharedC | sed -e 's/\.c/.h/g'`"

MoreSources="$MainCpp $MadeC $OsInc"
MoreH="$MadeH $TopH $PhaseH $GeneralH $PortH $StdcH cport.h fluid.h opsys.h storeconcept.hpp"
TestC="$GeneralT $PortT" # C-compatible self-test files
TestCpp="fluid_t.cpp priq_t.cpp buffer_t.cpp file_t.cpp fname_t.cpp msg_t.cpp symbol_t.cpp list_t.cpp dnf_t.cpp"         # C++ self-test files
Experimental="of_argsub.h of_argsub.c genssa.h genssa.c newjflow.c"

OKFILES="
        build.sh OkFiles.sh recomp.sh
        basic.typ  comsgdb.msg
        any_as.s $PhaseY
        $ConfFiles $MadeY
        $CompSources $CompH $MoreSources $MoreH $TestC $TestCpp $Experimental
        $MiscFiles
"

echo $OKFILES

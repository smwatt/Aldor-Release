/*****************************************************************************
 *
 * cmdline.h: OS Command line processing.
 *
 * This file is part of Aldor.
 *
 * Aldor is licensed under the Apache License, Version 2.0.
 *
 *
 * See legal/LICENSE in the Aldor distribution for details.
 *
 * Copyright (C) 1990-2026 Stephen M. Watt.
 */

#ifndef _CMDLINE_H_
#define _CMDLINE_H_

# include "axlgen.h"

extern int      cmdArguments(int argi0, int argc, MutString *argv);
                        /*
                         * Process the command line options and return index
                         * of first file name argument.  E.g., after
                         *
                         *   iargc = cmdArguments(1, argc, argv);
                         *
                         * the names of the files to compile are:
                         *
                         *   argv[iargc], ..., argv[iargc + cmdFileCount - 1].
                         *
                         * If -r is given, then the resulting program is
                         * passed the arguments:
                         *
                         *   argv[iargc + cmdFileCount], ..., argv[argc-1].
                         */

extern void     cmdHandleOption(int opt, MutString arg);
                        /*
                         * Process the command line option and its argument.
                         */

extern String cmdName;
                        /*
                         * Name used to invoke the command.
                         */

extern String cmdInitFile;
                        /*
                         * Name of the user initialization file for
                         * interactive mode.
                         */


/*****************************************************************************
 *
 * :: Functions to manipulate argument vectors.
 *
 ****************************************************************************/

extern void     cmdEcho(FILE *, int argc, MutString *argv);
                        /*
                         * Display a command line with appropriate quoting.
                         */

extern void     cmdParseOptions(MutString s, int *pargc, MutString **pargv);
                        /*
                         * Parse the options given in string s into a newly
                         * allocated null-terminated argument vector.
                         * The vector and length are returned via parameters.
                         */
extern void     cmdFreeOptions(int argc, MutString *argv);
                        /*
                         * Free the options returned by cmdParseOptions.
                         */

extern bool     cmdSubsumeResponseFiles(int argi0, int *pargc, MutString **pargv);
                        /*
                         * Form full command line following response file args.
                         * Returns true if any response files were encountered.
                         */

extern bool     cmdHasOption(int o,String a,int argc,MutString *argv);
extern String cmdOptionArg;
                        /*
                         * Test whether the cmd line has the given option.
                         * If 'a' is non-null, the argument must match it.
                         * The actual argument is saved as 'cmdOptionArg'.
                         */
bool cmdHasOptionPrefix(int opt0, String arg0, int argc, MutString *argv);
                        /*
                         * Test whether the cmd line has the given option
                         * prefix.  If 'a' is non-null, the argument must
                         * match it.  The actual argument is saved as
                         * 'cmdOptionArg'.
                         */


/*****************************************************************************
 *
 * :: Macros to probe an agrument vector without calling cmdArguments
 *
 ****************************************************************************/

#define cmdHasVerboseOption(ac,av)     cmdHasOption('V', NULL,   ac,av)
#define cmdHasHelpOption(ac,av)        cmdHasOption('H', NULL,   ac,av)
#define cmdHasDebugOption(ac,av)       cmdHasOption('Z',"db",    ac,av)
#define cmdHasSExprOption(ac,av)       cmdHasOption('W',"sexpr", ac,av)
#define cmdHasSEvalOption(ac,av)       cmdHasOption('W',"seval", ac,av)
#define cmdHasGcOption(ac,av)          cmdHasOption('W',"gc",    ac,av)
#define cmdHasNoGcOption(ac,av)        cmdHasOption('W',"no-gc", ac,av)
#define cmdHasGcFileOption(ac,av)      cmdHasOption('W',"gcfile",ac,av)
#define cmdHasRootOption(ac,av)        cmdHasOption('B', NULL,   ac,av)
#define cmdHasOptimizeOption(ac,av)    cmdHasOption('Q', NULL,   ac,av)
#define cmdHasInteractiveOption(ac,av) cmdHasOption('G',"loop",  ac,av)
#define cmdHasCfgFileOption(ac,av)     cmdHasOptionPrefix('N',"file=",  ac,av)
#define cmdHasCfgNameOption(ac,av)     cmdHasOptionPrefix('N',"sys=",  ac,av)


/*****************************************************************************
 *
 * :: Variables set after cmdArguments has been called.
 *
 ****************************************************************************/

extern bool     cmdVerboseFlag;     /* Had the verbose option    (-V)?      */
extern bool     cmdTrapFlag;        /* Had the trap option       (-Wtrap)?  */
extern bool     cmdSExprFlag;       /* Had the sexpr option      (-Wsexpr)? */
extern bool     cmdSEvalFlag;       /* Had the lisp option       (-Wseval)? */
extern bool     cmdGcFlag;          /* Had the gc option         (-Wgc)?    */
extern bool     cmdGcFileFlag;      /* Had the gcfile option     (-Wgcfile)?*/
extern bool     cmdFloatRepFlag;    /* Decrease double precision -Wfloatrep */

extern int      cmdFileCount;       /* Number of files to compile.          */

#endif  /* !_CMDLINE_H_ */

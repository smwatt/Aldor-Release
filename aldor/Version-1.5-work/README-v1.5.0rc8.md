# Aldor v1.5.0rc8

RC8 tidies the portability and diagnostic issues exposed by the RC7 macOS and
Ubuntu runs without changing the Foreign-C/runtime interface decisions already
locked in RC3--RC7.

## Corrections from RC7

1. **C23 generated `main` prototype.**  `aldormain.c` no longer declares
   `main()` and later defines `main(int, char **)`.  The generator emits the
   complete two-argument declaration, avoiding GCC 15's C23 interpretation of
   `()` as a zero-parameter prototype.

2. **Darwin system libraries.**  macOS no longer inherits an explicit `-lm`
   from the generic Unix configuration, and the direct generated-C build path
   likewise omits it.  Current Apple libSystem already supplies the math
   interfaces, so this removes the linker warning that made every otherwise
   successful AxlLib execution test compare different.

3. **Host C utility prototypes.**  The small bootstrap/test utilities which
   still used K&R definitions or no-prototype declarations have been converted
   to C99 prototypes.  This addresses the large Clang C23-warning burst seen on
   Ubuntu rather than suppressing it globally.  The cleanup includes `msgcat`,
   `cenum`, `cscan`, `exclude`, `strarray`, `dirname`, `echon`, `skimenum`,
   `memlay`, `dosfile`, `cwords`, `zacc`, `unnum`, and testaldor support code.

4. **Generated-C-only Clang controls.**  The two diagnostics that still arise
   from intentional generated-C forms are kept local to generated C instead of
   hiding host-source diagnostics.  Darwin-specific controls are no longer
   duplicated with these common Clang controls.

5. **Diagnostic handling.**  Build-log error detection now looks for compiler,
   linker and make diagnostic forms rather than the English word `error`.
   Warnings mentioning functions such as `error()` or `scanError()` therefore
   no longer abort a build.  A failed generated-runtime smoke now includes its
   stdout/stderr evidence inline in the main log.

## Final local validation

Two final builds were performed from empty build roots after the RC8 version
metadata and all source changes were fixed.  The package source is the source
used for these runs.

- **GCC 14.2.0 / Linux x86_64:** 870/870 primary tests, 0 DIFFERENT,
  0 ERROR, 0 warnings, 0 compiler/toolchain errors; all six generated-runtime
  smokes and all modernization release gates pass.
- **Clang 17.0.0 / Linux x86_64:** 870/870 primary tests, 0 DIFFERENT,
  0 ERROR, 0 warnings, 0 compiler/toolchain errors; all six generated-runtime
  smokes and all modernization release gates pass.

The final validation also exercises the strict C/prototype checks which caught
and removed the last old-style zero-argument module-initializer forms.

External validation remains for grice/ice, vbuntu26 with its newer GCC/Clang,
Cygwin and RISC-V.  Those are release-candidate platform gates rather than
known local failures.

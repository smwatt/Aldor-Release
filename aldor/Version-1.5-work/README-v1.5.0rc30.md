# Aldor v1.5.0rc30

rc30 is the varargs-ABI and build-evidence candidate following the Grice
Apple-Silicon GNU-GCC startup failure in rc29.

Principal changes:

- fix `StoCtl_GcFile` integer/pointer varargs mismatch;
- remove the untyped variadic SImpl constructor;
- make CCode token construction typed and CCode node construction compile-time
  homogeneous, correcting the raw-null/raw-word call sites this exposed;
- replace the Store tracer's obsolete variadic closure function view with a
  fixed-prototype call;
- make active `strlConcat` arguments type-exact;
- add the missing `va_end` in `gen0LazyBuiltinCCall`;
- make `distro/rebuild.sh` always preserve a per-toolchain rebuild log for
  build/all, including failed builds, both under the package/workspace `logs/`
  directory and as `build-.../evidence/rebuild.log`.

See `notes/aldor-v1.5.0rc30-varargs-audit.md`.

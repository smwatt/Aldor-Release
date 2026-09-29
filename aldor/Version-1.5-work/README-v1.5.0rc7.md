# Aldor v1.5.0rc7

RC7 fixes the remaining macOS native `-grun` failure diagnosed by RC6.

## Root cause and correction

On Darwin the build supplies Clang warning controls such as
`-Wno-int-conversion` and `-Wno-unused-value`.  `unicl` historically used
prefix matching for its private `-W` options, so every `-Wno-*` argument
matched the private `-Wn` (no execute) option.  `unicl` therefore returned
success without invoking Clang and no executable was produced.

RC7:

1. exact-matches private non-parameterised `unicl` options (`-Wn`, `-Wv`,
   `-Wstdc`, `-Wshared`, etc.);
2. retains prefix matching only for explicitly parameterised forms such as
   `-Wsys=`, `-Wopts=`, and `-Wv=`;
3. passes all other `-W...` options unchanged to the native compiler/linker;
4. adds a real `unicl` link smoke using `-Wno-unused-value`, so this failure
   cannot regress silently;
5. updates the deterministic generated-code references for the intentional RC4
   proper-prototype/CSig changes now confirmed by the grice RC6 run.

RC6's output-existence assertions remain in place, as do the RC4 generated-C
prototype changes and RC3 Foreign-C semantics.

Run a clean release validation with:

    ./build.sh all

The first external validation target should again be grice; the expected
change is that the `run ok` group now actually links and executes its programs.

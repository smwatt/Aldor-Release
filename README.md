# Aldor source distribution

This repository contains the public standalone source distribution for Aldor.

It is self-contained with respect to Aldor project material: a copy of this
repository is sufficient to configure, build, and test Aldor, subject only to
documented host prerequisites such as a C/C++ compiler, Bison, Bash, standard
build utilities, and GMP development headers/libraries.

Release source archives and prebuilt platform packages are available from:

https://github.com/smwatt/Aldor-Release/releases

## Prebuilt Aldor packages

Prebuilt Aldor 1.5.0 packages are available for the following
platform/compiler combinations:

| Platform | Architecture | Toolchain |
| --- | --- | --- |
| Cygwin | x86_64 | GCC |
| Cygwin | x86_64 | Clang |
| Debian Linux | riscv64 | GCC |
| Debian Linux | riscv64 | Clang |
| macOS | arm64 | GCC |
| macOS | arm64 | Apple Clang |
| macOS | x86_64 | GCC |
| macOS | x86_64 | Apple Clang |
| Ubuntu Linux | aarch64 | GCC |
| Ubuntu Linux | aarch64 | Clang |
| Ubuntu Linux | x86_64 | GCC |
| Ubuntu Linux | x86_64 | Clang |

For these configurations all tests pass.

Package names have the form:

```text
aldor-1.5.0-<platform>-<architecture>-<toolchain>.tgz
```

For example:

```text
aldor-1.5.0-ubuntu-x86_64-gcc.tgz
aldor-1.5.0-macos-arm64-clang.tgz
```

### Using a prebuilt package

Unpack the package in a convenient location:

```sh
tar xzf aldor-1.5.0-ubuntu-x86_64-gcc.tgz
```

Aldor uses the environment variable `ALDORROOT` to identify the root of the
installed Aldor tree. Set `ALDORROOT` to the directory created by unpacking
the package -- the directory containing the Aldor `bin` and library
directories -- and put its `bin` directory on your search path.

For example:

```sh
export ALDORROOT=/path/to/aldor
export PATH="$ALDORROOT/bin:$PATH"
```

`ALDORROOT` tells the compiler where to find the libraries and other files
belonging to that Aldor installation.

You can then invoke the compiler with:

```sh
aldor
```

or compile an Aldor source file with, for example:

```sh
aldor hello.as
```

The `ALDORROOT` and `PATH` settings may be placed in your shell startup file
if Aldor is to be used regularly.

## Building from source

`ALDORROOT` is the destination directory where the compiled Aldor
installation will be placed. After the build it will contain subdirectories
such as `bin/`, `lib/`, and other installation files.

For a normal build of one selected/default toolchain:

```sh
export ALDORROOT=/path/to/aldor
bash build.sh
```

A particular compiler family can be selected, for example:

```sh
ALDORROOT=/path/to/aldor ALDOR_TOOLCHAIN=gcc bash build.sh
ALDORROOT=/path/to/aldor ALDOR_TOOLCHAIN=clang bash build.sh
```

For a complete clean build and test for each supported native compiler family
found on the host:

```sh
bash rebuild.sh
```

`rebuild.sh` selects build roots automatically; `ALDORROOT` does not need to
be set when using it.

Generated build roots used by `rebuild.sh` are placed outside the source tree.

## Documentation

Guides and manuals:

- [Aldor User Guide](doc/aldor-ug-2003/aldorug.pdf)
- [libaldor Reference Manual](lib/aldor/doc/libaldor.pdf)
- [Algebra Reference Manual](lib/algebra/doc/algebra.pdf)

Additional historical and technical documentation is under [`doc/`](doc/).

## Releases

The `v1.5.0` tag identifies the Aldor 1.5.0 public source snapshot.

The corresponding GitHub Release provides the standalone source archive and
validated binary packages for supported platform/compiler combinations.

## Licensing and provenance

See [`legal/`](legal/) for copyright and licensing information.

# Aldor v0.18 CP12

CP12 is the Build 19 cleanup checkpoint after CP11.

It closes the remaining justified AxlLib reference differences, repairs the
cross-platform `emit0` harness, and fixes the false `where2` constant-domain
warning.  The full AxlLib suite in the packaging environment is now:

```
760 OK + 0 DIFFERENT + 1 ERROR = 761
```

The one ERROR is `recov1.sh`.

Read first:

- `RESUME-CP12.md`
- `checkpoint/CP12-CONSOLIDATION.md`
- `checkpoint/CP12-WHERE2-FIX.md`
- `checkpoint/CP12-REFERENCE-AND-HARNESS-CLEANUP.md`
- `reports/CP12-VALIDATION-STATUS.md`

Fresh acceptance build:

```sh
bash rebuild.sh
```

Focused gate after a build:

```sh
distro/modernization/cp12-regression.sh \
    "$PWD/build-root" "$PWD/distro"
```

The build log name must be:

```text
aldor-v0.18-cp12-<host>-YYYY-MM-DD--HH-MM.log
```

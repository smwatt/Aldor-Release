# Aldor v0.18 CP13

CP13 closes the last known AxlLib correctness failure (`recov1`) with an
explicit semantic-recovery lifetime model and corrected scope-tree rollback.

Local Trixie acceptance from a fresh empty root is fully clean:

- 29/29 compiler self-tests;
- 761/761 AxlLib;
- 8/8 libaldor;
- 72/72 Algebra embedded tests;
- 48/48 Algebra validation matrix;
- all modernization gates PASS;
- 0 build warnings, 0 build errors;
- 870/870 primary tests OK.

See `RESUME-CP13.md` and `checkpoint/CP13-SEMANTIC-RECOVERY.md`.

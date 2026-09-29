# Aldor v0.18 CP11

CP11 is a cumulative correctness checkpoint based on CP10.

New compiler repairs:

- `t1145`: correct large positive `SInt` transport through flat FOAM buffers;
- `opt1`: preserve stable alias identity during `emerge` scalar replacement.

Outstanding semantic investigations remain deliberately visible:

- `recov1`: interactive Syme lifetime/rollback;
- `where2`: general `D where S` scoping/normalization semantics.

Start with:

- `checkpoint/CHECKPOINT-STATE.txt`
- `checkpoint/CP11-CONSOLIDATION.md`
- `checkpoint/CP11-OUTSTANDING-INVESTIGATIONS.md`
- `checkpoint/RESUME-CP11.md`
- `reports/POINT-VERSION-HISTORY-v0.01-v0.18.md`
- `reports/CP11-VALIDATION-STATUS.md`

For a fresh acceptance run:

```sh
bash rebuild.sh
```

The build log naming convention is:

```text
aldor-v0.18-cp11-<host>-YYYY-MM-DD--HH-MM.log
```

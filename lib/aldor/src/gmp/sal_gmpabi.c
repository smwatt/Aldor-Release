/* C99 ABI shims for GMP functions whose C result type is narrower than
 * Aldor's MachineInteger on LP64 hosts. */

#include "foam_c.h"
#include <gmp.h>

FiSInt
aldorGmpzCmp(const void *a, const void *b)
{
    return (FiSInt) mpz_cmp((mpz_srcptr) a, (mpz_srcptr) b);
}

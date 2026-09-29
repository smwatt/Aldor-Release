#include "foam_c.h"

extern void quotient(FiWord, FiWord, FiWord *, FiWord *);
extern void print(FiWord);

void bar ()
{
	FiWord	quo;
	FiWord	rem;

	quotient(37, 5, &quo, &rem);

	print(quo);
	print(rem);
}


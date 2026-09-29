#include "foam_c.h"

extern FiWord fact(FiWord);
extern void print(FiWord);

void bar ()
{
	print(fact(5));
}


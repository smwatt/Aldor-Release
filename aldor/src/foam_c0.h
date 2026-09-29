/*
 * foam_c0.h: Subset of cport.h and store.h needed for foam_c.h
 *
 * This is the only file included in foam_c.h.
 *
 * It duplicates a small subset of the function of cport.h and store.h.
 * Care must be taken to ensure consistency with those files.
 *
 * It is included in them in the hope to generate errors if out sync,
 * but this is not guaranteed to catch errors.
 */
#ifndef _FOAMC0_H_
#define _FOAMC0_H_

#ifndef __cplusplus
# include <stdbool.h>
#endif


#ifndef CPortBasicTypedefs
#define CPortBasicTypedefs
  typedef int    CBool;
  typedef double MostAlignedType;
#endif /* CPortBasicTypedefs */

extern CBool             stoIsPointer      (void *);
extern int              stoMarkObject     (void *);
extern MostAlignedType *stoRecode         (void *, unsigned);
extern void             stoSetAldorTracer (int, void *);
extern void             stoSetTracer      (int, void *);
extern int              stoShowArgs       (const char *);
extern void             stoShowDetail     (int);
extern int              stoWritablePointer(void *);


#endif /* _FOAMC0_H_ */

#ifndef INCLseq_maskh
#define INCLseq_maskh

#include "epicsTypes.h"

typedef epicsUInt32 seqMask;				/* for event masks and options */

#define NBITS			(8*sizeof(seqMask))	/* # bits in seqMask word */
#define NWORDS(maxBitNum)	(1+(maxBitNum)/NBITS)	/* # words in seqMask */

#define bitSet(words, bitnum)	( (words)[(bitnum)/NBITS] |=  (1u<<((bitnum)%NBITS)))
#define bitClear(words, bitnum)	( (words)[(bitnum)/NBITS] &= ~(1u<<((bitnum)%NBITS)))
#define bitTest(words, bitnum)	(((words)[(bitnum)/NBITS] &  (1u<<((bitnum)%NBITS))) != 0)

#define WORD_BIN_FMT "%u%u%u%u%u%u%u%u'%u%u%u%u%u%u%u%u'%u%u%u%u%u%u%u%u'%u%u%u%u%u%u%u%u"
#define WORD_BIN(word) \
  ((word) & (1u<<31) ? 1u : 0), \
  ((word) & (1u<<30) ? 1u : 0), \
  ((word) & (1u<<29) ? 1u : 0), \
  ((word) & (1u<<28) ? 1u : 0), \
  ((word) & (1u<<27) ? 1u : 0), \
  ((word) & (1u<<26) ? 1u : 0), \
  ((word) & (1u<<25) ? 1u : 0), \
  ((word) & (1u<<24) ? 1u : 0), \
  ((word) & (1u<<23) ? 1u : 0), \
  ((word) & (1u<<22) ? 1u : 0), \
  ((word) & (1u<<21) ? 1u : 0), \
  ((word) & (1u<<20) ? 1u : 0), \
  ((word) & (1u<<19) ? 1u : 0), \
  ((word) & (1u<<18) ? 1u : 0), \
  ((word) & (1u<<17) ? 1u : 0), \
  ((word) & (1u<<16) ? 1u : 0), \
  ((word) & (1u<<15) ? 1u : 0), \
  ((word) & (1u<<14) ? 1u : 0), \
  ((word) & (1u<<13) ? 1u : 0), \
  ((word) & (1u<<12) ? 1u : 0), \
  ((word) & (1u<<11) ? 1u : 0), \
  ((word) & (1u<<10) ? 1u : 0), \
  ((word) & (1u<< 9) ? 1u : 0), \
  ((word) & (1u<< 8) ? 1u : 0), \
  ((word) & (1u<< 7) ? 1u : 0), \
  ((word) & (1u<< 6) ? 1u : 0), \
  ((word) & (1u<< 5) ? 1u : 0), \
  ((word) & (1u<< 4) ? 1u : 0), \
  ((word) & (1u<< 3) ? 1u : 0), \
  ((word) & (1u<< 2) ? 1u : 0), \
  ((word) & (1u<< 1) ? 1u : 0), \
  ((word) & (1u<< 0) ? 1u : 0)

#endif	/*INCLseq_maskh*/

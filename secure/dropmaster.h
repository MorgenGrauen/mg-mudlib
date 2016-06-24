#ifndef _DROPMASTER_
#define _DROPMASTER_

// path to dropmaster
#define DROPMASTER "/secure/dropmaster"
#define DROPSAVE "/secure/ARCH/DROPMASTER"

// rarelevels
#define COMMON					      0
#define UNCOMMON				      1
#define RARE					        2
#define VERY_RARE				      3
#define ULTRA_RARE				    4
#define RATHER_NOT_PROBABLE		5
#define DFLT_RND_NAMES	({"gewoehnlich","ungewoehnlich",\
                          "selten","sehr selten","aussergewoehnlich selten",\
                          "praktisch nie"})

#if defined(_NEED_DROPMASTER_IMPLEMENTATION_) && !defined(_DROPMASTER_IMPLEMENTATION_)
#define _DROPMASTER_IMPLEMENTATION_
// random helper
#define GLOBAL_KEY	0
#define DFLT_RND_VALUES  ({100,1000,10000,100000,200000,1000000})
#define RND_REDUCTION	10

#endif // _DROPMASTER_IMPLEMENTATION_

#endif // _DROPMASTER_

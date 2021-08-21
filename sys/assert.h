// MorgenGrauen MUDlib
//
// assert.h -- assert macro
 
#ifndef __ASSERT_H__
#define __ASSERT_H__

#ifdef NDEBUG
#define assert(e,msg)
#else
#define assert(e,msg)  \
    if (!(e)) \
        raise_error(sprintf( \
        "%s:%d in function %s: failed assertion `%s'\n", \
        __FILE__, __LINE__, __FUNCTION__, \
        stringp(msg) ? msg : "no details given"))
#endif

#endif // __ASSERT_H__

#ifdef NEED_PROTOTYPES

#ifndef __ASSERT_H_PROTO__
#define __ASSERT_H_PROTO__

// prototypes

#endif // __ASSERT_H_PROTO__

#endif	// NEED_PROTOYPES

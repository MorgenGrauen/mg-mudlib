// MorgenGrauen MUDlib
//
// sys_debug.h -- Definitionen fuer Debugzwecke
//
// $Id: sys_debug.h 6081 2006-10-23 14:12:34Z Zesstra $

#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__

#define DEBUG(x) printf("DEBUG: OBJ(/%O) %O\n", this_object(), (x))

#define DBG_ON(c) (traceprefix(object_name(this_object())[1..]), trace(c?c:239))
#define DBG_OFF() (traceprefix(""), trace(0))

#define LFUN    1   // debug lfunc calls
#define CALL    2   // debug call_other calls
#define RET     4   // debug return values
#define ARGS    8   // show arguments when calling a function
#define STACK   16  // show all stack machine code (DON'T USE!)
#define HB      32  // show calls to heart_beat()
#define APPLY   64  // debug calls to apply
#define NAME    128 // show name of file when debugging (don't use)

#endif

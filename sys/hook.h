// MorgenGrauen MUDlib
//
// /sys/hooks.h  - Hooksystem
//
// $Id: hook.h 9475 2016-02-19 21:16:17Z Zesstra $

#ifndef _HOOKS_H_
#define _HOOKS_H_

// global hooks
#define H_HOOK_MOVE           1
#define H_HOOK_DIE            2
#define H_HOOK_DEFEND         3
#define H_HOOK_ATTACK         4
#define H_HOOK_HP             5
#define H_HOOK_SP             6
#define H_HOOK_ATTACK_MOD     7
#define H_HOOK_ALCOHOL        8
#define H_HOOK_FOOD           9
#define H_HOOK_DRINK          10
#define H_HOOK_POISON         11
#define H_HOOK_CONSUME        12
#define H_HOOK_TEAMROWCHANGE  13
#define H_HOOK_INSERT         14
#define H_HOOK_EXIT_USE       15
#define H_HOOK_INIT           16

// the possible consumer types
#define H_HOOK_SURVEYOR       0
#define H_HOOK_MODIFICATOR    1
#define H_DATA_MODIFICATOR    2
#define H_LISTENER            3
#define H_CONSUMERCHECK(x)    ((intp(x)&& x>=0 && x<=H_LISTENER)?x:-1)
#define H_CONSUMERNAMES       ({"surveyors","hmods","dmods","listeners"})

// priorities
#define H_HOOK_PRIORITYRANGE  3
#define H_HOOK_LIBBASEPRIO    0
#define H_HOOK_GUILDBASEPRIO  (H_HOOK_LIBBASEPRIO+H_HOOK_PRIORITYRANGE)
#define H_HOOK_OTHERBASEPRIO  (H_HOOK_GUILDBASEPRIO+H_HOOK_PRIORITYRANGE)
#define H_HOOK_PRIOCHECK(x)   ((intp(x) && x>=0 && x<H_HOOK_PRIORITYRANGE)?x:-1)
#define H_HOOK_LIBPRIO(x)     ((H_HOOK_PRIOCHECK(x)!=-1)?(H_HOOK_LIBBASEPRIO+x):-1)
#define H_HOOK_GUILDPRIO(x)   ((H_HOOK_PRIOCHECK(x)!=-1)?(H_HOOK_GUILDBASEPRIO+x):-1)
#define H_HOOK_OTHERPRIO(x)   ((H_HOOK_PRIOCHECK(x)!=-1)?(H_HOOK_OTHERBASEPRIO+x):-1)
#define H_HOOK_VALIDPRIO(x)   ((intp(x) && x>=0 && x<(H_HOOK_OTHERBASEPRIO+H_HOOK_PRIORITYRANGE))?x:-1)

// maximum consumer per type
#define MAX_SURVEYOR          1
#define MAX_HOOK_MODIFICATOR  2
#define MAX_DATA_MODIFICATOR  3
#define MAX_LISTENER          5
#define MAX_HOOK_COUNTS ({MAX_SURVEYOR, MAX_HOOK_MODIFICATOR, \
                          MAX_DATA_MODIFICATOR, MAX_LISTENER, \
                         })

// data indices for return value of HookCallback() & Co.
#define H_RETCODE             0
#define H_RETDATA             1

// return codes for HookFlow & Co.
#define H_NO_MOD              0
#define H_CANCELLED           1
#define H_ALTERED             2

// debugging
#define H_DMSG(x)             (h_dbg() && (find_player("zesstra")) ? \
    tell_object(find_player("zesstra"),x):0)

#endif //_HOOKS_H_

// prototypes 
#ifdef NEED_PROTOTYPES
// provider
#ifndef __HOOK_PROVIDER_PROTO
#define __HOOK_PROVIDER_PROTO

// list of offered hooks
int* HListHooks();

// register to hook
int HRegisterToHook(int hookid, object|closure consumer, int hookprio,
    int consumertype, int timeInSeconds);

// unregister from hook
int HUnregisterFromHook(int hookid, object|closure consumer);

// check wether object is a consumer for a given hook
int HIsHookConsumer(int hookid, object|closure consumer);

// offer a hook or stop offering it
protected void offerHook(int hookid, int offerstate);

// trigger a hook
protected mixed HookFlow(int hookid, mixed hookdata);

#endif // __HOOK_PROVIDER_PROTO
#endif // NEED_PROTOTYPES

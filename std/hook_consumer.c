// MorgenGrauen MUDlib
//
// /std/hook_consumer.c  - used to contain some template for hook consumers,
//                         not needed anymore.
//

#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

#include <hook.h>

deprecated mixed HookCallback(object hookSource, int hookid, mixed hookData)
{
  return ({H_NO_MOD,hookData});
}


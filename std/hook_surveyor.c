// MorgenGrauen MUDlib
//
// /std/hook_surveyor.c  - Basisklasse fuer Surveyor-Hooks
//

#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

#include <hook.h>

// override as wished and needed

int HookRegistrationCallback(object registringObject, int hookid,
                        object hookSource, int registringObjectsPriority,
                        int registringObjectsType)
{
  return 1;
}

int HookCancelAllowanceCallback(object cancellingObject, int hookid,
                        object hookSource, int cancellingObjectsPriority,
                        mixed hookData)
{
  return 1;
}

int HookModificationAllowanceCallback(object modifyingObject, int hookid,
                        object hookSource, int modifyingObjectsPriority,
                        mixed hookData)
{
  return 1;
}

mixed HookCallback(object hookSource, int hookid, mixed hookData)
{
  return ({H_NO_MOD,hookData});
}


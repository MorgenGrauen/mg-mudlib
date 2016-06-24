// MorgenGrauen MUDlib
//
// living/moving.c -- moving of living objects
//
// $Id$
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/living/moving";

#include <hook.h>
#define NEED_PROTOTYPES
#include <moving.h>

//<int|<string|closure>* >* GuardExit(object room, int hookid,
//                                    <string|closure>* hdata)
//{
//}

protected int _reg_exit_hook(object dest, object oldenv)
{
  closure cl = symbol_function("GuardExit", this_object());
  if (cl)
  {
    if (oldenv)
      oldenv->HUnregisterFromHook(H_HOOK_EXIT_USE, cl);
    return dest->HRegisterModifier(H_HOOK_EXIT_USE, cl);
  }
  return 0;
}
// Krams nach dem Move machen und nebenbei zum Ueberschreiben.
protected void NotifyMove(object dest, object oldenv, int method)
{
  _reg_exit_hook(dest, oldenv);
  return ::NotifyMove(dest, oldenv, method);
}

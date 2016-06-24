// MorgenGrauen MUDlib
//
// living/description.c -- description for living objects
//
// $Id: description.c 7340 2009-11-19 21:44:51Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <living/description.h>
#include <living/skills.h>
#undef NEED_PROTOTYPES

#include <player/viewcmd.h>
#include <new_skills.h>
#include <container.h>
#include <player/base.h>
#include <wizlevels.h>

inherit "/std/container/light";

protected void create() {
  ::create();
  SetProp(P_LIGHT_TRANSPARENCY, 0);
}

static int _query_player_light()
{
  if (environment())
    return environment()->QueryProp(P_INT_LIGHT) + QueryProp(P_LIGHT_MODIFIER);
}

varargs int CannotSee(int silent)
{
   string is_blind;
   if (is_blind = QueryProp(P_BLIND)) {
      if (!silent) {
         if (stringp(is_blind))
            tell_object(this_object(), is_blind);
         else tell_object(this_object(), "Du bist blind!\n");
      }
      return 2;
   }
   if (UseSkill(SK_NIGHTVISION)<=0 &&
       environment() && QueryProp(P_PLAYER_LIGHT)<=0 &&
       (!IS_LEARNER(this_object()) || !Query(P_WANTS_TO_LEARN)))
   {
       if (!silent) tell_object(this_object(), "Es ist zu dunkel!\n");
       return 1;
   }
   return 0;
}

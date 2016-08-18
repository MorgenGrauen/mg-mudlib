#pragma strong_types,save_types

inherit "/std/thing";

#include <properties.h>

protected void create()
{
   ::create();
   SetProp(P_SHORT, "Eine leuchtende Kugel");
   SetProp(P_LONG, "Eine ziemlich normale Kugel, die magisch leuchtet.\n");
   SetProp(P_NAME, "Lichtkugel");
   SetProp(P_GENDER, 2);
   AddId(({"kugel", "lichtkugel", "leuchtende kugel"}));
   SetProp(P_WEIGHT, 0);
   SetProp(P_VALUE, 0);
   SetProp(P_LIGHT, 2);
}


public varargs int remove(int silent)
{
   if (!silent && environment())
   {
      if (living(environment()))
        tell_object(environment(), "Die Lichtkugel loest sich auf.\n");
      else
        tell_room(environment(), "Die Lichtkugel loest sich auf.\n");
   }
   return ::remove(silent);
}

public void reset() {
   remove();
}

public void start_remove(int zeit)
{
   if (zeit > 0);
     set_next_reset(zeit);
}


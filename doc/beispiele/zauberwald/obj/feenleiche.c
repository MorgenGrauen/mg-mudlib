// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <moving.h>

inherit "std/corpse";

int mampf( string str )
{
   object ob;
   if (QueryDecay()<3) return (int)::mampf(str);
   notify_fail("Was moechtest Du essen?\n");
   if (!str || !id(str)) return 0;
   if (!PL->eat_food(8)) return 1; // fehlermeldung gibt eat_food aus
   if (!objectp(ob=present(WALDID("leichenmp"), PL))) {
      ob=clone_object(OBJ("leichenmp"));
      ob->move(PL, M_NOCHECK|M_GET);
   }
   ob->new_corpse();
   PL->restore_spell_points(50+random(100));
   write("Du merkst wie die noch im Koerper verbliebene Energie in Dich ueberfliesst.\n");
   remove();
   return 1;
}

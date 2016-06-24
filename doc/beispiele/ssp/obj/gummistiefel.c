#include <properties.h>
#include <language.h>
#include <combat.h>
#include "../files.h"
 
inherit "std/armour";
 
create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Paar Gummistiefel");
   SetProp(P_LONG, "Damit bekommt man sicher keine nassen Fuesse.\n");
   SetProp(P_NAME, "Paar Gummistiefel");
   SetProp(P_GENDER, 0);
   AddId(({"stiefel", "gummistiefel", "paar", "paar stiefel"}));
   SetProp(P_ARMOUR_TYPE, AT_BOOT);
   SetProp(P_AC, 0);
   SetProp(P_WEIGHT, 700);
   SetProp(P_VALUE, 120);
   SetProp(P_REMOVE_FUNC, this_object());
}

RemoveFunc()
{
   object env;
   if ((env=environment(QueryProp(P_WORN)))==find_object(L2("m2x1")) ||
       env==find_object(L2("m1x1")))
   {
      write("Das waere keine so tolle Idee hier die Gummistiefel auszuziehen.\n");
      return 0;
   }
   return 1;
}
       



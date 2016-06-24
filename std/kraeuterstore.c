// (c) 2003 by Padreic (padreic@mg.mud.de)
// Stdstore fuer Kraeuterlaeden!

inherit "/std/store";

#include <properties.h>
#include <bank.h>

void create()
{
   if (object_name(this_object()) == __FILE__[0..<3])
   {
     set_next_reset(-1);
     return;
   }
   ::create();
   SetProp(P_MIN_STOCK, 50);
   // in jedem Reset verschwinden nur 2% aller Objekte
   SetProp(P_STORE_CONSUME, 2);
}

protected void create_super()
{
  set_next_reset(-1);
}

void reset()
{
   ::reset();
   // damit die Kraeuter im Laden nicht schimmeln, ggf. trocknen...
   object *obs = filter( all_inventory(), function int (object ob) {
                   return ob->TimeToLive()<3600;} );
   obs->DryPlant(95);
}


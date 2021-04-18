/* Changelog:
   * 11.09.2007, Zesstra
     strong_types und save_types aktiviert.
*/
#pragma strong_types
#pragma save_types

#include "/p/service/padreic/mnpc/mnpc.h"

inherit "/std/npc";
inherit MNPC_MOVING;

void create()
{
   npc::create();
   moving::mnpc_create();
}

void reset()
{
   npc::reset();
   moving::mnpc_reset();
}

public varargs void init(object origin)
{
   npc::init(origin);
   moving::mnpc_init();
}

int InsertEnemy(object enemy)
{
   int i = npc::InsertEnemy(enemy);
   if (!i) return 0;
   moving::mnpc_InsertEnemy(enemy);
   return i;
}

varargs int move(object|string dest, int meth, string dir, string textout,
  string textin)
{
   int i=npc::move(dest, meth, dir, textout, textin);
   if (i!=1) return i;
   moving::mnpc_move();
   return 1;
}

int PreventFollow(object dest)
{  return moving::mnpc_PreventFollow(dest);  }

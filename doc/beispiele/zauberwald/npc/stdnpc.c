// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"

inherit "/std/npc";
inherit "/p/service/mupfel/getkill";

void create()
{
   ::create();
   SetProp(ZAUBERWALDNPC, 1);
   SetProp(P_AGGRESSIVE, ([P_RACE:(["Dunkelelf":1, 0:0.0]) ]));
}

int AutoAttack(object ob)
{
   if (ob && ob->QueryProp(ZAUBERWALD)>time()) return 1;
   return ::AutoAttack(ob);
}

int InsertEnemy(object ob)
{
   int ret, is_enemy;
   is_enemy=IsEnemy(ob);
   ret=(int)::InsertEnemy(ob);
   if (!is_enemy) { // alle anderen Zauberwald-NPCs im Raum verstaendigen
      filter_objects(
        filter_objects(all_inventory(environment())-users(), "QueryProp", ZAUBERWALDNPC),
        "InsertEnemy", ob
      );
   }
   return ret;
}

int Defend(int dam, mixed dts, mixed spell, object enemy)
{
   if (enemy) enemy->SetProp(ZAUBERWALD, time()+AGGRESSIVE_TIME);
   return (int)::Defend(dam, dts, spell, enemy);
}

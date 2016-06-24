#include <properties.h>                    
#include <combat.h>
#include "../files.h"

inherit MMONST;
 
create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Eine Titanwalze");
   SetProp(P_LONG, "Pass nur auf, dass diese Titanwalze Dir nicht ueber die Zehen rollt.\nBei diesem Gewicht koennte das auesserst unangenehme Folgen haben.\n");
   SetProp(P_NAME, "Titanwalze");
   SetProp(P_GENDER, 2);
   AddId(({"walze", "titanwalze"}));
   SetProp(P_LEVEL, 14);
   SetProp(P_MAX_HP, 170);
   SetProp(P_ALIGN, 120);
   SetProp(P_XP, 51000);
   SetProp(P_BODY, 90);
   SetProp(P_HANDS, ({" mit wuchtigen Schlaegen", 60}));
   SetProp(P_SIZE, 210);
   SetProp(P_DIE_MSG, " zerfaellt in ihre Bestandteile.\n");
   SetAttackChats(30, ({
   "Die Titanwalze versucht Dich platt zu walzen.\n",
   "Die Titanwalze macht: Rumpel, polter ...\n",
   "Die Titanwalze roehrt.\n"}));
   set_living_name("titanwalze");
   AddItem(OBJ("titanring")); 
}

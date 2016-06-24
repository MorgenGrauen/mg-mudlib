// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <combat.h>
#include <moving.h>
#include <new_skills.h>

inherit NPC("stdnpc");
inherit MNPC_MOVING;

void create()
{
   if (!clonep(ME)) return;
   stdnpc::create();
   moving::mnpc_create();
   SetProp(P_NAME_ADJ, "alt");
   SetProp(P_NAME, "Eiche");
   SetProp(P_SHORT, "Eine alte Eiche");
   SetProp(P_MSGOUT, "wandert");
   SetProp(P_LONG,
     "Vor Dir steht eine sehr sehr alte Eiche. Doch diese Eiche ist keine\n"
    +"gewoehnliche Eiche, wie Du sie bereits an vielen anderen Stellen gesehn hast.\n"
    +"Diese Eiche lebt! Du kannst deutlich einen Mund, eine Nase und sogar zwei\n"
    +"Ohren erkennen. Sie ist eigentlich sehr friedlich gesonnen, kann aber auch\n"
    +"sehr sehr boese werden, wenn es jemand wagt die Ruhe des Waldes zu stoeren.\n");
   SetProp(P_RACE, "eiche");
   SetProp(P_LEVEL, 50);
   SetProp(P_ATTRIBUTES, (["int":50,"con":50,"str":70,"dex":15]) );
   SetProp(P_GENDER, FEMALE);
   SetProp(P_MAX_HP, 500);
   SetProp(P_HP, 500);
   SetProp(P_ALIGN, 500);
   SetProp(P_BODY, 100);
   SetProp(P_SIZE, 650+random(151));
   AddAdjective("alt");
   AddId(({"eiche", "eichen", WALDID("eiche") }));
   SetProp(P_HANDS, ({" mit einem ihrer langen Aeste",400, DT_WHIP}) );
   SetProp(P_SKILL_ATTRIBUTE_OFFSETS, ([SA_SPEED:180]));
   SetProp(P_NOCORPSE, 1);
   SetProp(P_XP, 500*400*5);
   SetProp(P_DEFAULT_INFO, "bleibt absolut regungslos und reagiert ueberhaupt nicht.\n");
   SetProp(P_RESISTANCE_STRENGTHS,
    ([DT_SLASH:     0.1,
      DT_MAGIC:    -1.0,
      DT_BLUDGEON: -0.5,
      DT_POISON:    0.25,
      DT_HOLY:     -1.0,
      DT_RIP:       0.25,
      DT_FIRE:      0.25,
      DT_PIERCE:   -0.5,
      DT_WHIP:     -1.0 ]) );
   SetProp(MNPC_AREA, ({ ROOM("weg2"), ROOM("lichtung") }) );
   SetProp(MNPC_RANDOM, 10);
   SetProp(MNPC_DELAY, 8);
   SetProp(MNPC_WALK_TIME, 600);
   SetProp(MNPC_FLAGS, MNPC_WALK|MNPC_NO_WALK_IN_FIGHT|MNPC_ONLY_EXITS);
}

static string _query_info()
{
   if (!PL || PL->QueryProp(ZAUBERWALD)<=time())
      return "Die alte Eiche ist Dir sehr friedlich gesonnen. Es ist jedoch auch sicher\n"
            +"klug dieses nicht zu aendern, da sie mit ihren Aesten wohl auch sehr kraeftig\n"
            +"zuschlagen kann.\n";
   return "Du hast den Zorn der alten Eiche auf Dich gezogen, das war ganz und gar nicht\n"
         +"klug von Dir. Jetzt lebe auch mit den Konsequenzen....\n";
}

int remove()
// wenn eine Eiche getoetet wird, dann verlangsamt das den reset der Eiche
// die auf weg2 blockt. In der Regel steht ja auch so schon immer eine der
// wanderden Eichen dort. Dies soll verhindern, das gerade eine Eiche resetet
// wenn man eine tod hat.
{
   call_other(ROOM("weg2"), "delay_reset");
   catch(call_other(QueryProp(MNPC_HOME), "delay_reset", 900+random(3600)));
   return (int)::remove();
}

// wenn man den MNPC mit einem anderen stdnpc kombiniert muessen einige
// funktionen per Hand ueberschrieben werden....

void reset()
{
   stdnpc::reset();
   moving::mnpc_reset();
}

void init()
{
   stdnpc::init();
   moving::mnpc_init();
}

varargs int move(mixed dest, int meth, string dir, string out, string in)
{
   int i;

   // extra fuer den Zauberwald eingebaut
   if (environment() && object_name(environment())==ROOM("weg2")) {
      // wenn es die einzige Eiche ist, dann bleibt sie hier stehn,
      // bewacht den Weg und bewegt sich nicht mehr hier weg!
      object *inv;

      inv = all_inventory(environment()) - ({ ME });
      for (i=sizeof(inv)-1; i>=0; i--)
         if (BLUE_NAME(inv[i])==NPC("laufeiche")) break;
      if (i<0) return ME_CANT_LEAVE_ENV;
   }
   //////////////////////////////////////
   i=(int)stdnpc::move(dest, meth, dir, out, in);
   if (i!=1) return i;
   moving::mnpc_move();
   return 1;
}

int PreventFollow(object dest)
{  return moving::mnpc_PreventFollow(dest);  }

void NotifyPlayerDeath(object who, object killer, object lost_exp)
{
  if (!who || killer!=ME) return; // uninteressant
  log_file("padreic/kill", ctime(time())+" "+capitalize(getuid(who))+" getoetet von /zauberwald/laufeiche\n");
}

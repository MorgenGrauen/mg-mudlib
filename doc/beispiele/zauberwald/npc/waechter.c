// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <combat.h>
#include <new_skills.h>

inherit NPC("stdnpc");

void create()
{
   ::create();
   SetProp(P_NAME, "Waechter des Waldes");
   SetProp(P_SHORT, "Der Waechter des Waldes");
   SetProp(P_LONG,
     "Vor Dir steht eine sehr sehr alte Eiche. Doch diese Eiche ist keine\n"
    +"gewoehnliche Eiche, wie Du sie bereits an vielen anderen Stellen gesehn hast.\n"
    +"Diese Eiche lebt! Du kannst deutlich einen Mund, eine Nase und sogar zwei\n"
    +"Ohren erkennen. Sie ist eigentlich sehr friedlich gesonnen, kann aber auch\n"
    +"sehr sehr boese werden, wenn es jemand wagt die Ruhe des Waldes zu stoeren.\n");
   SetProp(P_RACE, "eiche");
   SetProp(P_AGGRESSIVE, 0);
   SetProp(P_LEVEL, 100);
   SetProp(P_ATTRIBUTES, (["int":80,"con":100,"str":100,"dex":20]) );
   SetProp(P_GENDER, MALE);
   SetProp(P_MAX_HP, 1000);
   SetProp(P_HP, 1000);
   SetProp(P_ALIGN, 500);
   SetProp(P_BODY, 100);
   SetProp(P_SIZE, 1500);
   SetProp(P_DISABLE_ATTACK, -10000); // is nich :)
   AddId(({"eiche", "waechter","waechter des waldes", WALDID("waechtereiche") }));
   SetProp(P_HANDS, ({" mit einem seiner langen Aeste",550,DT_WHIP}) );
   SetProp(P_SKILL_ATTRIBUTE_OFFSETS, ([SA_SPEED:210]));
   SetProp(P_NOCORPSE, 1);
   SetProp(P_XP, 1000*550*5);
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
   AddCmd(({"osten", "westen"}), "cmd_blocken");
}

static string _query_info()
{
   if (!PL || PL->QueryProp(ZAUBERWALD)<=time())
      return "Die alte Eiche ist Dir sehr friedlich gesonnen. Es ist jedoch auch sicher\n"
            +"klug dieses nicht zu aendern, da sie mit ihren Aesten wohl auch sehr kraeftig\n"
            +"zuschlagen kann.\n";
   return "Du hast den Zorn der alten Waechtereiche auf Dich gezogen, das war ganz und\n"
         +"gar nicht klug von Dir. Jetzt lebe auch mit den Konsequenzen....\n";
}

static int cmd_blocken()
{
   if (PL->QueryProp(P_RACE)=="Dunkelelf" &&
       PL->QueryProp(AUSGANG)==query_verb()) {
      write(BS(Name(WER, 1)+" laesst Dich nicht vorbei.")
           +break_string("Du solltest hier besser verschwinden, "
                        +"so Typen wie Du sind hier nicht gern gesehn!",
                        78, Name(WER, 1)+" sagt: "));
      return 1;
   }
   if (PL && PL->QueryProp(ZAUBERWALD) &&
       PL->QueryProp(AUSGANG)==query_verb()) {
      write(BS(Name(WER, 1)+" steht Dir da leider im Weg und laesst Dich nicht vorbei."));
      return 1;
   }
}

int remove()
{
   call_other(ROOM("weg1"), "delay_reset");
   return (int)::remove();
}

void NotifyPlayerDeath(object who, object killer, object lost_exp)
{
  if (!who || killer!=ME) return; // uninteressant
  log_file("padreic/kill", ctime(time())+" "+capitalize(getuid(who))+" getoetet von /zauberwald/waechter\n");
}

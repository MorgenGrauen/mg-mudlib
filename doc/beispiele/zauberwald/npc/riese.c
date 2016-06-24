// (c) 2003 by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <properties.h>

inherit NPC("stdnpc");

void create()
{
   ::create();
   create_default_npc(30, 2500);
   SetProp(P_ARTICLE, 0);
   SetProp(P_SHORT, "Grimmbart Felsenschieber");
   SetProp(P_LONG,
     "Vor Dir sitzt der Riese Grimmbart Felsenschieber. Eigentlich scheint er\n"
    +"ein sehr gutmuetiger und freundlicher Riese zu sein. Aber irgendwie wirkt\n"
    +"er derzeit sehr traurig.\n");

   SetProp(P_NAME, "Grimmbart Felsenschieber");
   SetProp(P_GENDER, MALE);
   SetProp(P_SIZE, 297);
   SetProp(P_ALIGN, 200);
   SetProp(P_RESISTANCE_STRENGTHS,
     ([DT_HOLY:-1.0,
       DT_UNHOLY:0.5,
       DT_MAGIC:0.25,
       DT_POISON:-0.25,
       DT_WHIP:-0.6,
       DT_BLUDGEON:-0.3,
       DT_PIERCE:-0.1,
       DT_SLASH:-0.1
     ]));

   AddInfo(({"felsen", "fels"}),
     "Ich liebe es meine Muskeln anzuspannen und Felsen zu schieben.",
     "antwortet: ");
   AddInfo("schieben",
     "Ich habe doch keine Felsen mehr die ich schieben koennte.");
   AddInfo(({"trauer", "traurigkeit"}),
     "Frueher da habe ich den ganzen Tag hier meine Felsen hin- und "
    +"hergeschoben, aber nun hat Ulinia mir verboten hier im Wald mit "
    +"den Felsen zu spielen und ich musste sie alle wegtragen :o(",
     "jammert: ");
   AddInfo("tag", "Ja so ein Tag ist ganz schoen lang ohne Felsen",
     "sagt: ");
   AddInfo("ulinia",
      "Eine richtig bloede Fee und haette sie ihre doofen Zauberkraefte nicht,"
     +"koennte sie uns hier auch nicht alle so rumkommandieren *mopper*.",
     "sagt: ");
   AddInfo("grund",
      "Nur weil mir einmal ausversehen ein klitzekleiner Fels in den Tuempel "
     +"gefallen ist, musste ich all meine schoenen Felsen aus dem Wald bringen.",
     "sagt: ");
   AddInfo("tuempel",
      "Bloeder Tuempel, wer braucht den schon!", "antwortet gereizt: ");
   AddInfo("zauberkraefte",
      "Ich freu mich schon richtig auf den Tag, an dem Ulinias Zauberkraefte "
     +"nachlassen!",
      "sagt: ");
   AddInfo(({"wald", "zauberwald"}),
      "Wenn es hier wieder ganz viele schoene Felsen gaebe, waer er wieder das "
     +"reinste Paradies.", "antwortet: ");
   AddInfo("paradies",
      "Ja frueher war der Wald hier einmal das reinste Paradies.", "antwortet: ");

   SetChats(3,
     ({Name(WER)+" sagt: Ich bin so traurig.\n",
       Name(WER)+" schluchzt: meine schoenen Felsen.. wuaaahhhhh\n",
       Name(WER)+" sagt: nie wieder werde ich Felsen schieben koennen.\n",
       Name(WER)+" sagt: F E L S E N !\n",
       Name(WER)+" jammert: Die Welt ist so furchtbar ungerecht!\n",
       Name(WER)+" fragt: Wie soll ich so bloss weiterleben?\n"
     }));
   
   AddId("riese");
   AddId("grimmbart");
   AddId("felsenschieber");
}

void NotifyPlayerDeath(object who, object killer, object lost_exp)
{
  if (!who || killer!=ME) return; // uninteressant
  log_file("padreic/kill", ctime(time())+" "+capitalize(getuid(who))
    +"getoetet von /zauberwald/riese\n");
}

void die()
{
  log_file("padreic/kill", ctime(time())+" Grimmbart wurde von  "+get_killer()
    +" getoetet.\n");
  ::die();
}

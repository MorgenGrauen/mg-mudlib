// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"

inherit ROOM("stdroom");

void create()
{
   ::create();
   SetProp(P_INDOORS, 0);
   SetProp(P_LIGHT, 0);
   SetProp(P_INT_SHORT, "Auf einem Waldweg im Zauberwald");
   AddDetail(({"blaetterdach", "dach", "baeume"}),
     "Die Baeume rechts und links des Weges bilden ueber dem Weg ein so dichtes\n"
    +"Blaetterdach, das so gut wie ueberhaupt kein Sonnenlicht mehr auf den Weg\n"
    +"scheint.\n");
   AddSpecialDetail("szene", "_query_int_long");
   AddSpecialDetail(({"weg", "weges", "waldweg", "boden"}), "det_weg");
   AddDetail(({"licht", "sonnenlicht", "sonnenstrahlen"}),
     "Durch das dichte Blaetterdach schaffen es die Sonnenstrahlen nur noch sehr\n"
    +"vereinzelt bis auf den Weg durchzukommen. Dadurch ist es hier unten relativ\n"
    +"Feucht, kalt und dunkel.\n");
   AddDetail(({"himmel", "sonne", "wolke", "wolken"}),
     "Das Blaetterdach ist hier so dicht, das Du den Himmel nicht mal mehr erahnen\n"
    +"kannst. Du weisst nicht mal, ob es regnet oder ob die Sonne scheint.\n");
   AddDetail("feucht",
     "Bloss weil feucht grossgeschrieben ist es noch lange kein Substantiv, wenn\n"
    +"ueberhaupt heisst es dann _Feuchtigkeit_ :).\n");
   AddDetail("substantiv", "Nicht jedes Substantiv ist ein sinnvolles Detail :)\n");
   AddDetail("feuchtigkeit", "Ein wenig feucht, wie man es halt aus jedem Wald so kennt.\n");
   AddDetail("kaelte",
     "Durch das wenige Sonnenlicht, das bis nach hier unten durchdringt, ist es\n"
    +"hier verhaeltnismaessig frisch.\n");
   AddDetail("dunkelheit",
     "Wenn es hell ist, ist es nicht mehr dunkel und wenn es nicht hell ist, kannst\n"
    +"Du nicht viel sehn. Wie also willst Du die Dunkelheit untersuchen :)?\n");
   AddDetail("_feuchtigkeit_", "Meine Guete, nimm doch nich immer alles gleich _so_ woertlich.\n");
   AddDetail("guete", "Sei lieber vorsichtig, soo guetig bin ich nu auch nicht :).\n");
   AddDetail(({"lichtquelle", "nachtsicht"}),
     "Hier ist es eigentlich verdammt dunkel und ohne Nachtsicht oder eine eigene\n"
    +"Lichtquelle, koenntest Du hier _nichts_ sehn.\n");
   AddDetail(({"ausgang", "osten"}),
     "Wenn Du dem Weg Richtung Osten folgst, dann kommst Du ziemlich bald wieder\n"
    +"Richtung Ausgang.\n");
   AddDetail(({"stueck", "wald", "westen"}),
     "Naja.. Du stehst zwar schon im Wald drin, aber nach Westen hin, gehst es noch\n"
    +"ein Stueck tiefer hinein.\n");
   AddExit("osten", ROOM("eingang"));
   AddExit("westen", ROOM("weg2"));
   AddItem(NPC("waechter"), REFRESH_REMOVE, 1);
}

static string det_weg()
{
   if (present(WALDID("waechtereiche"), ME))
      return "Mitten auf dem Weg steht der Waechter des Waldes und kontrolliert sehr genau,\n"
            +"wer in den Wald hinein darf und wer nicht. Eigentlich ist er jedoch den\n"
            +"meisten immer sehr friedlich gesonnen.\n";
   return "Der Weg fuehrt hier nach Westen noch ein Stueck tiefer in den Wald hinein,\n"
         +"nach Osten kannst Du jedoch wieder in Richtung Ausgang des Zauberwalds gehn.\n";
}

static string _query_int_long()
{
   if (present(WALDID("waechtereiche"), ME))
      return "Hier ist es jetzt richtig duester und ohne eigene Lichtquelle koenntest Du\n"
            +"nichts mehr sehn, da so gut wie ueberhaupt kein Licht mehr durch das\n"
            +"Blaetterdach auf den Weg scheint. Mitten in dieser eigentlich lebens-\n"
            +"unfreundlichen Szene, steht eine grosse alte Eiche mitten auf dem Weg.\n";
   return "Hier ist es jetzt richtig duester und ohne eigene Lichtquelle koenntest Du\n"
         +"hier nichts mehr sehn, da so gut wie ueberhaupt kein Licht mehr durch das\n"
         +"Blaetterdach auf den Weg scheint. Du kannst noch ein Stueck tiefer in den\n"
         +"Wald hinein und nach Westen gehn, oder aber zurueck Richtung Kreuzung nach\n"
         +"Osten.\n";
}

// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"

inherit ROOM("stdroom");

void create()
{
   ::create();
   SetProp(P_INDOORS, 0);
   SetProp(P_LIGHT, 1);
   SetProp(P_INT_SHORT, "Am Rande einer Lichtung im Zauberwald");
   AddSpecialDetail(({"wald", "zauberwald"}), "_query_int_long");
   AddDetail(({"westen", "lichtung", "plaetzchen", "idyllisches plaetzchen"}),
     "Im Westen befindet sich eine grosse Lichtung mit einem kleinen Tuempel, am\n"
    +"Rande der Lichtung stehen vereinzelt noch einige Baeume aber zum Tuempel hin,\n"
    +"wird es dann mehr und mehr nur noch Gras das den Boden bedeckt.\n");
   AddDetail(({"rasen", "gras"}),
     "Am besten betrittst Du die Lichtung einfach und schaust Dir den Rasen\n"
    +"naeher an.\n");
   AddDetail("tuempel",
     "Wenn Du Dir den Tuempel naeher ansehen moechtest, dann solltest Du schon\n"
    +"ein Stueck naeher rangehen.\n");
   AddDetail(({"stueck", "raum"}),
     "Na wenigstens einen Raum weiter solltest Du schon gehn.\n");
   AddDetail("schatten",
     "Hier befindest Du Dich noch im Schatten der Baeume, die Lichtung im Westen\n"
    +"liegt jedoch komplett in der waermenden Sonne.\n");
   AddSpecialDetail("sonne", "det_sonne");
   AddDetail(({"wegesrand", "baeume", "wegrand", "rand"}),
     "Am Wegesrand stehen einige Baeume die ein schoenes Blaetterdach ueber den Weg\n"
    +"spannen und den Weg in ihren Schatten legen.\n");
   AddDetail(({"himmel", "blaetterdach", "dach", "licht", "sonnenstrahlen", "wolken", "wolke"}),
     "Das Blaetterdach ist ziemlich dicht, so dass Du nicht soo viel sehen kannst,\n"
    +"aber einige Sonnenstrahlen durchbrechen das Dach und erreichen den Weg.\n");
   AddDetail(({"weg", "boden", "erde", "ausgang", "osten", "richtung"}),
     "Im Westen fuehrt der Weg zu einer grossen Lichtung, waehrend er nach Osten hin\n"
    +"Richtung Ausgang des Zauberwalds fuehrt.\n");
   AddDetail("voegel", "Du kannst sie deutlich hoeren, bekommst aber keinen zu sehn.\n");
   AddExit("osten", ROOM("weg1"));
   AddExit("suedwesten", ROOM("lichtungso"));
   AddExit("westen", ROOM("lichtungo"));
   AddExit("nordwesten", ROOM("lichtungno"));
   AddCmd("osten", "cmd_osten");
   AddItem(NPC("laufeiche"), REFRESH_DESTRUCT);
}

static string det_sonne()
{
   if (PL) PL->SetProp(P_BLIND, 1);
   return "Du schaust fasziniert in die Sonne und untersucht sie naeher, doch das war\n"
         +"wohl ein grosser Fehler, denn ploetzlich wird es ganz ganz dunkel um Dich rum.\n";
}

void init()
{
   // wenn man von westen kommt und keine blockende Eiche hier ist, dann
   // so betrachten als waere man aus dem osten gekommen
   if (query_once_interactive(PL) && !present(WALDID("eiche")))
      PL->SetProp(AUSGANG, "westen");
   ::init();
}

static int cmd_osten()
// wenn gerade eine Eiche hierhin gelaufen ist, kommen feindliche
// Spieler nicht mehr vorbei :)
{
   if (PL && PL->QueryProp(AUSGANG)!="westen" &&
       PL->QueryProp(ZAUBERWALD)>time() && present(WALDID("eiche"), ME)) {
      if (present(WALDID("eiche 2"), ME))
         write("Die Eichen stehen Dir dabei leider im Weg und lassen Dich nicht vorbei.\n");
      else write("Die Eiche steht Dir dabei leider im Weg und laesst Dich nicht vorbei.\n");
      return 1;
   }
}

static string _query_int_long()
{
   if (!PL || PL->QueryProp(ZAUBERWALD)<=time())
     return
      "Du stehst nun mitten im Zauberwald. Im Westen erstreckt sich vor Dir eine\n"
     +"grosse Lichtung, auf die Du muehelos gelangen kannst. Nach Osten hin fuehrt\n"
     +"ein schmaler Weg zurueck zum Ausgang des Waldes. Alles scheint hier sehr\n"
     +"friedlich zu sein, die Voegel zwitschern und das Blaetterdach ist hier auch\n"
     +"nicht mehr so dicht wie im Osten, so dass einige Sonnenstrahlen den Weg\n"
     +"erreichen. Es zieht Dich foermlich nach Westen auf die Lichtung, mitten auf\n"
     +"dieses sonnige idyllische Plaetzchen.\n";
   return "Nichts wie raus hier. Es war ganz und gar keine gute Idee hier im Wald Deiner\n"
         +"Aggressivitaet freien Lauf zu lassen. Du kannst von Glueck haben, wenn Du mit\n"
         +"einem blauen Auge davon kommst und den Wald noch lebend verlassen kannst.\n"
         +"Nach Westen gelangst Du auf die grosse Lichtung, ratsamer waere jedoch wohl\n"
         +"eher der Waldweg im Osten, ueber den Du den Zauberwald wieder verlassen kannst.\n";
}

// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"

inherit ROOM("stdroom");

void create()
{
   ::create();
   SetProp(P_INDOORS, 0);
   SetProp(P_LIGHT, 1);
   SetProp(P_INT_SHORT, "Auf einem kleinen Waldweg");
   SetProp(P_INT_LONG,
     "Du stehst auf einem kleinen Waldweg der nach Westen hin tiefer in den Wald\n"
    +"hinein fuehrt. Nach Osten kannst Du jedoch auch wieder zurueck zur Kreuzung\n"
    +"gelangen. Alles ist hier recht duester, denn das Zweigdach der Baeume laesst\n"
    +"kaum noch Sonnenlicht hindurch.\n");
   AddDetail("wald", QueryProp(P_INT_LONG));
   AddDetail(({"boden", "weg", "erde", "waldweg"}),
     "Der Weg wurde nicht richtig angelegt und es scheint auch nicht so, als wuerde\n"
    +"ihn jemand pflegen. Dennoch wachsen keinerlei Pflanzen auf dem Weg und Du\n"
    +"gehst auf der blossen Erde.\n");
   AddDetail(({"wegrand", "wegesrand", "gestruepp"}),
     "Am Wegesrand wachsen einige alte Baeume und andere Pflanzen die zusammen ein\n"
    +"undurchdringliches Gestruepp ergeben. Dir bleibt also nichts weiter uebrig, als\n"
    +"nur den Weg nach Westen oder nach Osten zu folgen.\n");
   AddDetail(({"baeume", "alter"}),
     "Das Alter der Baeume laesst sich nicht genau bestimmen, sie scheinen jedoch\n"
    +"sehr sehr alt zu sein. In einem der Baeume siehst Du ein kleines Baumhaus.\n");
   AddDetail(({"pflanzen", "farne", "straeucher"}),
     "Verschiedene Straeucher, Farne und zahlreiche Baeume zieren den Wegesrand.\n"
    +"Du entdeckst dabei aber nichts weiter auffaelliges.\n");
   AddDetail(({"osten", "kreuzung"}),
     "Wenn Du dem Weg nach Osten hin folgst, kannst Du zurueck zur Kreuzung gelangen.\n");
   AddDetail(({"zweige", "zweigdach", "sonnenlicht", "licht", "dach", "westen"}),
     "Das Zweigdach der Baeume laesst hier so wenig Sonnenlicht hindurch, das Du ein\n"
    +"Stueck weiter im Westen kaum noch ohne eigene Lichtquelle weiterkommen kannst.\n"
    +"Du solltest Dich also besser nicht ohne eigene Lichtquelle tiefer in den Wald\n"
    +"hinein wagen.\n");
   AddDetail(({"fackel", "lampe", "licht", "lichtquelle", "lichtspruch"}),
     "Wenn Du beabsichtigst weiter nach Westen zu gehn, solltest Du eine Fackel,\n"
    +"eine Lampe oder sonst irgendeine Lichtquelle mitnehemn. Zumindest ist es dort\n"
    +"sehr dunkel.\n");
   AddDetail(({"wolke", "wolken", "sonne", "himmel"}),
     "Der Himmel ist durch das dichte Dach der Zweige, nicht mehr zu sehn.\n");
   AddExit("osten", "/d/unterwelt/raeume/wald4");
   AddExit("westen", ROOM("weg1"));
}

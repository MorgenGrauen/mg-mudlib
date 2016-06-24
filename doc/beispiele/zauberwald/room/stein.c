// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"

inherit ROOM("stdroom");

void create()
{
   ::create();
   SetProp(P_INDOORS, 0);
   SetProp(P_LIGHT, 1);
   SetProp(P_INT_SHORT, "Bei einem Stein");
   AddSpecialDetail(({"ort", "wald", "platz"}), "_query_int_long");
   AddDetail("stein",
     "Der Stein ist, durch die Sonnenstrahlen ordentlich aufgeheizt, angenehm warm\n"
    +"und bietet eigentlich den idealen Platz um es sich gut gehn zu lassen und in\n"
    +"der Sonne zu liegen.\n");
   AddDetail(({"sonne", "himmel", "sonnenstrahlen", "strahlen"}),
     "Der blaue Himmel ist nahezu wolkenfrei und so strahlt die Sonne in voller\n"
    +"Pracht oben am Himmel und ihre Strahlen prasseln auf Deine Haut.\n");
   AddDetail(({"boden", "weg", "waldweg", "erde"}),
     "Ein ganz normaler Waldweg halt, ein bisschen sandig, einige kleine Aestchen\n"
    +"ganz normal halt.\n");
   AddSpecialDetail("zeit", "det_zeit");
   AddDetail(({"aestchen", "stoeckchen"}),
     "Ganz kleine heruntergefallene Stoeckchen, durch reges drueber laufen\n"
    +"im Laufe der Zeit kleingemalen und vom Wetter aufgeloest...\n");
   AddDetail("wetter",
     "Auch wenn hier heute die Sonne scheint, gibt es natuerlich auch hier\n"
    +"gelegentlich verregnete Tage.\n");
   AddDetail("tage", "Naja... allzuviele sind es jedoch nicht.\n");
   AddDetail(({"nordosten", "lichtung", "ausgang", "weiher"}),
     "Im Nordosten kannst Du immer noch die grosse Lichtung mit den wandernden Eichen\n"
    +"und dem verwunschenen Weiher erkennen.\n");
   AddDetail(({"eichen", "wandernde eichen"}),
     "Im Nordosten auf der Lichtung hast Du vorhin wandernde Eichen gesehn,\n"
    +"vielleicht solltest Du einfach nochmal zurueck gehn und sie Dir naeher\n"
    +"anschaun.\n");
   AddDetail(({"wolke", "wolken", "schaefchenwolke"}),
     "Der Himmel ist eigentlich wolkenfrei nur hier und da zieht mal eine kleine\n"
    +"Schaefchenwolke vorbei.\n");
   AddExit("nordosten", ROOM("lichtungsw"));
   AddItem(NPC("titina"), REFRESH_REMOVE, 1);
   AddCmd(({"setz", "setze", "leg", "lege", "kletter", "klettere"}), "cmd_setzen");
   AddCmd(({"steh", "stehe"}), "cmd_aufstehn");
}

#define STEIN WALDID("stein")

void init()
{
  if (PL) PL->SetProp(STEIN, 0);
  ::init();
}

static int cmd_aufstehn(string str)
{
    if (!PL->QueryProp(STEIN)) return 0;
    if (str!="auf" && str!="von stein auf") {
       notify_fail("Von was moechtest Du aufstehn?\n");
       return 0;
    }
    PL->SetProp(STEIN, 0);
    write("Du kletterst also vom Stein wieder runter und stehst auf.\n");
    return 1;
}

#define VERB(x) (["setz": "setzt", "leg": "legst", "kletter": "kletterst"])[x]

static int cmd_setzen(string str)
{
   string verb;
   verb=query_verb();
   if (verb && verb[<1]=='e') verb=verb[0..<2];
   if (str!="auf stein" && str!="stein" && str!="von stein") {
      notify_fail(BS("Wohin moechtest Du "
                 +(verb!="kletter" ? "Dich " : "")+verb+"en?"));
      return 0;
   }
   if (PL->QueryProp(STEIN)) {
      if (verb!="kletter" || str=="auf stein") {
         write("Du sitzt doch bereits auf dem Stein!\n");
         return 1;
      }
      PL->SetProp(STEIN, 0);
      write("Du kletterst also vom Stein wieder runter und stehst auf.\n");
      return 1;
   }

   write(BS("Entspannt "+VERB(verb)+" Du "+(verb!="kletter" ? "Dich " : "")
        +"auf den Stein und geniesst die Sonne."));
   PL->SetProp(STEIN, time());
   return 1;
}

static string det_zeit()
{  return "Wir haben genau "+dtime(time())[<8..]+".\n"; }

static string _query_int_long()
{
   if (present(WALDID("fee"), ME))
      return
        "Hier mitten im Wald steht ein grosser Stein, auf dem Titina die wunderschoene\n"
       +"Waldfee sitzt und sich in aller Ruhe ihr langes goldenes Haar kaemmt. Die\n"
       +"Sonne strahlt genau auf den Stein und es ist wohl wirklich der gemuetlichste\n"
       +"Ort auf Erden. Links und rechts vom Weg schliesst der dichte Wald an und so\n"
       +"ist der einzige Ausgang Richtung Nordosten zurueck zur Lichtung.\n";
   return "Hier mitten im Wald steht ein grosser Stein, der von der Sonne ordentlich\n"
         +"aufgeheizt wird und ihn zu einem traumhaften Ort fuer alle Sonnenliebhaber\n"
         +"macht. Links und rechts vom Weg schliesst direkt der dichte Wald an und so\n"
         +"ist der einzige Ausgang Richtung Nordosten zurueck zur Lichtung.\n";
}

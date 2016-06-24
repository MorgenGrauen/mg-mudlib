// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"

inherit ROOM("stdroom");

void create()
{
   ::create();
   SetProp(P_INDOORS, 0);
   SetProp(P_LIGHT, 1);
   AddSpecialDetail(({"pixieschule", "schule", "sandplatz", "waldschule"}), "_query_int_long");
   AddDetail(({"spuren", "kampfplatz", "sand"}),
     "Die Spuren im Sand deuten auf einen laengeren Kampf hin, der hier getobt\n"
    +"haben muss. Wer auch immer ihn verloren hat, er hat sich wacker gewehrt.\n");
   AddDetail("wald",
     "Was meinst Du eigentlich, wo Du gerad die ganze Zeit rumlaeufst? Mach einfach\n"
    +"die Augen auf und schau Dir alles in Ruhe an.\n");
   AddDetail("ruhe", "In der Ruhe liegt die Kraft...\n");
   AddDetail(({"augen", "details", "kampfzauber", "kopf", "freund"}),
     "Nu uebertreib mal nich, nicht jedes Substantiv verbirgt wirklich ein\n"
    +"sinnvolles Detail.\n");
   AddDetail("kraft",
     "Wow! Deine Kraft ist ploetzlich auf 32 angestiegen...\n"
    +"Das hast Du jetzt aber nicht wirklich geglaubt, oder?\n"
    +"Wieso untersuchst Du solche Details eigentlich?\n");
   AddSpecialDetail("zeit", "det_zeit");
   AddDetail(({"pixies", "bloedsinn", "unfug", "zauberwesen", "streich"}), 
     "Pixies sind kleine Zauberwesen, die nichts als Unfug im Kopf haben. Wie\n"
    +"niemand sonst nutzen sie ihre magischen Faehigkeiten nicht um jemanden zu\n"
    +"heilen oder fuer Kampfzauber. Nein sie nutzen sie um jemanden zu Aergern\n"
    +"oder ihm einen Streich zu spielen. Einen Pixie zum Freund zu haben, kann\n"
    +"sicher sehr unterhaltsam sein, auf die Dauer aber wohl auch ziemlich\n"
    +"anstrengend.\n");
   AddSpecialDetail("boden", "det_boden");
   AddDetail("halbkreis",
     "Einige Pixies sitzen im Halbkreis um Arina herum und hoeren gespannt zu, wie\n"
    +"sie ihnen einen spannende Geschichte erzaehlt.\n");
   AddDetail(({"geschichten", "sprache"}),
     "Wenn Arina doch bloss in einer Sprache sprechen wuerde, Die Du verstehst...\n"
    +"Ihre Geschichten hoeren sich jedenfalls ausserordentlich spannend an. *seufz*\n");
   AddDetail("himmel", "Der Himmel ist klar und nahezu wolkenfrei.\n");
   AddDetail(({"osten", "ausgang", "lichtung", "weg", "waldweg"}),
     "Der einzige Ausgang von hier, ist der kleine Waldweg im Osten der zurueck\n"
    +"zur grossen Lichtung fuehrt.\n");
   SetProp(P_INT_SHORT, "Die Pixieschule");
   AddExit("osten", ROOM("lichtungnw"));
   AddItem(NPC("pixie"), REFRESH_MOVE_HOME);
   AddItem(NPC("pixie"), REFRESH_MOVE_HOME);
   AddItem(NPC("pixie"), REFRESH_MOVE_HOME);
   AddItem(NPC("arina"), REFRESH_REMOVE, 1);
}

static string det_boden()
{
   if (present(WALDID("fee"), ME))
      return GetDetail("halbkreis");
   return GetDetail("sand");
}

static string det_zeit()
{  return "Wir haben genau "+dtime(time())[<8..]+".\n"; }

static string _query_int_long()
{
   if (present(WALDID("fee"), ME))
      return "Du befindest Dich in der Waldschule der Pixies. Einige Pixies sitzen im\n"
            +"Halbkreis um die Waldfee Arina rum und hoeren gespannt ihren Geschichten zu.\n"
            +"Gelegentlich erhebt sich Arina um mit einem Stock etwas in den Sand zu\n"
            +"zeichnen, oder um einen der Pixies zu ermahnen weil er wieder irgendeinen\n"
            +"verrueckten Bloedsinn gemacht hat. Da sich ringsherum der dichte Wald\n"
            +"anschliesst, ist der einzige Ausgang, der Weg zurueck nach Osten auf die\n"
            +"grosse Lichtung.\n";
   return "Du stehst auf einem kleinen Sandplatz mitten im Wald. Hier und da siehst Du\n"
         +"im Sand noch deutlich die Spuren eines Kampfes. Umgeben wird dieser Platz\n"
         +"von einem dichten Wald, so dass der einzige Ausgang der Weg zurueck nach\n"
         +"Osten zur grossen Lichtung ist.\n";
}

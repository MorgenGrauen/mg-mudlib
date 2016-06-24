// (c) by Padreic (Padreic@mg.mud.de)

// Ein Raum zum schoenen einbinden eines Seherhauses in den Wald

#include "../files.h"
#include <moving.h>

inherit ROOM("stdroom");

void create()
{
   ::create();
   SetProp(P_INDOORS, 1);
   SetProp(P_LIGHT, 1);
   SetProp(P_INT_SHORT, "Mitten im Wald");
   SetProp(P_INT_LONG,
     "Du stehst nun inmitten des Zauberwalds genau vor einer riesigen Buche unter\n"
    +"der jemand eine kleine Huette gebaut hat. Ringsumher stehen weitere Buesche\n"
    +"und Baeume und bilden eine dichte Vegetation, doch im Vergleich zu dieser\n"
    +"riesigen Buche wirkt alles andere wirklich mickrig. Durch das dichte\n"
    +"Blaetterdach dringt nur wenig Licht und so wundert es auch nicht das der\n"
    +"Boden hier relativ feucht ist und die Pflanzen so gut gedeihen koennen. Nach\n"
    +"Norden fuehrt als einziger Ausgang ein kleiner Trampelpfad zur Lichtung.\n");
   AddDetail(({"natur", "wald", "zauberwald"}), QueryProp(P_INT_LONG));
   AddDetail(({"buesche", "baeume", "vegetation", "pflanzen", "farne"}),
     "Rings um die Huette herscht eine wilde Vegetation. Es wachsen verschiedenste\n"
    +"Farne und Buesche und eine vielfalt verschiedenster Baeume. Alles zusammen\n"
    +"bildet eine fast undurchdringliche Vegetation.\n");
   AddDetail(({"licht", "blaetterdach"}),
     "Durch das relativ dichte Blaetterdach der hohen Baeume, faellt eigentlich nur\n"
    +"sehr wenig Licht durch und so ist es hier doch schon relativ duester, auch\n"
    +"wenn man noch nicht unbedingt eine eigene Lichtquelle braucht um etwas sehen\n"
    +"zu koennen.\n");
   AddDetail("lichtquelle",
     "Eine eigene Lichtquelle ist hier nicht unbedingt erforderlich, da dann doch\n"
    +"noch genuegend Licht durch das Blaetterdach durchdringt.\n");
   AddDetail("himmel",
     "Es scheint Dir als wuerde die Sonne gerade scheinen, auch wen das dichte\n"
    +"Blaetterdach die meisten Sonnenstrahlen nicht hindurch laesst.\n");
   AddDetail("boden",
     "Der Boden ist hier eigentlich, mit Ausnahme des Trampelpfads natuerlich,\n"
    +"ziemlich zugewachsen und von zahlreichen Pflanzen ueberwuchert.\n");
   AddDetail("erde",
     "Die Erde ist hier ueberall ein wenig lehmig, aber wirklich besonderes kannst\n"
    +"Du an der Erde nicht entdecken.\n");
   AddDetail(({"trampelpfad", "ausgang", "pfad"}),
     "Der Trampelpfad scheint doch relativ oft benutzt zu werden, denn die Erde ist\n"
    +"fast voellig bloss gelegt und die wenigen Pflanzen die dann doch versuchen ihn\n"
    +"zu bewachsen sind fast alle niedergetrampelt.\n");
   AddDetail(({"lichtung", "norden"}),
     "Vielleicht solltest Du einfach dem Trampelpfad nach Norden folgen und Dir die\n"
    +"Lichtung einfach aus der Naehe anschauen.\n");
   AddDetail(({"buche", "riesige buche"}),
     "Eichen sollst Du weichen, Buchen sollst Du suchen!\n");
   AddDetail(({"eiche", "eichen"}),
     "Du siehst zwar alles moegliche an Baeumen hier, aber Eichen befinden sich zum\n"
    +"Glueck nicht dadrunter.\n");
   AddDetail(({"glueck", "frage"}),
     "Ob es soetwas wie Glueck ueberhaupt gibt, ist wohl eher eine philosophische\n"
    +"Frage und etwas wo man stundenlang drueber diskutieren koennte. Fuer den\n"
    +"Moment beschliesst Du jedenfalls lieber die schoene Natur zu geniessen und\n"
    +"dem zwitschern der Voegel zu lauschen, als Dich mit soetwas zu beschaeftigen.\n");
   AddDetail(({"voegel", "zipfel", "baumzipfel", "zipfel der baeume"}),
     "Oben in den Zipfeln der Baeume kannst Du wage einige Voegel erkennen, doch um\n"
    +"diese naeher spezifizieren zu koennen, sind diese dann doch zu weit weg.\n");
   AddDetail("moment",
     "Geniesse den Moment und lebe Dein Leben!\n");
   AddDetail("ruhe",
     "Die Ruhe wird hier und da lediglich vom zwitschern einiger Voegel unterbrochen.\n");
   AddSpecialDetail(({"huette"}), "det_huette");
   AddSounds(SENSE_DEFAULT,
     "Es ist hier wirklich herrlich ruhig und nur hier und da hoerst Du das\n"
    +"zwitschern einiger Voegel. Natur pur.\n");
   AddSounds(({"voegel", "zwitschern", "voegel zwitschern"}),
     "Du lauschst dem zwitschern der Voegel und geniesst die himmlische Ruhe.\n");
   AddCmd(({"such", "suche"}), "cmd_suchen");
   AddCmd(({"geniess", "geniesse"}), "cmd_geniessen");
   AddCmd(({"leb", "lebe"}), "cmd_leben");
   AddCmd(({"kletter", "klettere"}), "cmd_klettern");
   AddExit("norden", ROOM("lichtungs"));
}

static string det_huette()
// Langbeschreibung nicht fest setzen, damit der Spieler sie selbst
// aendern kann.
{
   object ob;
   if (ob=present("\nmorgeneshaus", ME))
      return ob->long();
}

static int cmd_suchen(string str)
{
   notify_fail("WAS moechtest Du suchen?\n");
   if (str!="buchen" && str!="buche") return 0;
   write("Du brauchst nicht lange suchen und schon entdeckt Du eine riesige Buche vor\n"
        +"Dir, aber eigentlich waere es wohl auch eher erstaunlich, wenn Du diese\n"
        +"riesige Buche direkt vor Deiner Nase uebersehen wuerdest :).\n");
   return 1;
}

static int cmd_geniessen(string str)
{
   notify_fail("WAS moechtest Du geniessen?\n");
   if (str!="moment" && str!="den moment" && str!="ruhe") return 0;
   write("Du haeltst einen Moment inne und geniesst die himmlische Ruhe, die nur durch\n"
        +"das zwitschern einiger Voegel durchbrochen aber nicht gestoert wird.\n");
   return 1;
}

static int cmd_leben(string str)
{
   notify_fail("WAS moechtest Du leben?\n");
   if (str!="leben" && str!="mein leben" && str!="dein leben") return 0;
   write("Nun, wenn die Welt so einfach waere das Du mit einem simplen Kommando Dein\n"
        +"ganzes Leben steuern koenntest, dann waer die Welt doch wirklich langweilig,\n"
        +"oder? Ich will doch stark hoffen, das es sooo weit noch nicht bei Dir ist :).\n");
   return 1;
}

static int cmd_klettern(string str)
{
   notify_fail("WOHIN oder WORAUF moechtest Du klettern?\n");
   if (member(({"auf baum", "auf baumzipfel", "baum hoch",
                "buche hoch", "auf buche"}), str)==-1) return 0;
   write("Du versuchst vergeblich die riesige Buche hochzuklettern, doch die unteren\n"
        +"Aeste haengen leider bereits viel zu hoch, um diese erklimmen zu koennen.\n");
   return 1;
}

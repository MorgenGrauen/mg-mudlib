// Ich fand es ein wenig langweilig in der Gilde, also habe ich ein wenig daran
// gebastelt.
//  Hate.
// 2002-12-06 Mandragons Projektbrett eingebaut, Humni.
// 2003-07-11 Bugfix
// 2005-08-19 Miril Arks Bienen eingebaut
// 2007-08-03 Zesstra, PFad fuers Vereinsbuero angepasst.
inherit  "/std/gilden_room";

#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <language.h>
#include <defines.h>
#include <moving.h>
#include <new_skills.h>
#include <events.h>
#include <player/fao.h>

#define P_ANFAENGER_BIENENORT_ "anfaenger_bienenort_"
#define P_ANFAENGER_BIENENZIEL_ "anfaenger_bienenziel_"

string Funktion(); // Funktion der Gilde
string Schmutz();  // Wohl bald ein Zaubertrank

int _query_light() {return 20;}

void create() {
  ::create();

  SetProp(P_GUILD_DEFAULT_SPELLBOOK,"ab_spells");
  SetProp(P_LIGHT,20);
  SetProp(P_INDOORS,1);
  SetProp(P_DOMAIN, "Ebene");

  SetProp(P_NAME, "Owen");
  SetProp(P_ARTICLE, 0);

  AddSpell("steinschlag");
  AddSpell("licht");
  AddSpell("kampfschrei");
  AddSpell("ausweichen");
  AddSpell("schaetz");
  AddSpell("pfeil");
  AddSpell("identifiziere");
  AddSpell("feuerball");
  AddSpell("schnell");
  AddSpell("saeurestrahl");

  SetProp(P_GUILD_MALE_TITLES, ([
       1 : "der hoffnungsvolle Anfaenger",
       2 : "der Landstreicher",
       3 : "der Streuner",
       4 : "der Wandergeselle",
       5 : "der Waldlaeufer",
       6 : "der Faehrtensucher",
       7 : "der Wegekundige",
       8 : "der Jaeger",
       9 : "der Kundschafter",
      10 : "der Reisende",
      11 : "der Abenteurer",
      12 : "der Weltenbummler",
      13 : "der Draufgaenger",
      14 : "der Schatzsucher",
      15 : "der Feuerbaendiger",
      16 : "der Entdecker",
      17 : "der Eroberer",
      18 : "der Held",
      19 : "der angehende Seher",
      20 : "der Seher",
      ]) );
  SetProp(P_GUILD_FEMALE_TITLES, ([
       1 : "die hoffnungsvolle Anfaengerin",
       2 : "die Landstreicherin",
       3 : "die Streunerin",
       4 : "die Wandergesellin",
       5 : "die Waldlaeuferin",
       6 : "die Faehrtensucherin",
       7 : "die Wegekundige",
       8 : "die Jaegerin",
       9 : "die Kundschafterin",
      10 : "die Reisende",
      11 : "die Abenteurerin",
      12 : "die Weltenbummlerin",
      13 : "die Draufgaengerin",
      14 : "die Schatzsucherin",
      15 : "die Feuerbaendigerin",
      16 : "die Entdeckerin",
      17 : "die Erobererin",
      18 : "die Heldin",
      19 : "die angehende Seherin",
      20 : "die Seherin",
      ]) );

  SetProp(P_INT_SHORT,"Die beruehmte Abenteurergilde");
  SetProp(P_INT_LONG,
 "Der grosse Raum mit seiner niedrigen Decke und den grob geschnittenen\n"
+"Querbalken hat Platz fuer sehr viele Personen. Knarrende Dielen erzaehlen\n"
+"ueber Heldentaten laengst vergessener Abenteurer, beruehmter als mancher\n"
+"Weise unserer Zeit.\n"
+"Abenteurer, aber auch andere Bewohner dieser Welt kommen hierher, um\n"
+"sich zu informieren, ihre Erfahrungen auszutauschen oder sich in den\n"
+"verschiedensten Wissenschaften zu verbessern.\n"
+"Durch die immer offene Tuer scheint die aufgehende Sonne herein und fuellt\n"
+"den Raum mit ihrem goldenen Licht. Sonst gibt es hier nur noch den Ausgang\n"
+"im Norden, welcher auf die Hafenstrasse fuehrt und Bilder an den Waenden,\n"
+"mit Zeichnungen und Gemaelden vom MorgenGrauen. Eins der Bilder erlaeutert\n"
+"die Funktion der Gilde und eine grosse Uhr an der Wand zeigt, was die\n"
//+"Stunde schlaegt. In einer Ecke steht ein kleines Projektbrett.\n"
+"Stunde schlaegt.\n"
+"Nach oben fuehrt eine Treppe in das Buero des Foerdervereins Projekt\n"
+"MorgenGrauen e.V., das Du unbedingt besuchen solltest.\n");

  AddDetail(({"raum", "gilde", "abenteurergilde", "haus", "gildenhaus",
              "gildehaus"}),
  "Du stehst mitten in der Abenteurergilde. Hier trifft man sich und kann\n"
 +"auch seine persoenlichen Werte verbessern.\n");
  AddDetail(({"tuer", "ausgang"}),
  "Der immer offenstehende Ausgang fuehrt hinaus auf die Hafenstrasse.\n"
 +"Goldenes Licht einer ewig aufgehenden Sonne scheint herein.\n");
  AddSpecialDetail(({"bild", "bilder", "zeichnungen", "gemaelde", "karten",
              "landschaftsbilder"}),"det_bild");
  AddDetail(({"funktion", "schild"}), #'Funktion);
  AddDetail(({"wand", "waende"}),
  "Aus fein saeuberlich geschlagenen Steinen sind die Waende der Gilde\n"+
 "zusammengesetzt. Viele interessante Bilder haengen daran.\n");
  AddDetail(({"stein", "steine"}), "Es sind ganz einfache Sandsteine.\n");
  AddDetail(({"fenster"}),
  "Eine wundervolle Welt eroeffnet sich Dir, genannt Realitaet. Aber gibt es\n"
 +"diese wirklich? Ist sie nicht voller Gefahren und Stress?\n");
  AddDetail(({"seite", "suedseite"}),
  "Am Suedende des Raumes ist ein eigenartiges Fenster auszumachen.\n");
  AddDetail(({"balken", "querbalken", "holzbalken", "deckenbalken"}),
  "Diese Balken halten das Obergeschoss des Gildenhauses, wohin nur Magier\n"
 +"kommen und sind an manchen Stellen ganz schoen durchgebogen. Das grob ge-\n"
 +"schnittene Holz ist teilweise von Schwertstreichen zernarbt.\n");
  AddDetail(({"hafen", "hafenstrasse", "strasse"}),
  "Zur Tuer hinaus gelangt man ueber die Hafenstrasse zum Hafen von Port Vain"
 +"\n");
  AddDetail(({"boden", "fussboden", "dielen"}),
  "Die knarrenden Dielen der Gilde sind schon alt und abgeschabt. In den\n"
 +"Ritzen zwischen ihnen ist viel Dreck von den Stiefeln der Abenteurer.\n");
  AddDetail(({"decke"}),
  "Die Decke des Raumes wird von Holzbalken gehalten.\n");
  AddDetail(({"licht", "sonnenlicht", "sonne"}),
  "Warmes Sonnenlicht scheint durch den Ausgang zur Strasse herein. Jedesmal\n"
 +"wenn jemand hereinkommt wird er vom Licht umspuelt und erscheint Gott-\n"
 +"gleich mit dieser Aura.\n");
  AddDetail(({"ebene"}),
  "Du betrachtest das Bild der Ebene und denkst, welche Schaetze in dieser\n"
 +"flachen, gruenen Landschaft verborgen sein muessen.\n");
  AddDetail(({"gebirge", "huegel", "berge", "berg"}),
  "Das Bild der Huegel und Berge betrachtend, schleicht sich ein wenig Sehn-\n"
 +"sucht nach dieser herrlichen, aber gefaehrlichen Landschaft ein.\n");
  AddDetail(({"perspektiven", "perspektive", "darstellung", "landschaft"}),
  "Manche der Landschaftsbilder sind aus der Vogelperspektive oder von\n"
 +"hohen Bergen aus oder von Zwergen gemalt worden.\n");
  AddDetail(({"see", "meer", "wellen"}),
  "Die Wellen auf dem Meer glitzern wie tausend Edelsteine.\n");
  AddDetail(({"spritzer", "lichtspritzer"}),
  "Wie tausend kleine Edelsteine verteilen sich diese winzigen, gluehenden\n"
 +"Lichtspritzer ueber das wundervolle Gemaelde\n");
  AddDetail(({"geschoss", "obergeschoss"}),
  "Ins obere Geschoss der Gilde kommen nur Magier, mittels eines besonderen\n"
 +"Zauberspruchs, der die Decke durchlaessig fuer sie macht.\n");
  AddDetail(({"ritzen", "fugen"}),
  "In den Fugen zwischen den Dielen befindet sich eine Menge Schmutz und\n"
 +"Dreck von den Besuchern der Gilde.\n");
  AddDetail(({"fugen", "dreck", "schmutz"}), #'Schmutz);
  AddDetail(({"holz"}), "Die Dielen und die Deckenbalken sind aus Holz.\n");
  AddDetail(({"kartograph", "kartographen"}),
  "Dieselben laufen hier zwar im Moment nicht herum, aber ihre Karten sehen\n"
 +"ziemlich verwirrend aus mit den ganzen Linien, welche diese Welt "
  "darstellen.\n");
  AddDetail(({"stellen", "stelle"}),
  "Nichts sieht aus wie das andere, und die Ecken sind da keine Ausnahmen.\n");
  AddDetail(({"ecken","ecke"}),
  "In einer der Ecken, gleich neben der Treppe, steht ein kleines braunes\n"
 +"Holzbrett, das sofort Deine Aufmerksamkeit erregt. Wozu es wohl dient?\n");
  AddDetail("aufmerksamkeit",
  "Du beschliesst, Dir das Brett sofort naeher zu betrachten, um sie zu\n"
      +"befriedigen.\n");
/*
  AddDetail(({"brett","holzbrett"}),
  "Ein unscheinbares kleines Brett steht in der Ecke des Raumes. Wenn man\n"
 +"bedenkt, wie viele kleine Zettel daran befestigt wurden, kann es sich\n"
 +"nur um ein schwarzes Brett handeln. Welche Informationen hier wohl\n"
 +"ausgetauscht werden? Du koenntest natuerlich anfangen, die Zettel zu\n"
 +"lesen, um das herauszufinden ...\n");
  AddDetail("zettel",
  "Viele verschiedene Zettel stecken an dem Brett. Rote, gelbe, blaue,\n"
 +"jeder scheint eine andere Farbe zu haben. Auf jedem von ihnen scheint\n"
 +"etwas geschrieben zu sein.\n");
  AddDetail(({"farbe","farben"}),
  "Es sind so viele, Du wuerdest bestimmt jahrelang zaehlen.\n");
  AddDetail("informationen",
  "Wenn Du die Zettel liest, findest Du bestimmt welche.");
*/
  AddDetail(({"wissenschaften", "eigenschaften"}), #'Funktion); //');
  AddDetail(({"narben", "schwertstreich"}),
  "Die Narben in den Holzbalken sind schon recht alt. Es ist lange her, dass\n"
   +"hier richtig gekaempft wurde.\n");
  AddDetail(({"aura"}),
 "Eine Aura von Licht umgibt jeden, der von der Strasse ins Haus kommt.\n");
  AddDetail(({"bewohner", "abenteurer"}),
 "Hier, im Gildenhaus, ist jeder gern willkommen.\n");
  AddDetail(({"weise", "weiser", "recken"}),
  "Die Weisen dieses Landes sind beruehmt, kommen aber nicht an die Be-\n"
  +"ruehmtheit ihrer Vorfahren, die grossen Recken der ersten Jahre, heran.\n");
  AddDetail(({"realitaet"}), "Was ist schon real?\n");
  AddSpecialDetail("uhr","zeige_reboot");


  AddDetail(({"buero","foerderverein"}),
    "Wichtige Informationen warten auf Dich im Buero des Foerdervereins!\n");
  AddDetail("informationen", "Du musst schon ins Buero gehen, wenn Du "
      "die Informationen erhalten willst.\n");
  AddDetail("treppe","Die Treppe fuehrt nach Oben in das Buero des "
      "Foerdervereins Projekt MorgenGrauen e.V.\n");
  AddExit("norden", "/d/ebene/room/PortVain/po_haf1");
  AddExit("oben","/p/verein/room/buero");

  AddCmd("gehe|geh&\ngehtnicht",0,"Um Dich hier fortzubewegen, "
      "solltest Du direkt das Kommando der Richtung eingeben,"
      " wie ,,norden'' oder ,,suedwesten''. Du kannst das auch "
      "abkuerzen, durch ,,n'' oder ,,sw''.");

  AddItem("/obj/zeitungsautomat", REFRESH_REMOVE);

  AddSpecialDetail("karte","karte");
  AddSpecialDetail("brett","brett_fun");

  EVENTD->RegisterEvent(EVT_LIB_ADVANCE, "handle_advance", this_object());

  // nur auf dem Mudrechner laden (Stichwort: oeffentlich Mudlib)
#if MUDHOST == __HOST_NAME__
  AddItem("/p/service/troy/obj/buchautomat", REFRESH_REMOVE);
//  AddItem("/p/service/mandragon/projektbrett/board", REFRESH_REMOVE);
  // Die Biene fuer den Transport zur Wonnewichtlichtung.
  // Ark, 19.08.05
  AddItem("/d/anfaenger/ark/mon/mantisbiene",REFRESH_DESTRUCT,
   ([P_ANFAENGER_BIENENORT_:"Mensch",
     P_ANFAENGER_BIENENZIEL_:({"/d/anfaenger/ark/rooms/wald16",
                               "zum Wald der Wonnewichte",
                               "/gilden/abenteurer"})]));
#endif
}

string det_bild()
{
  return
   "Die Zeichnung und Gemaelde stellen MorgenGrauen aus den verschiedensten\n"
   "Perspektiven dar. Einige davon zeigen abstrakte Darstellungen, wie sie\n"
   "gerne von Kartographen gemacht werden, und andere zeigen wunderschoene\n"
   "Darstellungen von Landschaften, wie dem Gebirge oder der Ebene. Besonders\n"
   "schoen ist ein Bild der See. Tausende von Lichtspritzern verteilen sich\n"
   "ueber die Wellen.\n"
   "Ein kleines Schild zeigt folgendes:\n"+Funktion();
}

string brett_fun()
{
  if (present("party\nbrett",this_object()))
    return "Welches Brett meinst Du denn? Das Partybrett oder das Holzbrett?\n";
  return GetDetail("holzbrett");
}

int beitreten()
{
  if (::beitreten()<1)
    return 1; // Meldung vom Gildenmaster oder Gildenobjekt
  write("Du bist jetzt ein Abenteurer.\n");
  adjust_title(PL);
  return 1;
}

varargs int austreten(int loss)
{
  if (::austreten()<1)
    return 1; // Meldung kommt vom Gildenmaster
  write("Du bist nun kein Abenteurer mehr.\n");
  return 1;
}

string Funktion()
{   string re;
  re = ("Moegliche Befehle sind:\n"
        +"                liste\n"
        +"                erhoehe (stufe)\n"
        +"                kosten\n"
        +"                lerne\n");
  if (this_player()->QueryProp(P_GUILD)!="abenteurer")
  re += ("                trete in gilde ein\n"+
         "                trete aus gilde aus\n");
  return re;
}

string Schmutz()
{
  // hier sollte sich doch glatt ein Zaubertrank lohnen!!
  return "In den Fugen befindet sich viel Schmutz.\n";
}

// Fuer Spielerstufenaufstiege gerufen - egal in welcher Gilde.
public void handle_advance(string eid, object trigob, mixed data) {

  object pl = data[E_OBJECT];
  if (!pl || !IsGuildMember(pl) || !interactive(pl))
    return;

  int lvl=pl->QueryProp(P_LEVEL);
  // nicht fuer Level 1, da ist genug scroll...
  if (lvl == 1) return;

  // Alle Spells raussuchen, die der Spieler lernen kann.
  string *spells=filter(m_indices(Query(P_GUILD_SKILLS,F_VALUE)), 
      function int (string sp)
      {
        mapping ski = QuerySpell(sp);
        if (!ski) return 0; // kein Spell
        if (pl->QuerySkillAbility(sp) > 0
            || check_restrictions(pl, ski[SI_SKILLRESTR_LEARN]))
          return 0; // Vorraussetzungen nicht erfuellt oder schon gelernt.
        return 1; // offenbar ist sp lernbar.
      }
      );

  int count=sizeof(spells);
  if (!count)
    return;
  
  if (count == 1) {
    tell_object(pl, break_string("Dir faellt auf einmal auf, dass Du "
       "versuchen koenntest, den Zauberspruch \"" + spells[0] 
       + "\" in der Abenteurergilde zu erlernen.",78));
  }
  else {
     tell_object(pl, break_string("Dir faellt auf einmal auf, dass Du "
       "versuchen koenntest, die folgenden Zaubersprueche in der "
       "Abenteurergilde zu erlernen:",78));
     tell_object(pl, CountUp(spells) + "\n");
  }
}

// wird aus Spielerobjekt, /std/gilde.c und Gildenmaster gerufen (mindestens)
// aktualisiert den Gildenlevel auf den Spielerlevel und setzt den per
// get_new_title() aus den Gildentiteln ermittelten Gildentitel.
void adjust_title(object pl) {
  int lv;

  // bei P_LEVEL < 0 ist der Spieler noch "nicht fertig".
  // Der Titel wird anschliessend noch korrekt gesetzt.
  if (!pl || !IsGuildMember(pl) || (lv=pl->QueryProp(P_LEVEL)) < 0)
    return;

  pl->SetProp(P_GUILD_LEVEL,lv);
  if (!lv) lv=1;
  pl->SetProp(P_GUILD_TITLE, get_new_title(lv,pl));
  // Spielertitel nullen, damit der Gildentitel angezeigt wird.
  if (!IS_SEER(pl) && !FAO_HAS_TITLE_GIFT(pl))
    pl->SetProp(P_TITLE,0);
}

string karte() {
  if (this_player() && present("karte",this_player())) return 0;
  return GetDetail("karten");
}

public varargs void init(object origin) {
  int res;
  object ob;

  ::init(origin);

  // Von Ark, an alle Rassenstartpunkte
  if (ob=present("\nentdecker_obj",this_player()))
      ob->Check_And_Update(this_player(),"Menschen");
}


// MorgenGrauen MUDlib
//
// gilde.c -- Standardgilde
//
// $Id: gilde.c 8388 2013-02-16 17:28:31Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/room";

#include <properties.h>
#include <defines.h>
#include <rooms.h>
#include <wizlevels.h>
#include <language.h>
#include <new_skills.h>
#include <exploration.h>
#include <ansi.h>
#include "/secure/questmaster.h"
#include "/secure/lepmaster.h"
#include "/secure/config.h"
#include <events.h>

#define kosten_0 \
  ([0:                "Bis Du Dich das naechste Mal hier blicken laesst, solltest Du eine Menge Stufenpunkte sammeln. Sonst wird das nix!",\
    "zauberer":       "Jetzt fang hier ja nicht an zu jammern. Du hast doch grade erst erhoeht. Fuer die naechste Stufe musst Du erstmal Leistung zeigen!",\
    "kaempfer":       "Hat grade erst erhoeht und faengt schon an zu jammern wie ein altersschwacher Zauberer. Nimm gefaelligst Haltung an!",\
    "klerus":         "Wer hat Dir denn das Gehirn gelaeutert? Zeig erstmal was von Deinem Koennen, bevor Du schon wieder erhoehen willst!",\
    "chaos":          "So chaotisch kannst Du doch nicht sein zu glauben, dass Du schon wieder erhoehen kannst!",\
    "karate":         "Ruh Deine Knochen erstmal ein bisschen aus. Bis zur naechsten Stufe werden sie noch genug leiden muessen!",\
    "katzenkrieger":  "Da hilft kein Maunzen und kein Schnurren. Du hast grade erst erhoeht und dabei bleibt das!",\
    "tanjian":        "Jetzt schrei nicht gleich nach Siamil. Du musst noch eine Menge leisten bis zur naechsten Stufe!",\
    "bierschuettler": "Du hast das Bier auf Deinen letzten Erfolg noch nicht einmal ausgetrunken. Werd jetzt ja nicht groessenwahnsinnig!",\
    "werwoelfe" :     "Und wenn Du noch so grimmig guckst und den Mond anheulst. Du hast gerade erst erhoeht. Nun musst Du erstmal wieder etwas tun!",\
  ])
#define kosten_20 \
  ([0:                "Da liegt aber noch ein weiter Weg vor Dir. Aber Du schaffst es ganz sicher!",\
    "zauberer":       "Du kannst zwar zaubern, aber nicht hexen. Fuer die naechste Stufe musst Du schon noch einiges tun!",\
    "kaempfer":       "Oh weh, da hast Du aber noch ne Menge vor Dir. Beiss die Zaehne zusammen und los gehts!",\
    "klerus":         "Du wirst bestimmt noch einige Heiltraenke brauchen bis zur naechsten Stufe!",\
    "chaos":          "Hast Du versucht Dein Chaos zu beseitigen, oder was hast Du gemacht? Sehr weit gekommen biste jedenfalls noch nicht. Musst Dich schon ein bisschen anstrengen!",\
    "karate":         "Ganz schoen anstrengend immer nur mit Hand und Fuss zu metzeln, oder? Aber es hat Dich schon etwas naeher an die naechste Stufe rangebracht!",\
    "katzenkrieger":  "Bisschen weniger Fellpflege betreiben, und ein bisschen mehr Stufenpunkte sammeln. Du hast naemlich noch ein gutes Stueck vor Dir!",\
    "tanjian":        "Du hast noch ne Menge zu tun bis zur naechsten Stufe. Sterb nicht so oft, aber sowas tun Tanjian ja eh nicht!",\
    "bierschuettler": "Bier trinken alleine wird Dich nicht weiter bringen, obwohl Du schon ein Stueck weit gekommen bist!",\
    "werwoelfe" :     "Staerke Deine Willenskraft. Du hast noch einiges vor Dir!",\
  ])
#define kosten_40 \
  ([0:                "Also die Haelfte hast Du schon ungefaehr geschafft. Aber ruh Dich jetzt ja nicht darauf aus. Sieh mal zu, dass Du die zweite Haelfte zur naechsten Stufe auch noch schaffst. Auf einem Bein kann man schliesslich nicht lange stehn!",\
    "zauberer":       "Teile Llystrathe mit Hilfe. Och schade scheint nicht zu funktionieren, dann musst Du eben selber noch ne Menge Stufenpunkte sammeln!",\
    "kaempfer":       "Kaempfer schnell an die Waffen! Die Haelfte hast Du ungefaehr geschafft, aber es gibt noch viel zu tun!",\
    "klerus":         "Beile Dich! Das Boese sitzt Dir im Nacken und Du hast noch ungefaehr die Haelfte vor Dir!",\
    "chaos":          "Schnapp Dir Deinen Daemon und teil Dir die zweite Haelfte der Arbeit mit ihm!",\
    "karate":         "Du scheinst eingesehen zu haben, dass hirnloses Draufhauen nix nutzt. Die Haelfte haste ungefaehr schon geschafft!",\
    "katzenkrieger":  "So ungefaehr bis zur Haelfte hast Du Dich schon an die neue Stufe herangeschlichen!",\
    "tanjian":        "Man kann den Wert zwischen den beiden Stufen durchaus als ausgewogen bezeichnen. Trotzdem hast Du noch ein Stueck des Weges vor Dir!",\
    "bierschuettler": "War das Fass nu halb voll oder halb leer? Jedenfalls biste ungefaehr bei der Haelfte angekommen!",\
    "werwoelfe" :     "Noch einige Gegner werden sich vor Dir fuerchten muessen, obwohl Du schon viel geschafft hast!",\
  ])
#define kosten_60 \
  ([0:                "Also ein bisschen was musst Du noch tun, aber es sieht schon ziemlich gut aus!",\
    "zauberer":       "Ein bisschen Wille und Magie und Du schaffst es wieder ein Stueck naeher an die neue Stufe heranzukommen!",\
    "kaempfer":       "Staerke Deinen Kampfwillen. Die groessten Anstrengungen hast Du schon hinter Dir!",\
    "klerus":         "So nah am Ziel wirst Du doch nicht aufgeben wollen. Hol Dir noch ein bisschen goettliche Unterstuetzung und weiter gehts!",\
    "chaos":          "Chaotisch sein alleine hilft nicht. Ein bisschen was musst Du schon noch tun!",\
    "karate":         "Die Handkanten geschaerft, den Gi fest zugebunden. Bald hast Du es geschafft!",\
    "katzenkrieger":  "Auf Samtpfoten schleichst Du mit grossen Schritten ans Ziel heran. Aber ein bisschen fehlt noch!",\
    "tanjian":        "Eine Quest hier, ein paar Monster da. Aber immer schoen ausgeglichen bleiben, dann klappts auch weiterhin!",\
    "bierschuettler": "Noch ein paar kraeftige Schlucke und Du hast es fast geschafft!",\
    "werwoelfe" :     "Verlass Dein Rudel und zieh hinaus ins Abenteuer. Dann wirst Du bald siegreich heimkehren!",\
  ])
#define kosten_80 \
  ([0:                "Huch na sowas, Du hast die naechste Stufe ja schon fast erreicht. Na also! Der Rest ist ja wirklich nur noch Kleinkram!",\
    "zauberer":       "Die paar fehlenden Puenktchen haste Dir doch rasch zusammengezaubert!",\
    "kaempfer":       "Das fehlende bisschen machst Du als gnadenlos guter Kaempfer doch mit Links. Jetzt halt Dich aber mal ran. Oder willst Du Dich auf dem letzten Stueck noch von so einem luschigen Zauberer ueberrunden lassen?",\
    "klerus":         "Bei Saphina, Kandri und Lembold, Du bist fast bei der naechsten Stufe angekommen!",\
    "chaos":          "Boese waehrt am laengsten. Fast hast Du es geschafft!",\
    "karate":         "So viele blaue Flecken wie Du an Armen und Beinen hast, kann die naechste Stufe nicht mehr weit sein!",\
    "katzenkrieger":  "Pass bloss auf, dass Dich auf den letzten Zentimetern nicht noch ein Hund erwischt!",\
    "tanjian":        "Siamil wird stolz auf Dich sein. Du hast Dich gegen Gut und Boese durchgesetzt und das Ziel fast erreicht!",\
    "bierschuettler": "Lange musst Du nicht mehr warten. Kannst ruhig schon mal ein frisches Bier in Auftrag geben!",\
    "werwoelfe" :     "Noch ein letztes Mal die fuerchterlichen Krallen schaerfen, dann hast Du Deinen neuen Level erreicht!",\
  ])

void create()
{
  if (object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);
      return;
  }
  room::create();

  AddCmd("kosten","kosten");
  AddCmd("liste","liste");
  AddCmd("erhoehe","advance");
  AddCmd(({"treff"}), "GotoMagierTreff");
 }

void init()
{
  int lvl;
  room::init();

  if (PL && query_once_interactive(PL)
      && (lvl=PL->QueryProp(P_LEVEL)) <= 6
      && LEPMASTER->QueryLevel(PL->QueryProp(P_LEP)) > lvl)
  {
    tell_object(PL,
      "\nDu koenntest Deine Stufe mit \"erhoehe spieler\" hier in der Gilde "
      "erhoehen.\n\n");
  }
}

string zeige_reboot()
{
  string str;
  int t,t2;

  t=time()-last_reboot_time();
  t2=t;
  str="Seit dem letzten Shutdown sind "+t+" Sekunden vergangen.\n";
  if (t<60) return str;
  str+="Das sind ";
  if (t>=86400)
  {
    str+=t/86400+" Tag";
    if (t>=86400*2)
      str+="e";
    str+=", ";
    t2=t2%86400;
  }
  if (t>=3600)
  {
    str+=t2/3600+" Stunde";
    if (t2/3600!=1)
      str+="n";
    str+=", ";
    t2=t2%3600;
  }
  if (t>=60)
  {
    str+=t2/60+" Minute";
    if (t2/60!=1)
      str+="n";
    str+=" und ";
    t2=t2%60;
  }
  str+=t2+" Sekunde";
  if (t2!=1)
    str+="n";
  str+=".\n";
  return str;
}

int seer_cond(int silent)
{
  int cond;

  cond=LEPMASTER->QueryReadyForWiz(this_player());

  if (!silent)
    write(break_string(LEPMASTER->QueryReadyForWizText(this_player()),
          78, 0, 1));

  return cond;
}

varargs int kosten(string str)
{
  string tmp;

  int lep = PL->QueryProp(P_LEP);
  int lvl = PL->QueryProp(P_LEVEL);
  int diff = LEPMASTER->QueryNextLevelLEP(lvl, lep);

  switch ( diff ) {
    case 101..__INT_MAX__:
      // Falls mal LEPs abhandengekommen sind...
      tmp = "Wie bist Du ueberhaupt an Deinen Level gekommen?\n"
      "Sei froh, dass Du nicht wieder abgestuft wirst.";
      break;

    case 81..100:
      tmp=kosten_0[PL->QueryProp(P_GUILD)] || kosten_0[0];
      break;

    case 61..80:
      tmp=kosten_20[PL->QueryProp(P_GUILD)] || kosten_20[0];
      break;

    case 41..60:
      tmp=kosten_40[PL->QueryProp(P_GUILD)] || kosten_40[0];
      break;

    case 21..40:
      tmp=kosten_60[PL->QueryProp(P_GUILD)] || kosten_60[0];
      break;

    case 1..20:
      tmp=kosten_80[PL->QueryProp(P_GUILD)] || kosten_80[0];
      break;

    default:
      if ( lvl < 9 )
        tmp = "Probier mal den Befehl 'erhoehe'.";
      else
        tmp = "Den Befehl 'erhoehe' kennst Du aber, ja?";
      break;
  }

  write( break_string( tmp, 78, 0, BS_LEAVE_MY_LFS ) );

  if (!IS_SEER(this_player()) 
      && ( (str == "lang") ||
        (this_player()->QueryProp(P_LEVEL) > 12 && str != "kurz"))) {
    seer_cond(0);
    write (break_string("\nMit 'kosten kurz' kannst Du die Angabe der "
          "Seher-Anforderungen unterdruecken.", 78,0,1));
  }

  return 1;
}

// ermittelt Gildentitel zum Level <lev>.
// Der letzte verfuegbare Gildentitel wird nur fuer Seher vergeben.
// TODO: mit dem entsprechenden Code aus gilden_ob.c vereinigen?
string get_new_title(int lev, object pl)
{
  mapping titles;

  if (!pl) return 0;

  if (lev<0) lev=0;

  if (pl->QueryProp(P_GENDER) == MALE)
    titles=(mapping)QueryProp(P_GUILD_MALE_TITLES);
  else
    titles=(mapping)QueryProp(P_GUILD_FEMALE_TITLES);

  if (!mappingp(titles) || !sizeof(titles)) return 0;

  int maxlevel = max(m_indices(titles));

  // Level begrenzen. Max-Level fuer Seher.
  if (lev >= maxlevel)
    lev = IS_SEER(pl) ? maxlevel : maxlevel-1;

  return titles[lev];
}

// versucht das Spielerlevel zu erhoehen. Macht nicht den ganzen Krams
// drumrum, den advance() aus hysterischen Gruenden tut.
int try_player_advance(object pl) {

  if (PL->QueryProp(P_KILLS)>0)
    return -1;

  int level = pl->QueryProp( P_LEVEL );
  if (level == -1) level = 0;

  if (LEPMASTER->QueryNextLevelLEP(level, pl->QueryProp(P_LEP)) > 0)
      return 0;
  else
      ++level;

  pl->SetProp( P_LEVEL, level );

  // Aufstiegs-Event ausloesen
  EVENTD->TriggerEvent(EVT_LIB_ADVANCE, ([
        E_OBJECT: PL, E_PLNAME: getuid(PL),
        E_ENVIRONMENT: environment(PL),
        E_GUILDNAME: PL->QueryProp(P_GUILD),
        P_LEVEL: PL->QueryProp(P_LEVEL),
        ]) );

  // Falls die konkrete Gilde des Spielern irgedwas mit dem Titel in
  // ABhaengigkeit des Spielerlevels tun will. Ausnahmsweise per call_other,
  // die Funktion kommt eigentlich aus /std/gilden_ob.c.
  string gname=(string)pl->QueryProp(P_GUILD);
  (GUILD_DIR+"/"+gname)->adjust_title(pl);

  return 1;
}

// "erhoehe"-Kommando.
// erhoeht das Spielerlevel, falls moeglich. Setzt den passenden
// Abenteurergildentitel.
int advance(string arg)
{

  if (arg &&
      arg != "stufe" && arg != "spielerstufe" && arg != "spieler"
      && arg != "spielerlevel")
    return 0;

  int res = try_player_advance(PL);

  if (!res)
    return kosten("kurz");
  else if (res < 0) {
    notify_fail(break_string(
        "Du hast einen Mitspieler umgebracht!\n"+
        "In diesem Fall kannst Du Deine Stufe nicht erhoehen.\n"+
        "Bitte wende Dich an den Sheriff (oder einen Erzmagier) und bring "
        "das in Ordnung.\n",78,BS_LEAVE_MY_LFS));
    say(break_string(PL->Name(WER) 
          + " hat soeben auf schmerzliche Weise erfahren muessen, dass "
          "es wirklich nicht foerderlich ist, Mitspieler umzubringen.\n",
          78), PL);
    return 0;
  }

  string name_of_player = PL->name(WER);
  int level = PL->QueryProp(P_LEVEL);
  say( name_of_player + " hat jetzt Stufe " + level + " erreicht.\n");

  string title = PL->QueryProp(P_TITLE);

  switch(random(3)) {
    case 0:
      write("Du bist jetzt " + name_of_player + " " + title +
        " (Stufe " + level + ").\n");
      break;

    case 1:
      write("Gut gemacht, " + name_of_player + " " + title +
        " (Stufe " + level + ").\n");
      break;

    default:
      write("Willkommen auf Deiner neuen Stufe.\n" +
        "Du bist jetzt " + name_of_player+" "+ title +
        " (Stufe " + level + ").\n");
      break;
  }

  return 1;
}

// Spielerkommando fuer Abenteurerliste
varargs int liste(mixed pl)
{
  string str; 
  if (!this_player()) return 0;

  if(!objectp(pl))
    if(stringp(pl))
      pl=find_player(pl)||find_netdead(pl);
  if(!objectp(pl))
    pl=PL;
  if (pl != this_player()) {
    write ("Du kannst Dir nur Deine eigenen Abenteuer ansehen.\n");
    return 1;
  }

  str = QM->liste(pl);

  this_player()->More( str, 0 );
  return 1;
}

int GotoMagierTreff()
{
  if(IS_LEARNER(this_player()))
  {
    write("Ein Zauberspruch zieht vor Deinem geistigen Auge vorbei und Du\n"
    "sprichst ihn nach.\n");
    say(PL->name()+" murmelt einen geheimen Zauberspruch und schwebt langsam\n"
    "zur Decke hinauf und verschwindet durch die Wand.\n");
    write("Du schwebst langsam zur Decke hinauf und als ob diese nicht da\n"
    "waere mitten hindurch in den Magiertreff.\n");
    return (PL->move("/secure/merlin", M_TPORT | M_SILENT ) >= 0);
  }
  write("Du springst zur Decke hinauf und nix passiert.\n");
  return 1;
}


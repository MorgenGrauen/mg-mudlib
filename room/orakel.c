// MorgenGrauen MUDlib
//
// orakel.c -- Der Raum, in dem man sich neue ZT-Sprueche holen kann
//
// $Id: orakel.c 9371 2015-10-22 19:01:48Z Zesstra $

#define TIPS(x) "/secure/ARCH/ZT/"+x

#define POTIONMASTER "/secure/potionmaster"

inherit "/std/room";

#include <properties.h>

// WANNWIEVIEL zeigt an, mit wieviel Stufenpunkten man wieviele Zaubertraenke
// haben kann, so ist zum Beispiel an fuenfter Stelle die Zahl 25 einge-
// tragen, also kann man mit 25 Stufenpunkten 5 Zaubertraenke bekommen.
#define WANNWIEVIEL ({ 0, 0, 0, 0,\
   25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375,\
   400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725,\
   750,\
   790, 830, 870, 910, 950, 990, 1030, 1070, 1110, 1150, 1190, 1230, 1270,\
   1310, 1350, 1390, 1430, 1470, 1510, 1550, 1590, 1630, 1670, 1710, 1750,\
   1790,\
   1860, 1930, 2000, 2070, 2140, 2210, 2280, 2350, 2420, 2490,\
   2590, 2690, 2790, 2890, 2990, 3090, 3190, 3290, 3390, 3490 })
#define KEINTIPTEXT "Der Nebel ueber der Kugel ruehrt sich nicht.\n"



protected void create()
{
   ::create();
   SetProp(P_INDOORS, 1);
   SetProp(P_LIGHT, 1);
   SetProp(P_INT_SHORT, "Das heilige Orakel von Tingan");
   SetProp(P_INT_LONG,
   "Du befindest Dich in einer mittelgrossen Hoehle, die sich halbkugel-\n"+
   "foermig ueber Deinem Kopf erhebt. Die Hoehle wird fast komplett vom\n"+
   "gruenlich schimmernden Quellsee des Osterbachs ausgefuellt. Ueber dem\n"+
   "See schwebt eingehuellt in langsam ziehende Nebelschwaden eine magisch\n"+
   "leuchtende Kugel, das heilige Orakel von Tingan. Im Osten fliesst das\n"+
   "Wasser des Sees durch einen schmalen Gang ab, dem Du folgen koenntest.\n");
   AddDetail("boden", "Der Boden ist groesstenteils mit Wasser bedeckt.\n");
   AddDetail("decke", "Die Decke ist aus massivem Felsgestein, Du willst gar nicht wissen,\nwieviele Tonnen davon ueber Deinem Kopf lasten.\n");
   AddDetail(({"fels", "gestein", "felsgestein"}), "Sowas ist im Gebirge recht haeufig.\n");
   AddDetail(({"wand", "waende"}), "Die Waende sind feucht.\n");
   AddDetail(({"quellsee", "see"}), "Er ist die Quelle des Osterbachs.\n");
   AddDetail("quelle", "Das heisst, der Bach entspringt hier.\n");
   AddDetail(({"bach", "osterbach"}), "Er fliesst nach Osten ab.\n");
   AddDetail("wasser", "Es ist klar und frisch.\n");
   AddDetail("gang", "Er fuehrt nach Osten.\n");
   AddDetail("orakel", "Du fuehlst grosses Wissen in Deiner Umgebung.\n");
   AddDetail("umgebung", "Die Umgebung ist das, was Dich umgibt.\n");
   AddDetail("wissen", "Vielleicht nuetzt es Dir ja was.\n");
   AddDetail(({"nebel", "schwaden", "nebelschwaden"}), "Sie ziehen langsam ueber den See.\n");
   AddDetail("kugel", "Die Kugel leuchtet in magischem Licht und laedt Dich fast zum Meditieren ein.\n");
   AddDetail("licht", "Magisches Licht erhellt den Raum.\n");
   AddDetail("raum", "Schau Dich einfach mal um.\n");
   AddDetail("hoehle", "Du befindest Dich tatsaechlich in einer Hoehle.\n");
   AddDetail("kopf", "Das anzuschauen duerfte Dir schwer fallen.\n");
   AddCmd(({"trink", "trinke"}), "trinken");
   AddCmd("meditiere", "meditieren");
   AddCmd(({"wirf", "werfe"}), "werfen");
   AddExit("osten", "/players/boing/orakel/organg2");
}

// Es wird berechnet ob der Spieler einen neuen Tip bekommen kann.
int NeuerTip()
{
   mapping platt;   // Attributmapping des Spielers
   int	   summe;   // Summe der Attribute des Spielers + der noch zu findenen
		    // aber bekannten Zaubertrankstellen
   int	   sps;     // Stufenpunkte des Spielers

   platt  = this_player()->QueryProp(P_ATTRIBUTES);
   summe  = (int)platt["int"] + (int)platt["con"] + 
	    (int)platt["dex"] + (int)platt["str"] - 4; 
   summe  += sizeof(this_player()->QueryProp(P_KNOWN_POTIONROOMS));
   sps	  = this_player()->QueryProp(P_LEP);

// Wenn genug Stufenpunkte da sind, dann wird 1 zurueckgegeben, sonst 0
  if (summe>79)
    return 0;
  if (sps > WANNWIEVIEL[summe])
    return 1;
   return 0;
}

// Gibt die Nummer des ausgewaehlten Zaubertranks zurueck oder 0, wenn keiner
// mehr uebrig ist.
int ZTAuswahl()
{
   int* pllist;
   int	pllsize;
   int i;
   mixed sorted_list; // ({ ({liste1}), ({liste2}), ..., ({liste8}) })

// Initialisierung
   sorted_list = ({ ({ }), ({ }), ({ }), ({ }), ({ }), ({ }), ({ }), ({ }) });
   pllist      = this_player()->QueryProp(P_POTIONROOMS);
   pllsize     = sizeof(pllist);

// Zaubertraenke werden gemaess ihrer Liste in den neuen Array eingetragen.
   for (i=0; i<pllsize; i++)
      sorted_list[POTIONMASTER->GetListByNumber(pllist[i])] += ({ pllist[i] });

// Alle Unterarrays werden aneinandergehaengt.
   pllist = ({ });
   for (i=0; i<8; i++)
      pllist += sorted_list[i];
   pllist -= this_player()->QueryProp(P_KNOWN_POTIONROOMS);

// Jetzt sind alle Zaubertraenke wieder in der Liste, sortiert nach dem
// Schwierigkeitsgrad, bis auf die Zaubertraenke, die der Spieler bereits
// kennt (d.h. wo er schon den Tip bekommen hat, sie aber noch nicht
// gefunden hat).


// Einer der leichtesten Zaubertraenke wird ausgewaehlt und zurueckgegeben.
   pllsize = sizeof(pllist);
   if (!pllsize)
      return -1;
   if (pllsize>10)
      return pllist[random(10)];
   else
      return pllist[random(pllsize)];
}


string Vision()
{
  int	  nr;
  string* text;
  int*	  liste;

  if (!NeuerTip())
    return KEINTIPTEXT + "Du hast wahrscheinlich zu wenig Stufenpunkte.\n";

  nr = ZTAuswahl();
  if (nr == -1)
    return KEINTIPTEXT + "Du hast schon alle Zaubertraenke gefunden.\n";

  text = POTIONMASTER->TipLesen(nr);
// Das Folgende kann passieren, wenn entweder die Datei zu einem ZT nicht
// existiert oder das Einlesen sonstwie fehlschlaegt. Zur Behebung des
// Fehlers sollte man sich als erstes die Datei <nr>.zt anschauen.
  if (intp(text) || !sizeof(text)) 
    return 
  "Ploetzlich wird es totenstill im Raum, die Nebelschwaden beginnen sich\n"+
  "um die Kugel zu drehen. Immer schneller wird der wirbelnde Tanz, bis\n"+
  "sich das Ganze unmerklich zu einer Schrift formt:\n"+
  "   Fehler im Raum-Zeit-Gefuege, bitte wende Dich an Rikus\n"+
  "   und melde ihm die Zahl '"+nr+"'!\n"+
  "Dann zerfliesst der Nebel und alles ist beim Alten.\n";

  liste = this_player()->QueryProp(P_KNOWN_POTIONROOMS);
  this_player()->AddKnownPotion(nr);

//bis nach Reboot noetig!!! Rikus
//  "/players/boing/orakel/orakel"->AddKnownPotion(nr);

  return "Der Nebel ueber der Kugel beginnt sich zu bewegen, dann hoerst Du eine\ntiefe Stimme in Deinem Kopf erklingen:\n\n"+text[random(sizeof(text))];
}

string TipListe()
{
  int*	  pll;
  string* tip;
  string  ret;
  int	  plsize, i;

  ret	 = "";
  pll	 = this_interactive()->QueryProp(P_KNOWN_POTIONROOMS);
  plsize = sizeof(pll);

  for (i=0; i<plsize; i++)
  {
    tip = POTIONMASTER->TipLesen(pll[i]);
    if (sizeof(tip))
    {
      ret += tip[random(sizeof(tip))];
      ret += "--------------------------------------------------------------------------\n";
    }
  }

  if (ret=="")
    return "Du kennst momentan keine Tips zu Zaubertraenken.\n";
  else
    return "--------------------------------------------------------------------------\n"+ret;
}

int trinken(string str)
{
  notify_fail("Was moechtest Du trinken?\n");
  if (!str || str=="") return 0;
  notify_fail("Das kannst Du hier nicht trinken!\n");
  if (str!="wasser")
    return 0;
  write("Du trinkst einen Schluck Quellwasser und fuehlst Dich koestlich erfrischt.\n");
  return 1;
}

meditieren()
{
  write(
  "Du sammelst Deine geistigen Kraefte und verfaellst schon schnell in einen\n"+
  "tranceartigen Zustand. Eine Vision eines Helden, der eine Muenze in den\n"+
  "See wirft erscheint vor Deinen Augen, verschwindet aber schon schnell\n"+
  "wieder. Als Du wieder aufwachst, ist alles wie vorher.\n");
  return 1;
}

int werfen(string str)
{
  if (str=="muenze in see" || str=="muenze in wasser" ||
      str=="muenze in quellsee")
  {
    if (this_player()->QueryMoney()<1)
    {
      write("Du hast keine Muenze.\n");
      return 1;
    }
    this_player()->AddMoney(-1);
    write("Du wirfst eine Muenze ins Wasser.\n");
    say(capitalize(this_player()->name())+" wirft eine Muenze ins Wasser.\n");
    write(Vision());
    return 1;
  }
  return 0;
}

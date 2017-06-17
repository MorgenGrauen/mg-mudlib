//
//  haus.c  -- Das Rohgeruest eines Seherhauses.
//
//  Grundobjekt (c) 1994 Boing@MorgenGrauen
//  Abschliessen und Rauswerfen von Jof
//  Fuer Aenderungen ab dem 06.10.94 verantwortlich: Wargon
//
// $Date: 1996/02/21 18:09:55 $
// $Revision: 2.8 $
/* $Log: haus.c,v $
 * Revision 2.8  1996/02/21  18:09:55  Wargon
 * SmartLog() fuer Rueckmeldungen eingebaut.
 *
 * Revision 2.7  1995/07/06  11:18:07  Wargon
 * Langbeschreibung aenderbar.
 *
 * Revision 2.6  1995/04/21  10:48:20  Wargon
 * Bugfix in "beschreibe tuerstatus".
 *
 * Revision 2.5  1995/04/21  08:56:55  Wargon
 * Haustuer ausgelagert.
 * 'beschreibe tuerzustand' ist dazugekommen.
 *
 * Revision 2.4  1995/02/27  20:47:32  Wargon
 * Diverse Bugs gefixed...
 *
 * Revision 2.2  1995/02/15  11:19:17  Wargon
 * Die Haustuer kann jetzt auch beschrieben werden (H_DOOR).
 *
 * Revision 2.1  1995/02/04  14:51:14  Wargon
 * Statt "Haus" wird weitestgehend name() verwendet.
 *
 * Revision 2.0  1995/02/01  20:40:26  Wargon
 * Das Haus ist jetzt nur noch ein Objekt in einem Raum, nicht
 * mehr ein Raum im Raum.
 *
 */
#include "haus.h"
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>

inherit "/std/thing";
inherit HAUSTUER;

private nosave string text;
private nosave string owner;

void create()
{
  if (!clonep(this_object())) return;

  thing::create();
  haustuer::create();

  SetProp(P_NAME, ({ "Haus", "Hauses", "Haus", "Haus" }));
  SetProp(P_IDS, ({"haus","sehe\rhaus"}));
  SetProp(P_GENDER, NEUTER);
  SetProp(P_NOGET,1);

  Set(P_LONG, SAVE, F_MODE);
  Set(P_SHORT, SAVE, F_MODE);
  Set(P_IDS, SAVE, F_MODE);
  Set(P_GENDER, SAVE, F_MODE);
  Set(P_NAME, SAVE, F_MODE);

  text = 0;

  AddCmd( ({"betritt", "betrete"}), "betritt" );
  AddCmd( ({"beschreib", "beschreibe"}), "beschreiben");
  AddCmd( ({"aender", "aendere"}), "aendern");
  AddCmd( ({"klopf","klopfe"}), "klopfen");
  AddCmd("verbiete", "verbieten");
  AddCmd("notiz", "merktext");
}

varargs void Save(int crunched)
{
  save_object(HAUSSAVEPATH+owner);
}

void Load()
{
  restore_object(HAUSSAVEPATH+owner);
}

void init()
{
  // statt im Haus landet man im Hauptraum Raum 0, sollte ansich nicht passieren
  // da man nomalerweise nicht ins Haus sondern direkt in den Hauptraum gemoved wird
  if (environment(this_player()) == this_object()) {
    this_player()->move(RAUMNAME(owner,0), M_GO | M_NO_SHOW | M_SILENT);
    return;
  }
  ::init();
}

// Extra fuer Boing! ;)
// Setzt man P_SHORT des Hauses auf 0, so verschwindet es zwar aus
// der Beschreibung des Raumes, ist aber immer noch ansprechbar!
string short()
{
  string ret;

  ret = ::short();
  if (previous_object() != environment() && !ret)
    ret ="";

  return ret;
}

// Zustand der Haustuer und evtl. Notiz mit einbauen.
varargs string long(int mode)
{
  string ret, door;

  door = haustuer::long(mode);

  ret = QueryProp(P_LONG)+door;
  if (text) {
    ret += break_string( "An "+name(WEM,1)+" befindet sich eine Notiz: '"+text+"'.", 75);
  }
  return break_string(ret, 75);
}

// Setze neuen Besitzer fuer dieses Haus
// setzt Default-Short, -Long und Id fuer neuen Besitzer,
// leitet Besitzer an Haustuer weiter und laedt den Hauptraum
varargs string SetOwner(string o)
{
  SetProp(P_SHORT, "Das Haus von "+capitalize(o));
  SetProp(P_LONG, "Das Haus von "+capitalize(o)+", es sieht sehr nach einem Rohbau aus.\n");
  AddId( ({ "\n"+o+"haus", "haus von "+o }) );
  haustuer::SetTuerOwner(o);
  load_object(RAUMNAME(o,0));
  return owner = o;
}

// liefert den Name des Haus-Besitzers
string QueryOwner() { return owner; }

private int tp_ownercheck()
{
  if(getuid(this_player()) != owner)
  {
    notify_fail( "Das ist nicht Dein Haus!\n" );
    return 0;
  }
  return 1;
}

// Aktion zum Betreten
static int betritt(string str)
{
  notify_fail("Was moechtest Du betreten?\n");

  // Haus war nicht angesprochen
  if (!str || !id(str))
    return 0;

  // Haustuer ist zu
  if (Query(H_DOORSTAT) & D_CLOSED) {
    printf("%s von %s ist zu.\n", capitalize(name(WER,1)), capitalize(owner));
    return 1;
  }
  // bewege Spieler in Hauptraum
  this_player()->move(RAUMNAME(owner,0), M_GO, 0, "betritt "+name(WEN,1), "kommt herein");
  return 1;
}

// Aktion zum Setzen/Loeschen von Notizen am Haus
static int
merktext(string str)
{
  // darf der Spieler das, d.h. ist das sein Haus?
  if(!tp_ownercheck())
  {
    notify_fail( "Du kannst keine Notiz an "+capitalize(name(WEM,1))+" von "+capitalize(owner)+" anbringen!\n" );
    return 0;
  }

  str = this_player()->_unparsed_args();

  // kein Text => Notiz loeschen, ansonsten => Text als Notiz setzen
  if (!str || str == "") {
    write( "OK, Notiz ist geloescht.\n" );
    text = 0;
  }
  else {
    write( "OK, Notiz lautet '"+str+"'\n");
    text = str;
  }

  return 1;
}

// Aktion Anklopfen am Haus
static int
klopfen(string str)
{
  int i;
  object room;

  notify_fail( "Syntax: klopfe an <haus> an\n" );
  // parsen von klopfe an <haus> an
  if (!str || sscanf(str, "an %s an", str) != 1)
    return 0;

  if (str == "tuer" || str == "haustuer" || id(str)) {
    // Meldungen an Spieler und Raum
    printf( "Du klopfst an "+name(WEM,1)+" von "+capitalize(owner)+" an.\n");
    say( capitalize(this_player()->name(WER))+" klopft an "+name(WEM,1)+" von "+capitalize(owner)+" an.\n" );
    // in allen geladenen Raeumen Klopfen melden
    for (i=VERWALTER->HausProp(owner,HP_ROOMS); i>=0; i--) {
      if (room = find_object(RAUMNAME(owner,i)))
	tell_room(room, "Du hoerst, wie jemand an der Haustuer anklopft.\n" );
    }
    return 1;
  }
  return 0;
}

// Aktion Tuer/Haus beschreiben
static int
beschreiben(string str)
{
  string *p, *ds;

  // darf der Spieler das, d.h. ist das sein Haus?
  if (!tp_ownercheck())
    return 0;

  notify_fail( "Hier draussen funktioniert nur 'beschreibe <haus> lang',\n"
	      +"'beschreibe haustuer' oder 'beschreibe tuerzustand'!\n");

  // zu wenig Eingabe zum Parsen
  if (!str || str == "")
    return 0;

  p = old_explode(str," ");

  if (p[0] == "tuer" || p[0] == "haustuer") {
    // beschreibe (haus)tuer
    write( "Beschreibung der Haustuer angeben. Es ist nur eine Zeile moeglich.\n"
	  +"Als Platzhalter fuer den Zustand der Tuer dient %s. Falls die Tuer\n"
	  +"nicht in der Langebschreibung des Hauses auftauchen soll, bitte ein-\n"
	  +"fach nur <RETURN> druecken.\n]" );
    input_to("doorDesc");
    return 1;
  }
  else if (p[0] == "tuerzustand") {
    // beschreibe tuerzustand
    ds = Query(H_DOORLSTAT);
    write( "Beschreibung des Tuerzustandes angeben. Es ist nur eine Zeile moeg-\n"
	  +"lich. Die Zustaende muessen durch Kommata getrennt in der Reihen-\n"
	  +"folge 'offen', 'geschlossen', 'abgeschlossen' angegeben werden.\n"
	  +"Momentaner Zustand:\n");
    printf("'%s', '%s', '%s'.\n", ds[0], ds[1], ds[3]);
    write( "(Abbruch: Einfach <RETURN> druecken!)\n]");
    input_to("doorLDesc");
    return 1;
  }
  else if (p[<1] == "lang" && id(implode(p[0..<2]," "))) {
    object home = load_object(RAUMNAME(owner,0));
    // beschreibe <haus> lang
    home->beschreiben("haus lang", AUSSEN);
    return 1;
  }
  // Eingabe Syntax nicht passend => notify_fail von oben
  return 0;
}

// Aktion Haus Aendern
static int
aendern(string str)
{
  string *p, *ds;

  // Darf der Spieler das, d.h. ist das sein Haus?
  if (!tp_ownercheck())
    return 0;

  notify_fail( "Hier draussen funktioniert nur 'aendere <haus> lang'!\n");
  // zu wenig Eingabe zum Parsen
  if (!str || str == "")
    return 0;

  p = old_explode(str," ");

  if (p[<1] == "lang" && id(implode(p[0..<2]," "))) {
    object home = load_object(RAUMNAME(owner,0));
    // aendere <haus> lang
    home->aendern("haus lang", AUSSEN);
    return 1;
  }
  // Eingabe Syntax nicht passend => notify_fail von oben
  return 0; 
}

// input_to zu beschreibe (haus)tuer
static void doorDesc(string str)
{
  if (!str)
    str = "";
  if (sizeof(str))
    str += "\n";

  Set(H_DOOR, str);
  Save();

  if (str == "")
    write("OK, Haustuer wurde geloescht.\n");
  else
    write("OK, Beschreibung der Haustuer wurde gesetzt.\n");
}

// input_to zu beschreibe tuerzustand
static void doorLDesc(string str)
{
  string *s;

  if (!str || sizeof(s=old_explode(str, ","))!=3) {
    write("ABBRUCH!\nNichts geaendert!\n");
    return;
  }
  Set(H_DOORLSTAT, ({ s[0], s[1], 0, s[2] }));
  printf("OK, Tuerzustaende sehen wie folgt aus:\n'%s', '%s', '%s'.\n",
	 s[0], s[1], s[2]);
  Save();
}

int SmartLog(string ofile, string typ, string msg, string date)
{
  object home;
  string foo, bar;
  
  string hn=RAUMNAME(owner,0);

  write_file(REPFILE(owner), sprintf("%s von %s vor dem Haus (%s):\n%s\n",
				     typ,
				     capitalize(getuid(this_player())),
				     date,
				     break_string(msg,78)));

  if (IS_LEARNER(owner)) {
    log_file("report/"+owner+".rep",
	     sprintf("MELDUNG von %s im Seherhaus, vor dem Haus (%s):\n"
		     +"Bitte zur Kenntnis nehmen! (Mit dem Befehl 'meldungen')  -Wargon\n",
		     capitalize(getuid(this_player())),
		     date));
  }

  home = load_object(hn);
  home->Set(H_REPORT, home->Query(H_REPORT)+1);
  home->Save();

  return 1;
}



//  raum.c  -- Das Rohgeruest eines Seherhauses.
//
//  Grundobjekt (c) 1994 Boing@MorgenGrauen
//  Abschliessen und Rauswerfen von Jof
//  Fuer Aenderungen ab dem 06.10.94 verantwortlich: Wargon
//  Ab 03.02.98 Wurzel
//
// $Id: raum.c,v 1.5 2003/11/15 14:03:58 mud Exp $
#pragma strong_types,rtt_checks

#include "haus.h"

inherit "/std/room";
inherit USERCMD;    // selbstdefinierte Kommandos
inherit LOSA;       // Load() und Save()
inherit "/mail/nedit";

#include <wizlevels.h>
#include <properties.h>
#include <defines.h>
#include <moving.h>

static mixed detail;
static string owner;
static int flag, csaved;
static int raumNr;
static string *ausgaenge = ({ "oben", "norden", "nordosten", "osten", "suedosten",
                  "unten", "sueden", "suedwesten", "westen", "nordwesten" });

// Prototypes fuer Funktionen aus LOSA
varargs void Save(int crunched);
void Load();

// Prototype
// Falls noetig, mache einen Notausgang, falls nicht loesche vorhandenen
void make_emergency_exit();

// ersetzt @@ durch **
private string normstr(string str)
{
  return implode(explode(str,"@@"),"**");
}

// liefert kommagetrennte Teile ohne zusaetzliche Leerzeichen
// "Bla blubb ,  foo bar,blubber" => ({ "Bla blubb", "foo bar", "blubber" })
private string *brk(string str)
{
  string *t1;

  t1 = explode(str, ",");
  map(t1, #'trim /*'*/);
  return t1;
}

// teste Befehl
private int befCheck(string bef)
{
  // Befehl enthaelt Leerzeichen => return 0
  if (member(bef, ' ') > -1)
    return 0;

  // Befehl ist bereits Kommando (ausser Oeffnen/Schliessen)
  // oder Richtung inclusive "raus" in die kein Ausgang fuehrt
  // => return 0
  if (member(m_indices(QueryProp(P_COMMANDS)) -
                       ({"oeffne","schliess","schliesse"}), bef) > -1
      || (member(ausgaenge+({"raus"}), bef) > -1
          && member(m_indices(QueryProp(P_EXITS)), bef) == -1))
    return 0;

  return 1;
}

// Test auf Hausbesitzer
int tp_owner_check()
{
  if(getuid(this_player()) != owner)
  {
    notify_fail( "Das ist nicht Dein Haus!\n" );
    return 0;
  }
  return 1;
}

// Test auf Hausbesitzer oder Person mit Erlaubnis
int allowed_check(object _test)
{
   string *ext;

   ext = VERWALTER->HausProp(owner, HP_ALLOWED) + ({ capitalize(owner) });
   if(member(ext, capitalize(getuid(_test))) < 0)
   {
     return 0;
   }
   else
   {
     return 1;
   }
}

// Funktion ersetzt durch tp_owner_check() und allowed_check(object _test)
// falls allowed == 0 => test ob this_player() Hausbesitzer
// falls allowed != 0 => test ob allowed/this_player() Hausbesitzer
//                       oder Person mit Erlaubnis
deprecated varargs int owner_check(mixed allowed)
{
  object test;

  if(objectp(allowed) && query_once_interactive(allowed))
  {
    // Parameter ist Spieler
    test = allowed;
  }
  else
  {
    // ansonsten TP
    test = this_player();
  }
  
  if(allowed)
  {
    // Test auf Hausbesitzer oder Person mit Erlaubnis
    if(!allowed_check(test))
    {
      notify_fail( "Das darfst Du in diesem Haus nicht!\n" );
      return 0;
    }
  }
  else if(getuid(test) != owner)
  {
    // Test auf Hausbesitzer
    notify_fail( "Das ist nicht Dein Haus!\n" );
    return 0;
  }
  // Erlaubnis OK
  return 1;
}

// Gestaltet Meldung aus Texten in einem Array:
// array leer               -> melde Text in none
// array hat ein Element    -> melde Text in one und ersetze %s durch Element
// array hat viele Elemente -> melde Text in many und danach
//                             sortierte Aufzaehlung der Elemente
// flag gesetztes Bit 2: Aufzaehlung nur mit Komma, kein 'und'
// flag gesetztes Bit 1: Meldung wird als String returnt, sonst per write ausgegeben
varargs string arr_out(mixed array, string none, string one, string many, int flag)
{
  string tmp, lastsep;
  mixed arr;

  switch(sizeof(array))
  {
    case 0:
      tmp = none;
      break;
    case 1:
      tmp = sprintf(one, array[0]);
      break;
    default:
      tmp = many;
      arr = sort_array(array, #'> /*'*/);
      lastsep = (flag & 2) ? ", " : " und ";
      tmp += CountUp(arr, ", ", lastsep)+".";
      break;
  }
  if (flag & 1)
    return(break_string(tmp, 75, 0, 1));
  else
    write(break_string(tmp, 75, 0, 1));

  return 0;
}

protected void create()
{
  if (!clonep(this_object()) && object_name(this_object())==(RAUM)) {
      set_next_reset(-1);
      return;
  }
  room::create();
  usercmd::create();
  losa::create();

  Set(P_INT_LONG, SAVE, F_MODE_AS);
  Set(P_INT_SHORT, SAVE, F_MODE_AS);
  Set(P_EXITS, SAVE, F_MODE_AS);
  Set(P_DETAILS, SAVE, F_MODE_AS);
  Set(P_READ_DETAILS, SAVE, F_MODE_AS);
  Set(P_LIGHT, SAVE, F_MODE_AS);

  SetProp(P_IDS, ({"raum","sehe\rhaus"}));
  SetProp(P_NAME, "Haus");
  SetProp(P_GENDER, 0);
  SetProp(P_TRANSPARENT, 0);
  SetProp(P_NOGET, 1);
  SetProp(P_LIGHT, 1);
  SetProp(P_INDOORS, 1);

  AddCmd( ({ "beschreib", "beschreibe" }), "beschreiben" );
  AddCmd("uebersicht", "uebersicht");
  AddCmd("ausgang", "ausgang");
  AddCmd( ({"loesche", "loesch"}), "loesch");
  AddCmd( ({"wirf", "werf", "werfe" }), "wirf");
  AddCmd( ({"sperr", "sperre"}), "sperren");
  AddCmd( ({"kopier", "kopiere" }), "kopieren");
  AddCmd("licht", "licht");
  AddCmd( ({"aendere", "aender" }), "aendern");
  AddCmd( ({"meldung", "meldungen"}), "report");
}

void init()
{
	object haus;
  
  room::init();
  nedit::init_rescue();

  // Magier betritt fremdes Seherhaus
  if (query_once_interactive(PL) && IS_LEARNER(PL) && getuid(PL) != owner) {
    if (PL->QueryProp(P_INVIS))
      // Magier invis => im Raum melden
      tell_room(this_object(),
          capitalize(getuid(PL))+" betritt unsichtbar den Raum.\n",
          ({PL}));

    if ((haus = load_object(HAUSNAME(owner))) &&
        (haus->QueryProp(H_DOORSTAT) & D_LOCKED))
      // Tuer ist zu => Magier einen Hinweis geben
      tell_object(PL, " ******* ACHTUNG *********************************\n *\n * "+
        capitalize(owner)+" moechte vielleicht ungestoert bleiben!\n *\n"
        " ******* ACHTUNG *********************************\n");
  }
}
  
int PreventInsertLiving(object ob)
{  
  return 0;
}

// Gegenstaende kommen in den Raum
void NotifyInsert(object ob, object oldenv)
{
	object *found;

	if (!objectp(ob)) return;
	
	if (ob->QueryProp(H_FURNITURE)!=0)
	{ 
    // das was reinkam ist ein autoload Moebelstueck
    // betrachte alle schon vorhandenen autoload Moebelstuecke
      found = filter_objects(all_inventory(this_object()),
		        "QueryProp", H_FURNITURE);
	  if (sizeof(found)>=MAX_FURNITURE_PER_ROOM)        
      {
      // Maximal-Anzahl vorhandener autoload Seherhausmoebel ist schon
      // erreicht
	    tell_object(this_player(), break_string(
	      "Ein Raum kann nur "+MAX_FURNITURE_PER_ROOM+" rebootfeste "
	      "Moebel speichern. Hier befinden sich schon "+
	      CountUp(map_objects(found, "name"))+"."));
	    return 0;
      }
      
      queued_save(); // Speichern vorbereiten (falls mehrere Objekte 
                     // gedroppt werden, wird so ein overflow verhindert.
	}
}

// Gegenstaende verlassen den Raum
void NotifyLeave(object ob, object dest)
{
	if (!ob &&!objectp(ob)) return 0;
	
  // rebootfestes Moebelstueck verlaesst den Raum
	if (ob->QueryProp(H_FURNITURE)!=0)
	{	
    // Speichern vorbereiten (falls mehrere Objekte 
    // gedroppt werden, wird so ein overflow verhindert.
		queued_save();
	}
	return 0;
}

void
reset()
{
  room::reset();
  losa::reset();
  // Wenn ein Notausgang vorhanden ist, checken, ob der noch noetig ist.
  if (member(m_indices(QueryProp(P_EXITS)),"notausgang")!=-1)
  {
      make_emergency_exit();
  }
}

int clean_up(int arg)
{
  mixed inv;

  if (arg > 1)
    return 1;

  losa::reset();  // Evtl. gepackt speichern...

  // nichts im Raum oder nur leere Seherhaustruhe => Raum kann weg
  if ((sizeof(inv=all_inventory()) == 0) ||
      (sizeof(inv) == 1 && inv[0]->id(TRUHE) &&
       sizeof(all_inventory(inv[0]))==0))
    remove(1);

  return 0;
}

// Haeufungen von gleichen Objekten sind hier egal => nix tun
protected varargs void remove_multiple(int limit) {}


// Spieler wird im Raum Netztot
void BecomesNetDead(object pl)
{
//  Nicht erlaubte Spieler werden rausgeworfen, alle anderen bleiben drin.
  if(!allowed_check(pl))
    pl->move(VERWALTER->HausProp(owner, HP_ENV), M_GO, 0, "schlafwandelt heraus", "schlafwandelt herein" );
}


/* Ist dieser Raum ein Ausgang? */
int is_exit(string path)
{ 
  return (sizeof(path) > 4) && (path[<5..] == "raum0");
}

/*Pruefung, ob es einen Hausausgang gibt*/
int room_has_exit() {

    string room = object_name(this_object());
  // sind wir schon in einem Hauptraum mit Ausgang?
    if (is_exit(room)) return 1;

    mapping r_todo = ([room]);
    mapping r_done = ([:0]);
    /* Die Schleife hat maximal 9 * Anzahl der verb. Haeuser Durchlaeufe */
    while (sizeof(r_todo))
    {
      mapping r_exits = ([]);

      // naechsten Raumpfad abarbeiten, d.h. dessen Ausgaenge betrachten
      room = m_indices(r_todo)[0];
      // abgearbeiteten Raumpfad von r_todo nach r_done schieben
      r_done += ([room]);
      m_delete(r_todo, room);

      // alle Ausgaenge betrachten
      foreach(string command, string subpath : room->QueryProp(P_EXITS))
      {
        // Notausgaenge nicht betrachten, da 'echte' Ausgaenge gesucht sind
        if(command != "notausgang")
          // alle anderen Ausgaenge sammeln
          r_exits += ([ subpath ]);
      }
      // nur Raumpfade die noch nicht abgearbeitet sind testen
      r_exits -= r_done;
      // ist da ein Hauptraum (der dann Ausgang liefert) dabei?
      if (sizeof(filter_indices(r_exits, #'is_exit/*'*/)))
        return 1;

      // neue Raumpfade in die TODO-Liste aufnehmen
      r_todo += r_exits;
    }
    return 0;
}

// damit man Spieler nicht durch Entfernen von Ausgaengen einsperren kann
// werden bei Bedarf Notausgaenge in den Hauptraum ergaenzt
void make_emergency_exit()
{
  // Ist der Raum weder Hauptraum mit Ausgang noch fuehrt ein Weg dorthin?
  // dann Notausgang machen
  if(!room_has_exit())
  {
    // tell_room(this_object(),"DEBUG: Ein Notausgang wurde angelegt.\n");
    room::AddExit("notausgang",
      "durch den Notausgang#"+to_string(this_object())[0..<2]+"0");
  }
  else
  {
    // nicht mehr benoetigten Notausgang entfernen
    room::RemoveExit("notausgang");
  }
}

// ruft direkt das geerbte AddExit auf, ohne ggf. einen Notausgang zu
// erzeugen. Das wird vom Haus und vom Verwalter benutzt, um Ausgaenge zu
// erzeugen. Das ist noetig, weil der Notausgangmechanismus noch nicht
// funktoniert, wenn der Raum gerade geladen wird (hat noch nicht den
// endgueltigen Namen) und der Check waere ohnehin unnoetig.
public void AddExitNoCheck(mixed cmd, mixed dest)
{
  room::AddExit(cmd, dest);
}

// Wird benutzt beim Laden von Raeumen, um alle Ausgaenge zu loeschen. Dabei
// darf _kein_ Notausgang erstellt werden.
void RemoveExitNoCheck(mixed cmd)
{
  room::RemoveExit(cmd);
}

// beim Ausgang setzten testen ob vorhanderner Notausgang noch benoetigt wird
void AddExit(mixed cmd, mixed dest)
{
  room::AddExit(cmd, dest);

  if (member(m_indices(QueryProp(P_EXITS)),"notausgang")!=-1)
  {
    // loescht Notausgang falls nicht mehr benoetigt
     make_emergency_exit();
  }
}

// beim Ausgang loeschen testen ob Notausgang benoetigt wird
void RemoveExit(mixed cmd)
{
  room::RemoveExit(cmd);
  // setzt Notausgang falls benoetigt
  make_emergency_exit();
}

// der Langbeschreibung werden alle unsichtbaren Magier
// (ausgenommen Seherhausbesitzer) in Klammern angefuegt
varargs string int_long(mixed viewer, mixed viewpoint, int flags)
{
  string ret;
  object *inv;
  string *tmp;

  ret = ::int_long(viewer, viewpoint, flags);

  // nur was machen wenn der Betrachter im Raum ist
  if (!ret || !objectp(viewer) || environment(viewer) != this_object())
    return ret;

  if(viewpoint == 0)
    viewpoint = ({ viewer });
  else if(!pointerp(viewpoint))
    viewpoint = ({ viewpoint });

  // alle Lebewesen ausser denen aus deren Sicht betrachtet wird
  inv = filter(all_inventory(this_object()),#'interactive) - viewpoint;
  foreach(object pl : inv)
  {
    // raussuchen wer ausser Seherhausbesitzer unsichtbar ist
    if(pl && IS_LEARNER(pl) && pl->QueryProp(P_INVIS) && getuid(pl)!=owner)
    {
      // Name in Klammer sammeln
      tmp += ({ "("+capitalize(getuid(pl))+")" });
    }
  }
  return ret + break_string(CountUp(tmp), 78);
}
/*
// TODO: Testen ob das hier nicht das Standard-Verhalten ist
int lies(string str)
{
  string txt;

  notify_fail("Was moechtest Du lesen?\n");
  if (!str) return 0;

  if (this_player()->CannotSee())
    return 1;
  if (txt = QueryProp(P_READ_DETAILS)[str]) {
    this_player()->More(txt);
    return 1;
  }
  return 0;
}
*/
// Aktion Lebewesen oder Gegenstaende aus dem Haus werfen
int wirf(string str)
{
  string wen, args;
  object pl, target, *list, tp, to;

  tp = this_player();
  to = this_object();
  args = tp->_unparsed_args(1);

  // klappt nur vor Ort und mit der passenden Syntax
  if ((environment(tp)!=to) ||
      !args || args == "" ||
      (sscanf(args,"%s raus",wen) != 1) )
    return 0;
  // Raum, in dem das Haus steht, ermitteln
  target=find_object(VERWALTER->HausProp(owner, HP_ENV));
  if (!target) {
    notify_fail("Dieses Haus steht leider gerade in einem "
                "Fehler im Raum-Zeit-Gefuege.\n");
    return 0;
  }
  // Rauswerfen darf nur der Besitzer
  if (!tp_owner_check()) {
    notify_fail("Aber Du kannst doch niemanden aus dem Haus von "+capitalize(owner)+" werfen!\n");
    return 0;
  }
  if (wen=="alle") {
    // alle Lebewesen ausser tp (== Hausbesitzer)
    list=filter(all_inventory(to),#'living)-({tp,0});//')
    if (sizeof(list)==0) {
      notify_fail("Aber Du bist doch allein in Deinem Haus!\n");
      return 0;
    }
  } else if (wen == "alles") {
    // alle Gegenstaende ausser Lebewesen und Moebel
    // (Seherhaustruhe, Autoloader-Moebel oder Seherhaus-Moebel)
    list = filter(all_inventory(to),
                  function int(object ob)
                  {
                      return objectp(ob) &&
                             !living(ob) &&
                             !ob->id(TRUHE) &&
                          // TODO Test auf nicht Seherhausmoebel von August
                             strstr(load_name(ob),"/d/seher/haeuser/moebel/")
                               == 0;
                  } );

    if (sizeof(list)==0) {
      notify_fail("Aber hier ist nichts zum wegwerfen!\n");
      return 0;
    }
  } else {
    pl=present(wen,to);
    if (!pl) {
      notify_fail("So jemanden sehe ich hier nicht.\n");
      return 0;
    }
    else if (pl->id(TRUHE) ||
             // TODO Test auf Seherhausmoebel von August
              strstr(load_name(pl),"/d/seher/haeuser/moebel/")==0
             ) {
      notify_fail("Du kannst "+pl->name(WEN)+" nicht wegwerfen!\n" );
      return 0;
    }
    list=({pl});
  }
  string msg_wen = sprintf("%s wirft Dich aus %s Haus.\n",
                           tp->Name(WER),
                           tp->QueryPossPronoun(NEUTER,WEM));
  // fuer alle rauszuwerfenden Opfer Meldungen machen und rausbewegen
  foreach(object ob : list)
  {
    tell_object(ob, msg_wen);
    tell_room(target,
        sprintf("%s kommt in hohem Bogen aus dem Haus von %s geflogen.\n",
                 ob->Name(WER),tp->name(WEM)));
    ob->move(target,M_SILENT|M_GO);
    tell_room(to,sprintf("%s wirft %s aus %s Haus.\n",
                    tp->Name(WER),
                    ob->name(WEN),tp->QueryPossPronoun(NEUTER,WEM)),
              ({tp}));
    printf("Du wirfst %s aus Deinem Haus.\n",ob->name(WEN));
    // Verfolger abmelden, damit sie nicht gleich zurueckkommen
    // TODO wenn man einzelne Lebewesen rauswirft kann das ja auch
    // ein Verfolger von einem anderen Gast sein...
    tp->RemovePursuer(ob);
  }
  return 1;
}

// Besitzer und Raumnummer fuer diesen Raum setzen
varargs string SetOwner(string o, int num)
{
  // Default Kurz- und Langbeschreibung setzen
  SetProp(P_INT_SHORT, "Im Haus von "+capitalize(o));
  SetProp(P_INT_LONG, "Ein total leerer Raum.\n");
  // Raumnummer und Besitzer merken
  raumNr = num;
  return owner = o;
}

// Liefert den Besitzer
// falls withNr != 0 mit angehaengter Raumnummer
varargs string QueryOwner(int withNr)
{
  return (withNr ? owner+raumNr : owner);
}

// Prototype
static int befEingabe(string *bef);

// Aktion zum Beschreiben des Raumes
varargs int
beschreiben(string str, int f)
{
  string *parts;
  int sp, ret;

  // nur der Besitzer darf
  if(!tp_owner_check())
    return 0;

  if (!f && (!(str=UP_ARGS(this_player())) || str == "")) {
    notify_fail("Was willst Du denn beschreiben?\n" );
    return 0;
  }

  sp = sizeof(parts = old_explode(str, " "));
  // je nachdem was beschrieben wird, setze detail und flag
  // und starte damit Editor, bzw. behandle Befehle extra
  detail = 0;
  flag = f;

  switch(parts[0][0..3]) {
    case "raum":      // Lang- oder Kurzbeschreibung
    case "haus":
      if (sp == 1 || parts[1] == "lang")
    flag |= LANG;
      else if (parts[1] == "kurz")
    flag |= KURZ;
      printf("Bitte %sbeschreibung des %s eingeben.\n", (flag & LANG ? "Lang" : "Kurz"), (flag & AUSSEN ? "Hauses" : "Raumes") );
      break;
    case "deta":      // Details
      if (sp==1) {
    notify_fail("Welches Detail willst Du denn beschreiben?\n");
    return 0;
      }
      flag |= DETAIL;
      str = implode(parts[1..]," ");
      write( "Bitte Beschreibung fuer '"+str+"' eingeben.\n");
      break;
    case "lesb":      // lesbare Details
      notify_fail("Welches lesbare Detail willst Du denn beschreiben?\n");
      if (sp == 1) return 0;
      if (parts[1] == "details" || parts[1] == "detail") {
    if (sp == 2) return 0;
    str = implode(parts[2..]," ");
      }
      else
    str = implode(parts[1..]," ");
      flag |= RDETAIL;
      write( "Bitte Beschreibung fuer '"+str+"' eingeben.\n");
      break;
    case "befe":      // Befehle
      ret = 0;
      if (sp == 1)
    notify_fail("Welchen Befehl willst Du denn beschreiben?\n");
      else
    ret = befEingabe(brk(implode(parts[1..]," ")));
      return ret;
      break;
    default:
      notify_fail("Das kannst Du nicht beschreiben! Eine Liste der Dinge, die Du hier\n"
         +"beschreiben kannst, erhaeltst Du mit 'hilfe beschreibe'.\n" );
      return 0;
      break;
  }
  detail = brk(str);
  write( "(Beenden mit . oder **, Abbruch mit ~q)\n" );
  nedit( "beschreibung" );
  return 1;
}

// nedit von beschreibe xxx
static void beschreibung(string str)
{
  if (!str) {
    write("Nichts geaendert!\n");
    return;
  }

  str = normstr(str);

  if (flag & LANG)
  {
    // Langbeschreibung
    if (sizeof(explode(str,"\n")) > 100
        || sizeof(str) > 7800)
    {
      // ueber 100 Zeilen oder ueber 7800 Zeichen
      write( "Das ist fuer eine Langbeschreibung zu lang!\n"
             "Sorry, bitte denke Dir eine andere Langbeschreibung aus.\n" );
      return;
    }
    if (flag & AUSSEN) {
      // Langbeschreibung vom Haus
      object haus;
      haus = find_object(HAUSNAME(owner));
      haus->SetProp(P_LONG, str);
      haus->Save();
    }
    else
      // Langbeschreibung von diesem Raum
      SetProp(P_INT_LONG, str);
  }
  else if (flag & KURZ) {
    // Kurzbeschreibung vom Raum
    if (sizeof(old_explode(str,"\n")) > 1 || sizeof(old_explode(str,".")) > 2 || sizeof(str) > 75) {
      write( "Das ist fuer eine Kurzbeschreibung zu lang!\nSorry, bitte denke Dir eine andere Kurzbeschreibung aus.\n" );
      return;
    }
    else
      // Vanion, 27.07.02, Bugfix
      // Die Zeile buggte, wenn man "." oder "\n" oder "\n." oder sowas
      // in str hat. (also z.B. bei "beschreibe raum kurz <return> .")
      // SetProp(P_INT_SHORT, old_explode(old_explode(str,"\n")[0],".")[0]);
      SetProp(P_INT_SHORT, explode(explode(str,"\n")[0],".")[0]);
  }
  else if (flag & DETAIL)
    // Raum-Detail
    AddDetail(detail, str);
  else if (flag & RDETAIL)
    // lesbares Raum-Detail
    AddReadDetail(detail, str);
  else {
    write( "Huch! Unbekanntes Flag ("+flag+")... Sag mal "
          + CountUp(MAINTAINER, ", ", " oder ")
          + " Bescheid...\n");
    return;
  }

  write("OK.\n");
  Save();
}

// wird in beschreiben(str, int) 'beschreibe befehl' aufgerufen
static int befEingabe(string *befehle)
{
  string* com = ({});

  notify_fail("Kein Befehl zum Beschreiben uebrig... ;)\n");
  foreach(string bef : befehle)
  {
    // schon vorhandener Befehl (ausser oeffnen/schlissen),
    // Richtung ohne zugehoerigen Ausgang,
    // oder Befehl enthaelt Leerzeichen 
    if (!befCheck(bef))
      write("Der Befehl '"+bef+"' kann nicht beschrieben werden!\n");
    else
      com += ({ bef });
  }
  if (!sizeof(com))
    return 0;

  arr_out(com, 0, "Zu beschreibender Befehl: %s",
          "Zu beschreibende Befehle:\n");

  write( "Bitte Parameter eingeben (evtl. durch Kommata getrennt).\n]");
  input_to("getBefParam", 0, com);
  return 1;
}

// input_to aus befEingabe(string) zu beschreibe befehl ...
static void getBefParam(string param, string *bef)
{
  string txt = "Gib nun bitte den Text ein, der fuer diesen Befehl "
      "ausgegeben werden soll.\n";

  // Fuehrende und abschliessende Leerzeichen entfernen
  if (param)
    param = trim(param);

  if (!param || param == "")
    // kein Parameter, z.b. bei beschreibe befehl klopfe 
    detail = ({ bef, "" });
  else if (param == "@NF@" || param == "@nf@") {
    // Parameter fuer notify fail zum Ersetzen von Wie bitte?
    // z.B. bei beschreibe befehl druecke
    // fuer danach Du kannst hier kein @PARA druecken, nur den Knopf!
    detail = ({ bef, 1 });
    txt = "Gib nun bitte den Text ein, der als Fehlermeldung "
          "ausgegeben werden soll.\n@PARA dient dabei als Platzhalter fuer "
          "die ungueltige Eingabe.\n";
  }
  else
    // sonstige Parameter
    // z.B. knopf, klingel bei beschreibe befehl druecke
    detail = ({ bef }) + brk(lower_case(param));

  printf(txt+"(Beenden mit . oder **, Abbruch mit ~q)\n");
  nedit("getBefText");
}

// Prototype
private string preparse(string str, string *invalid);

// nedit fuer die Eingabe der Texte  (Fehlermeldung/Meldungen) fuer den Befehl
static void getBefText(string text)
{
  string my, *txt, *warn;
  mixed bef;

  if (!text || text == "") {
    write("** Abbruch! **\n");
    detail = 0;
    return;
  }
  // gemerktes Befehls-Array
  bef = detail[0];

  txt = old_explode(text, "@@\n");

  warn = ({});

  // Meldung an this_player() parsen und in warn falsche Platzhalter sammeln
  my = preparse(txt[0], &warn);
  string other = 0;
  if (sizeof(txt) > 1)
    // Meldung an andere parsen und in warn falsche Platzhalter sammeln
    other = preparse(txt[1], &warn);

  AddUserCmd(bef, (stringp(detail[1]) ? detail[1..] : ({ "@NF@" })), my, other);
  Save();
  arr_out(warn, "OK.", "WARNUNG! Ungueltiger Platzhalter: %s",
               "WARNUNG! Ungueltige Platzhalter: ");

  detail = 0;
}

// check, ob an Position pos in txt ein Buchstabe aus dem array choice steht
// return 0 falls nicht, prefix + Position des Buchstabens in choice ansonsten
// check_placeholder(({"R","S","M","N"}), 2, "WESSEN", "X"); -> X1
string check_placeholder(string* choice, int pos, string txt, string prefix)
{
   int idx;
    
   if(sizeof(txt) < pos+1 ||
      ((idx=member(choice, txt[pos..pos])) < 0))
   {
     return 0;
   }
   else
   {
     return prefix+to_string(idx);
   }
}

// Dann drueckt @PWER den Knopf -> Dann drueckt @P0 den Knopf
private string preparse(string str, string *invalid)
{
  string *txt;

  txt = explode(str, "@");
  // fuer jeden Textteil nach einem @
  // suche Ersatz fuer den Begriff direkt nach dem @
  // AN: wuerde es nicht theoretisch reichen, hier nur bis i>0
  // runterzuzaehlen? Das erste Element des Arrays ist immer irrelevant, weil
  // entweder Leerstring oder kein zu ersetzender Ausdruck.
  for (int i=sizeof(txt)-1; i>=0; i--) {
    int warn = 0;
    string rpl = 0;
    // falls Teil zu kurz nix ersetzen
    if (sizeof(txt[i])<3)
      continue;
    // anhand der ersten Buchstaben, Ersatz bestimmen
    // warn signalisiert, ob dies schiefging:
    switch(txt[i][0..1]) {
    case "WE":  // Name
      // WER      -> W0
      // WES(SEN) -> W1
      // WEM      -> W2
      // WEN      -> W3
      rpl = check_placeholder(({"R","S","M","N"}), 2, txt[i], "W");  
      warn = !rpl;
      break;
    case "PW":  // Personalpronomen
      // PWER      -> P0
      // PWES(SEN) -> P1
      // PWEM      -> P2
      // PWEN      -> P3
      rpl = check_placeholder(({"R","S","M","N"}), 3, txt[i], "P");  
      warn = !rpl;
      break;
    case "BN":  // Possessivpronomen
    case "BM":
    case "BF":
      // BNSWER      -> B000  BMSWER      -> B010  BFSWER      -> B020
      // BNSWES(SEN) -> B100  BMSWES(SEN) -> B110  BFSWES(SEN) -> B120
      // BNSWEM      -> B200  BMSWEM      -> B210  BFSWEM      -> B220
      // BNSWEN      -> B300  BMSWEN      -> B310  BFSWEN      -> B320
      // 
      // BNPWER      -> B001  BMPWER      -> B011  BFPWER      -> B021
      // BNPWES(SEN) -> B101  BMPWES(SEN) -> B111  BFPWES(SEN) -> B121
      // BNPWEM      -> B201  BMPWEM      -> B211  BFPWEM      -> B221
      rpl = check_placeholder(({"R","S","M","N"}), 5, txt[i], "B");
      warn = !rpl;
      if(!warn)
      {
        rpl = check_placeholder(({"N","M","F"}), 1, txt[i], rpl);
        warn = !rpl;
        if(!warn)
        {
          rpl = check_placeholder(({"S","P"}), 2, txt[i], rpl);
          warn = !rpl;
        }
      }
      break;
    case "PA":
      // PARA -> F
      // kein Ersatz, sondern Textteil hier direkt ersetzen:
      if(sizeof(txt[i]) > 4)
        txt[i] = "F"+txt[i][4..];
      break;
    default:
      // kein Ersatz, nix aendern
      warn = 0;
      rpl = 0;
    }

    // falls Ersatz vorhanden, ersetze Pronomen durch ""+rpl und lasse den Rest t2[2] wie ist
    if (rpl) {
      string* t2;
      warn = sizeof(t2 = regexplode(txt[i], "(WER|WESSEN|WEM|WEN)")) < 2;
      if (!warn) {
        t2[1] = rpl;
        t2[0] = "";
        txt[i] = implode(t2, "");
      }
    }
    // falls es Probleme gab, diese merken
    if (warn)
      invalid += ({ "@"+old_explode(txt[i]," ")[0] });

  } // for (i=sizeof(txt)-1; i>=0; i--)
  // die eventuelle teilweise ersetzetn Teile wieder zusammenfuegen
  return implode(txt, "@");
}

static void loesch_alles(string str)
{
  if (str == "ja" || str == "Ja") {
    RemoveDetail(0);
    RemoveReadDetail(0);
    //SetProp(P_READ_DETAILS, ([]));
    SetProp(H_COMMANDS, ([]));
    write( "OK, alle Details, lesbaren Details und Befehle geloescht!\n" );
    Save();
  }
  else
    write( "Nichts geloescht!\n" );
}

static void loesch_etwas(string str, string prop)
{
  if (str == "ja" || str == "Ja") {
    if ( prop == P_DETAILS )
      RemoveDetail(0);
    else if ( prop == P_READ_DETAILS )
      RemoveReadDetail(0);
    else
      SetProp(prop, ([]));
    write("OK.\n");
    Save();
  }
  else
    write( "Nichts geloescht!\n" );
}

int loesch(string str)
{
  string *s, *t, p, q;
  int i, ret;
  mapping com;

  if (!tp_owner_check())
    return 0;

  if (!(str=UP_ARGS(this_player())) || str == "") {
    notify_fail("Welches Detail oder welchen Befehl moechtest Du loeschen?\n");
    return 0;
  }

  if (str == "alles") {
    write( "Wirklich alles loeschen (ja/nein)?\n]");
    input_to("loesch_alles");
    return 1;
  }

  if(str=="meldungen") {
    if(file_size(PATH+"rep/"+owner+".rep")>0) {
      rm(PATH+"rep/"+owner+".rep");
      write("Meldungen geloescht.\n");
    }else{
      write("Keine Meldungen gefunden.\n");
    }
    return 1;
  }

  s = brk(str);
  s = ({ (t=old_explode(s[0], " "))[0] })+({ implode(t[1..]," ") })+s[1..];
  ret = 1;
  flag = 0;

  switch(s[0]) {
    case "detail":
      s = s[1..];
      flag |= DETAIL;
      break;
    case "lesbar":
      flag |= RDETAIL;
      s = s[1..];
      break;
    case "lesbares":
    case "lesbare":
      flag |= RDETAIL;
      if (s[1][0..5] =="detail") {
    s = ({ old_explode(s[1]," ")[<1] });
    if (sizeof(s)>2)
      s += s[2..];
      }
      else
    s = s[1..];
      break;
    case "befehl":
      s = s[1..];
      break;
    case "alle":
      switch (s[1]) {
      case "details":
    q = "Details";
    p = P_DETAILS;
    break;
      case "lesbaren details":
    q = "lesbaren Details";
    p = P_READ_DETAILS;
    break;
      case "befehle":
    q = "Befehle";
    p = H_COMMANDS;
    break;
      default:
    write("Du kannst alle Befehle, alle Details und alle lesbaren Details loeschen!\n");
    return 1;
      }
      printf("Wirklich alle %s loeschen (ja/nein)?\n]", q);
      input_to("loesch_etwas", 0, p);
      return 1;
    default:
      flag |= (DETAIL|RDETAIL);
      ret = 0;  // Koennte auch ein Artikel in der Zeitung sein...
      break;
  }
  for (i=sizeof(s)-1; i>=0; i--) {
    if (!flag) {  // Befehl soll geloescht werden...
      if (member(com=Query(H_COMMANDS), s[i])) {
    com = m_copy_delete(com, s[i]);
    write("Befehl '"+s[i]+"' wurde geloescht.\n");
      }
      else if (sizeof(t=old_explode(s[i], " ")) > 1 &&
           member(com, t[0]) &&
           member(com[t[0]], p=implode(t[1..], " "))) {
    com[t[0]] = m_copy_delete(com[t[0]], p);
    write("Befehl '"+s[i]+"' wurde geloescht.\n");
      }
      Set(H_COMMANDS, com);
    }
    else {
      if (flag & DETAIL) {
    if (!QueryProp(P_DETAILS)[s[i]])
      notify_fail("Das Detail '"+s[i]+"' gibt es nicht.\n");
    else {
      RemoveDetail(s[i]);
      write("Detail '"+s[i]+"' wurde geloescht.\n");
      ret = 1;
    }
      }
      if (flag & RDETAIL) {
    if (!QueryProp(P_READ_DETAILS)[s[i]])
      notify_fail("Das lesbare Detail '"+s[i]+"' gibt es nicht.\n");
    else {
      RemoveReadDetail(s[i]);
      write("Lesbares Detail '"+s[i]+"' wurde geloescht.\n");
      ret = 1;
    }
      }
    }
  }
  Save();
  return ret;
}

int ausgang(string str)
{
  int nr, maxNr, hin, zurueck;
  string hier, da, ext;
  closure hausProp;
  mapping known_exits;

  if (!tp_owner_check()) {
    return 0;
  }

  hier = da = 0;
  hausProp = symbol_function("HausProp",VERWALTER);

  if (!(str=UP_ARGS(this_player())) ||
      (sscanf(str, "%s %d", hier, nr) != 2 &&
       sscanf(str, "%s %s %d",hier, ext, nr) != 3) ) {
    notify_fail( "Syntax: ausgang <richtung> [name] <nr>\n" );
    return 0;
  }

  if (ext) {
    if (funcall(hausProp, ext, HP_ENV) != funcall(hausProp, owner, HP_ENV)) {
      printf("Das Haus von %s steht nicht im gleichen Raum wie Dein Haus!\n",
             capitalize(ext));
      return 1;
    }
    else
      da = RAUMNAME(ext, nr);

    // der allowed_check() wird im Eingangsraum des Zielhauses aufgerufen,
    // da wir von anderen Raumen noch nicht wissen, ob sie ueberhaupt
    // existieren.
    if (!(RAUMNAME(ext, 0)->allowed_check(this_player()))) {
      printf("Du darfst keinen Ausgang von Deinem Haus zu dem von %s legen!\n",
             capitalize(ext));
      return 1;
    }
  }
  else {
    ext = owner;
    da = RAUMNAME(ext, nr);
  }

  maxNr = funcall(hausProp, ext, HP_ROOMS);

  if ( (hin = member(ausgaenge, lower_case(hier))) < 0) {
    arr_out(ausgaenge, 0, 0, "Es sind nur folgende Ausgaenge moeglich:\n" );
    return 1;
  }
  else
    zurueck = (hin + sizeof(ausgaenge)/2) % sizeof(ausgaenge);

  hier = RAUMNAME(owner, raumNr);

  // Kopie des Ausgaenge-Mappings erzeugen
  known_exits=deep_copy(QueryProp(P_EXITS));
  // und den Notausgang entfernen. Somit bleiben nur die zu betrachtenden 
  // Ausgaenge ueber.
  known_exits["notausgang"]=0;

  if (nr < 0 || nr > maxNr)
    printf( "Die Nummer darf sich nur im Bereich zwischen 0 und %d bewegen!\n",
            maxNr );
  else if ( ext == owner && nr == raumNr)
    printf( "Aber dies IST Raum %d!\n", raumNr );
  else if (member(m_indices(known_exits), ausgaenge[hin]) != -1)
    write( "Aus diesem Raum fuehrt schon ein Ausgang in diese Richtung!\n" );
  //else if (member(m_values(QueryProp(P_EXITS)), da) != -1)
  // Notausgang wird hier zwar geloescht, aber im AddExit
  // gibt's eh einen neuen, so das noetig ist, V*
  else if (member(m_values(known_exits), da) != -1)
    printf( "Es gibt hier schon einen Ausgang zu Raum %d!\n", nr );
  else if (member(m_indices(da->QueryProp(P_EXITS)), ausgaenge[zurueck]) != -1)
    printf( "Es fuehrt schon irgendwo ein Ausgang in Richtung '%s'\n"
            "nach Raum %d!\n", ausgaenge[hin], nr);
  else {
    AddExit( ausgaenge[hin], da );
    Save();
    da->AddExit(ausgaenge[zurueck], hier);
    da->Save();
    printf( "OK, der Ausgang '%s' zum Raum %d wurde eingerichtet.\n",
            ausgaenge[hin], nr );
  }
  return 1;
}

int
sperren(string str)
{
  mapping ex, cmds;
  int hin, zurueck;

  if (!tp_owner_check())
    return 0;

  if (!(str=UP_ARGS(this_player())) || str == "") {
    notify_fail( "Syntax: sperre <ausgang>\n" );
    return 0;
  }
  str = lower_case(str);
  ex = QueryProp(P_EXITS);

  if (raumNr == 0 && str == "raus") {
    write( "Du kannst doch nicht Deine Haustuer loeschen!\n" );
    return 1;
  }
  if (!member(ex,str) || (hin = member(ausgaenge,str)) < 0) {
    printf( "Es gibt hier keinen Ausgang '%s'!\n", str);
    return 1;
  }
  else
    zurueck = (hin + sizeof(ausgaenge)/2) % sizeof(ausgaenge);

  ex[str]->RemoveExit(ausgaenge[zurueck]);
  tell_room(find_object(ex[str]), sprintf("Der Ausgang '%s' verschwindet ploetzlich...\n", ausgaenge[zurueck]));
  cmds = ex[str]->QueryProp(H_COMMANDS);
  cmds = m_copy_delete(cmds, ausgaenge[zurueck]);
  ex[str]->SetProp(H_COMMANDS, cmds);
  ex[str]->Save();
  RemoveExit(str);
  cmds = QueryProp(H_COMMANDS);
  cmds = m_copy_delete(cmds, str);
  SetProp(H_COMMANDS, cmds);
  Save();
  printf( "OK, der Ausgang '%s' wurde entfernt.\n", str );

  Save();

  return 1;
}

varargs int
uebersicht(string dummy, string pre)
{
  string *xc, *xd, o, raus, str;
  mixed *com;
  mapping tmp;
  int i,j,k;

  if ( (getuid(this_player()) != owner) &&
       !(PATH+"access_rights")->access_rights(geteuid(this_player()), "") )
    return 0;

  i = VERWALTER->HausProp(owner, HP_ROOMS);

  if (i)
    str = sprintf( "Dein Haus verfuegt ueber %d Raeume.\nDu stehst in Raum %d (%s).\n\n", i+1, raumNr, QueryProp(P_INT_SHORT) );
  else
    str = sprintf( "Dein Haus verfuegt ueber einen Raum (%s)\n\n", QueryProp(P_INT_SHORT));

  str += arr_out(m_indices(QueryProp(P_DETAILS)),
         "Du hast keine Details beschrieben.",
         "Du hast das Detail '%s' beschrieben.",
         "Du hast folgende Details beschrieben:\n", 1 );

  str += ("\n" + arr_out(m_indices(QueryProp(P_READ_DETAILS)),
         "Du hast keine lesbaren Details beschrieben.",
         "Du hast das lesbare Detail '%s' beschrieben.",
         "Du hast folgende lesbaren Details beschrieben:\n", 1 ) );

  tmp = Query(H_COMMANDS);
  xc = sort_array(m_indices(tmp),#'<);
  if (!sizeof(xc))
    str += ("\nDu hast keine Befehle beschrieben.\n");
  else {
    if (sizeof(xc) == 1 && sizeof(xd=m_indices(tmp[xc[0]])) == 1)
      str += ("\nDu hast den Befehl '"+
          xc[0]+((xd[0] == "") ? "" : " "+xd[0])+
          "' beschrieben.\n");
    else {
      str += "\nDu hast folgende Befehle beschrieben:\n";

      for (com = ({}), j=sizeof(xc)-1; j >= 0; j--) {
    xd = sort_array(m_indices(tmp[xc[j]])-({"@NF@"}),#'>);
    if ((sizeof(xd) > 1) && (xd[0] == "")) {
      raus = "* "+xc[j]+", "+xc[j]+" ";
      xd = xd[1..];
    }
    else
      raus = "* "+xc[j]+" ";

    str += arr_out(xd, "", raus+"%s", raus, 3);
      }
    }
  }

  raus = (member(QueryProp(P_EXITS),"raus") ? "raus: Nach draussen.\n" : 0 );
  tmp = m_copy_delete(QueryProp(P_EXITS), "raus");
  m_delete(tmp, "notausgang");
  xc = m_indices(tmp);
  xd = m_values(tmp);

  if (!sizeof(xc) && !raus)
    str += "\nES GIBT KEINE AUSGAENGE!\n";
  else {
    str +=  "\nEs gibt folgende Ausgaenge:\n";
    for (i=sizeof(xc)-1; i>=0; i--)
      str += sprintf( "%s: Nach Raum %d %s(%s).\n",
             xc[i],
             (j=to_int(xd[i][<1..])),
             (((o=old_explode(xd[i],"/")[<1][0..<6])==owner) ?
             "" : "von "+capitalize(o)+" "),
             xd[i]->QueryProp(P_INT_SHORT) );
  }
  str += ((raus||"")+(pre||""));
  this_player()->More(str);
  return 1;
}

int kopieren(string str)
{
  string was, alt, n, *neu, *par, err;
  mixed das;
  mapping com;

  if (!tp_owner_check())
    return 0;

  notify_fail("'kopiere detail <von> nach <nach>' oder\n"
         +"'kopiere lesbares detail <von> nach <nach>' oder\n"
         +"'kopiere befehl <befehl> [<parameter>] nach <befehl> [<parameter>]'!\n");

  if (!(str=UP_ARGS(this_player())) || str == "")
    return 0;

  neu = old_explode(str, " ");
  was = neu[0][0..5];
  if (was == "detail" || was == "befehl")
    str = implode(neu[1..], " ");
  else if (was == "lesbar")
    str = implode(neu[2..], " ");
  else
    return 0;

  if (sscanf(str, "%s nach %s", alt, n) != 2)
    return 0;

  neu = brk(n);
  switch(was) {
    case "detail":
      err = "Detail";
      if (das = GetDetail(alt)) {
    AddDetail(neu, das);
    Save();
      }
      break;
    case "lesbar":
      err = "lesbares Detail";
      if (das = QueryProp(P_READ_DETAILS)[alt]) {
    AddReadDetail(neu, das);
    Save();
      }
      break;
    case "befehl":
      err = "Befehl";
      was = (par=old_explode(alt, " "))[0];
      if (member(com=QueryProp(H_COMMANDS),was)) {
    int i;
    if (sizeof(par) == 1) { // <bef> nach <bef1,bef2,...>
      das = com[was];
      for (i=sizeof(neu)-1; i>=0; i--) {
        if (befCheck(neu[i])) {
          if (com[neu[i]])
        com[neu[i]] += das;
          else
        com += ([ neu[i] : das ]);
        }
        else
          write("Ungueltiger Befehl: '"+neu[i]+"'.\n");
      }
    }
    else {        // <bef> <parameter> nach <bef1,bef2,...>
      alt = implode(par[1..]-({""})," ");
      if (das = com[was][alt]) {
        for (i=sizeof(neu)-1; i>=0; i--) {
          if (befCheck(neu[i])) {
        das = ([ alt : com[was][alt];com[was][alt,1] ]);
        if (com[neu[i]])
          com[neu[i]] += das;
        else
          com += ([ neu[i] : das ]);
          }
          else {
        par = old_explode(neu[i], " ");
        n = par[0];
        if (befCheck(n)) {
          das = ([ implode(par[1..], " ") : com[was][alt];com[was][alt,1] ]);
          if (com[n])
            com[n] += das;
          else
            com += ([ n : das ]);
        }
        else
          write("Ungueltiger Befehl: '"+neu[i]+"'.\n");
          }
        }
      }
    }
    Save();
      }
      break;
    default:
      write( "Du kannst nur Details, lesbare Details und Befehle kopieren!\n" );
      return 1;
  }
  if (!das)
    printf( "Kann %s '%s' nicht finden!\n", err, alt);
  else
    write( "OK!\n" );

  return 1;
}

int licht(string str)
{
  int ll, tl;

  if (!allowed_check(this_player()))
    return 0;

  if (!str || (str != "an" && str != "aus")) {
    notify_fail("Syntax: 'licht an' oder 'licht aus'\n");
    return 0;
  }

  ll = QueryProp(P_LIGHT);
  tl = PL->QueryProp(P_PLAYER_LIGHT);

  switch(str) {
  case "an":
    if (tl > 0)
      write("Aber es ist doch schon hell!\n");
    else {
      SetProp(P_LIGHT, 1);
      tell_room(this_object(), "Es wird wieder hell.\n");
    }
    break;
  case "aus":
    if (tl <= 0)
      write("Aber es ist doch schon dunkel!\n");
    else {
      SetProp(P_LIGHT, 0);
      tell_room(this_object(), "Es wird dunkel.\n");
    }
    break;
  }
  return 1;
}

#define CASUS ({ "WER", "WESSEN", "WEM", "WEN" })
static string rpXchg(string s)
{
  int c,p,g;

  switch(s[0..1]) {
    case "@W":
      c = to_int(s[2..2]);
      return ("@"+CASUS[c]);
    case "@P":
      c = to_int(s[2..2]);
      return ("@P"+CASUS[c]);
    case "@B":
      c = to_int(s[2..2]);
      g = to_int(s[3..3]);
      p = to_int(s[4..4]);
      return ("@B"+({"N", "M", "F"})[g]+({"S", "P"})[p]+CASUS[c]);
    case "@F":
      return "@PARA";
  }
  return s;
}

private string reParse(string s1, string s2)
{
  string *p;

  if (s2)
    s1 = s1+"@@\n"+s2;

  p = regexplode(s1, "(@W[0-3]|@P[0-3]|@B[0-3][0-2][0-1])");
  p = map(p, #'rpXchg);
  return implode(p, "");
}

private string getPreText(string prop, string expr)
{
  mixed crunched;
  int i;

  crunched = VERWALTER->PCrunch(QueryProp(prop));
  if (!crunched || !pointerp(crunched))
    return 0;

  if (prop == H_COMMANDS && strstr(expr, " ") < 0)
    expr = expr+" ";

  for (i=sizeof(crunched)-1; i>=0; i--)
    if (member(crunched[i][0], expr) >= 0)
      break;

  if (i<0)
    return 0;

  detail = crunched[i][0];

  if (prop == H_COMMANDS)
    return reParse(crunched[i][1], crunched[i][2]);
  else
    return crunched[i][1];
}

varargs int
aendern(string str, int f)
{
  string *parts, pre;
  int sp, sr, ret;

  if (!tp_owner_check())
    return 0;

  if (!f && (!(str=UP_ARGS(this_player())) || str == "")) {
    notify_fail("Was willst Du denn aendern?\n" );
    return 0;
  }

  sp = sizeof(parts = old_explode(str, " "));
  sr = sizeof(brk(str));
  detail = 0;
  flag = f;

  switch(parts[0][0..3]) {
    case "raum":      // Lang- oder Kurzbeschreibung
    case "haus":
      if (sp == 1 || parts[1] == "lang")
    flag |= LANG;
      else if (parts[1] == "kurz") {
    write("Nimm dazu doch bitte 'beschreibe'!\n");
    return 1;
      }
      pre = ((flag & AUSSEN) ? (find_object(HAUSNAME(owner)))->QueryProp(P_LONG) : QueryProp(P_INT_LONG));
      break;
    case "meld":
      if (file_size(REPFILE(owner)) > 0)
    pre = read_file(REPFILE(owner));
      else {
    write("Ich finde keine Meldungen aus Deinem Haus!\n");
    return 1;
      }
      flag |= REPORT;
      break;
    case "deta":      // Details
      if (sp==1) {
    notify_fail("Welches Detail willst Du denn aendern?\n");
    return 0;
      }
      if (sr>1) {
    notify_fail("Du kannst immer nur ein Detail aendern!\n");
    return 0;
      }
      flag |= DETAIL;
      pre = getPreText(P_DETAILS, implode(parts[1..], " "));
      break;
    case "lesb":      // lesbare Details
      notify_fail("Welches lesbare Detail willst Du denn aendern?\n");
      if (sp == 1) return 0;
      if ((parts[1] == "details" || parts[1] == "detail") && (sp==2))
    return 0;
      if (sr>1) {
    notify_fail("Du kannst immer nur ein lesbares Detail aendern!\n");
    return 0;
      }
      flag |= RDETAIL;
      pre = getPreText(P_READ_DETAILS, implode(parts[1..], " "));
      break;
    case "befe":      // Befehle
      ret = 0;
      if (sp == 1) {
    notify_fail("Welchen Befehl willst Du denn aendern?\n");
    return 0;
      }
      if (sr>1) {
    notify_fail("Du kannst immer nur einen Befehl aendern!\n");
    return 0;
      }
      flag |= BEFEHL;
      pre = getPreText(H_COMMANDS, implode(parts[1..], " "));
      break;
    default:
      notify_fail("Das kannst Du nicht aendern! Eine Liste der Dinge, die Du hier aendern\n"
         +"kannst, erhaeltst Du mit 'hilfe aendere'.\n" );
      return 0;
      break;
  }
  if (!pre)
    write("Hm, sowas ist hier noch nicht beschrieben...\n");
  else {
    write( "Aendere nun den Text.\n(Beenden mit . oder **, Abbruch mit ~q, Hilfe mit ~h)\n" );
    nedit( "aenderung", pre );
  }
  return 1;
}

void aenderung(string str)
{
  string *warn;

  if (!str) {
    write("Nichts geaendert!\n");
    return;
  }

  if (flag && !(flag & BEFEHL))
    str = normstr(str);

  warn = ({ });

  if (flag & LANG) {
    if (flag & AUSSEN) {
      object haus;
      haus = find_object(HAUSNAME(owner));
      haus->SetProp(P_LONG, str);
      haus->Save();
    }
    else
      SetProp(P_INT_LONG, str);
  }
  else if (flag & DETAIL) {
    if (str == "")
      RemoveDetail(detail);
    else
      AddDetail(detail, str);
  }
  else if (flag & RDETAIL) {
    if (str == "")
      RemoveReadDetail(detail);
    else
      AddReadDetail(detail, str);
  }
  else if (flag & BEFEHL) {
    if (str == "")
      RemUserCmd(detail);
    else {
      string *s;

      s = old_explode(preparse(str, &warn), "@@\n");
      if (sizeof(s) > 1 && s[1] != "")
    AddUserCmd(detail, 0, normstr(s[0]), normstr(s[1]));
      else
    AddUserCmd(detail, 0, normstr(s[0]), 0);
    }
  }
  else if (flag & REPORT) {
    rm(REPFILE(owner));
    if (str != "")
      write_file(REPFILE(owner), str);
  }
  else
    write( "Huch! Unbekanntes Flag ("+flag+")... Sag mal Wargon Bescheid...\n");

  arr_out(warn, "OK.", "WARNUNG! Ungueltiger Platzhalter: %s",
               "WARNUNG! Ungueltige Platzhalter: ");
  Save();
}

int SmartLog(string ofile, string typ, string msg, string date)
{
  object home;

  // speichere Meldung im Rep-Log des Seherhaus-Besitzers
  write_file(REPFILE(owner), sprintf("%s von %s in Raum %d (%s):\n%s\n",
                     typ,
                     capitalize(getuid(this_player())),
                     raumNr,
                     date,
                     break_string(msg,78)));

  if (IS_LEARNER(owner)) {
    log_file("report/"+owner+".rep",
         sprintf("MELDUNG von %s im Seherhaus, Raum %d (%s):\n"
             +"Bitte zur Kenntnis nehmen! (Mit dem Befehl 'meldungen')  -Wargon\n",
             capitalize(getuid(this_player())),
             raumNr,
             date));
  }

  // erhoehe im Hauptraum den Rep-Zaehler und speichere
  home = load_object(RAUMNAME(owner,0));
  home->Set(H_REPORT, home->Query(H_REPORT)+1);
  home->Save();

  return 1;
}

static int report(string str)
{
  string rep, *lines;
  int rNum, l, s;

  if (!allowed_check(this_player()))
    return 0;

  if (file_size(REPFILE(owner)) <= 0) {
    write( "Keine Meldungen zu finden... Du bist wunschlos gluecklich.\n" );
    return 1;
  }
  rep = read_file(REPFILE(owner));

  if (!rep) {
    write( "Oha! Die Datei mit den Meldungen ist zu gross! Sag doch bitte mal\n"
      +"Wargon Bescheid!\n");
    return 1;
  }

  if (str) {
    string d, *new, *tmp, prev;
    int nr, nextNr, m;

    if (str == "hier")
      rNum = raumNr;
    else
      rNum = to_int(str);

    if (rNum > VERWALTER->HausProp(owner, HP_ROOMS)) {
      write( "So viele Raeume hast Du gar nicht!\n");
      return 1;
    }

    lines = old_explode(rep, "\n");
    s = sizeof(lines);
    for (l=0; prev == 0; l++)
      if (sscanf(lines[l], "%s von %s in Raum %d %s:", d, d, nr, d)==4)
    prev=lines[l];

    for ( new = ({}), tmp=({}); l<s; l++) {
      m=sscanf(lines[l], "%s von %s in Raum %d %s:", d, d, nextNr, d);
      if (m != 4 && nr == rNum)
    tmp += ({ lines[l] });

      if (m==4) {
    if (sizeof(tmp)) {
      new = new + ({ prev }) + tmp;
      tmp = ({});
    }
    nr = nextNr;
    prev = lines[l];
      }
    }
    if (sizeof(tmp))
      new = new + ({prev}) + tmp;
    rep = implode(new, "\n");
  }

  this_player()->More(rep);
  return 1;
}

// $Log: raum.c,v $
// Revision 1.5  2003/11/15 14:03:58  mud
// Lichtaenderungen von Zook
//
// Revision 1.4  2003/02/17 20:00:00  mud
// Im Reset wird nun getestet, ob der Raum einen Ausgang in einen Null-Raum
// hat. Dies wurde notwengig, damit Spieler nicht in Seherhaeuser eingesperrt 
// werden koennen. Die Funktionen AddExit(), RemoveExit() und Reset starten 
// gegebenenfalls den Ausgangstest. Einige Funs mussten leicht angepasst 
// werden.
// Die Funktionen room_has_exit() und is_exit() wurden von Vardion@MG 
// entwickelt und zur Verfuegung gestellt. - Vanion
//
// Revision 1.3  2001/02/04 21:21:34  mud
// (brk,getBefParam): Vorkehrungen gegen fuehrende und schliessende
// Leerzeichen in Befehlsparametern und anderen Listen.
//
// Revision 1.2  2001/01/01 18:17:47  mud
// (ausgang): Wenn ein Ausgang zu einem anderen Seherhaus gelegt wird,
// wird die Erlaubnis in dessen Eingangsraum abgefragt, und nicht in
// dem angeforderten Zielraum (der Eingangsraum existiert auf jeden
// Fall, der Zielraum vielleicht nicht).
//
// Revision 1.1.1.1  2000/08/20 20:22:42  mud
// Ins CVS eingecheckt
//
// 04.02.98 Meldungen koennen geloescht werden.
//
// Revision 2.16  1997/11/15 19:33:23  Wargon
// arr_out(), preparse(): kleine Bugfixes
//
// Revision 2.15  1997/10/06 15:24:38  Wargon
// Unsichtbare Magier melden/anzeigen
// Meldung beim Betreten abgeschlossener Haeuser fuer Magier
//
// Revision 2.14  1996/02/21  18:12:47  Wargon
// SmartLog() rein, dafuer die eigenen Rueckmeldungsbefehle raus
//
// Revision 2.13  1995/10/31  12:56:16  Wargon
// Rueckmeldungen fuer Objekte werden ans Spielerobjekt weitergegeben.
//
// Revision 2.12  1995/08/07  18:35:12  Wargon
// Einige Bugs bei "aendere" behoben.
//
// Revision 2.11  1995/06/29  08:57:05  Wargon
// Hausbesitzer, die schon Magier sind, bekommen bei Rueckmeldungen auch einen
// Eintrag in ihr /log/report/xyz.rep-File
// "licht an/aus" ist seit 2.9 drin ;)
//
// Revision 2.10  1995/06/28  08:59:57  Wargon
// Neue Befehle aendere, meldungen fuer den Hausbesitzer.
// typo, bug/fehler, idee werden dem Haus-.rep-File zugefuehrt.
// Jetzt koennen die Seher ihre Typos selber fixen! ;^)
//
// Revision 2.9  1995/06/20  07:49:15  Wargon
// *** empty log message ***
//
// Revision 2.8  1995/04/21  10:48:39  Wargon
// Bugfix in beschreiben(), wenn die Hausaussenbeschreibung
// verlangt wird (war schon seit Ewigkeiten buggy... ;)
//
// Revision 2.7  1995/04/21  08:55:32  Wargon
// Load()/Save() und eigene Kommandos ausgelagert.
// Kommandos koennen mit notify_fail() versehen werden.
//
// Revision 2.6  1995/03/07  13:55:36  Wargon
// Add/RemUserCmd(), Beschreibungen werden bei reset()/clean_up()
// gepackt gespeichert.
// Bei Kommandos nur noch more(), wenn es auch noetig ist.
//
// Revision 2.5  1995/02/27  20:48:26  Wargon
// Kleine Schoenheitsfehler in selbstdefinierten Befehlen beseitigt.
//
// Revision 2.4  1995/02/22  21:30:52  Wargon
// Noch mehr Aenderungen an den Befehlen:
// - Preparsing der Platzhalter
// - Platzhalter fuer Possessivpronomen
// - Meldung fuer Ausfuehrenden wird geMore()t
// - Rassen- und Geschlechtespezifische Meldungen moeglich
// - Auch fuer Ausgaenge koennen Befehle definiert werden
//   (nur fuer existierende; wird der Ausgang gesperrt, wird auch
//   der Befehl geloescht)
// Im Zuge des Preparsings hat sich die Befehlauswertung etwas
// vereinfacht.
//
// Revision 2.3  1995/02/20  22:15:44  Wargon
// READ_DETAILS werden jetzt mit More() ausgegeben.
// Selbstdefinierte Befehle: mit @PWER, ... koennen die Personalpronomina
// eingebaut werden; Einbau jetzt auch in die Meldung fuer den Ausloeser
// moeglich; _unparsed_args() in der Auswertung.
//
// Revision 2.2  1995/02/15  11:23:04  Wargon
// NEU: Selbstdefinierbare Befehle.
//
// Revision 2.1  1995/02/04  15:02:36  Wargon
// Die Truhe wird nun ueber die Property CHEST verwaltet. Der AddItem()-
// Aufruf wurde deshalb von create() nach Load() verlegt. Geladen wird
// sie nur, wenn das Load() von Hausverwalter erfolgte.
// clean_up(), wenn Raum leer ist oder nur eine leere Truhe drin steht.
//
// Revision 2.0  1995/02/01  20:36:49  Wargon
// Entruempelt und Massnahmen fuer _unparse_args() getroffen.


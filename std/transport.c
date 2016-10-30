// MorgenGrauen MUDlib
//
// transport.c -- Basisklasse fuer Schiffe und aehnliche Transporter
//
// $Id: transport.c 9400 2015-12-11 21:56:14Z Zesstra $
#pragma strong_types,rtt_checks
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/thing/moving";
inherit "/std/room";

#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <language.h>
#include <transport.h>
#include <regexp.h>

/* transport.c
 * 
 * Ueberarbeitete und 
 * erweiterte Version : Tilly@MorgenGrauen, 10.01.02
 * Basierend auf      : transport.c@SilberLand (Woody@SilberLand), 05.12.99
 * Basierend auf      : Hates und Rumatas generisches Transport Objekt
 *                      MorgenGrauen 15.02.93
 */
     
/*
 ********************* Variablen *********************
 */

// TODO: langfristig waer ja private schoen...
//
// Datenstruktur von 'route' (bei HP_ROOM)
// 0 ({string ID,      : HP_ROOM
// 1   string room,    : Dateiname Zielraum
// 2   int stay,       : Dauer Haltezeit
// 3   int next,       : Dauer naechste Fahrtzeit
// 4   string code,    : Haltestellenname fuer QueryArrived
// 5   mixed dest,     : Haltestellen-IDs fuer HasRoute (reise nach)
// 6   mixed deststr }): unbenutzt.
//
// Datenstruktur von 'route' (bei HP_MSG, HP_FUN)
// 0 ({string ID,      : HP_MSG
// 1   string message, : Meldung    oder    string fun : Funktionsname
// 2   int next})      : Dauer bis zum naechsten Ereignis
nosave mixed *route;    /* Liste der Haltepunkte. */
nosave int rpos;        /* Momentane Position in obiger Liste. */
nosave string roomCode; /* Code des aktuellen Raumes (oder 0). */

/*
 ********** Management der builtin-properties **********
 */

string _query_short()
{ 
  if (roomCode) return Query(P_SHORT); 
  return 0; 
}

mixed _query_transparent()
{ 
  if (roomCode) return Query(P_TRANSPARENT);
  return 0; 
}

mixed *_set_route(mixed *r) { return route = r; }
mixed *_query_route()       { return route; }

/*
 **************** Zugriffsfunktionen ***************
 */

void Halt()
{
  while (remove_call_out( "changeHp" )>-1);
  while (remove_call_out( "disconnect" )>-1);
}

// Aktualisiert/Setzt die Route im TravelD, wenn erlaubt (d.h. kein
// P_NO_TRAVELING)
private void ReportRoute()
{
  if(!QueryProp(P_NO_TRAVELING))
  {
    mixed tmp = filter(route, function int (mixed arr)
        {
          return arr[0] == HP_ROOM;
        } );
    string *route = map(tmp, function string (mixed arr)
        { return arr[1]; }
        );
    TRAVELD->AddRoute(object_name(this_object()),route);
  }
}

varargs void Start(int pos)
{
  Halt();
  rpos = (pos >= sizeof(route))?-1:pos-1;
  call_out("changeHp",0);
  // Tell TRAVELD our current route
  ReportRoute();
}

void SetTravelCmds()
{
  if (pointerp(QueryProp(P_LEAVECMDS)))
     AddCmd(QueryProp(P_LEAVECMDS),"GoOutside");
  if (pointerp(QueryProp(P_ENTERCMDS)))
    AddCmd(QueryProp(P_ENTERCMDS),"GoInside");
  if (pointerp(QueryProp(P_TRAVEL_CMDS)))
    AddCmd(QueryProp(P_TRAVEL_CMDS),"GoInAndOutside");
  return;
}

mixed HasRoute(mixed dest)
{
  int i,s,z;
  string str;
  object ob;
  mixed harb;
  
  s = sizeof(route);

  for (i = rpos;i <= rpos+s-1;i++)
  {
    if (route[i%s][0] == HP_ROOM)
    {
      if (member(route[i%s][5],dest) != -1 &&
          objectp(ob=load_object(route[i%s][1])) &&
          pointerp(harb=ob->QueryProp(P_HARBOUR)) &&
          sizeof(harb))
      {
        return ({ route[i%s][1], harb[0] });
      }
    }
  }
  return 0;
}

public varargs void AddRoute(string room, int stay, int next, 
    string harbour_desc, string|string* dest_ids, string deststr)
{
  // Daten aus dem Zielanleger abfragen.
  <string|string*>* harbour = room->QueryProp(P_HARBOUR)||({});
  string* harbour_ids = ({});

  // IDs des Zielanlegers fuer Syntaxpruefung 
  if ( sizeof(harbour)==2 )
  {
    if ( pointerp(harbour[1]) )
      harbour_ids = harbour[1];
    else
      harbour_ids = ({harbour[1]});
  }
  
  // <dest_ids> in ein Array umwandeln, ist dann ggf. leer
  if ( !dest_ids )
  {
    dest_ids = ({});
  }
  if ( stringp(dest_ids) )
  {
    dest_ids = ({dest_ids});
  }

  // explizit angegebene IDs stehen jetzt in <dest_ids>, die IDs des 
  // Zielhafens aus P_HARBOUR werden addiert.
  dest_ids += harbour_ids;

  // Ist <dest> immer noch leer, versuchen wir, aus <harbour_desc> ein paar
  // Stichwoerter zu erzeugen, die man als Zielangabe in der Syntax 
  // "reise nach <ziel>" verwenden kann.
  if ( !sizeof(dest_ids) )
  {
    // Grossgeschriebene Begriffe in <harbour_desc> in <dest> eintragen. Dazu:
    // 1) <code> erstmal so zerschneiden, dass alle ueblichen Satzzeichen
    // rausfliegen (es gibt Transporter, die sowas in <harbour_desc> 
    // uebergeben).
    dest_ids = regexplode(harbour_desc, "[(),.;:&\+_ ]", 
                          RE_OMIT_DELIM|RE_GLOBAL);
    // 2a) So filtern, dass nur grossgeschriebene Woerter uebrig bleiben,
    //     von 1) uebriggebliebene Leerstrings gleich mit wegwerfen.
    // 2b) Ergebnis kleinschreiben, damit die Syntaxpruefung damit arbeiten
    //     kann.
    dest_ids = map( filter(dest_ids, function int (string key) { 
                  return (key!="" && key[0]>='A' && key[0]<='Z');
               }), #'lower_case);
  }
  // Sollte <dest> jetzt immer noch leer sein, wurde an allen drei Stellen
  // nichts oder nur Muell uebergeben.
  if ( !sizeof(dest_ids) )
  {
    raise_error("Transporterfehlfunktion in AddRoute(): Identifikations"
      "matrix unzureichend definiert. Transporter unbenutzbar fuer "
      "Spieler. Bitte mindestens eine Ziel-ID via P_HARBOUR oder als "
      "Argument to AddRoute().");
  }
  route += ({ ({ HP_ROOM, room, stay, next, harbour_desc, dest_ids, 
                 deststr }) });
}

varargs void AddMsg(string msg, int next) 
{
  route += ({ ({ HP_MSG, msg, next }) }); 
}

void AddFun(string fun, int next) { route += ({ ({ HP_FUN, fun, next }) }); }

string QueryArrived() { return roomCode; }

mixed* QueryPosition()
{
  return ({ route[rpos][1],route[(rpos+1)<sizeof(route)?(rpos+1):0][1] });
}

object* QueryPassengers()
{
  return filter(all_inventory(),#'query_once_interactive); 
}

varargs string *QueryHarbours(int textflag)
{
  string *ret = ({});

  foreach( mixed* entry : route )
  {
    if ( entry[0] == HP_ROOM )
    {
      if ( textflag )
      {
        string *hp_ids = entry[1]->QueryProp(P_HARBOUR)[1];
        if (pointerp(hp_ids) && sizeof(hp_ids))
        {
          string *h = map( explode(hp_ids[0]," "), #'capitalize);
          ret += ({ implode(h, " ") });
        }
      }
      else
      {
        ret += ({ entry[1] });
      }
    }
  }
  return ret;
}

// beim zerstoeren sollte auch die route und der Transporter aus dem traveld
// abgemeldet werden.
public varargs int remove(int silent)
{
  TRAVELD->RemoveTransporter(this_object());
  return ::remove(silent);
}

void RemoveRoute()
{
  Halt();
  route = ({ });
  rpos  =   0;
  TRAVELD->RemoveTransporter(this_object());
}

varargs int Enter(object who)
{
  string *emsg;
  mixed efail;

  if (!objectp(who)) who = this_player();
  if (environment(who) == this_object())
  {
    tell_object(who,"Da bist Du doch bereits, schon vergessen?\n");
    return 1;
  }
  if (!QueryArrived()) return 0;
  if (QueryProp(P_MAX_PASSENGERS) && 
     (sizeof(QueryPassengers()) >= QueryProp(P_MAX_PASSENGERS)))
  {
    if (pointerp(efail=QueryProp(P_ENTERFAIL)))
    {
      if (sizeof(efail) == 2)
        tell_room(this_object(),who->Name(WER,2)+" "+process_string(efail[1])+
                                                   ".\n",({who}));      
      tell_object(who,process_string(efail[0])+".\n");
    }
    else if (stringp(efail))
       tell_object(who,process_string(efail)+".\n");
    else if (closurep(efail)) funcall(efail);
    return 1;
  }
  
  tell_object(who,"Du betrittst "+name(WEN,1)+".\n");
  if (pointerp(emsg=QueryProp(P_ENTERMSG)) && sizeof(emsg) == 2)
     return who->move(this_object(),M_GO,"",process_string(emsg[0]),
                                             process_string(emsg[1]));
  return who->move(this_object(),M_GO,
        name(WEN,1),"betritt","kommt herein");
}

varargs int Leave(object who)
{
  string *lmsg;
  mixed lfail;

  if (!objectp(who)) who = this_player();
  if (environment(who) != this_object())
  {
    if (QueryArrived())
    {
      tell_object(who,"Dafuer muesstest Du erstmal dort sein.\n");
      return 1;
    }
    return 0;
  }
  if (!QueryArrived())
  { 
    if (lfail=QueryProp(P_LEAVEFAIL))
    {
      if (pointerp(lfail) && sizeof(lfail))
      {
        if (sizeof(lfail) == 2)
           tell_room(this_object(),who->Name(WER,2)+" "+process_string(
               lfail[1])+".\n",({who}));
        tell_object(who,process_string(lfail[0])+".\n");
      }
      else if (stringp(lfail))
        tell_object(who,process_string(lfail)+".\n");
      else if (closurep(lfail)) funcall(lfail);
      return 1;
    }
    tell_object(who,"Fehler beim Verlassen des Transporters.\n"
                    "Bitte zustaendigen Magier verstaendigen.\n");
    return 1;
  }

  if (who->QueryProp(P_TRAVEL_INFO)) who->SetProp(P_TRAVEL_INFO,0);
  tell_object(who,"Du verlaesst "+name(WEN,1)+".\n");
  if (pointerp(lmsg=QueryProp(P_LEAVEMSG)) && sizeof(lmsg) == 2)
      return who->move(environment(),M_GO,"",process_string(lmsg[0]),
                                             process_string(lmsg[1]));
  return who->move(environment(),M_GO,
           name(WEN,1),"verlaesst","kommt herein");
}

/*
 ****************** Internal Functions ******************
 */

static int GoInside(string str)
{
  _notify_fail("Was moechtest Du denn genau?\n");
  if (stringp(str) && id(str)) {
      Enter();
      return 1;
  }
  return 0;
}

static int GoOutside(string str)
{
  _notify_fail("Was moechtest Du denn genau?\n");
  if (stringp(str) && id(str)) {
      Leave();
      return 1;
  }
  return 0;
}

static int GoInAndOutside(string str)
{
  string to;

  _notify_fail("Was moechtest Du denn genau?\n");
  if (!sizeof(str)) return 0;  
  if ((sscanf(str,"auf %s",to) == 1 || sscanf(str,"in %s",to) == 1) && id(to))
    return Enter(),1;
  if ((sscanf(str,"von %s",to) == 1 || sscanf(str,"aus %s",to) == 1) && id(to))
    return Leave(),1;
  return 0;
}

protected void create()
{
  ::create();

  route = ({});

  SetProp(P_LEAVEFAIL,"Das ist momentan viel zu gefaehrlich");
  SetProp(P_ENTERFAIL,"Dort ist kein Platz mehr fuer Dich");
  SetProp(P_TRANSPARENT,1);

  AddId("Transporter");
  
  call_out("SetTravelCmds",1);
}

static varargs void disconnect(int change, int change_time)
{
  object room;
  mixed *departmsg;

  departmsg = QueryProp(P_DEPARTMSG);

  if ((room = environment()) && pointerp(departmsg))
  {
    tell_room(this_object(),process_string(departmsg[0]));
    tell_room(room,process_string(departmsg[1]));
  }

  roomCode = 0;

  if (change) call_out("changeHp",change_time);
}

static varargs void connect(string room, string code)
{
  mixed *arrivemsg, *t;
  object *trav, ob;
  string *trs, *msgs;
  int i;

  if (roomCode) disconnect();
  
  roomCode = code?code:"";

  if (catch(move(room,M_SILENT|M_NOCHECK);publish))
  {
    roomCode = 0;
    return;
  }

  arrivemsg = QueryProp(P_ARRIVEMSG);

  if (pointerp(arrivemsg))
  {
    tell_room(this_object(),process_string(arrivemsg[0]));
    tell_room(room,process_string(arrivemsg[1]));
  }

  trav = filter(all_inventory(this_object()),#'living);

  i = sizeof(trav);
  while(i--)
  {
    if (pointerp(t = trav[i]->QueryProp(P_TRAVEL_INFO))&&
        t[0]==this_object()&&t[2]==room)
    { 
      if (trav[i]->InFight())
        tell_object(trav[i],break_string("Du solltest Deinen Kampf "
                "schnell beenden,denn eigentlich wolltest Du hier "
                "aussteigen.",78));
      else
        Leave(trav[i]);
      if (environment(trav[i])!=this_object())
        trav[i]->SetProp(P_TRAVEL_INFO,0);
    }
  }
  trav = filter(all_inventory(find_object(room))-trav,#'living);
  i=sizeof(trav);
  while(i--)
  {
    if (objectp(trav[i]) && pointerp(t = trav[i]->QueryProp(P_TRAVEL_INFO))&&
        t[0] == environment(trav[i]) && t[1] == this_object())
    {
      if ( trav[i]->InFight() )
        tell_object(trav[i],
           break_string("Du solltest Deinen Kampf schnell beenden, denn "
                        "eigentlich wolltest Du mit "+name(WEM,1)+
                        " reisen.",78));
      else
        Enter(trav[i]);
      if (environment(trav[i]) == this_object()) 
      {
        t[0] = this_object();
        trav[i]->SetProp(P_TRAVEL_INFO,t);
      }
    }
  }
}

// this object never performs any clean-up, the driver should not call it
// again.
int clean_up(int arg) { return 0; }

void changeHp()
{
  if (++rpos == sizeof(route))
  {
    rpos = 0;
    //TRAVELD die aktuelle Route uebermitteln
    ReportRoute();
  }
  if (route[rpos][0] == HP_MSG)
  {
    call_out("changeHp",route[rpos][2]);
    tell_room(this_object(),route[rpos][1]);
  }
  else if (route[rpos][0] == HP_FUN)
  {
    call_out("changeHp",route[rpos][2]);
    call_other(this_object(),route[rpos][1]);
  }
  else 
  {
    call_out("disconnect",route[rpos][2],1,route[rpos][3]);
    connect(route[rpos][1],route[rpos][4]);
  }
}

void __restart(string funname)
{
  if (!funname || funname == "" || (funname != "changeHp" && 
                                    funname != "disconnect"))
    return;
  while(remove_call_out(funname) != -1);
  call_out(funname,funname == "changeHp"?15:5);
}

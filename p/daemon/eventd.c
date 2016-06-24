// MorgenGrauen MUDlib
//
// /p/daemon/eventd.c -- Event Dispatcher
//
// $Id$
#pragma strict_types,save_types
#pragma no_clone
#pragma no_shadow
#pragma pedantic
#pragma range_check

#include <wizlevels.h>
#include <defines.h>
#include <events.h>

// Fuer Statistiken
#define STATISTICS

#define HOME(x) (__PATH__(0)+x)
#define STORE HOME("save/eventd")


#define LOG(x) log_file("EVENTS", sprintf(\
      "[%s] %s\n",dtime(time()),x))
//#define LOGEVENT(x,y,z) log_file("EVENTS", sprintf(\
//      "[%s] Event %s triggered by %O (Args: %.40O)\n",dtime(time()),x,y,z))
#define LOGEVENT(x,y,z)
//#define LOGREGISTER(w,x,y,z) log_file("EVENTLISTENER",sprintf(\
//      "[%s] %O (Fun: %.25s) registered for %s by %O\n",dtime(time()),w,x,y,z))
#define LOGREGISTER(w,x,y,z)
//#define LOGUNREGISTER(x,y,z) log_file("EVENTLISTENER",sprintf(\
//      "[%s] %O was unregistered from %s by %O\n",dtime(time()),x,y,z))
#define LOGUNREGISTER(x,y,z)
//#define LOGEVENTFINISH(x,y) log_file("EVENTS", sprintf(\
//      "[%s] Event %s (triggered by %O) finished\n",dtime(time()),x,y))
#define LOGEVENTFINISH(x,y)

#ifndef DEBUG
#define DEBUG(x)  if (find_player("zesstra"))\
          tell_object(find_player("zesstra"),\
                      "EDBG: "+x+"\n")
#endif

#define TICK_RESERVE 300000
#define TICKSPERCALLBACK 30000

// Indizes fuer Callback-Infos (events, active)
#define CB_FUN        0
#define CB_CLOSURE    1
#define CB_OBJECT     2
// Indizes fuer Pending
#define P_EID         0
#define P_TRIGOB      1
#define P_TRIGOBNAME  2
#define P_ARGS        3
#define P_TIME        4

// Indizes fuer Active
#define A_EID         0
#define A_TRIGOB      1
#define A_TRIGOBNAME  2
#define A_ARGS        3
#define A_LISTENERS   4
#define A_TIME        5

/* alle events, die er kennt.
   Datenstruktur events:
   ([id:([obname:fun;closure;object, ... ]),
     id2: ([....]),
   ]) */
mapping events=([]);
/* abzuarbeitende Events, Datenstruktur:
   ({ ({id, trigob, trigobname, args}), 
      ({id2, trigob, trigobname, args}), ... })  */
nosave mixed pending=({});
/* Der gerade aktive Event, der wird gerade abgearbeitet.
   Datenstruktur active:
   ({ id:trigob;trigobname;args;([obname:fun;closure;object, ...]), trigtime})
*/
nosave mixed active=({});

int lastboot; // Zeitstempel des letzten Reboots
// Flag, wenn gesetzt, zerstoert sich der Eventd, wenn keine Events
// abzuarbeiten sind.
nosave int updateme; 

// einfache Statistik, gespeichert wird ein Histogramm. KEys sind die
// Zeitdifferenzen zwschen Eintragen und Erledigen des Events, Values die
// Haeufigkeiten.
nosave mapping stats=([]);


protected void process_events();
protected void save_me();
varargs int remove(int silent);

// ist der Event-Typ "eid" schon bekannt, d.h. gib es min. 1 Listener?
// Liefert Anzahl der Listener zurueck.
int CheckEventID(string eid) {
    if (!stringp(eid) || !member(events,eid))
        return 0;
    return(sizeof(events[eid]));
}

// entscheidet, ob ein Objekt fuer einen Event eingetragen werden darf. Zum
// Ueberschreiben in geerbten Dispatchern, in diesem Scheduler sind alle
// Events oeffentlich. Bekommt die Event-ID, das einzutragende Objekt und das
// eintragende Objekt uebergeben.
// 1 fuer Erlaubnis, 0 sonst.
protected int allowed(string eid, object ob, object po) {
    return(1);
}

// entscheidet, ob ein Objekt einen bestimmten Event triggern darf. Zum
// Ueberschreiben in geerbten Dispatchern, in diesem Scheduler sind alle
// Events oeffentlich. Bekommt die Event-ID und das triggernde Objekt
// uebergeben.
// 1 fuer Erlaubnis, 0 sonst.
protected int allowedtrigger(string eid, object trigger) {
    return(1);
}

// registriert sich fuer einen Event. Wenn es den bisher nicht gibt, wird er
// implizit erzeugt. Wenn das Objekt ob schon angemeldet war, wird der
// bisherige Eintrag ueberschrieben.
// 1 bei Erfolg, <=0 bei Misserfolg
int RegisterEvent(string eid, string fun, object ob) {
    object po;
    if (!stringp(eid) || !stringp(fun) || 
        !objectp(ob) || !objectp(po=previous_object()))
        return -1;
    if (!allowed(eid, ob, po)) return -2;
    closure cl=symbol_function(fun,ob);
    if (!closurep(cl))
        return -3;
    if (!mappingp(events[eid]))
        events[eid]=m_allocate(1,3); // 3 Werte pro Key
    events[eid]+=([object_name(ob):fun;cl;ob]);
//    if (find_call_out(#'save_me)==-1)
//        call_out(#'save_me,15);
    LOGREGISTER(ob,fun,eid,po);
    return 1;
}

// entfernt das Objekt als Listener aus dem Event eid
// Mehr oder weniger optional, wenn ein event verarbeitet wird und das Objekt
// ist nicht mehr auffindbar, wird es ebenfalls geloescht. Bei selten
// getriggerten Events muellt es aber bis dahin den Speicher voll.
// +1 fuer Erfolg, <= 0 fuer Misserfolg
int UnregisterEvent(string eid, object ob) {
    object po;
    if (!stringp(eid) || !objectp(ob) || 
        !objectp(po=previous_object()) || !mappingp(events[eid]))
        return -1;
    string oname=object_name(ob);
    if (!member(events[eid],oname)) 
        return -2;
    m_delete(events[eid],oname);
    if (!sizeof(events[eid]))
        m_delete(events,eid);
    // aus aktivem Event austragen, falls es drin sein sollte.
    if (sizeof(active) && active[A_EID] == eid)
    {
      m_delete(active[A_LISTENERS], object_name(ob));
    }
//    if (find_call_out(#'save_me)==-1)
//      call_out(#'save_me,15);
    LOGUNREGISTER(ob, eid, po);
    return 1;
}

// Loest einen Event aus.
// 1 fuer Erfolg, <= 0 fuer Misserfolg
varargs int TriggerEvent(string eid, mixed args) {
    object trigger;
    if (!stringp(eid) || 
        !objectp(trigger=previous_object())) 
        return -1;
    if (!allowedtrigger(eid, trigger)) return -2;
    if (!member(events,eid)) return -3;
    if (sizeof(pending) > __MAX_ARRAY_SIZE__/5)
        return -4;
    pending+=({ ({eid,trigger,object_name(trigger), args, time()}) });
    if (find_call_out(#'process_events) == -1)
      call_out(#'process_events,0);
    LOGEVENT(eid,trigger,args);
    //DEBUG(sprintf("%O",pending));
    return 1;
}

protected void process_events() {
  // erstmal wieder nen call_out eintragen.
  call_out(#'process_events, 1);

  // solange ueber active und pending laufen, bis keine Ticks mehr da sind,
  // bzw. in der Schleife abgebrochen wird, weil keine Events mehr da sind.
  while(get_eval_cost() > TICK_RESERVE) {
    // HB abschalten, wenn nix zu tun ist.
    if (!sizeof(active)) {
      if (!sizeof(pending)) {
        remove_call_out(#'process_events);
        break;
      }
      // scheint noch min. ein Eintrag in pending zu sein, nach active kopieren,
      // plus die Callback-Infos aus events
      active=({pending[0][P_EID],
               pending[0][P_TRIGOB],pending[0][P_TRIGOBNAME],
               pending[0][P_ARGS],
               deep_copy(events[pending[0][P_EID]]),
               pending[0][P_TIME] });

      if (sizeof(pending)>1)
        pending=pending[1..]; // und aus pending erstmal loeschen. ;-)
      else
        pending=({});
      //DEBUG(sprintf("Pending: %O",pending));
      //DEBUG(sprintf("Active: %O",active));
    }
    // jetzte den aktiven Event abarbeiten.
    // Infos aus active holen...
    string eid=active[A_EID];
    object trigob=active[A_TRIGOB];
    string trigobname=active[A_TRIGOBNAME];
    mixed args=active[A_ARGS];
    mapping listeners=active[A_LISTENERS];
    // und ueber alle Listener iterieren
    foreach(string obname, string fun, closure cl, object listener: 
                 listeners) {
      // erst pruefen, ob noch genug Ticks da sind. wenn nicht, gehts im
      // naechsten Zyklus weiter.
      if (get_eval_cost() < TICK_RESERVE) {
        return;
      }
      // wenn Closure und/oder zugehoeriges Objekt nicht existieren, versuchen
      // wir erstmal, es wiederzufinden. ;-)
      if (!objectp(query_closure_object(cl))) {
        if (objectp(listener=find_object(obname)) &&
            closurep(cl=symbol_function(fun,listener))) {
            //geklappt, auch in events wieder ergaenzen
            events[eid][obname,CB_CLOSURE]=cl;
            events[eid][obname,CB_OBJECT]=listener;
        }
        else {
          // Objekt nicht gefunden oder Closure nicht erzeugbar, austragen
          m_delete(listeners,obname);
          // und aus events austragen.
          m_delete(events[eid],obname);
          // und naechster Durchgang
          continue;
        }
      }
      // Objekt noch da, Closure wird als ausfuehrbar betrachtet. 
      catch(limited(#'funcall,({TICKSPERCALLBACK}),cl,eid,trigob,args);publish);
      // fertig mit diesem Objekt.
      m_delete(listeners,obname);
    }
    // Statistik? Differenzen zwische Erledigungszeit und Eintragszeit bilden
    // die Keys, die Values werden einfach hochgezaehlt.
#ifdef STATISTICS
    stats[time()-active[A_TIME]]++;
#endif // STATISTICS
    // ok, fertig mit active.
    active=({});
    //DEBUG(sprintf("Fertig: %O %O",eid, trigobname));
    LOGEVENTFINISH(eid,trigobname);
  }  // while(get_eval_cost() > TICK_RESERVE)

  // Soll dies Ding neugeladen werden? Wenn ja, Selbstzerstoerung, wenn keine
  // Events mehr da sind.
  if (updateme && !sizeof(active) && !sizeof(pending)) {
    DEBUG(sprintf("Update requested\n"));
    remove(1);
  }
}

protected void create() {
    seteuid(getuid(ME));
    restore_object(STORE);
    if (lastboot != __BOOT_TIME__) {
      // Oh. Reboot war... Alle Events wegschmeissen (es koennten zwar die
      // Eventanmeldungen von BPs ueberleben, aber auch die sollen sich lieber
      // im create() anmelden.)
      events=([]);
      lastboot=__BOOT_TIME__;;
    }
    LOG("Event-Dispatcher loaded");
}


protected void save_me() {
  save_object(STORE);
}

varargs int remove(int silent) {
    save_me();
    DEBUG(sprintf("remove() called by %O - destructing\n", previous_object())); 
    LOG(sprintf("remove called by %O - destructing",previous_object()));
    destruct(ME);
    return 1;
}

public void reset() {
  if (updateme && !sizeof(active) && !sizeof(pending)) {
    DEBUG(sprintf("Update requested\n"));
    remove(1);
  }
}


// fuer Debugzwecke. Interface und Verhalten kann sich jederzeit ohne
// Vorankuendigung aendern.
mapping QueryEvents() {
    return(deep_copy(events));
}

mixed QueryPending() {
    return(deep_copy(pending));
}

mixed QueryActive() {
    return(deep_copy(active));
}

mapping QueryStats() {
    return(copy(stats));
}

int UpdateMe(int flag) {
    updateme=flag;
    if (find_call_out(#'process_events)==-1)
      reset();
    return flag;
}


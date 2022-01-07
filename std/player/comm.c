// MorgenGrauen MUDlib
//
// player/comm.c-- basic player communiction commands
//
// $Id: comm.c 9576 2016-06-18 15:00:01Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma no_clone
//#pragma range_check

inherit "/std/living/comm";
inherit "/std/player/channel";
inherit "/std/player/comm_structs";

#include <input_to.h>

#define NEED_PROTOTYPES
#include <player/quest.h>
#include <player/gmcp.h>
#include <living/description.h>
#undef NEED_PROTOTYPES

#include <sys_debug.h>

#include <thing/properties.h>
#include <player/comm.h>
#include <player/base.h>

#include <properties.h>
#include <config.h>
#include <ansi.h>
#include <wizlevels.h>
#include <language.h>
#include <udp.h>
#include <defines.h>
#include <daemon.h>
#include <strings.h>
#include <regexp.h>
#include <interactive_info.h>

#define TELLHIST_DISABLED   0
#define TELLHIST_NO_MESSAGE 1
#define TELLHIST_ENABLED    2
#define TELLHIST_LONGLIFE   3

#define ECHO_COST 50
#define ERWIDER_PARAM ","

#define ZDEBUG(x)  if (find_player("zesstra"))\
              efun::tell_object(find_player("zesstra"),"CommDBG: "+x+"\n")

private int tell_history_enabled = TELLHIST_NO_MESSAGE;
private nosave mapping tell_history=([]);
private nosave string *commreceivers = ({});
private nosave string last_comm_partner;
private nosave int last_beep_time;

// Statusreporte aktiviert? Binaere Flags (s. set_report())
private int stat_reports;
// interner Cache fuer die LP/KP/Gift-Werte fuer die Statusreport-Ausgaben
// Eintraege (in dieser Reihenfolge): P_HP, P_SP, Giftstatus
// Initialisierung erfolgt beim ersten Report nach Login
private nosave mixed *report_cache;

// Puffer fuer Kobold.
private nosave struct msg_buffer_s kobold = (<msg_buffer_s>
                                             buf: allocate(32),
                                             index: -1,);
#define MAX_KOBOLD_LIMIT 256

varargs string name(int casus, int demonst);

//local property prototypes
static int _query_intermud();
public int RemoveIgnore(string ign);
public int AddIgnore(string ign);

public varargs int ReceiveMsg(string msg, int msg_type, string msg_action,
                              string msg_prefix, object origin);

// erzeugt sortierte Liste an Kommunikationspartnern
private string *sorted_commpartners(int reversed);

void create()
{
  ::create();
  Set(P_EARMUFFS, 0);
  Set(P_EARMUFFS, SAVE, F_MODE);
  Set(P_EARMUFFS, SECURED, F_MODE);
  Set(P_INTERMUD, SAVE, F_MODE);
  Set(P_IGNORE, ([]), F_VALUE);
  Set(P_IGNORE, SAVE, F_MODE);
  Set(P_BUFFER, SAVE, F_MODE);
  Set(P_MESSAGE_PREPEND, SAVE, F_MODE_AS);
  Set(P_MESSAGE_BEEP, SAVE, F_MODE_AS);
}

void create_super()
{
  set_next_reset(-1);
}

// called from base.c in Reconnect()
protected void reconnect() {
  // Cache fuer den report zuruecksetzen, der koennte veraltet sein (insb.
  // falls in der letzten Session GMCP benutzt wurde und jetzt nicht).
  report_cache = 0;
}

// uebermittelt eine MT_NOTIFICATION an this_object(), welche nicht ignoriert
// werden kann und auch nicht gespeichert wird.
protected void _notify(string msg, string action) {
  ReceiveMsg(msg,
             MT_NOTIFICATION|MSG_DONT_BUFFER|MSG_DONT_IGNORE|MSG_DONT_STORE,
             action, 0, this_object());
}

protected void updates_after_restore(int newflag) {
  // Altes Ignoriere loeschen...
  mixed ign = Query(P_IGNORE,F_VALUE);
  if (!mappingp(ign))
  {
    if (pointerp(ign))
      _notify(break_string(
        "Deine Ignoriere-Einstellungen wurden soeben geloescht, "
        "weil es eine Aktualisierung der Ignorierefunktion gab, "
        "bei der eine Konversion der Daten leider nicht "
        "moeglich war.",78), 0);

    Set(P_IGNORE, ([]), F_VALUE);
  }
}

static int set_report(string str)
{
  int canflags = QueryProp(P_CAN_FLAGS);
  if(!str)
  {
    if (stat_reports)
    {
      string *res=({});
      if (stat_reports & DO_REPORT_HP)
        res+=({"Lebenspunkte"});
      if (stat_reports & DO_REPORT_SP)
        res+=({"Konzentrationspunkte"});
      if (stat_reports & DO_REPORT_POISON)
        res+=({"Vergiftungen"});
      if (stat_reports & DO_REPORT_WIMPY)
        res+=({"Vorsicht"});

      tell_object(ME,break_string(
        "Dir werden jetzt Veraenderungen Deiner "
        +CountUp(res) + " berichtet.",78));
      if (GMCP_Status("MG.char") || GMCP_Status("char")
          || GMCP_Status("Char"))
      {
        tell_object(ME,break_string(
            "Achtung: Dein Client laesst sich den Report per GMCP "
            "(s. 'hilfe GMCP') uebermitteln. Daher wird er Dir nicht "
            "in der Textausgabe des Spiels angezeigt! Moechtest Du "
            "dies nicht, schalte bitte in Deinem Client GMCP-Module mit "
            "Namen wie 'MG.char', 'char', 'Char' oder aehnliche aus."));
      }
    }
    else
      tell_object(ME,
        "Alle Statusreports sind ausgeschaltet.\n");

    return 1;
  }
  else if (str == "aus")
  {
    if (stat_reports & DO_REPORT_HP || stat_reports & DO_REPORT_WIMPY)
    {
      string s="";
      if (stat_reports & DO_REPORT_HP)
      {
        str="ebenfalls ";
        tell_object(ME, "Der Report wurde ausgeschaltet.\n");
      }
      if ( stat_reports & DO_REPORT_WIMPY )
      {
        tell_object(ME, "Der Vorsicht-Report wurde "+s+
          "ausgeschaltet.\n");
      }
      stat_reports=0;
    }
    else
    {
      tell_object(ME, "Der Report ist bereits ausgeschaltet.\n");
    }
    return 1;
  }
  else if (str == "ein")
  {
    if ( stat_reports & DO_REPORT_HP )
    {
      tell_object(ME, "Der Report ist bereits eingeschaltet.\n");
      return 1;
    }
    tell_object(ME, "Der Report wurde eingeschaltet.\n");
    stat_reports |= DO_REPORT_HP;
    if (!(canflags & CAN_REPORT_SP))
    {
      if (QueryQuest("Hilf den Gnarfen")==1)
      {
        SetProp(P_CAN_FLAGS, canflags | CAN_REPORT_SP);
        stat_reports |= DO_REPORT_SP;
      }
      else
      {
        tell_object(ME, break_string(
          "Fuer den Statusreport Deiner Konzentration musst Du jedoch "
          "zunaechst die Quest \"Hilf den Gnarfen\" bestehen.",78));
      }
    }
    else
    {
      stat_reports |= DO_REPORT_SP;
    }
    if (!(canflags & CAN_REPORT_POISON))
    {
      if (QueryQuest("Katzenjammer")==1)
      {
        SetProp(P_CAN_FLAGS, canflags | CAN_REPORT_POISON);
        stat_reports |= DO_REPORT_POISON;
      }
      else
      {
        tell_object(ME, break_string(
          "Fuer den Statusreport Deiner Vergiftung musst Du jedoch "
          "zunaechst die Quest \"Katzenjammer\" bestehen.",78));
      }
    }
    else
    {
      stat_reports |= DO_REPORT_POISON;
    }
    // Cache loeschen, damit beim naechsten Report-Event alle Daten neu
    // eingetragen werden muessen. Muss beim Einschalten des Reports
    // passieren, weil auch in der inaktiven Zeit weiterhin Aenderungen in
    // status_report() eingehen, so dass der Cache zwar erst einmal leer ist,
    // aber beim Wiedereinschalten nicht mehr ungueltig waere und somit
    // veraltete Daten an den Spieler ausgegeben werden. Im unguenstigsten
    // Fall wuerde das sogar dazu fuehren, dass die veralteten Daten lange
    // Zeit nicht aktualisiert werden, wenn z.B. P_HP == P_MAX_HP, so dass
    // kein P_HP-Event mehr eingeht.
    report_cache=0;
    // Fall-through fuer Statusausgabe
  }
  else if (str == "vorsicht")
  {
    if (!(canflags & CAN_REPORT_WIMPY))
    {
      if (QueryQuest("Schrat kann nicht einschlafen")==1)
      {
        SetProp(P_CAN_FLAGS, canflags | CAN_REPORT_WIMPY);
        tell_object(ME, "Der Vorsicht-Report wurde eingeschaltet.\n");
        stat_reports |= DO_REPORT_WIMPY;
      }
      else
      {
        tell_object(ME, break_string(
          "Fuer den Statusreport Deiner Vorsicht musst Du "
          "zunaechst die Quest \"Schrat kann nicht einschlafen\" "
          "bestehen.",78));
      }
    }
    else
    {
      stat_reports |= DO_REPORT_WIMPY;
    }
    // fuer Seher auch Bericht der Fluchtrichtung einschalten.
    if ((stat_reports & DO_REPORT_WIMPY)
        && !(stat_reports & DO_REPORT_WIMPY_DIR)
        && ((canflags & CAN_REPORT_WIMPY) || IS_SEER(ME)))
    {
      stat_reports |= DO_REPORT_WIMPY_DIR;
    }
    // Fall-through fuer Statusausgabe
  }
  // sendet einmalig genau jetzt den konfigurierten report. Kann zum testen
  // (von Triggern) oder beim Login benutzt werden, wenn man einen initialen
  // Datenbestand erhalten will.
  else if (str=="senden")
  {
    // Es wird Ausgabe von LP und Vorsicht getriggert, das sendet beide
    // Zeilen.
    status_report(DO_REPORT_HP, QueryProp(P_HP));
    status_report(DO_REPORT_WIMPY, QueryProp(P_WIMPY));
    return 1;
  }
  else 
    return 0;
  // nur aktuellen Zustand berichten
  set_report(0);
  return 1;
}

private string get_poison_desc(int p) {
  string ret;
  if ( intp(p) ) {
    switch(p) {
      case 0:    ret="keins";       break;
      case 1..3: ret="leicht";      break;
      case 4..8: ret="gefaehrlich"; break;
      default:   ret="sehr ernst";  break;
    }
    return ret;
  }
  else return "(nicht verfuegbar)";
}

// sprintf()-Formatstrings fuer die Reportausgabe.
#define REPORTLINE "LP: %3d, KP: %3s, Gift: %s.\n"
#define REPORTLINE_WIMPY "Vorsicht: %d, Fluchtrichtung: %s.\n"
// Defines zur Adressierung der Cache-Eintraege
#define REP_HP     0
#define REP_SP     1
#define REP_POISON 2

protected void status_report(int type, mixed val) {
  // Wenn der Spieler GMCP hat und das sich um die Information kuemmert,
  // erfolgt keine textuelle Ausgabe mehr. Daher return, wenn GMCP_Char()
  // erfolg vermeldet hat.
  int flags = QueryProp(P_CAN_FLAGS);
  switch (type) {
    case DO_REPORT_HP:
      if (GMCP_Char( ([ P_HP: val ]) ) ) return;
      break;
    case DO_REPORT_SP:
      if (!(flags & CAN_REPORT_SP)) return;
      if (GMCP_Char( ([ P_SP: val ]) ) ) return;
      break;
    case DO_REPORT_POISON:
       if (!(flags & CAN_REPORT_POISON)) return;
       if (GMCP_Char( ([ P_POISON: val ]) ) ) return;
      break;
    case DO_REPORT_WIMPY:
      if (!(flags & CAN_REPORT_WIMPY)) return;
      if (GMCP_Char( ([ P_WIMPY: val ]) ) ) return;
      break;
    case DO_REPORT_WIMPY_DIR:
      if (!(flags & CAN_REPORT_WIMPY_DIR)) return;
      if (GMCP_Char( ([ P_WIMPY_DIRECTION: val ]) ) ) return;
      break;
  }

  // konventionelle textuelle Ausgabe des Reports ab hier.
  if (!(type & stat_reports))
    return;

  if ( !report_cache ) {
    report_cache = ({
      QueryProp(P_HP),
      (stat_reports&DO_REPORT_SP) ? to_string(QueryProp(P_SP)) : "###",
      (stat_reports&DO_REPORT_POISON) ?
          get_poison_desc(QueryProp(P_POISON)) : "(nicht verfuegbar)"
    });
  }

  switch(type) {
      // LP berichten: Cache aktualisieren und Meldung ausgeben.
      case DO_REPORT_HP:
        report_cache[REP_HP]=val;
        tell_object(ME, sprintf(REPORTLINE, report_cache[REP_HP],
          report_cache[REP_SP], report_cache[REP_POISON]));
        break;
      // KP berichten: Wenn der Spieler den Report freigeschaltet hat,
      // wird bei Aenderungen gemeldet. Wenn nicht, aendert sich nur der
      // Cache-Eintrag. So wird verhindert, dass ein Spieler ueber KP-
      // Veraenderungen auch dann informiert wuerde, wenn er den KP-Report
      // gar nicht benutzen koennte.
      case DO_REPORT_SP:
        report_cache[REP_SP]=to_string(val);
        tell_object(ME, sprintf(REPORTLINE, report_cache[REP_HP],
          report_cache[REP_SP], report_cache[REP_POISON]));
        break;
      // Giftstatus berichten: Wenn der Giftreport freigeschaltet ist,
      // Cache aktualisieren und berichten. Wenn nicht, aendert sich nur
      // der Cache-Eintrag. Erlaeuterung hierzu s.o. beim KP-Report.
      case DO_REPORT_POISON:
        report_cache[REP_POISON] = get_poison_desc(val);
        tell_object(ME, sprintf(REPORTLINE, report_cache[REP_HP],
          report_cache[REP_SP], report_cache[REP_POISON]));
        break;
      // Vorsicht-Report: kann ohne weitere Abfragen ausgegeben werden, da
      // alle noetigen Checks schon zu Beginn dieser Funktion erledigt wurden.
      // Lediglich der Inhalt der Meldung muss abhaengig vom Seherstatus
      // konfiguriert werden.
      case DO_REPORT_WIMPY:
        string res;
        if (IS_SEER(ME)) {
          // QueryProp() aus Kostengruenden im if(), damit die Aufruf-
          // Haeufigkeit zumindest ein wenig reduziert wird.
          string dir = QueryProp(P_WIMPY_DIRECTION)||"keine";
          res = sprintf(REPORTLINE_WIMPY, val, dir);
        }
        else
          res = sprintf(REPORTLINE_WIMPY, val, "(nicht verfuegbar)");
        tell_object(ME, res);
        break;
      // Fluchtrichtungs-Report: wird nur bei Sehern ausgegeben, damit
      // nicht auch Spieler eine VS-/FR-Meldung bekommen, wenn z.B. eine
      // externe Manipulation der Fluchtrichtung stattfindet, sie aber den
      // Report mangels Seherstatus gar nicht freigeschaltet haben.
      case DO_REPORT_WIMPY_DIR:
        if (IS_SEER(ME)) {
          if (!val) val = "keine";
          tell_object(ME,sprintf(REPORTLINE_WIMPY, QueryProp(P_WIMPY), val));
        }
        break;
  }
}

#undef REPORTLINE
#undef REPORTLINE_WIMPY
#undef REP_HP
#undef REP_SP
#undef REP_POISON

private string permutate(string msg)
{
  // Kontrollzeichen rausfiltern. *seufz*
  msg = regreplace(msg,"[[:cntrl:]]","",RE_PCRE|RE_GLOBAL);
  object ob=QueryProp(P_PERM_STRING);
  if (!objectp(ob))
    return msg;

  return ({string})ob->permutate_string(msg)||"";
}

// neue nachricht an den Kobold anhaengen
// Rueckgabewerte: MSG_BUFFER_FULL oder MSG_BUFFERED
private int add_to_kobold(string msg, int msg_type, string msg_action,
                          string msg_prefix, object origin)
{
  // Nachricht soll im Kobold gespeichert werden.
  // Kobold speichert Rohdaten und gibt spaeter das ganze auch wieder via
  // ReceiveMsg() aus - dabei wird MSG_DONT_BUFFER | MSG_DONT_STORE gesetz,
  // damit keine erneute Speicher in Kobold oder Komm-History erfolgt.

  // wenn der Puffer zu klein ist, Groesse verdoppeln, wenn noch unterhalb
  // des Limits.
  if (kobold->index >= sizeof(kobold->buf)-1) {
    if (sizeof(kobold->buf) < MAX_KOBOLD_LIMIT)
      kobold->buf += allocate(sizeof(kobold->buf));
    else
      return MSG_BUFFER_FULL;
  }
  kobold->index = kobold->index +1;
  // neue Nachricht an den Puffer anhaengen.
  string sendername = query_once_interactive(origin) ?
                      origin->query_real_name() :
                      origin->name(WER) || "<Unbekannt>";
  kobold->buf[kobold->index] = (<msg_s> msg: msg,
      type : msg_type, action : msg_action, prefix : msg_prefix,
      sendername : sendername);
  return MSG_BUFFERED;
}

private void _flush_cache(int verbose) {
  // nur mit genug Evalticks ausgeben.
  if (get_eval_cost() < 100000) return;
  if (kobold->index >= 0)
  {
    ReceiveMsg("Ein kleiner Kobold teilt Dir folgendes mit:",
               MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_BUFFER,
               0, 0, this_object());
    int prepend = QueryProp(P_MESSAGE_PREPEND);
    foreach(int i: 0 .. kobold->index) // '0 ..' ist wichtig!
    {
      struct msg_s msg = kobold->buf[i];
      // dies ist dient der Fehlerabsicherung, falls es nen Fehler (z.B. TLE)
      // in der Schleife unten gab: dann ist index nicht auf -1 gesetzt
      // worden, aber einige Nachrichten sind schon geloescht.
      if (!structp(msg)) continue;
      // Ausgabe via efun::tell_object(), weil die Arbeit von ReceiveMsg()
      // schon getan wurde. Allerdings muessen wir uns noch um den UMbruch
      // kuemmern.
      if ((msg->type) & MSG_DONT_WRAP)
        msg->msg = (msg->prefix ? msg->prefix : "") + msg->msg;
      else
      {
        int bsflags = msg->type & MSG_ALL_BS_FLAGS;
        if (prepend)
          bsflags |= BS_PREPEND_INDENT;
        msg->msg = break_string(msg->msg, 78, msg->prefix, bsflags);
      }
      efun::tell_object(this_object(), msg->msg);
      kobold->buf[i]=0;
    }
    kobold->index=-1;
  }
  else if (verbose)
  {
    ReceiveMsg("Der kleine Kobold hat leider nichts Neues fuer Dich.",
               MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_BUFFER,
               0, 0, this_object());
  }
}

varargs int cmd_kobold(string arg)
{
  switch(arg)
  {
    case "ein":
      SetProp(P_BUFFER, 1);
      printf("Der Kobold merkt sich jetzt alles!\n"); break;
    case "aus":
      SetProp(P_BUFFER, 0);
      printf("Der Kobold wird Dich nicht stoeren!\n"); break;
    default: if(arg) printf("Der Kobold sagt: kobold ein oder kobold aus\n");
  }
  _flush_cache(1);
  return 1;
}

public int TestIgnoreSimple(string *arg)
{   mapping ignore;

    if (!pointerp(arg) || !mappingp(ignore=Query(P_IGNORE,F_VALUE)))
        return 0;
    foreach(string s: arg)
    {
      if (member(ignore,s))
        return 1;
    }
    return 0;
}

//TODO: deprecated - entfernen, wenn Message() entfernt wird.
private int check_ignore(mixed ignore, string verb, string name)
{
  if (ignore == verb)
    return 1;
  ignore = explode(ignore, ".");
  return ((sizeof(ignore) > 1) &&
     (name == ignore[0] && member(ignore[1..], verb) != -1));
}

private int comm_beep() {
  if (QueryProp(P_VISUALBELL)) return 0; // kein ton
  int beep_interval=({int})QueryProp(P_MESSAGE_BEEP);
  if (!beep_interval || ((time()-last_beep_time) < beep_interval)) return 0;
  last_beep_time=time();
  return 1;
}

private varargs void add_to_tell_history( string uid, int sent, int recv,
                                 string message, string indent, int flags )
{
  /* tell_history ist ein Mapping mit UIDs der Gespraechspartner als Key.
     Als Wert ist eine Strukur vom Typ chat_s eingetragen.
     Strukturen chat_s und stored_msg_s sind in /std/player/comm_structs.c
     definiert.
     TODO fuer spaeter, gerade keine Zeit fuer:
     Als Wert ist ein Array von chat_s enthalten, wobei das 0. Element das
     jeweils juengste Gespraech mit diesem Gespraechspartner ist und alle
     weiteren Elemente in der zeitlichen Reihenfolge kommen (also letztes
     Element ist aeltestes Gespraech).
     */

  //TODO: Entfernen, wenn das nicht mehr passiert.
  if (!stringp(uid))
  {
    ReceiveMsg(sprintf(
      "\nadd_to_tell_history(): got bad uid argument %O."
      "sent: %d, recv: %d, flags: %d, msg: %s", 
      uid, sent, recv, flags, message),MT_DEBUG|MSG_BS_LEAVE_LFS,0,0,ME);              
  }

  // letzten Gespraechspartner fuer erwidere.
  if (flags & (MSGFLAG_TELL|MSGFLAG_RTELL))
    last_comm_partner = uid;

  // ist ein sortiertes Array von max. MAX_SAVED_CHATS Groesse, welches die
  // Spieler enthaelt, denen man schon was mitgeteilt hat. Aktuellste am
  // Anfang.
  if (sent) {
    if (!sizeof(commreceivers))
      commreceivers = ({uid});
    else if (commreceivers[0] != uid) {
      // nur wenn der aktuelle Partner nicht am Anfang steht, muss man hier was
      // tun. Comm-Partner an den Anfang stellen und ggf. alten Eintrag
      // entfernen.
      // TODO: Effizienter gestalten.
      commreceivers = ({uid}) + (commreceivers-({uid}));
      // ggf. kuerzen. (wenn !tell_history_enabled, wird es ggf. unten
      // gemacht, denn die Hist muss min. alle UID enthalten, die auch in
      // commreceivers drin sind.)
      if (!tell_history_enabled && sizeof(commreceivers) > MAX_SAVED_CHATS)
        commreceivers = commreceivers[0..MAX_SAVED_CHATS];
    }
  }

  if (!tell_history_enabled)
    return;

  if (!indent && message[<1] == 10)
      message = message[..<2];

  struct chat_s chat;
  // Gespraechspartner unbekannt?
  if (!member(tell_history, uid)) {
    // zuviele Gespraeche in Hist? >= ist Absicht weil ja gleich noch eins
    // dazu kommt.
    if (sizeof(tell_history) >= MAX_SAVED_CHATS) {
      string deluid;
      int zeit = __INT_MAX__;
      foreach(string tuid, chat : tell_history) {
        // aeltestes Gespraech suchen
        if (zeit > chat->time_last_msg) {
          deluid = tuid;
          zeit = chat->time_last_msg;
        }
      }
      // aeltestes Gespraech raus.
      m_delete(tell_history, deluid);
      if (member(commreceivers,deluid)>-1)
        commreceivers-=({deluid});
    }
    // neues Gespraech anlegen
    chat = (<chat_s> uid: uid, time_first_msg: time(), 
               time_last_msg: time(),
         sentcount: sent, recvcount: recv,
         msgbuf: 0, ptr: 0 );
    tell_history[uid] = chat;
  }
  else {
    // Gespraechspartner bekannt, altes Gespraech weiterbenutzen
    chat = tell_history[uid];
    chat->time_last_msg = time();
    chat->sentcount += sent;
    chat->recvcount += recv;
  }

  if (tell_history_enabled < TELLHIST_ENABLED)
    return;

  // ggf. Array fuer Messages anlegen
  if (!pointerp(chat->msgbuf))
    chat->msgbuf = allocate(MAX_SAVED_MESSAGES);

  // Message-Struktur ermitteln oder neu anlegen
  struct stored_msg_s msg;
  if (!structp(chat->msgbuf[chat->ptr])) {
    // neue Struct ins Array schreiben
    chat->msgbuf[chat->ptr] = msg = (<stored_msg_s>);
  }
  else {
    // alte Struct ueberschreiben
    msg = chat->msgbuf[chat->ptr];
  }
  // Index auf naechste Messagestruktur ermitteln
  chat->ptr = (chat->ptr + 1) % MAX_SAVED_MESSAGES;
  // Message speichern
  msg->msg = message;
  msg->prefix = indent;
  msg->timestamp = time();
}

protected void clear_tell_history()
{
  /* Nach einem "schlafe ein" werden die gespeicherten Mitteilungen geloescht,
     sofern der Spieler nichts abweichendes eingestellt hat. */

#ifdef TELLHIST_LONGLIFE
  if (tell_history_enabled == TELLHIST_LONGLIFE)
    return;
#endif

  foreach (string uid, struct chat_s chat: tell_history)
    if (pointerp(chat->msgbuf)) {
      chat->msgbuf = 0;
      chat->ptr = 0;
    }
}

protected void reset(void)
{
  /* Wird 15 Minuten nach dem Verlust der Verbindung aufgerufen. Falls der
     Spieler nicht inzwischen eine Verbindung wiederhergestellt hat, werden
     wie bei einem "schlafe ein" die Mitteilungen geloescht. */

  if (!interactive())
    clear_tell_history();

}

// gerufen, wenn zielgerichtet mit jemandem kommuniziert wird _und_ das
// Ergebnis des ReceiveMsg() geprueft werden und eine Meldung ausgegeben
// werden soll.
private void _send(object ob, string msg, int msg_type,
                   string msg_action, string msg_prefix)
{
  int res = ob->ReceiveMsg(msg, msg_type, msg_action, msg_prefix, ME);
  switch(res) {
    case MSG_DELIVERED:
      break;  // nix machen
    case MSG_BUFFERED:
      ReceiveMsg(ob->Name(WER) + " moechte gerade nicht gestoert werden. "
          "Die Mitteilung wurde von einem kleinen Kobold in Empfang "
          "genommen. Er wird sie spaeter weiterleiten!",
          MT_NOTIFICATION, msg_action, 0, this_object());
      break;
    case MSG_IGNORED:
    case MSG_VERB_IGN:
    case MSG_MUD_IGN:
      ReceiveMsg(ob->Name(WER) + " hoert gar nicht zu, was Du sagst.",
           MT_NOTIFICATION, msg_action, 0, this_object());
      break;
    case MSG_SENSE_BLOCK:
      ReceiveMsg(ob->Name(WER) + " kann Dich leider nicht wahrnehmen.",
          MT_NOTIFICATION, msg_action, 0, this_object());
      break;
    case MSG_BUFFER_FULL:
      ReceiveMsg(ob->Name(WER) + " moechte gerade nicht gestoert werden. "
          "Die Mitteilung ging verloren, denn der Kobold kann sich "
          "nichts mehr merken!", MT_NOTIFICATION, msg_action, 
          0, this_object());
      break;
    default:
      ReceiveMsg(ob->Name(WER) + " hat Deine Nachricht leider nicht "
          "mitbekommen.", MT_NOTIFICATION, msg_action, 0, this_object());
      break;
  }
}

// Ausgabe an das Objekt selber und Aufzeichnung in der Kommhistory, falls
// noetig. Wird bei _ausgehenden_ Nachrichten im eigenen Objekt gerufen, damit
// die Nachricht ggf. in den Kommhistory erfasst wird.
// TODO: entfernen, wenn alles Aufrufer ersetzt sind durch ReceiveMsg().
protected varargs int _recv(object ob, string message, int flag, string indent)
{
  write(break_string(message, 78, indent,
        QueryProp(P_MESSAGE_PREPEND) ? BS_PREPEND_INDENT : 0));
  if ((flag & MSGFLAG_TELL || flag & MSGFLAG_REMOTE) &&
      query_once_interactive(ob))
  {
    if (flag & MSGFLAG_WHISPER)
      add_to_tell_history(getuid(ob), 1, 0,
        "Du fluesterst " + ob->name(WEM) + " aus der Ferne etwas zu.", 0,
        flag);
    else
      add_to_tell_history(getuid(ob), 1, 0, message, indent, flag);
  }
  return 1;
}

// <sender> sollte ein Objekt sein. In seltenen Faellen (z.B.
// Fehlerbehandlung) ist es jedoch auch mal ein String.
varargs int Message(string msg, int flag, string indent,
                    string cname, mixed sender)
{
  object ti;
  string verb, reply, *ignore, tin;
  int em, te;
  mixed deaf;

  // Bei den Kanaelen 'Debug' und 'Entwicklung' kann man gezielt Bugs
  // einzelner Magier ignorieren. Dazu wird der Kanalname zum 'verb',
  // damit 'ignoriere name.debug' funktioniert.
  if( flag == MSGFLAG_CHANNEL ){
      if((msg[1..5] == "Debug" || msg[1..11] == "Entwicklung"
            || msg[1..9]=="Warnungen"))
      {
        // Missbrauch der Variable 'ignore' als Zwischenspeicher
        ignore = regexplode( msg, ":| |\\]" );
        verb = lower_case(ignore[0][1..]);
        tin = lower_case(ignore[2]);
      }
      else
      {
        if(cname)
          verb=lower_case(cname);
        else
          verb=query_verb();
        if( ti = this_interactive() )
        {
          tin = getuid(this_interactive());
        }
        else
        {
          //falls doch kein Objekt...
          if (objectp(sender))
            tin=lower_case(sender->name(RAW)||"<Unbekannt>");
        }
      }
  }
  else {
    if( ti = this_interactive() )
      tin = getuid(this_interactive());
    verb = query_verb();
  }

  te = flag & (MSGFLAG_TELL | MSGFLAG_WHISPER);

  // fuer "erwidere"
  if (ti && (flag & MSGFLAG_TELL || flag & MSGFLAG_REMOTE)) {
    if (!ti->QueryProp(P_INVIS)||IS_LEARNER(ME)) {
      if (flag & MSGFLAG_WHISPER)
        add_to_tell_history(getuid(ti), 0, 1,
          capitalize((((IS_LEARNER(ti) && !ti->QueryProp(P_INVIS) &&
            (ti->QueryProp(P_CAN_FLAGS) & CAN_PRESAY)) ?
            ti->QueryProp(P_PRESAY) : "") + ti->name()) || "") +
          " fluestert Dir aus der Ferne etwas zu.", 0, flag, 0);
      else
        add_to_tell_history(getuid(ti), 0, 1, msg, indent, flag, 0);
    }
  }
  // Hoert der Spieler nicht?
  em = (ti &&
        (te || flag & MSGFLAG_SHOUT) &&
        (QueryProp(P_EARMUFFS) &&
          (query_wiz_level(ti) < QueryProp(P_EARMUFFS))));
  ignore = (pointerp(ignore = QueryProp(P_IGNORE)) ? ignore : ({}));

  // Werden der Sender oder das Verb ignoriert?
  if(!ti && tin && flag == MSGFLAG_CHANNEL)
  {
     if((member(ignore, tin) != -1))
     {
       return MESSAGE_IGNORE_YOU;
     }
     if(verb &&  sizeof(filter(ignore, #'check_ignore, verb, tin)) )
     {
       return MESSAGE_IGNORE_YOU;
     }
  }
  if (ti && (member(ignore, getuid(ti)) != -1)) {
    if(te && (IS_LEARNER(ti)||!QueryProp(P_INVIS)))
      efun::tell_object(ti, capitalize(name())+
                      " hoert gar nicht zu, was Du sagst.\n");
    return MESSAGE_IGNORE_YOU;
  }
  if(tin && verb &&
     sizeof(filter(ignore, #'check_ignore/*'*/, verb, tin)))
  {
    if(ti && verb[0..2] != "ruf" && verb[0..3] != "mruf" &&
       verb[0..3] != "echo" && verb[0] != '-' && !(flag & MSGFLAG_CHANNEL) )
      efun::tell_object(ti, name()+" wehrt \""+verb+"\" ab.\n");
    return MESSAGE_IGNORE_VERB;
  }
  if (flag & MSGFLAG_RTELL) {
    int at;

    verb = lower_case(old_explode(msg, " ")[0][1..]);
    at = member(verb, '@');
    /* verb wird hier eh missbraucht, also auch fuer ein intermud-erwidere*/
    add_to_tell_history(verb, 0, 1, msg, indent, flag, 0);

    if ((member(ignore, verb) >= 0) || (member(ignore,verb[0..at]) >= 0))
      return MESSAGE_IGNORE_YOU;
    else if (at > 0 && member(ignore, verb[at..]) >= 0)
      return MESSAGE_IGNORE_MUD;
  }

  // Taubheit/Oropax
  te |= (flag & MSGFLAG_SAY);

  if (QueryProp(P_DEAF) && (flag & MSGFLAG_DEAFCHK) && !(flag & MSGFLAG_CHIST)) {
    deaf = QueryProp(P_DEAF);
    if (te)
      reply = stringp(deaf) ?
        capitalize(sprintf(deaf, name())) :
        capitalize(name())+" ist momentan leider taub.\n";
  }
  else if (em)
    reply = capitalize(name())+" hat Oropax in den Ohren.\n";

  msg = break_string(msg, 78, indent,
    (QueryProp(P_MESSAGE_PREPEND) ? BS_PREPEND_INDENT : 0) | BS_LEAVE_MY_LFS);

  if(QueryProp(P_BUFFER) &&
     (deaf ||
      query_editing(this_object()) ||
      query_input_pending(this_object())))
  {
    deaf = MESSAGE_DEAF;
    if(flag & MSGFLAG_CACHE)
    {
      if(!stringp(reply))
        reply = name()+" moechte gerade nicht gestoert werden.\n";

      msg = msg[0..<2]+" [" + strftime("%H:%M",time()) + "]\n";

      int res =  add_to_kobold(msg, 0, 0, 0,
                               objectp(sender) ? sender : ME);
      if(res == MSG_BUFFERED)
      {

        reply += "Die Mitteilung wurde von einem kleinen Kobold in Empfang "+
                 "genommen.\nEr wird sie spaeter weiterleiten!";
        deaf = MESSAGE_CACHE;
      }
      else {
        reply += "Die Mitteilung ging verloren, denn "+
                 "der Kobold kann sich nichts mehr merken!";
        deaf = MESSAGE_CACHE_FULL;
      }
      if(ti && (IS_LEARNER(ti)||!QueryProp(P_INVIS)))
        efun::tell_object(ti, reply+"\n");
    }
    return deaf;
  }
  else if((deaf || em) &&
          ( (flag & MSGFLAG_RTELL) ||
            (ti && (IS_LEARNER(ti)||!QueryProp(P_INVIS))))) {
    if (te && ti)
      efun::tell_object(ti, reply);
    return MESSAGE_DEAF;
  }

  _flush_cache(0);
  if(te && QueryProp(P_AWAY))
    msg = msg[0..<2]+" [" + strftime("%H:%M",time()) + "]\n";

  if (flag & (MSGFLAG_SAY | MSGFLAG_TELL) && comm_beep()) {
    msg=MESSAGE_BEEP+msg;
  }
  efun::tell_object(ME, msg);
  return MESSAGE_OK;
}

static int ignoriere(string str)
{
  str = _unparsed_args(1);
  mapping ignore=Query(P_IGNORE, F_VALUE);

  if (!str)
  {
    string* ignarr = m_indices(ignore);
    if (!sizeof(ignarr))
        tell_object(ME, "Du ignorierst niemanden.\n");
      else
        ReceiveMsg("Du ignorierst:\n"
                   + break_string(CountUp(map(sort_array(ignarr, #'> ),
                                          #'capitalize ) 
                                         ) + ".",78),
                   MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_STORE|MSG_DONT_WRAP,
                   0,0,this_object());
      return 1;
  }
  // trim spaces from args and convert to lower case.
  str = lower_case(trim(str, TRIM_BOTH));

  if (member(ignore, str))
  {
    RemoveIgnore(str);
    tell_object(ME, sprintf("Du ignorierst %s nicht mehr.\n", capitalize(str)));
  }
  else if (sizeof(ignore)>100)
  {
   tell_object(ME, "Du ignorierst schon genuegend!\n");
  }
  else if (AddIgnore(str) == 1)
  {
    tell_object(ME,
        sprintf("Du ignorierst jetzt %s.\n", capitalize(str)));
  }
  else
  {
    tell_object(ME,
        sprintf("'%s' kannst Du nicht ignorieren.\n",str));
  }
  return 1;
}


static int _msg_beep(string str) {
  int beep_interval;
  notify_fail("Syntax: klingelton <1 bis 3600 Sekunden> oder klingelton aus\n");
  if (stringp(str)) {
    if (str=="aus")
      SetProp(P_MESSAGE_BEEP,0);
    else if ((beep_interval=to_int(str)) > 0 && beep_interval<=3600)
      SetProp(P_MESSAGE_BEEP,beep_interval);
    else return 0;
  }

  beep_interval=({int})QueryProp(P_MESSAGE_BEEP);
  _notify("Ton bei Mitteilungen: "+
        (beep_interval ? "aller "+beep_interval+" Sekunden." : "aus."),
        query_verb());
  return 1;
}

static int _msg_prepend(string str) {
  notify_fail("Syntax: senderwiederholung ein/aus\n");
  if (stringp(str)) {
    if (str=="aus")  
      SetProp(P_MESSAGE_PREPEND,1);
    else if (str=="ein")  
      SetProp(P_MESSAGE_PREPEND,0);
    else return 0;
  }

  _notify("Senderwiederholung bei Mitteilungen: "+ 
          (({int})QueryProp(P_MESSAGE_PREPEND) ?  "aus" : "ein")+".",
          query_verb());

  return 1;
}

static int _communicate(mixed str, int silent)
{
  string  verb;
  string  myname;
  string  msg;

  if (!str || extern_call()) str=_unparsed_args()||"";
  /* str=_unparsed_args()||""; */
  verb = query_verb();
  if(stringp(verb) && verb[0] == '\'') str = verb[1..] + " " + str;
  if (str==""||str==" "||!str)
  {
    _notify("Was willst Du sagen?",MA_SAY);
    return 1;
  }
  msg=permutate(str);

  myname=(((QueryProp(P_INVIS)||!IS_LEARNER(ME))||
     !(QueryProp(P_CAN_FLAGS)&CAN_PRESAY)?
    "":QueryProp(P_PRESAY))+name())||"";

  // an alles im Raum senden. (MT_LISTEN, weil dies gesprochene Kommunikation
  // ist, keine MT_COMM)
  send_room(environment(), msg, MT_LISTEN, MA_SAY,
            capitalize(myname)+" sagt: ", ({this_object()}) );

  if(!silent)
  {
    ReceiveMsg(msg, MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_STORE,
               MA_SAY, "Du sagst: ", ME);
  }
  return 1;
}

static int _shout_to_all(mixed str)
{
  string pre, myname, realname, wizards_msg, players_msg;
  string wizard_prefix, player_prefix;
  int chars;

  if (!(str=_unparsed_args()))
  {
    _notify("Was willst Du rufen?",MA_SHOUT);
    return 1;
  }
  chars=sizeof(str)/2;
  if (chars<4) chars=4;
  pre = (!IS_LEARNER(ME) ||
   QueryProp(P_INVIS) ||
   !(QueryProp(P_CAN_FLAGS) & CAN_PRESAY)) ? "" : QueryProp(P_PRESAY);
  realname = capitalize((pre + capitalize(getuid()))||"");
  myname = capitalize(pre + name()||"");
  if (QueryProp(P_INVIS))
    realname = "("+realname+")";

  wizards_msg = permutate(str);
  wizard_prefix = myname+" ruft: ";

  if(QueryProp(P_FROG)) {
    players_msg = "Quaaak, quaaaaak, quuuuaaaaaaaaaaaaaaaaaaaak !!";
    player_prefix = myname+" quakt: ";
  }
  else {
    players_msg = wizards_msg;
    player_prefix = wizard_prefix;
  }

  if(!IS_LEARNER(this_player()))
  {
    if(QueryProp(P_GHOST)) {
        _notify("So ganz ohne Koerper bekommst Du keinen Ton heraus.",
                MA_SHOUT);
        return 1;
    }
    if (QueryProp(P_SP) <(chars+20))
    {
      _notify("Du musst erst wieder magische Kraefte sammeln.",
              MA_SHOUT);
      _notify("Tip: Benutz doch mal die Ebenen (Hilfe dazu mit 'hilfe "
              "Ebenen').", MA_SHOUT);
      return 1;
    }
    SetProp(P_SP, QueryProp(P_SP) - chars - 20);
  }

  ReceiveMsg(wizards_msg, MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_STORE,
             "rufe", "Du rufst: ", ME);

  foreach ( object ob : users()-({this_object()}) )
    if ( IS_LEARNER(ob) )
      ob->ReceiveMsg(wizards_msg, MT_LISTEN|MT_FAR, MA_SHOUT, wizard_prefix,
                    this_object());
    else
      ob->ReceiveMsg(players_msg, MT_LISTEN|MT_FAR, MA_SHOUT, player_prefix,
                    this_object());

  return 1;
}

varargs int _tell(string who, mixed msg)
{
  object    ob;
  string    away,myname,ret;
  mixed     it;
  string    *xname;
  int       i,visflag;

  if (extern_call() && this_interactive()!=ME) return 1;
  if (!who || !msg) {
    _notify("Was willst Du mitteilen?",MA_TELL);
    return 1;
  }

  if(who == ERWIDER_PARAM)
  {
    if (!last_comm_partner)
    {
      _notify_fail("Du hast aber noch keine Mitteilungen erhalten, auf die "
          "Du was erwidern\nkoenntest.\n");
      return 0;
    }
    who=last_comm_partner;
  }

  // teile .x mit teilt bisherigen Gespraechspartnern etwas mit.
  if (who == ".")
   who = ".1";

  if ( sscanf(who, ".%d", i) == 1 ) {
    if(i > 0 && i <= sizeof(commreceivers))
      who = commreceivers[i-1];
    else {
      _notify_fail("So vielen Leuten hast Du noch nichts mitgeteilt!\n");
      return 0;
    }
  }

  xname = explode(who, "@");

  if (sizeof(xname) == 2) 
  {
    if ( QueryProp(P_QP) )
    {
      if (ret=({string})INETD->_send_udp(xname[1],
                                    ([ REQUEST:   "tell",
                                       RECIPIENT: xname[0],
                                       SENDER:    getuid(ME),
                                       DATA:     msg ]), 1))
      {
        _notify(ret, MA_TELL);
      }
      else
      {
        write("Nachricht abgeschickt.\n");
        add_to_tell_history(who, 1, 0, msg,
          "Du teilst " + capitalize(who) + " mit: ", MSGFLAG_RTELL, 1);
      }
    }
    else
      write("Du hast nicht genug Abenteuerpunkte, um Spielern in anderen \n"
        "Muds etwas mitteilen zu koennen.\n");
    return 1;
  }

  if (!ob=find_player(it = lower_case(who)))
  {
    it = match_living(it, 0);
    if (!stringp(it))
      switch(it) {
      case -1:
        _notify("Das war nicht eindeutig!",MA_TELL);
        return 1;
      case -2:
        _notify("Kein solcher Spieler!",MA_TELL);
        return 1;
      }
    ob = find_player(it) || find_living(it);
    if(!ob) {
      _notify("Kein solcher Spieler!",MA_TELL);
      return 1;
    }
  }

  if(QueryProp(P_INVIS)){
    if(!IS_LEARNER(ob))
      myname = name();
    else
      myname="("+
             ((QueryProp(P_CAN_FLAGS) & CAN_PRESAY)?QueryProp(P_PRESAY):"")+
             capitalize(getuid()) + ")";
  }
  else
    myname=((IS_LEARNER(ME) && (QueryProp(P_CAN_FLAGS) & CAN_PRESAY)) ?
              QueryProp(P_PRESAY):"") + capitalize(getuid(ME));
  if (myname && sizeof(myname)) myname=capitalize(myname);
  // erstmal an Empfaenger senden
  _send(ob, permutate(msg), MT_COMM|MT_FAR, MA_TELL,
        myname + " teilt Dir mit: ");

  // dann evtl. noch an Absender ausgeben...
  if (visflag = !ob->QueryProp(P_INVIS) || IS_LEARNER(this_player()))
    _recv(ob, msg, MSGFLAG_TELL, "Du teilst " + capitalize(it) + " mit: ");
  // oder irgendwas anderes an den Absender ausgeben...
  if (!visflag && interactive(ob))
      _notify("Kein solcher Spieler!",MA_TELL);
  else if (away = ({string})ob->QueryProp(P_AWAY))
      ReceiveMsg( break_string( away, 78, capitalize(it)
                           + " ist gerade nicht da: ", BS_INDENT_ONCE ),
          MT_NOTIFICATION|MSG_DONT_WRAP|MSG_DONT_IGNORE,
          MA_TELL, 0, this_object());
  else if (interactive(ob) && (i=query_idle(ob))>=600)
  { //ab 10 Mins
      if (i<3600)
        away=time2string("%m %M",i);
      else
        away=time2string("%h %H und %m %M",i);

      _notify(sprintf("%s ist seit %s voellig untaetig.",
              capitalize(it),away),
              MA_TELL);
    }

  return 1;
}

static int _teile(string str)
{
  string who, message;
  if (!(str=_unparsed_args())) return 0;
  if (sscanf(str, "%s mit %s", who, message) == 2)
    return _tell(who, message,1);
  return 0;
}
static int _teile_mit_alias(string str)
{
  str = _unparsed_args(), TRIM_LEFT;
  if (!str) return 0;
  str = trim(str, TRIM_LEFT);
  // Ziel muss min. 2 Buchstaben haben (.<nr>)
  if (sizeof(str) < 4) return 0;
  int pos = strstr(str, " ");
  if (pos >= 2)
    return _tell(str[..pos-1], str[pos+1..]);
  return 0;
}

static int _erzaehle(string str)
{
  string who, message;

  if (!(str=_unparsed_args())) return 0;
  if (sscanf(str, "%s %s", who, message) == 2)
    return _tell(who, message,1);
  return 0;
}

static int _whisper(string str)
{
  object    ob;
  string    who;
  string    msg;
  string    myname;

  if (!(str=_unparsed_args()) ||
       (sscanf(str, "%s zu %s", who, msg) != 2 &&
        sscanf(str, "%s %s", who, msg) !=2 )) {
    _notify("Was willst Du wem zufluestern?",MA_SAY);
    return 1;
  }
  who = lower_case(who);
  if (!(ob = present(who, environment(this_player()))) || !living(ob)) {
    _notify(capitalize(who)+" ist nicht in diesem Raum.",MA_SAY);
    return 1;
  }

  myname = capitalize((((IS_LEARNER(ME) &&
       !QueryProp(P_INVIS) &&
       (QueryProp(P_CAN_FLAGS) & CAN_PRESAY))?
      QueryProp(P_PRESAY) : "") + name()) || "");

  _send(ob, permutate(msg), MT_LISTEN|MSG_DONT_STORE,
        MA_SAY, myname + " fluestert Dir zu: ");
  send_room(environment(),
            myname + " fluestert " + ob->name(WEM, 1) + " etwas zu.",
            MT_LISTEN|MSG_DONT_STORE, MA_SAY, 0, ({this_object(),ob}));

  _recv(ob, msg, MSGFLAG_WHISPER, "Du fluesterst " + ob->name(WEM) + " zu: ");


  return 1;
}

static int _remote_whisper(string str)
{
  /* Wie 'teile mit', nur mit MSGFLAG_WHISPER. Dadurch wird der Inhalt der
     Nachricht nicht in der tell_history verewigt. */

  object    ob;
  string    who, it;
  string    msg;
  string    myname;

  if (!(str=_unparsed_args()) ||
       (sscanf(str, "%s zu %s", who, msg) != 2 &&
        sscanf(str, "%s %s", who, msg) !=2 )) {
    _notify("Was willst Du wem aus der Ferne zufluestern?",MA_EMOTE);
    return 1;
  }

  if (!ob=find_player(it = lower_case(who)))
  {
    it = match_living(it, 0);
    if (!stringp(it))
      switch(it){
      case -1:
        _notify("Das war nicht eindeutig!",MA_EMOTE);
        return 1;
      case -2:
        _notify("Kein solcher Spieler!",MA_EMOTE);
        return 1;
      }
    ob = find_player(it);
    if(!ob) ob = find_living(it);
    if(!ob){
      _notify("Kein solcher Spieler!",MA_EMOTE);
      return 1;
    }
  }
  if (environment(ob) == environment()) {
    _notify("Wenn jemand neben Dir steht, nimm fluester.",MA_EMOTE);
    return 1;
  }

  myname = capitalize((((IS_LEARNER(ME) &&
       !QueryProp(P_INVIS) &&
       (QueryProp(P_CAN_FLAGS) & CAN_PRESAY))?
      QueryProp(P_PRESAY) : "") + name()) || "");

  // An Empfaenger senden.
  _send(ob, permutate(msg), MT_COMM|MT_FAR|MSG_DONT_STORE, MA_EMOTE,
         myname + " fluestert Dir aus der Ferne zu: ");

  // wenn Empfaenger invis und wir kein Magier , ggf. fakefehler ausgeben.
  if (ob->QueryProp(P_INVIS) && !IS_LEARNER(this_player())) {
    _notify("Kein solcher Spieler!",MA_EMOTE);
    return 1;
  }
  // sonst eigene Meldung via _recv() ausgeben.
  else 
    _recv(ob, msg, MSGFLAG_WHISPER | MSGFLAG_REMOTE,
        "Du fluesterst " + ob->name(WEM) + " aus der Ferne zu: ");

  return 1;
}

static int _converse(string arg)
{
  _notify("Mit '**' wird das Gespraech beendet.",MA_SAY);
  if (stringp(arg) && strstr(arg, "-s") == 0)
    input_to("_converse_more", INPUT_PROMPT, "]", 1);
  else
    input_to("_converse_more", INPUT_PROMPT, "]", 0);
  return 1;
}

static int _converse_more(mixed str, int silent)
{
  if (str == "**") {
    _notify("Ok.",MA_SAY);
    return 0;
  }

  if(str != "")
    _communicate(str, silent);

  input_to("_converse_more", INPUT_PROMPT, "]", silent);
  return 1;
}

private int is_learner(object o) { return IS_LEARNER(o); }

static int _shout_to_wizards(mixed str)
{
  string    myname;

  str = _unparsed_args();
  if (!str||!sizeof(str)) {
    _notify("Was willst Du den Magiern zurufen?",MA_SHOUT);
    return 1;
  }
  // Kontrollzeichen rausfiltern.
  str = regreplace(str,"[[:cntrl:]]","",RE_PCRE|RE_GLOBAL);
  myname = capitalize(getuid(this_object()));
  if (!IS_LEARNER(this_object()))
    _recv(0, str, MSGFLAG_MECHO, "Du teilst allen Magiern mit: ");

  // mrufe ist nicht ignorierbar, da es nur fuer schwere Probleme gedacht ist.
  filter(users(), #'is_learner)->ReceiveMsg(str,
      MT_COMM|MT_FAR|MSG_DONT_IGNORE|MSG_DONT_STORE,
      MA_SHOUT, myname+" an alle Magier: ", this_object());

  return 1;
}

static int _echo(string str) {
  if (!IS_SEER(ME) || (!IS_LEARNER(ME)
        && !(QueryProp(P_CAN_FLAGS) & CAN_ECHO)))
    return 0;

  if (!(str=_unparsed_args())) {
    _notify("Was moechtest Du 'echoen'?", 0);
    return 1;
  }

  if (!IS_LEARNER(this_interactive()))
  {
    if (QueryProp(P_GHOST))
    {
      _notify_fail("Ohne Koerper fehlt Dir dazu die noetige magische Kraft.\n");
      return 0;
    }
    if (QueryProp(P_SP)<ECHO_COST)
    {
      _notify_fail("Du musst erst wieder magische Kraefte sammeln.\n");
      return 0;
    }
    SetProp(P_SP,QueryProp(P_SP)-ECHO_COST);
    str=">\b"+str;
    log_file("ARCH/ECHO_SEHER", sprintf("%s %s: %s\n", dtime(time()), getuid(),
           str));
  }
  // An den Raum senden. Typ ist MT_COMM, aber das Echo soll weder in der
  // Kommhistory noch im Kobold landen.
  send_room(environment(ME), str, MT_COMM|MSG_DONT_STORE|MSG_DONT_BUFFER,
            MA_UNKNOWN, 0, 0);
  return 1;
}

// Dient als Verteidigung gegen Leute, die eher unbedacht reinschreiben, nicht
// gegen Leute, die da absichtlich reinschreiben. Die werden geteert
// und gefedert.
static string *_set_ignore(mixed arg)
{
  raise_error("Direktes Setzen von P_IGNORE ist nicht erlaubt. "
      "Benutze AddIgnore/RemoveIgnore()!\n");
}
// Kompatibiltaet zum alten Ignore: Array von Indices liefern. Aendert aber
// nix dran, dass alle TestIgnore() & Co benutzen sollen.
static string *_query_ignore() {
  mixed ign=Query(P_IGNORE, F_VALUE);
  if (mappingp(ign))
    return m_indices(ign);
  return ({});
}

public int AddIgnore(string ign) {
  // Einige strings sind nicht erlaubt, z.B. konsekutive .
  if (!sizeof(ign)
      || regmatch(ign,"[.]{2,}",RE_PCRE)
      || regmatch(ign," ",RE_PCRE)
      || sizeof(explode(ign,"."))>3)
    return 0;

  mapping ignores=Query(P_IGNORE, F_VALUE);
  ignores[ign]=time();
  // kein Set() noetig.
  return 1;
}

public int RemoveIgnore(string ign)
{
  mapping ignores=Query(P_IGNORE,F_VALUE);
  m_delete(ignores,ign);
  // Kein Set() noetig
  return 1;
}

static int _query_intermud()
{
  mixed tmp;
  return member(pointerp(tmp=Query(P_CHANNELS))?tmp:({}), "Intermud") > -1;
}


int erwidere(string str)
{
  str=_unparsed_args();
  if (!str) return 0;
  return _tell(ERWIDER_PARAM, str ,1);
}

static int tmhist(string str)
{

  if (str == "aus") {
    tell_history_enabled = TELLHIST_DISABLED;
    write("Ok, es wird nichts mehr gespeichert.\n");
    if (sizeof(tell_history)) {
      tell_history = ([]);
      commreceivers = ({});
      write("Deine Mitteilungsgeschichte wurde geloescht.\n");
    }
    return 1;
  }

  if (str == "namen") {
    int flag;
    tell_history_enabled = TELLHIST_NO_MESSAGE;
    write("Ok, die Namen zukuenftiger Gespraechspartner werden gespeichert.\n");
    foreach (string uid, struct chat_s chat: tell_history)
      if (pointerp(chat->msgbuf)) {
        chat->msgbuf = 0;
        chat->ptr = 0;
        flag = 1;
      }
    if (flag)
      write("Der Inhalt Deiner Mitteilungen wurde geloescht.\n");
    return 1;
  }

  if (str == "ein" || str == "an") {
    tell_history_enabled = TELLHIST_ENABLED;
    write("Ok, zukuenftige Mitteilungen werden gespeichert.\n");
    return 1;
  }

#ifdef TELLHIST_LONGLIFE
  if (str == "langlebig") {
    tell_history_enabled = TELLHIST_LONGLIFE;
    write("Ok, zukuenftige Mitteilungen werden jeweils bis zum naechsten "
          "Ende/Crash/\nReboot gespeichert.\n");
    return 1;
  }
#endif

  if (str == "status") {
    switch (tell_history_enabled) {
      case TELLHIST_DISABLED:
        write("Die Namen Deiner Gespraechspartner werden nicht gespeichert.\n");
        break;
      case TELLHIST_NO_MESSAGE:
        write("Die Namen Deiner Gespraechspartner werden gespeichert.\n");
        break;
      case TELLHIST_ENABLED:
        write("Deine Mitteilungen werden gespeichert.\n");
        break;
#ifdef TELLHIST_LONGLIFE
      case TELLHIST_LONGLIFE:
        write("Deine Mitteilungen werden jeweils bis zum naechsten Ende/"
              "Crash/Reboot\ngespeichert.\n");
        break;
#endif
    }
    return 1;
  }

  if (tell_history_enabled == TELLHIST_DISABLED) {
    _notify_fail("Deine Gespraechspartner werden nicht gespeichert.\n");
    return 0;
  }

  if (!sizeof(tell_history)) {
    _notify_fail("Du hast noch keinem etwas mitgeteilt "
                 "und noch keine Mitteilungen erhalten.\n");
    return 0;
  }

  if (str && sizeof(str)) {

    if (tell_history_enabled < TELLHIST_ENABLED) {
      _notify_fail("Der Inhalt Deiner Mitteilungen wird nicht gespeichert.\n");
      return 0;
    }

    string uid;
    if (member(tell_history, str)) {
      // Name gewuenscht, da der String in der History vorkommt.
      uid = str;
    }
    else {
      // evtl. ne Zahl angegeben.
      int i;
      string *partners = sorted_commpartners(0);
      if ((i = to_int(str) - 1) >= 0 && i < sizeof(partners))
        uid = partners[i];
      else {
        notify_fail("Mit so vielen Leuten hast Du nicht gesprochen!\n");
        return 0;
      }
    }

    mixed *data = tell_history[uid]->msgbuf;
    if (!data) {
      _notify_fail(
        "Der Inhalt dieser Mitteilung ist nicht (mehr) gespeichert.\n");
      return 0;
    }

    int ptr = tell_history[uid]->ptr;

    More(sprintf("%@s", map(data[ptr..MAX_SAVED_MESSAGES-1] +
                              data[0..ptr-1],
         function string (struct stored_msg_s msg) {
             if (!structp(msg)) return "";
               return break_string( msg->msg + " <"
                 + strftime("%H:%M:%S",msg->timestamp) + ">", 78,
                 msg->prefix || "", msg->prefix ? BS_LEAVE_MY_LFS : 0);
         } ) ) );
    return 1;
  }

  string history = "Folgende Gespraeche hast Du bereits gefuehrt:\n";
  int i;
  foreach (string uid : sorted_commpartners(0) ) {
    int j;
    struct chat_s chat = tell_history[uid];
    history += sprintf("%2d.%-4s %s  %-11s  %d gesendet/%d empfangen\n", ++i,
      ((j=member(commreceivers,uid))>-1 ? sprintf("/%2d.",j+1) : ""),
      strftime("%a, %e.%m.%y",chat->time_last_msg),
      capitalize(chat->uid), chat->sentcount, chat->recvcount);
  }

  More(history);

  return 1;
}

static mixed _query_localcmds()
{
  return ({
    ({"kobold", "cmd_kobold",0,0}),
     ({"sag","_communicate",0,0}),
     ({"sage","_communicate",0,0}),
     ({"'","_communicate",1,0}),
     ({"mruf","_shout_to_wizards",0,0}),
     ({"mrufe","_shout_to_wizards",0,0}),
     ({"ruf","_shout_to_all",0,0}),
     ({"rufe","_shout_to_all",0,0}),
     ({"erzaehl","_erzaehle",0,0}),
     ({"erzaehle","_erzaehle",0,0}),
     ({"teil","_teile",0,0}),
     ({"teile","_teile",0,0}),
     ({"tm","_teile_mit_alias",0,0}),
     ({"fluester","_whisper",0,0}),
     ({"fluestere","_whisper",0,0}),
     ({"rfluester","_remote_whisper",0,0}),
     ({"rfluestere","_remote_whisper",0,0}),
     ({"gespraech","_converse",0,0}),
     ({"echo","_echo",0,0}),
     ({"ignorier","ignoriere",0,0}),
     ({"ignoriere","ignoriere",0,0}),
     ({"tmhist","tmhist",0,0}),
     ({"erwider","erwidere",0,0}),
     ({"erwidere","erwidere",0,0}),
     ({"klingelton","_msg_beep",0,0}),
     ({"senderwiederholung","_msg_prepend",0,0}),
     ({"report","set_report",0,0}),
    })+channel::_query_localcmds();
}

private string *sorted_commpartners(int reversed) {
  return sort_array(m_indices(tell_history),
      function int (string uid1, string uid2) {
          if (reversed)
            return tell_history[uid1]->time_last_msg >
                   tell_history[uid2]->time_last_msg;
          else
            return tell_history[uid1]->time_last_msg <=
                   tell_history[uid2]->time_last_msg;
      } );
}

// Setzt den Prompt eines Interactives. Gerufen bei Objekterstellung,
// Reconnect und bei Magiern, wenn diese ihren Prompt oder ihr aktuelles
// Verzeichnis aendern.
static void modify_prompt() {
    string text = Query(P_PROMPT, F_VALUE);

    if ( !stringp(text) || !sizeof(text) )
        text = "> ";
    else {
        string path = Query(P_CURRENTDIR, F_VALUE);
        if (stringp(path) && sizeof(path))
          text = regreplace(text,"\\w",path,0); // Pfad einsetzen
    }
    configure_interactive(this_object(), IC_PROMPT, text);
}

// Prueft auf Ingoriereeintraege.
// Rueckgabe: 0 (nicht ignoriert) oder MSG_IGNORED
public int TestIgnore(string|string* srcnames)
{
  mapping ign = Query(P_IGNORE, F_VALUE);
  if (stringp(srcnames))
    srcnames = ({srcnames});

  foreach(string srcname: srcnames)
  {
    // einfachster Fall, exakter Match
    if (member(ign, srcname))
      return MSG_IGNORED;
    // ansonsten muss aufgetrennt werden.
    if (strstr(srcname,".") > -1)
    {
      string *srcparts=explode(srcname,".");
      switch(sizeof(srcparts))
      {
        // case 0 und 1 kann nicht passieren.
        case 3:
          // zu pruefen: [sender].aktion.qualifizierer.
          // Der Fall, dass der Spieler dies _genau_ _so_ ignoriert hat, wird
          // oben schon geprueft. im Spieler geprueft werden muss noch:
          // spieler, .aktion, spieler.aktion und .aktion.qualifizierer
          if ( (sizeof(srcparts[0]) && member(ign,srcparts[0])) // spieler
              || member(ign, "."+srcparts[1])      // .aktion
              || member(ign, srcparts[0]+"."+srcparts[1]) // [spieler].aktion
              || member(ign, "."+srcparts[1]+"."+srcparts[2]) // .akt.qual
             )
          {
            return MSG_IGNORED;
          }
          break;
        case 2:
          // zu pruefen: spieler.aktion
          // Der Fall, dass der Spieler das _genau_ _so_ eingegeben hat, ist
          // oben schon geprueft. Im Spieler zu pruefen ist noch:
          // spieler und .aktion
          if ((sizeof(srcparts[0]) && member(ign,srcparts[0]))
              || member(ign, "."+srcparts[1]))
          {
            return MSG_IGNORED;
          }
          break;
        default: // mehr als 3 Teile...
          raise_error(sprintf("TestIgnoreExt(): too many qualifiers, only 1 "
                "is supported. Got: %s\n",srcname));
      }
    }
  }
  // Default: nicht ignorieren.
  return 0;
}

#ifdef __LPC_UNIONS__
public int TestIgnoreExt(string|string* srcnames)
#else
public int TestIgnoreExt(mixed srcnames)
#endif
{
  return TestIgnore(srcnames);
}

// Prueft fuer ReceiveMsg() auf Ingoriereeintraege. Ignoriert aber nicht alle
// Typen. 
// Rueckgabe: 0 oder MSG_IGNORED | MSG_VERB_IGN | MSG_MUD_IGN
private int check_ignores(string msg, int msg_type, string msg_action,
                            string msg_prefix, object origin)
{
  // Einige Dinge lassen sich nicht ignorieren.
  if (msg_type & (MT_NEWS|MT_NOTIFICATION))
    return 0;
  // alles andere geht zur zeit erstmal, wenn origin bekannt UND NICHT das
  // eigene Objekt ist. Waer ggf. sonst doof. Ausserdem muss es natuerlich
  // eine ignorierbare msg_action geben.
  else if (stringp(msg_action) && origin && origin != ME)
  {
    string srcname =
      (query_once_interactive(origin) ? origin->query_real_name()
                                      : origin->name(WER) || "");
    mapping ign = Query(P_IGNORE, F_VALUE);

    if (member(ign, srcname))
      return MSG_IGNORED;
    // vielleicht wird irgendwas a la name.aktion ignoriert?
    // dies ignoriert auch spieler.ebenen.<ebene> (s. msg_action bei
    // Ebenenmeldungen)
    if (member(ign, srcname+"."+msg_action))
      return MSG_VERB_IGN;
    // Oder die Aktion komplett? Dies ignoriert auch .ebenen.<ebene>, obwohl
    // das reichlich sinnfrei ist.
    if (member(ign, "."+msg_action))
      return MSG_VERB_IGN;
    // Spieler auf Ebenen ignoriert?
    // msg_action ist hier nach diesem Muster: MA_CHANNEL.<ebene>
    if (strstr(msg_action, MA_CHANNEL) == 0)
    {
      // spieler.ebenen? (spieler.ebenen.<ebene> oben schon geprueft)
      if (member(ign, srcname + "."MA_CHANNEL))
        return MSG_IGNORED;
      // spieler.ebenen.ebenenname ist oben schon abgedeckt.
      // .ebenen halte ich fuer sinnfrei, nicht geprueft.
    }
    // Spieler aus anderem mud? *seufz*
    if (strstr(srcname,"@") > -1)
    {
      string *srcparts = explode(srcname,"@");
      if (sizeof(srcparts)==2)
      {
        // spieler@?
        if (member(ign, srcparts[0]+"@"))
          return MSG_IGNORED;
        // oder Mud per @mud?
        if (member(ign, "@" + srcparts[1]))
          return MSG_MUD_IGN;
        // BTW: spieler@mud wurde schon ganz oben erfasst.
      }
    }
  }
  // Default: nicht ignorieren.
  return 0;
}

// Wird die nachricht wahrgenommen? Die Pruefung erfolgt aufgrund von
// msg_type. Zur wird MT_LOOK und MT_LISTEN beruecksichtigt (Pruefung auf
// BLindheit/Taubheit). In Zukunft koennten aber weitere Typen und weitere
// Kriterien wichtig werden und weitere Argumente uebergeben werden. (Dies
// bitte beachten, falls diese Funktion protected/public werden sollte.)
// Wichtig: enthaelt msg_action weder MT_LOOK noch MT_LISTEN, wird die
// Nachricht wahrgenommen, da davon ausgegangen wird, dass sie mit den beiden
// Sinn gar nix zu tun hat.
// Rueckgabe: 0 oder MSG_SENSE_BLOCK
private int check_senses(int msg_type)
{
  int senses = msg_type & (MT_LOOK|MT_LISTEN);
  // Wenn von vorherein kein Sinn angesprochen, dann ist es eine nachricht,
  // die von keinem der beiden wahrgenommen wird und sollte demnach nicht
  // unterdrueckt werden.
  if (!senses)
    return 0;

  if ((senses & MT_LOOK) && CannotSee(1))
    senses &= ~MT_LOOK;  // Sinn loeschen

  if ((senses & MT_LISTEN) && QueryProp(P_DEAF))
    senses &= ~MT_LISTEN;

  // wenn kein Sinn mehr ueber, wird die Nachricht nicht wahrgenommen.
  if (!senses)
    return MSG_SENSE_BLOCK;

  return 0;
}

public varargs int ReceiveMsg(string msg, int msg_type, string msg_action,
                              string msg_prefix, object origin)
{
  if (!msg) return MSG_FAILED;

  // Flags und Typen spalten
  int flags = msg_type & MSG_ALL_FLAGS;
  int type = msg_type & ~flags;

  // ggf. defaults ermitteln
  origin ||= previous_object();
  msg_action ||= comm_guess_action();
  type ||= comm_guess_message_type(msg_action, origin);

  // Debugmeldungen nur an Magier oder Testspieler mit P_WIZ_DEBUG
  if (msg_type & MT_DEBUG)
  {
    if (!QueryProp(P_WIZ_DEBUG)
        || (!IS_LEARNER(ME) && !QueryProp(P_TESTPLAYER)) )
    return MSG_FAILED;
  }

  // Zuerst werden Sinne und P_IGNORE sowie ggf. sonstige Filter geprueft. In
  // dem Fall ist direkt Ende, kein Kobold, keine Komm-History, keine
  // Weiterbearbeitung.
  // aber bestimmte Dinge lassen sich einfach nicht ignorieren.
  if (!(flags & MSG_DONT_IGNORE))
  {
    // Sinne pruefen. (nur typen uebergeben, keine Flags)
    int res=check_senses(type);
    if (res) return res;

    // Spieler-definiertes Ignoriere? (nur typen uebergeben, keine Flags)
    res=check_ignores(msg, type, msg_action, msg_prefix, origin);
    if (res) return res;
  }

  // Fuer MT_COMM gibt es ein paar Sonderdinge zu machen.
  if ((type & MT_COMM))
  {
    // erstmal in der Komm-History ablegen, wenn gewuenscht.
    if ((!(flags & MSG_DONT_STORE)))
    {
      string uid;
      if (query_once_interactive(origin))
        uid = origin->query_real_name();
      else
        uid = origin->name(WER) || "<unbekannt>";
      add_to_tell_history(uid, 0, 1, msg, msg_prefix,
                          (msg_action == MA_TELL ? MSGFLAG_TELL : 0 ) );
    }

    // ggf. Uhrzeit bei abwesenden Spielern anhaengen, aber nicht bei
    // Ebenenmeldungen. (Die haben ggf. schon.)
    if (stringp(msg_action) && QueryProp(P_AWAY)
        && strstr(msg_action, MA_CHANNEL) != 0)
    {
      // Uhrzeit anhaengen, aber ggf. muss ein \n abgeschnitten werden.
      if (msg[<1] == '\n')
        msg = msg[0..<2]+" [" + strftime("%H:%M",time()) + "]\n";
      else
        msg = msg + " [" + strftime("%H:%M",time()) + "]";
    }
    // Kobold erlaubt und gewuenscht? Kobold ist fuer die
    // direkte Kommunikation mittels MT_COMM vorgesehen.
    // Oropax von Magiern leitet inzwischen auch nur in Kobold um statt zu
    // ignorieren.
    // die if-Konstruktion ist so, weil ich das _flush_cache() im else
    // brauche.
    if (query_editing(this_object()) || query_input_pending(this_object())
        || QueryProp(P_EARMUFFS))
    {
      if (!(flags & MSG_DONT_BUFFER)
            && QueryProp(P_BUFFER))
      {
        // Nachricht soll im Kobold gespeichert werden.
        return add_to_kobold(msg, msg_type, msg_action, msg_prefix, origin);
      }
    }
    else
    {
      // wenn nicht in Editor/input_to, mal versuchen, den Kobold zu
      // entleeren.
      _flush_cache(0);
    }

    // ggf. Piepston anhaengen. NACH Koboldablage, die sollen erstmal keinen
    // Pieps kriegen.
    if (comm_beep())
      msg=msg + MESSAGE_BEEP;
  }

  // Ausgabenachricht bauen und an den Spieler senden.
  if (flags & MSG_DONT_WRAP)
    msg = (msg_prefix ? msg_prefix : "") + msg;
  else
  {
    int bsflags = flags & MSG_ALL_BS_FLAGS;
    if (QueryProp(P_MESSAGE_PREPEND))
      bsflags |= BS_PREPEND_INDENT;
    msg = break_string(msg, 78, msg_prefix, bsflags);
  }
  efun::tell_object(ME, msg);

  return MSG_DELIVERED;
}

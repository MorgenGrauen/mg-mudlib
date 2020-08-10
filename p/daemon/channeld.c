// channeld.c
//
// $Id: channeld.c 9138 2015-02-03 21:46:56Z Zesstra $
//

#pragma strong_types
#pragma no_shadow // keine Shadowing...
#pragma no_clone
#pragma no_inherit
#pragma save_types

#include <sys_debug.h>
#include <lpctypes.h>
#include <wizlevels.h>
#include <regexp.h>

#include <properties.h>
#include <config.h>
#include <language.h>

#define NEED_PROTOTYPES
#include "channel.h"

#define CHANNEL_SAVE  "/p/daemon/save/channeld"
#define MEMORY        "/secure/memory"
#define MAX_HIST_SIZE 200
#define MAX_CHANNELS  90
#define CMDS          ({C_FIND, C_LIST, C_JOIN, C_LEAVE, C_SEND, C_NEW})


/* Ebenenliste und die zugehoerigen Daten, z.B. Mitglieder oder Beschreibung
   channels = ([string channelname : ({ ({object* members}),
                                          closure access_rights,
                                          string channel_desc,
                                          string|object supervisor,
                                          string readable_channelname }) ])
 The master_object is also called the supervisor.
 */
private nosave mapping channels = ([]);
//private nosave mapping lowerch; // unused

/* Ebenenhistory
   mapping channelH = ([ string channelname : ({ ({string channelname,
                                                   string sender,
                                                   string msg,
                                                   int msg_type}) }) ]) */
// channelH wird in create() geeignet initialisiert
// HINWEIS: Bitte beachten, dass channelH immer nur so manipuliert werden
// darf, dass keine Kopie erstellt wird, weder direkt noch implizit. Die
// History wird via Referenz in /secure/memory hinterlegt, damit sie einen
// Reload des Channeld ueberlebt. Das funktioniert aber nur, wenn die Mapping-
// Referenz in Memory und Channeld dieselbe ist.
private nosave mapping channelH;

/* Globale channeld-Stats (Startzeit, geladen von, Anzahl erstellte und
   zerstoerte Ebenen.
   mapping stats = ([ "time" : int object_time(),
                      "boot" : string getuid(previous_object()),
                      "new"  : int total_channels_created,
                      "disposed" : int total_channels_removed ]) */
// stats wird in create() geeignet initialisiert
private nosave mapping stats;

/* Ebenen-Cache, enthaelt Daten zu inaktiven Ebenen.
   mapping channelC = ([ string channelname : ({ string I_NAME,
                                                 string I_INFO,
                                                 int time() }) ]) */
private mapping channelC = ([]);

/* Liste von Spielern, fuer die ein Bann besteht, mit den verbotenen Kommandos
   mapping channelB = ([ string playername : string* banned_command ]) */
private mapping channelB = ([]);

/* Timeout-Liste der Datenabfrage-Kommandos; die Timestamps werden verwendet,
   um sicherzustellen, dass jedes Kommando max. 1x pro Minute benutzt werden
   kann.

   mapping Tcmd = ([ "lag": int timestamp,
                     "uptime":  int timestamp,
                     "statistik":  int timestamp]) */
private mapping Tcmd = ([]);

/* Flag, das anzeigt, dass Daten veraendert wurden und beim naechsten
   Speicherevent das Savefile geschrieben werden soll.
   Wird auf 0 oder 1 gesetzt. */
private nosave int save_me_soon;


// BEGIN OF THE CHANNEL MASTER ADMINISTRATIVE PART

// Indizes fuer Zugriffe auf das Mapping <admin>.
#define RECV    0
#define SEND    1
#define FLAG    2

// Ebenenflags, gespeichert in admin[ch, FLAG]
// F_WIZARD kennzeichnet reine Magierebenen
#define F_WIZARD 1
// Ebenen, auf denen keine Gaeste erlaubt sind, sind mit F_NOGUEST markiert.
#define F_NOGUEST 2

/* Speichert Sende- und Empfangslevel sowie Flags zu den einzelnen Channeln.
   Wird beim Laden des Masters via create() -> initalize() -> setup() mit den
   Daten aus dem Init-File ./channeld.init befuellt.
   mapping admin = ([ string channel_name : int RECV_LVL,
                                            int SEND_LVL,
                                            int FLAG ]) */
private nosave mapping admin = m_allocate(0, 3);

// check_ch_access() prueft die Zugriffsberechtigungen auf Ebenen.
//
// Gibt 1 zurueck, wenn Aktion erlaubt, 0 sonst.
// Wird von access() gerufen; access() gibt das Ergebnis von
// check_ch_access() zurueck.
//
// Verlassen (C_LEAVE) ist immer erlaubt. Die anderen Aktionen sind in zwei
// Gruppen eingeteilt:
// 1) RECV. Die Aktionen dieser Gruppe sind Suchen (C_FIND), Auflisten
//          (C_LIST) und Betreten (C_JOIN).
// 2) SEND. Die Aktion dieser Gruppe ist zur Zeit nur Senden (C_SEND).
//
// Aktionen werden zugelassen, wenn Spieler/MagierLevel groesser ist als die
// fuer die jeweilige Aktionsgruppe RECV oder SEND festgelegte Stufe.
// Handelt es sich um eine Magierebene (F_WIZARD), muss die Magierstufe
// des Spielers groesser sein als die Mindeststufe der Ebene. Ansonsten
// wird gegen den Spielerlevel geprueft.
//
// Wenn RECV_LVL oder SEND_LVL auf -1 gesetzt ist, sind die Aktionen der
// jeweiligen Gruppen komplett geblockt.

public int check_ch_access(string ch, object pl, string cmd)
{
  // <pl> ist Gast, es sind aber keine Gaeste zugelassen? Koennen wir
  // direkt ablehnen.
  if ((admin[ch, FLAG] & F_NOGUEST) && pl->QueryGuest())
    return 0;

  // Ebenso auf Magier- oder Seherebenen, wenn ein Spieler anfragt, der
  // noch kein Seher ist.
  if ((admin[ch, FLAG] & F_WIZARD) && query_wiz_level(pl) < SEER_LVL)
    return 0;

  // Ebene ist Magierebene? Dann werden alle Stufenlimits gegen Magierlevel
  // geprueft, ansonsten gegen Spielerlevel.
  int level = (admin[ch, FLAG] & F_WIZARD
                  ? query_wiz_level(pl)
                  : pl->QueryProp(P_LEVEL));

  switch (cmd)
  {
    case C_FIND:
    case C_LIST:
    case C_JOIN:
      if (admin[ch, RECV] == -1)
        return 0;
      if (admin[ch, RECV] <= level)
        return 1;
      break;

    case C_SEND:
      if (admin[ch, SEND] == -1)
        return 0;
      if (admin[ch, SEND] <= level)
        return 1;
      break;

    // Verlassen ist immer erlaubt
    case C_LEAVE:
      return 1;

    default:
      break;
  }
  return (0);
}

/* CountUsers() zaehlt die Anzahl Abonnenten aller Ebenen. */
// TODO: Mapping- und Arrayvarianten bzgl. der Effizienz vergleichen
private int CountUsers()
{
  object* userlist = ({});
  foreach(string ch_name, mixed* ch_data : channels)
  {
    userlist += ch_data[I_MEMBER];
  }
  // Das Mapping dient dazu, dass jeder Eintrag nur einmal vorkommt.
  return sizeof(mkmapping(userlist));
}

// Ist das Objekt <sender> Abonnent der Ebene <ch>?
private int IsChannelMember(string ch, object sender)
{
  return (member(channels[ch][I_MEMBER], sender) != -1);
}

// Besteht fuer das Objekt <ob> ein Bann fuer die Ebenenfunktion <command>?
private int IsBanned(string|object ob, string command)
{
  if (objectp(ob))
    ob = getuid(ob);
  return(pointerp(channelB[ob]) &&
         member(channelB[ob], command) != -1);
}

private void banned(string plname, string* cmds, string res)
{
  res += sprintf("%s [%s], ", capitalize(plname), implode(cmds, ","));
}

#define TIMEOUT       (time() - 60)

// IsNotBlocked(): prueft fuer die Liste der uebergebenen Kommandos, ob
// die Zeitsperre fuer alle abgelaufen ist und sie ausgefuehrt werden duerfen.
// Dabei gilt jedes Kommando, dessen letzte Nutzung laenger als 60 s
// zurueckliegt, als "nicht gesperrt".
private int IsNotBlocked(string* cmd)
{
  string* res = filter(cmd, function int (string str) {
                    return (Tcmd[str] < TIMEOUT);
                  });
  // Wenn das Ergebnis-Array genauso gross ist wie das Eingabe-Array, dann
  // sind alle Kommandos frei. Sie werden direkt gesperrt; return 1
  // signalisiert dem Aufrufer, dass das Kommando ausgefuehrt werden darf.
  if (sizeof(res) == sizeof(cmd)) {
    foreach(string str : cmd) {
      Tcmd[str] = time();
    }
    return 1;
  }
  return 0;
}

// Prueft, ob der gesendete Befehl <cmd> als gueltiges Kommando <check>
// zugelassen wird. Anforderungen:
// 1) <cmd> muss Teilstring von <check> sein
// 2) <cmd> muss am Anfang von <check> stehen
// 3) <cmd> darf nicht laenger sein als <check>
// 4) die Nutzung von <cmd> darf nur einmal pro Minute erfolgen
// Beispiel: check = "statistik", cmd = "stat" ist gueltig, nicht aber
// cmd = "statistiker" oder cmd = "tist"
// Wenn die Syntax zugelassen wird, wird anschliessend geprueft
private int IsValidChannelCommand(string cmd, string check) {
  // Syntaxcheck (prueft Bedingungen 1 bis 3).
  if ( strstr(check, cmd)==0 && sizeof(cmd) <= sizeof(check) ) {
    string* cmd_to_check;
    // Beim Kombi-Kommando "lust" muessen alle 3 Befehle gecheckt werden.
    // Der Einfachheit halber werden auch Einzelkommandos als Array ueber-
    // geben.
    if ( cmd == "lust" )
      cmd_to_check = ({"lag", "statistik", "uptime"});
    else
      cmd_to_check = ({cmd});
    // Prueft die Zeitsperre (Bedingung 4).
    return (IsNotBlocked(cmd_to_check));
  }
  return 0;
}

#define CH_NAME   0
#define CH_SENDER 1
#define CH_MSG    2
#define CH_MSG_TYPE 3
// Gibt die Channelmeldungen fuer die Kommandos up, stat, lag und bann des
// <MasteR>-Channels aus. Auszugebende Informationen werden in <ret> gesammelt
// und dieses per Callout an send() uebergeben.
// Argument: ({string channels[ch][I_NAME], object pl, string msg, int type})
// Funktion muss public sein, auch wenn der erste Check im Code das Gegenteil
// nahezulegen scheint, weil sie von send() per call_other() gerufen wird,
// was aber bei einer private oder protected Funktion nicht moeglich waere.
public void ChannelMessage(<string|object|int>* msg)
{
  // Wir reagieren nur auf Meldungen, die wir uns selbst geschickt haben,
  // aber nur dann, wenn sie auf der Ebene <MasteR> eingegangen sind.
  if (msg[CH_SENDER] == this_object() || !stringp(msg[CH_MSG]) ||
      msg[CH_NAME] != CMNAME || previous_object() != this_object())
    return;

  float* lag;
  int max, rekord;
  string ret;
  string mesg = msg[CH_MSG];

  if (IsValidChannelCommand(mesg, "hilfe"))
  {
    ret = "Folgende Kommandos gibt es: hilfe, lag, uptime, statistik, lust, "
      "bann. Die Kommandos koennen abgekuerzt werden.";
  }
  else if (IsValidChannelCommand(mesg, "lag"))
  {
    lag = "/p/daemon/lag-o-daemon"->read_ext_lag_data();
    ret = sprintf("Lag: %.1f%%/60, %.1f%%/15, %.1f%%/5, %.1f%%/1, "
              "%.1f%%/20s, %.1f%%/2s",
              lag[5], lag[4], lag[3], lag[2], lag[1], lag[0]);
    // Erster Callout wird hier schon abgesetzt, um sicherzustellen, dass
    // die Meldung in zwei Zeilen auf der Ebene erscheint.
    call_out(#'send, 2, CMNAME, this_object(), ret);
    ret = query_load_average();
  }
  else if (IsValidChannelCommand(mesg, "uptime"))
  {
    if (file_size("/etc/maxusers") > 0 && file_size("/etc/maxusers.ever"))
    {
      string unused;
      sscanf(read_file("/etc/maxusers"), "%d %s", max, unused);
      sscanf(read_file("/etc/maxusers.ever"), "%d %s", rekord, unused);
      ret = sprintf("Das MUD laeuft jetzt %s. Es sind momentan %d Spieler "
              "eingeloggt; das Maximum lag heute bei %d und der Rekord "
              "bisher ist %d.", uptime(), sizeof(users()), max, rekord);
    }
    else
    {
      ret = "Diese Information liegt nicht vor.";
    }
  }
  else if (IsValidChannelCommand(mesg, "statistik"))
  {
    ret = sprintf(
        "Im Moment sind insgesamt %d Ebenen mit %d Teilnehmern aktiv. "
        "Der %s wurde das letzte mal am %s von %s neu gestartet. "
        "Seitdem wurden %d Ebenen neu erzeugt und %d zerstoert.",
        sizeof(channels), CountUsers(), CMNAME,
        dtime(stats["time"]), stats["boot"], stats["new"], stats["dispose"]);
  }
  // Ebenenaktion beginnt mit "bann"?
  else if (strstr(mesg, "bann")==0)
  {
    string pl, cmd;

    if (mesg == "bann")
    {
      if (sizeof(channelB))
      {
        ret = "Fuer folgende Spieler besteht ein Bann: ";
        // Zwischenspeicher fuer die Einzeleintraege, um diese spaeter mit
        // CountUp() in eine saubere Aufzaehlung umwandeln zu koennen.
        string* banlist = ({});
        foreach(string plname, string* banned_cmds : channelB) {
          banlist += ({ sprintf("%s [%s]",
            capitalize(plname), implode(banned_cmds, ", "))});
        }
        ret = CountUp(banlist);
      }
      else
      {
        ret = "Zur Zeit ist kein Bann aktiv.";
      }
    }
    else
    {
      if (sscanf(mesg, "bann %s %s", pl, cmd) == 2 &&
          IS_DEPUTY(msg[CH_SENDER]))
      {
        pl = lower_case(pl);
        cmd = lower_case(cmd);

        if (member(CMDS, cmd) != -1)
        {
          // Kein Eintrag fuer <pl> in der Bannliste vorhanden, dann anlegen;
          // ist der Eintrag kein Array, ist ohnehin was faul, dann wird
          // ueberschrieben.
          if (!pointerp(channelB[pl]))
            m_add(channelB, pl, ({}));

          if (IsBanned(pl, cmd))
            channelB[pl] -= ({ cmd });
          else
            channelB[pl] += ({ cmd });

          ret = "Fuer '" + capitalize(pl) + "' besteht " +
                (sizeof(channelB[pl])
                  // TODO: implode() -> CountUp()?
                  ? "folgender Bann: " + implode(channelB[pl], ", ") + "."
                  : "kein Bann mehr.");

          // Liste der gebannten Kommandos leer? Dann <pl> komplett austragen.
          if (!sizeof(channelB[pl]))
            m_delete(channelB, pl);

          //TODO: save_me_soon=1 sollte auch reichen...
          save_object(CHANNEL_SAVE);
        }
        else
        {
          ret = "Das Kommando '" + cmd + "' ist unbekannt. "
                "Erlaubte Kommandos: "+ CountUp(CMDS);
        }
      }
      else
      {
        if (IS_ARCH(msg[CH_SENDER]))
          ret = "Syntax: bann <name> <kommando>";
      }
    }
  }
  else if (IsValidChannelCommand(mesg, "lust"))
  {
    lag = "/p/daemon/lag-o-daemon"->read_lag_data();
    if (file_size("/etc/maxusers") > 0 && file_size("/etc/maxusers.ever"))
    {
      string unused;
      sscanf(read_file("/etc/maxusers"), "%d %s", max, unused);
      sscanf(read_file("/etc/maxusers.ever"), "%d %s", rekord, unused);
    }

    int t = time() - last_reboot_time();

    // TODO: fuer solche Anwendungen ein separates Inheritfile bauen, da
    // die Funktionalitaet oefter benoetigt wird als nur hier.
    string up = "";
    if (t >= 86400)
      up += sprintf("%dT", t / 86400);

    t %= 86400;
    if (t >= 3600)
      up += sprintf("%dh", t / 3600);

    t %= 3600;
    if (t > 60)
      up += sprintf("%dm", t / 60);

    up += sprintf("%ds", t % 60);

    ret = sprintf("%.1f%%/15 %.1f%%/1 %s %d:%d:%d E:%d T:%d",
            lag[1], lag[2], up, sizeof(users()), max, rekord,
            sizeof(channels), CountUsers());
  }
  else
  {
    return;
  }

  // Nur die Ausgabe starten, wenn ein Ausgabestring vorliegt. Es kann
  // vorkommen, dass weiter oben keiner zugewiesen wird, weil die Bedingungen
  // nicht erfuellt sind.
  if (stringp(ret) && sizeof(ret))
    call_out(#'send, 2, CMNAME, this_object(), ret);
}

// setup() -- set up a channel and register it
//            arguments are stored in the following order:
//            string* chinfo = ({ channel_name, receive_level, send_level,
//                                flags, description, supervisor })
private void setup(string* chinfo)
{
  string desc = "- Keine Beschreibung -";
  object supervisor = this_object();

  if (sizeof(chinfo) && sizeof(chinfo[0]) > 1 && chinfo[0][0] == '\\')
    chinfo[0] = chinfo[0][1..];

  switch (sizeof(chinfo))
  {
    // Alle Fallthroughs in dem switch() sind Absicht.
    case 6:
      if (stringp(chinfo[5]) && sizeof(chinfo[5]))
        catch(supervisor = load_object(chinfo[5]); publish);
      if (!objectp(supervisor))
        supervisor = this_object();

    case 5:
      if (stringp(chinfo[4]) || closurep(chinfo[4]))
        desc = chinfo[4];
    // Die admin-Daten sind nicht fuer die Ebene wichtig, nur fuer die
    // check_ch_access().
    case 4:
      admin[lower_case(chinfo[0]), FLAG] = to_int(chinfo[3]);

    case 3:
      admin[lower_case(chinfo[0]), SEND] = to_int(chinfo[2]);

    case 2:
      admin[lower_case(chinfo[0]), RECV] = to_int(chinfo[1]);
      break;

    case 0:
    default:
      return;
  }

  if (new(chinfo[0], supervisor, desc) == E_ACCESS_DENIED)
  {
    log_file("CHANNEL", sprintf("[%s] %s: %O: error, access denied\n",
      dtime(time()), chinfo[0], supervisor));
  }
  return;
}

private void initialize()
{
  string ch_list;
#if !defined(__TESTMUD__) && MUDNAME=="MorgenGrauen"
  ch_list = read_file(object_name(this_object()) + ".init");
#else
  ch_list = read_file(object_name(this_object()) + ".init.testmud");
#endif

  if (!stringp(ch_list))
    return;

  // Channeldatensaetze erzeugen, dazu zuerst Datenfile in Zeilen zerlegen
  // "Allgemein:   0: 0: 0:Allgemeine Unterhaltungsebene"
  // Danach drueberlaufen und in Einzelfelder splitten, dabei gleich die
  // Trennzeichen (Doppelpunkt, Tab und Space) rausfiltern.
  foreach(string ch : old_explode(ch_list, "\n"))
  {
    if (ch[0]=='#')
      continue;
    setup( regexplode(ch, ":[ \t]*", RE_OMIT_DELIM) );
  }
}

// BEGIN OF THE CHANNEL MASTER IMPLEMENTATION
protected void create()
{
  seteuid(getuid());
  restore_object(CHANNEL_SAVE);

  //TODO: weitere Mappings im MEMORY speichern, Savefile ersetzen.

  /* Die Channel-History wird nicht nur lokal sondern auch noch im Memory
     gespeichert, dadurch bleibt sie auch ueber ein Reload erhalten.
     Der folgende Code versucht, den Zeiger aus dem Memory zu holen. Falls
     das nicht moeglich ist, wird ein neuer erzeugt und gegebenenfalls im
     Memory abgelegt. */

  // Hab ich die noetigen Rechte im Memory?
  if (call_other(MEMORY, "HaveRights"))
  {
    // Objektpointer laden
    channelH = ({mapping}) call_other(MEMORY, "Load", "History");

    // Wenns nich geklappt hat, hat der Memory noch keinen Zeiger, dann
    if (!mappingp(channelH))
    {
      // Zeiger erzeugen
      channelH = ([]);
      // und in den Memory schreiben
      call_other(MEMORY, "Save", "History", channelH);
    }
  }
  else
  {
    // Keine Rechte im Memory, dann wird mit einem lokalen Zeiger gearbeitet.
    channelH = ([]);
  }

  stats = (["time": time(),
            "boot": capitalize(getuid(previous_object()) || "<Unbekannt>")]);

  // <MasteR>-Ebene erstellen. Channeld wird Ebenenbesitzer und somit auch
  // Zuhoerer, damit er auf Kommandos auf dieser Ebene reagieren kann.
  new(CMNAME, this_object(), "Zentrale Informationen zu den Ebenen");

  initialize();
  users()->RegisterChannels();

  // Die Zugriffskontrolle auf die Ebenen wird von der Funktion access()
  // erledigt. Weil sowohl externe Aufrufe aus dem Spielerobjekt, als auch
  // interne Aufrufe aus diesem Objekt vorkommen koennen, wird hier ein
  // explizites call_other() auf this_object() gemacht, damit der
  // Caller-Stack bei dem internen Aufruf denselben Aufbau hat wie bei
  // einem externen.
  this_object()->send(CMNAME, this_object(),
    sprintf("%d Ebenen mit %d Teilnehmern initialisiert.",
      sizeof(channels),
      CountUsers()));
}

varargs void reset()
{
  //TODO reset nur 1-2mal am Tag mit etwas random.

  // Cache bereinigen entsprechend dessen Timeout-Zeit (12 h)
  // TODO: Zeit auf 2-3 Tage erhoehen.
  // TODO 2: Zeit dynamisch machen und nur expiren, wenn mehr als n Eintraege.
  // Zeit reduzieren, bis nur noch n/2 Eintraege verbleiben.
  channelC = filter(channelC,
      function int (string ch_name, <string|int>* data)
      {
        if (channelC[ch_name][2] + 43200 > time())
          return 1;
        // Ebenendaten koennen weg, inkl. History, die also auch loeschen
        m_delete(channelH, ch_name);
        return 0;
      });

  if (save_me_soon)
  {
    save_me_soon = 0;
    save_object(CHANNEL_SAVE);
  }
}

// name() - define the name of this object.
string name()
{
  return CMNAME;
}

string Name()
{
  return CMNAME;
}

// Low-level function for adding members without access checks
private int add_member(string ch, object m)
{
  if (IsChannelMember(ch, m))
    return E_ALREADY_JOINED;

  channels[ch][I_MEMBER] += ({ m });
  return 0;
}


#define CHAN_NAME(x) channels[x][I_NAME]
#define SVISOR_OB(x) channels[x][I_SUPERVISOR]
#define ACC_CLOSURE(x) channels[x][I_ACCESS]

// Aendert das Supervisor-Objekt einer Ebene, ggf. mit Meldung.
// Wenn kein neuer SV angegeben, wird der aelteste Zuhoerer gewaehlt.
private int change_sv_object(string ch, object old_sv, object new_sv)
{
  if (!new_sv)
  {
    channels[ch][I_MEMBER] -= ({0});
    if (sizeof(channels[ch][I_MEMBER]))
      new_sv = channels[ch][I_MEMBER][0];
    else
      return 0; // kein neuer SV moeglich.
  }
  SVISOR_OB(ch) = new_sv;
  if (old_sv && new_sv
      && !old_sv->QueryProp(P_INVIS)
      && !new_sv->QueryProp(P_INVIS))
  {
    // Die Zugriffskontrolle auf die Ebenen wird von der Funktion access()
    // erledigt. Weil sowohl externe Aufrufe aus dem Spielerobjekt, als auch
    // interne Aufrufe aus diesem Objekt vorkommen koennen, wird hier ein
    // explizites call_other() auf this_object() gemacht, damit der
    // Caller-Stack bei dem internen Aufruf denselben Aufbau hat wie bei
    // einem externen.
    this_object()->send(ch, old_sv,
        sprintf("uebergibt die Ebene an %s.",new_sv->name(WEN)),
        MSG_EMOTE);
  }
  else if (old_svn && !old_sv->QueryProp(P_INVIS))
  {
    this_object()->send(ch, old_sv,
        "uebergibt die Ebene an jemand anderen.", MSG_EMOTE);
  }
  else if (new_sv && !new_sv->QueryProp(P_INVIS))
  {
    this_object()->send(ch, new_sv,
        "uebernimmt die Ebene von jemand anderem.", MSG_EMOTE);
  }
  return 1;
}

// Stellt sicher, dass einen Ebenen-Supervisor gibt. Wenn dies nicht moeglich
// ist (z.b. leere Ebene), dann wird die Ebene geloescht und 0
// zurueckgegeben. Allerdings kann nach dieser Funktion sehr wohl die I_ACCESS
// closure 0 sein, wenn der SV keine oeffentliche definiert! In diesem Fall
// wird access() den Zugriff immer erlauben.
private int assert_supervisor(string ch)
{
  // Es ist keine Closure vorhanden, d.h. der Ebenenbesitzer wurde zerstoert.
  if (!closurep(ACC_CLOSURE(ch)))
  {
    // Wenn der Ebenenbesitzer als String eingetragen ist, versuchen wir,
    // die Closure wiederherzustellen. Dabei wird das Objekt gleichzeitig
    // neugeladen und eingetragen.
    if (stringp(SVISOR_OB(ch)))
    {
      closure new_acc_cl;
      string err = catch(new_acc_cl=
                          symbol_function("check_ch_access", SVISOR_OB(ch));
                          publish);
      /* Wenn das SV-Objekt neu geladen werden konnte, wird es als Mitglied
       * eingetragen. Auch die Closure wird neu eingetragen, allerdings kann
       * sie 0 sein, wenn das SV-Objekt keine oeffentliche check_ch_access()
       * mehr definiert. In diesem Fall gibt es zwar ein SV-Objekt, aber keine
       * Zugriffrechte(pruefung) mehr. */
      if (!err)
      {
        ACC_CLOSURE(ch) = new_acc_cl;
        // Der neue Ebenenbesitzer tritt auch gleich der Ebene bei. Hierbei
        // erfolgt keine Pruefung des Zugriffsrechtes (ist ja unsinnig, weil
        // er sich ja selber genehmigen koennte), und auch um eine Rekursion
        // zu vermeiden.
        add_member(ch, find_object(SVISOR_OB(ch)));
        // Rueckgabewert ist 1, ein neues SV-Objekt ist eingetragen.
      }
      else
      {
        log_file("CHANNEL", sprintf("[%s] Channel deleted. %O -> %O\n",
                  dtime(time()), SVISOR_OB(ch), err));
        m_delete(channels, ch);
        return 0;
      }
    }
    else
    {
      // In diesem Fall muss ein neues SV-Objekt gesucht und ggf. eingetragen
      // werden. Wir nehmen das aelteste Mitglied der Ebene.
      // TODO: kaputte Objekte raussortieren, neuen Master bestimmen, wenn
      // dieser nicht mehr existiert.

    }
  }
  return 1;
}

// access() - check access by looking for the right argument types and
//            calling access closures respectively
// SEE: new, join, leave, send, list, users
// Note: <pl> is usually an object, only the master supplies a string during
//       runtime error handling.
// Wertebereich: 0 fuer Zugriff verweigert, 1 fuer Zugriff erlaubt, 2 fuer
// Zugriff erlaubt fuer privilegierte Objekte, die senden duerfen ohne
// Zuhoerer zu sein. (Die Aufrufer akzeptieren aber auch alle negativen Werte
// als Erfolg und alles ueber >2 als privilegiert.)
varargs private int access(string ch, object|string pl, string cmd,
                           string txt)
{
  if (!sizeof(ch))
    return 0;

  ch = lower_case(ch);
  if(!pointerp(channels[ch]))
    return 0;

  // Dieses Objekt  und Root-Objekte duerfen auf der Ebene senden, ohne
  // Mitglied zu sein. Das ist die Folge der zurueckgegebenen 2.
  if ( !previous_object(1) || !extern_call() ||
       previous_object(1) == this_object() ||
       getuid(previous_object(1)) == ROOTID)
    return 2;

  // Nur dieses Objekt darf Meldungen im Namen anderer Objekte faken,
  // ansonsten muss <pl> der Aufrufer sein.
  if (!objectp(pl) ||
      ((previous_object(1) != pl) && (previous_object(1) != this_object())))
    return 0;

  if (IsBanned(pl, cmd))
    return 0;

  // Wenn kein SV-Objekt mehr existiert und kein neues bestimmt werden konnte,
  // wurde die Ebene ausfgeloest. In diesem Fall auch den Zugriff verweigern.
  if (!assert_supervisor(ch))
    return 0;
  // Wenn closure jetzt dennoch 0, wird der Zugriff erlaubt.
  if (!ACC_CLOSURE(ch))
    return 1;

  // Das SV-Objekt wird gefragt, ob der Zugriff erlaubt ist. Dieses erfolgt
  // fuer EM+ aber nur, wenn der CHANNELD selber das SV-Objekt ist, damit
  // nicht beliebige SV-Objekt EMs den Zugriff verweigern koennen. Ebenen mit
  // CHANNELD als SV koennen aber natuerlich auch EM+ Zugriff verweigern.
  if (IS_ARCH(previous_object(1))
      && find_object(SVISOR_OB(ch)) != this_object())
    return 1;

  return funcall(ACC_CLOSURE(ch), ch, pl, cmd, &txt);
}

// Neue Ebene <ch> erstellen mit <owner> als Ebenenbesitzer.
// <info> kann die statische Beschreibung der Ebene sein oder eine Closure,
// die dynamisch aktualisierte Infos ausgibt.
// Das Objekt <owner> kann eine Funktion check_ch_access() definieren, die
// gerufen wird, wenn eine Ebenenaktion vom Typ join/leave/send/list/users
// eingeht.
// check_ch_access() dient der Zugriffskontrolle und entscheidet, ob die
// Nachricht gesendet werden darf oder nicht.

// Ist keine Closure angegeben, wird die in diesem Objekt (Channeld)
// definierte Funktion gleichen Namens verwendet.
#define IGNORE  "^/xx"

// TODO: KOMMENTAR
//check may contain a closure
//         called when a join/leave/send/list/users message is received
public varargs int new(string ch_name, object owner, string|closure info)
{
  // Kein Channelmaster angegeben, oder wir sind es selbst, aber der Aufruf
  // kam von ausserhalb. (Nur der channeld selbst darf sich als Channelmaster
  // fuer eine neue Ebene eintragen.)
  if (!objectp(owner) || (owner == this_object() && extern_call()) )
    return E_ACCESS_DENIED;

  // Kein gescheiter Channelname angegeben.
  if (!stringp(ch_name) || !sizeof(ch_name))
    return E_ACCESS_DENIED;

  // Channel schon vorhanden oder schon alle Channel-Slots belegt.
  if (channels[lower_case(ch_name)] || sizeof(channels) >= MAX_CHANNELS)
    return E_ACCESS_DENIED;

  // Der angegebene Ebenenbesitzer darf keine Ebenen erstellen, wenn fuer ihn
  // ein Bann auf die Aktion C_NEW besteht, oder das Ignore-Pattern auf
  // seinen Objektnamen matcht.
  if (IsBanned(owner,C_NEW) || regmatch(object_name(owner), IGNORE))
    return E_ACCESS_DENIED;

  // Keine Infos mitgeliefert? Dann holen wir sie aus dem Cache.
  if (!info)
  {
    if (channelC[lower_case(ch_name)])
    {
      ch_name = channelC[lower_case(ch_name)][0];
      info = channelC[lower_case(ch_name)][1];
    }
    else
    {
      return E_ACCESS_DENIED;
    }
  }
  else
  {
    channelC[lower_case(ch_name)] = ({ ch_name, info, time() });
  }

  object* pls = ({ owner });
  m_add(channels, lower_case(ch_name),
           ({ pls,
              symbol_function("check_ch_access", owner) || #'check_ch_access,
              info,
              (!living(owner) && !clonep(owner) && owner != this_object()
                  ? object_name(owner)
                  : owner),
              ch_name }));

  // History fuer eine Ebene nur dann initialisieren, wenn es sie noch
  // nicht gibt.
  if (!pointerp(channelH[lower_case(ch_name)]))
    channelH[lower_case(ch_name)] = ({});

  // Erstellen neuer Ebenen loggen, wenn wir nicht selbst der Ersteller sind.
  if (owner != this_object())
    log_file("CHANNEL.new", sprintf("[%s] %O: %O %O\n",
        dtime(time()), ch_name, owner, info));

  // Erfolgsmeldung ausgeben, ausser bei unsichtbarem Ebenenbesitzer.
  if (!owner->QueryProp(P_INVIS))
  {
    // Die Zugriffskontrolle auf die Ebenen wird von der Funktion access()
    // erledigt. Weil sowohl externe Aufrufe aus dem Spielerobjekt, als auch
    // interne Aufrufe aus diesem Objekt vorkommen koennen, wird hier ein
    // explizites call_other() auf this_object() gemacht, damit der
    // Caller-Stack bei dem internen Aufruf denselben Aufbau hat wie bei
    // einem externen.
    this_object()->send(CMNAME, owner,
      "laesst die Ebene '" + ch_name + "' entstehen.", MSG_EMOTE);
  }

  stats["new"]++;
  save_me_soon = 1;
  return (0);
}

// Objekt <pl> betritt Ebene <ch>. Dies wird zugelassen, wenn <pl> die
// Berechtigung hat und noch nicht Mitglied ist. (Man kann einer Ebene nicht
// zweimal beitreten.)
public int join(string ch, object pl)
{
  ch = lower_case(ch);
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  if (!funcall(#'access, ch, pl, C_JOIN))
    return E_ACCESS_DENIED;

  return add_member(ch, pl);
}

// Objekt <pl> verlaesst Ebene <ch>.
// Zugriffsrechte werden nur der Vollstaendigkeit halber geprueft; es duerfte
// normalerweise keinen Grund geben, das Verlassen einer Ebene zu verbieten.
// Dies ist in check_ch_access() so geregelt, allerdings koennte dem Objekt
// <pl> das Verlassen auf Grund eines Banns verboten sein.
// Wenn kein Spieler mehr auf der Ebene ist, loest sie sich auf, sofern nicht
// noch ein Ebenenbesitzer eingetragen ist.
public int leave(string ch, object pl)
{
  ch = lower_case(ch);
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  if (!funcall(#'access, ch, pl, C_LEAVE))
    return E_ACCESS_DENIED;

  channels[ch][I_MEMBER] -= ({0}); // kaputte Objekte erstmal raus

  if (!IsChannelMember(ch, pl))
    return E_NOT_MEMBER;

  // Erstmal den Zuhoerer raus.
  channels[ch][I_MEMBER] -= ({pl});

  // Wenn auf der Ebene jetzt noch Objekte zuhoeren, muss ggf. der SV
  // wechseln.
  if (sizeof(channels[ch][I_MEMBER]))
  {
    // Kontrolle an jemand anderen uebergeben, wenn der Ebenensupervisor
    // diese verlassen hat. change_sv_object() waehlt per Default den
    // aeltesten Zuhoerer.
    if (pl == channels[ch][I_SUPERVISOR])
    {
      change_sv_object(ch, pl, 0);
    }
  }

  // Ebene loeschen, wenn keiner zuhoert und auch kein Masterobjekt
  // existiert.
  // Wenn Spieler, NPC, Clone oder Channeld als letztes die Ebene verlassen,
  // wird diese zerstoert, mit Meldung.
  if (!sizeof(channels[ch][I_MEMBER]) && !stringp(channels[ch][I_SUPERVISOR]))
  {
    // Der Letzte macht das Licht aus, aber nur, wenn er nicht unsichtbar ist.
    if (!pl->QueryProp(P_INVIS))
    {
      // Die Zugriffskontrolle auf die Ebenen wird von der Funktion access()
      // erledigt. Weil sowohl externe Aufrufe aus dem Spielerobjekt, als auch
      // interne Aufrufe aus diesem Objekt vorkommen koennen, wird hier ein
      // explizites call_other() auf this_object() gemacht, damit der
      // Caller-Stack bei dem internen Aufruf denselben Aufbau hat wie bei
      // einem externen.
      this_object()->send(CMNAME, pl,
        "verlaesst als "+
        (pl->QueryProp(P_GENDER) == 1 ? "Letzter" : "Letzte")+
        " die Ebene '"+channels[ch][I_NAME]+"', worauf diese sich in "
        "einem Blitz oktarinen Lichts aufloest.", MSG_EMOTE);
    }
    channelC[lower_case(ch)] =
      ({ channels[ch][I_NAME], channels[ch][I_INFO], time() });

    // Ebene loeschen
    m_delete(channels, lower_case(ch));
    // History wird nicht geloescht, damit sie noch verfuegbar ist, wenn die
    // Ebene spaeter nochmal neu erstellt wird. Sie wird dann bereinigt, wenn
    // channelC bereinigt wird.

    stats["dispose"]++;
    save_me_soon = 1;
  }
  return (0);
}

// Nachricht <msg> vom Typ <type> mit Absender <pl> auf der Ebene <ch> posten,
// sofern <pl> dort senden darf.
public varargs int send(string ch, object pl, string msg, int type)
{
  ch = lower_case(ch);
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  int a = funcall(#'access, ch, pl, C_SEND, msg);
  if (!a)
    return E_ACCESS_DENIED;

  // a<2 bedeutet effektiv a==1 (weil a==0 oben rausfaellt), was dem
  // Rueckgabewert von check_ch_access() entspricht, wenn die Aktion zugelassen
  // wird. access() allerdings 2 fuer "privilegierte" Objekte (z.B.
  // ROOT-Objekte oder den channeld selber). Der Effekt ist, dass diese
  // Objekte auf Ebenen senden duerfen, auf denen sie nicht zuhoeren.
  if (a < 2 && !IsChannelMember(ch, pl))
    return E_NOT_MEMBER;

  if (!msg || !stringp(msg) || !sizeof(msg))
    return E_EMPTY_MESSAGE;

  // Jedem Mitglied der Ebene wird die Nachricht ueber die Funktion
  // ChannelMessage() zugestellt. Der Channeld selbst hat ebenfalls eine
  // Funktion dieses Namens, so dass er, falls er Mitglied der Ebene ist, die
  // Nachricht ebenfalls erhaelt.
  // Um die Kommandos der Ebene <MasteR> verarbeiten zu koennen, muss er
  // demzufolge Mitglied dieser Ebene sein. Da Ebenenbesitzer automatisch
  // auch Mitglied sind, wird die Ebene <MasteR> im create() mittels new()
  // erzeugt und der Channeld als Besitzer angegeben.
  // Die Aufrufkette ist dann wie folgt:
  // Eingabe "-< xyz" => pl::ChannelParser() => send() => ChannelMessage()
  channels[ch][I_MEMBER]->ChannelMessage(
                            ({ channels[ch][I_NAME], pl, msg, type}));

  if (sizeof(channelH[ch]) > MAX_HIST_SIZE)
    channelH[ch] = channelH[ch][1..];

  channelH[ch] +=
    ({ ({ channels[ch][I_NAME],
          (stringp(pl)
              ? pl
              : (pl->QueryProp(P_INVIS)
                    ? "/(" + capitalize(getuid(pl)) + ")$"
                    : "")
                  + (pl->Name(WER, 2) || "<Unbekannt>")),
          msg + " <" + strftime("%a, %H:%M:%S") + ">\n",
          type }) });
  return (0);
}

// Gibt ein Mapping mit allen Ebenen aus, die das Objekt <pl> lesen kann,
// oder einen Integer-Fehlercode
public int|mapping list(object pl)
{
  mapping chs = ([]);
  foreach(string chname, <object*|closure|string|object>* chdata : channels)
  {
    /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
       zu erzeugen, weil access() mit extern_call() und previous_object()
       arbeitet und sichergestellt sein muss, dass das in jedem Fall das
       richtige ist. */
    if(funcall(#'access, chname, pl, C_LIST))
    {
      m_add(chs, chname, chdata);
      chs[chname][I_MEMBER] = filter(chs[chname][I_MEMBER], #'objectp);
    }
  }

  if (!sizeof(chs))
    return E_ACCESS_DENIED;
  return (chs);
}

// Ebene suchen, deren Name <ch> enthaelt, und auf der Objekt <pl> senden darf
// Rueckgabewerte:
// - den gefundenen Namen als String
// - String-Array, wenn es mehrere Treffer gibt
// - 0, wenn es keinen Treffer gibt
public string|string* find(string ch, object pl)
{
  ch = lower_case(ch);

  // Suchstring <ch> muss Formatanforderung erfuellen;
  // TODO: soll das ein Check auf gueltigen Ebenennamen als Input sein?
  // Wenn ja, muesste laut Manpage mehr geprueft werden:
  // "Gueltige Namen setzen sich zusammen aus den Buchstaben a-z, A-Z sowie
  // #$%&@<>-." Es wuerden also $%&@ fehlen.
  if (!regmatch(ch, "^[<>a-z0-9#-]+$"))
    return 0;

  // Der Anfang des Ebenennamens muss dem Suchstring entsprechen und das
  // Objekt <pl> muss auf dieser Ebene senden duerfen, damit der Ebenenname
  // in das Suchergebnis aufgenommen wird.
  string* chs = filter(m_indices(channels), function int (string chname) {
                 /* funcall() auf Closure-Operator, um einen neuen Eintrag
                    im Caller Stack zu erzeugen, weil access() mit
                    extern_call() und previous_object() arbeitet und
                    sichergestellt sein muss, dass das in jedem Fall das
                    richtige ist. */
                  return ( stringp(regmatch(chname, "^"+ch)) &&
                           funcall(#'access, chname, pl, C_SEND) );
                });

  int num_channels = sizeof(chs);
  if (num_channels > 1)
    return chs;
  else if (num_channels == 1)
    return channels[chs[0]][I_NAME];
  else
    return 0;
}

// Ebenen-History abfragen.
public int|<int|string>** history(string ch, object pl)
{
  ch = lower_case(ch);
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  if (!funcall(#'access, ch, pl, C_JOIN))
    return E_ACCESS_DENIED;
  else
    return channelH[ch];
}

// Wird aus der Shell gerufen, fuer das Erzmagier-Kommando "kill".
public int remove_channel(string ch, object pl)
{
  //TODO: integrieren in access()?
  if (previous_object() != this_object())
  {
    if (!stringp(ch) ||
        pl != this_player() || this_player() != this_interactive() ||
        this_interactive() != previous_object() ||
        !IS_ARCH(this_interactive()))
      return E_ACCESS_DENIED;
  }

  if (member(channels, lower_case(ch)))
  {
    // Einer geloeschten Ebene kann man nicht zuhoeren: Ebenenname aus der
    // Ebenenliste aller Mitglieder austragen. Dabei werden sowohl ein-, als
    // auch temporaer ausgeschaltete Ebenen beruecksichtigt.
    foreach(object listener : channels[lower_case(ch)][I_MEMBER])
    {
      string* pl_chans = listener->QueryProp(P_CHANNELS);
      if (pointerp(pl_chans))
      {
        listener->SetProp(P_CHANNELS, pl_chans-({lower_case(ch)}));
      }
      pl_chans = listener->QueryProp(P_SWAP_CHANNELS);
      if (pointerp(pl_chans))
      {
        listener->SetProp(P_SWAP_CHANNELS, pl_chans-({lower_case(ch)}));
      }
    }
    // Anschliessend werden die Ebenendaten geloescht.
    m_delete(channels, lower_case(ch));

    // Zaehler fuer zerstoerte Ebenen in der Statistik erhoehen.
    stats["dispose"]++;
  }
  // Dies auuserhalb des Blocks oben ermoeglicht es, inaktive Ebenen bzw.
  // deren Daten zu entfernen.
  m_delete(channelC, lower_case(ch));
  // In diesem Fall der gezielten Loeschung wird auch die History geloescht.
  m_delete(channelH, lower_case(ch));

  save_me_soon = 1;
  return (0);
}

// Wird aus der Shell aufgerufen, fuer das Erzmagier-Kommando "clear".
public int clear_history(string ch)
{
  //TODO: mit access() vereinigen?
  // Sicherheitsabfragen
  if (previous_object() != this_object())
  {
    if (!stringp(ch) ||
        this_player() != this_interactive() ||
        this_interactive() != previous_object() ||
        !IS_ARCH(this_interactive()))
      return E_ACCESS_DENIED;
  }

  // History des Channels loeschen (ohne die ebene als ganzes, daher Key nicht
  // aus dem mapping loeschen.)
  if (pointerp(channelH[lower_case(ch)]))
    channelH[lower_case(ch)] = ({});

  return 0;
}

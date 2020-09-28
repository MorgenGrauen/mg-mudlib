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
// Default-Zeit (in Sekunden), die eine Ebene und ihr History inaktiv sein darf,
// bevor sie expired wird. Das reduziert sich jedoch, falls es zuviele
// inaktive Ebenen gibt. Entspricht 30 Tagen.
#define INACTIVE_EXPIRE 2592000
// Aber eine Ebene darf min. solange inaktiv sein, bevor sie geloescht wird
#define MIN_INACTIVE_LIFETIME 3600
// max. Anzahl der inaktiven Ebenen. Wenn die Haelfte davon ueberschritten
// wird, wird mit zunehmend kleinerem CHANNEL_EXPIRE gearbeitet.
#define MAX_INACTIVE_CHANNELS 500
#define CMDS          ({C_FIND, C_LIST, C_JOIN, C_LEAVE, C_SEND, C_NEW})


// Datenstrukturen fuer die Ebenen.
// Basisdaten, welche auch inaktive Ebenen in channelC haben
struct channel_base_s {
  string          name;    // readable channelname, case-sensitive
  string|closure  desc;    // stat. oder dyn. Beschreibung
  string          creator; // Ersteller der Ebene (Objektname), Besitzer
  int             flags;   // Flags, die bestimmtes Verhalten steuern.
};

// Basisdaten + die von aktiven Ebenen
struct channel_s (channel_base_s) {
  object  supervisor;      // aktueller Supervisor der Ebene
  closure access_cl;       // Closure fuer Zugriffsrechtepruefung
  object  *members;        // Zuhoerer der Ebene
};

/* Ebenenliste und die zugehoerigen Daten in struct (<channel>).
   channels = ([string channelname : (<channel_s>) ])
// HINWEIS: Bitte beachten, dass channels immer nur so manipuliert werden
// darf, dass keine Kopie erstellt wird, weder direkt noch implizit. Die
// History wird via Referenz in /secure/memory hinterlegt, damit sie einen
// Reload des Channeld ueberlebt. Das funktioniert aber nur, wenn die Mapping-
// Referenz in Memory und Channeld dieselbe ist.
 */
private nosave mapping channels;

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

/* Ebenen-Cache, enthaelt Daten zu inaktiven Ebenen.
   mapping channelC = ([ string channelname : (<channel_base_s>);
                                              int time() ])
   Der Zeitstempel ist die letzte Aenderung, d.h. in der Regel des Ablegens in
   channelC.
 */
private mapping channelC = ([:2]);

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

/* Globale channeld-Stats (Startzeit, geladen von, Anzahl erstellte und
   zerstoerte Ebenen.
   mapping stats = ([ "time" : int object_time(),
                      "boot" : string getuid(previous_object()),
                      "new"  : int total_channels_created,
                      "disposed" : int total_channels_removed ]) */
// stats wird in create() geeignet initialisiert
private nosave mapping stats;

/* Flag, das anzeigt, dass Daten veraendert wurden und beim naechsten
   Speicherevent das Savefile geschrieben werden soll.
   Wird auf 0 oder 1 gesetzt. */
private nosave int save_me_soon;

// Private Prototypen
public int join(string chname, object joining);


/* CountUsers() zaehlt die Anzahl Abonnenten aller Ebenen. */
// TODO: Mapping- und Arrayvarianten bzgl. der Effizienz vergleichen
private int CountUsers()
{
  object* userlist = ({});
  foreach(string ch_name, struct channel_s ch : channels)
  {
    userlist += ch.members;
  }
  // Das Mapping dient dazu, dass jeder Eintrag nur einmal vorkommt.
  return sizeof(mkmapping(userlist));
}

// Ist das Objekt <sender> Abonnent der Ebene <ch>?
private int IsChannelMember(struct channel_s ch, object sender)
{
  return (member(ch.members, sender) != -1);
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
// Argument: ({channel.name, object pl, string msg, int type})
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
                  ? "folgender Bann: " + CountUp(channelB[pl]) + "."
                  : "kein Bann mehr.");

          // Liste der gebannten Kommandos leer? Dann <pl> komplett austragen.
          if (!sizeof(channelB[pl]))
            m_delete(channelB, pl);

          save_me_soon = 1;
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
//                                adminflags, channelflags, description,supervisor })
private void setup(string* chinfo)
{
  string desc = "- Keine Beschreibung -";
  object supervisor;
  int chflags;

  if (sizeof(chinfo) && sizeof(chinfo[0]) > 1 && chinfo[0][0] == '\\')
    chinfo[0] = chinfo[0][1..];

  // Wenn der channeld nur neugeladen wurde, aber das Mud nicht neugestartet,
  // sind alle Ebenen noch da, weil sie im MEMORY liegen. D.h. ist die Ebene
  // noch bekannt, muss nichts gemacht werden.
  if (member(channels, lower_case(chinfo[0])))
    return;

  // Nur die Angabe des SV (Index 6) im initfile ist optional, alle Elemente
  // davor muessen da sein.
  if (sizeof(chinfo) < 6)
    return;
  // Bei genug Elementen schauen, ob der SV ladbar ist.
  if (sizeof(chinfo) >= 7)
  {
    if (stringp(chinfo[6]) && sizeof(chinfo[6]))
      catch(supervisor = load_object(chinfo[6]); publish);
  }
  // Aber falls kein SV angegeben wird oder das Objekt nicht ladbar war, wird
  // ein Default-SV genutzt.
  if (!supervisor)
    supervisor = load_object(DEFAULTSV);

  desc = chinfo[5];
  chflags = to_int(chinfo[4]);

  if (new(chinfo[0], supervisor, desc, chflags) == E_ACCESS_DENIED)
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
  int do_complete_init;

  seteuid(getuid());
  restore_object(CHANNEL_SAVE);

  // Altes channelC aus Savefiles konvertieren...
  if (widthof(channelC) == 1)
  {
    mapping new = m_allocate(sizeof(channelC), 2);
    foreach(string chname, mixed arr: channelC)
    {
      struct channel_base_s ch = (<channel_base_s> name: arr[0],
          desc: arr[1]);
      // die anderen beiden Werte bleiben 0
      m_add(new, chname, ch, arr[2]);
    }
    channelC = new;
  }

  /* Die aktiven Ebenen und die Channel-History wird nicht nur lokal sondern
   * auch noch im Memory gespeichert, dadurch bleibt sie auch ueber ein Reload
   * erhalten.
     Der folgende Code versucht, den Zeiger auf die alten Mappings aus dem
     Memory zu holen. Falls das nicht moeglich ist, wird ein neuer erzeugt und
     gegebenenfalls fuer spaeter im Memory abgelegt. */
  // Hab ich die noetigen Rechte im Memory?
  if (MEMORY->HaveRights())
  {
    // channelH und channels laden
    channels = ({mapping}) MEMORY->Load("Channels");
    // Wenns nich geklappt hat, hat der Memory noch keinen Zeiger
    if (!mappingp(channels))
    {
      // Mapping erzeugen
      channels = ([]);
      // und Zeiger auf das Mapping in den Memory schreiben
      MEMORY->Save("Channels", channels);
      do_complete_init = 1;
    }
    // Und das gleiche fuer die History
    channelH = ({mapping}) MEMORY->Load("History");
    // Wenns nich geklappt hat, hat der Memory noch keinen Zeiger
    if (!mappingp(channelH))
    {
      // Mapping erzeugen
      channelH = ([]);
      // und Zeiger auf das Mapping in den Memory schreiben
      MEMORY->Save("History", channelH);
      // In diesem Fall muessen die Ebenenhistories auch erzeugt werden, falls
      // es aktive Ebenen gibt.
      foreach(string chname: channels)
        channelH[chname] = ({});
    }
  }
  else
  {
    // Keine Rechte im Memory, dann liegt das nur lokal und ist bei
    // remove/destruct weg.
    channelH = ([]);
    channels = ([]);
    do_complete_init = 1;
  }

  stats = (["time": time(),
            "boot": capitalize(getuid(previous_object()) || "<Unbekannt>")]);

  // Das muss auch laufen, wenn wir die alten Ebenen aus dem MEMORY bekommen
  // haben, weil es ja neue Ebenen geben koennte, die dann erstellt werden
  // muessen (verschwundete werden aber nicht aufgeraeumt!)
  initialize();
  // <MasteR>-Ebene betreten, damit der channeld auf seine Kommandos auf
  // dieser Ebene reagieren kann.
  this_object()->join(CMNAME, this_object());

  // Wenn wir die alten Ebenen nicht aus MEMORY hatten, gibts noch Dinge zu
  // erledigen.
  if (do_complete_init)
  {
    // Spieler muessen die Ebenen abonnieren. NPC und andere Objekte haben
    // leider Pech gehabt, falls das nicht das erste Laden nach Reboot war.
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
  else
  {
    this_object()->send(CMNAME, this_object(),
      sprintf(CMNAME " neugeladen. %d Ebenen mit %d Teilnehmern sind aktiv.",
        sizeof(channels),
        CountUsers()));
  }
}

varargs void reset()
{
  // Im Durchschnitt 1 Tag: 21.6h + random(4.8h), d.h. naechster reset ist
  // zwischen 21.6h und 26.4h von jetzt.
  set_next_reset(77760 + random(17280));

  // inaktive Ebenen bereinigen 
  int timeout = INACTIVE_EXPIRE;
  // Wir behalten immer ungefaehr die Haelfte von MAX_INACTIVE_CHANNELS
  // inaktive Ebenen. In  jeder Iteration wird das erlaubte Timeout reduziert,
  // bis genug inaktive Ebenen weg sind, aber MIN_INACTIVE_LIFETIME bleiben
  // Ebenen min. inaktiv bestehen.
  while (sizeof(channelC) > MAX_INACTIVE_CHANNELS/2
         && timeout > MIN_INACTIVE_LIFETIME)
  {
    channelC = filter(channelC,
      function int (string ch_name, mixed values)
      {
        struct channel_base_s data = values[0];
        int ts = values[1];
        if (ts + timeout > time())
          return 1;
        // Ebenendaten koennen weg, inkl. History, die also auch loeschen
        m_delete(channelH, ch_name);
        return 0;
      });
    // timeout halbieren und neu versuchen wenn noetig.
    timeout /= 2;
  }
  // achja, speichern sollten wir uns ggf. auch noch.
  if (save_me_soon)
  {
    save_me_soon = 0;
    save_object(CHANNEL_SAVE);
  }
}

varargs int remove(int silent)
{
  if (save_me_soon)
  {
    save_me_soon = 0;
    save_object(CHANNEL_SAVE);
  }
  log_file("CHANNEL", sprintf("[%s] remove() durch %O gerufen. Speichern und "
           "Ende.\n", dtime(time()),
            this_interactive()||this_player()||previous_object()));
  destruct(this_object());
  return 1;
}

// name() - define the name of this object.
public varargs string name(int casus,int demon)
{
  return CMNAME;
}

public varargs string Name(int casus, int demon)
{
  return capitalize(CMNAME);
}

// Low-level function for adding members without access checks
// return values < 0 are errors, success is 1.
private int add_member(struct channel_s ch, object m)
{
  if (IsChannelMember(ch, m))
    return E_ALREADY_JOINED;

  ch.members += ({ m });
  return 1;
}

private void remove_all_members(struct channel_s ch)
{
  // Einer geloeschten/inaktiven Ebene kann man nicht zuhoeren: Ebenenname
  // aus der Ebenenliste aller Mitglieder austragen. Dabei werden sowohl ein-,
  // als auch temporaer ausgeschaltete Ebenen beruecksichtigt.
  string chname = lower_case(ch.name);
  foreach(object listener : ch.members)
  {
    string* pl_chans = listener->QueryProp(P_CHANNELS);
    if (pointerp(pl_chans))
    {
      listener->SetProp(P_CHANNELS, pl_chans-({chname}));
    }
    pl_chans = listener->QueryProp(P_SWAP_CHANNELS);
    if (pointerp(pl_chans))
    {
      listener->SetProp(P_SWAP_CHANNELS, pl_chans-({chname}));
    }
  }
}

private void delete_channel(string chname, int force);

// Deaktiviert eine Ebene, behaelt aber einige Stammdaten in channelC und die
// History, so dass sie spaeter reaktiviert werden kann.
// Wenn <force>, dann wird wie Ebene sogar deaktiviert, wenn noch Zuhoerer
// anwesend sind.
private void deactivate_channel(string chname, int force)
{
  // Wenn zuviele inaktive Ebenen, wird sie geloescht statt deaktiviert.
  if (sizeof(channelC) > MAX_INACTIVE_CHANNELS)
  {
    log_file("CHANNEL",
        sprintf("[%s] Zuviele inaktive Ebenen. Channel %s geloescht statt "
                "deaktiviert.\n", dtime(time()), chname));
    this_object()->send(CMNAME, this_object(),
        sprintf("Zuviele inaktive Ebenen. Ebene %s geloescht statt "
                "deaktiviert.\n", chname));
    delete_channel(chname, force);
    return;
  }
  chname = lower_case(chname);
  struct channel_s ch = channels[chname];
  // Deaktivieren kann man nur aktive Ebenen.
  if (!structp(ch))
    return;
  // Falls sie noch Zuhoerer hat, muss man sich erstmal um die kuemmern.
  if (sizeof(ch.members))
  {
    // ohne <force> nur Ebenen ohne Zuhoerer deaktivieren.
    if (!force)
    {
      raise_error(
          sprintf("Attempt to deactivate channel %s with listeners.\n",
                  ch.name));
    }
    else
    {
      remove_all_members(ch);
    }
  }
  // Einige Daten merken, damit sie reaktiviert werden kann, wenn jemand
  // einloggt, der die Ebene abonniert hat.
#if __VERSION_MINOR__ == 6 && __VERSION_MICRO__ < 4
  // Workaround fuer Bug in to_struct: erst in array wandeln, dann in die
  // richtige struct.
  m_add(channelC, chname, to_struct(to_array(channels[chname])[0..3],
                                    (<channel_base_s>)),
        time());
#else
  m_add(channelC, chname, to_struct(channels[chname], (<channel_base_s>)),
        time());
#endif
  // aktive Ebene loeschen bzw. deaktivieren.
  m_delete(channels, chname);
  // History wird nicht geloescht, damit sie noch verfuegbar ist, wenn die
  // Ebene spaeter nochmal neu erstellt wird. Sie wird dann bereinigt, wenn
  // channelC bereinigt wird.

  stats["dispose"]++;
  save_me_soon = 1;
}

// Loescht eine Ebene vollstaendig inkl. Stammdaten und History.
// Wenn <force>, dann wird wie Ebene sogar deaktiviert, wenn noch Zuhoerer
// anwesend sind.
private void delete_channel(string chname, int force)
{
  chname = lower_case(chname);
  struct channel_s ch = channels[chname];
  // Ist die Ebene noch aktiv?
  if (ch)
  {
    // Und hat sie Zuhoerer?
    if (sizeof(ch.members))
    {
      // ohne <force> nur Ebenen ohne Zuhoerer loeschen.
      if (!force)
      {
        raise_error(
            sprintf("Attempt to delete channel %s with listeners.\n",
                    ch.name));
      }
      else
      {
        remove_all_members(ch);
      }
    }
    stats["dispose"]++;
    m_delete(channels, chname);
  }
  // Ab hier das gleiche fuer aktive und inaktive Ebenen.
  m_delete(channelC, chname);
  m_delete(channelH, chname);
  save_me_soon = 1;
}

// Aendert das Supervisor-Objekt einer Ebene, ggf. mit Meldung.
// Wenn kein neuer SV angegeben, wird der aelteste Zuhoerer gewaehlt.
private int change_sv_object(struct channel_s ch, object new_sv)
{
  if (!new_sv)
  {
    ch.members -= ({0});
    if (sizeof(ch.members))
      new_sv = ch.members[0];
    else
      return 0; // kein neuer SV moeglich.
  }
  // evtl. darf der supervisor aber nicht zu was anderes als dem creator
  // wechseln. Ausserdem darf niemand supervisor werden, der nicht auf der
  // Ebene ist.
  if ( ((ch.flags & CHF_FIXED_SUPERVISOR)
         && new_sv != find_object(ch.creator))
      || !IsChannelMember(ch, new_sv)
      )
    return 0;

  object old_sv = ch.supervisor;

  ch.supervisor = new_sv;
  ch.access_cl = symbol_function("ch_check_access", new_sv);

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
    this_object()->send(ch.name, old_sv,
        sprintf("uebergibt die Ebene an %s.",new_sv->name(WEN)),
        MSG_EMOTE);
  }
  else if (old_sv && !old_sv->QueryProp(P_INVIS))
  {
    this_object()->send(ch.name, old_sv,
        "uebergibt die Ebene an jemand anderen.", MSG_EMOTE);
  }
  else if (new_sv && !new_sv->QueryProp(P_INVIS))
  {
    this_object()->send(ch.name, new_sv,
        "uebernimmt die Ebene von jemand anderem.", MSG_EMOTE);
  }
  return 1;
}

// Stellt sicher, dass einen Ebenen-Supervisor gibt. Wenn dies nicht moeglich
// ist (z.b. leere Ebene), dann wird die Ebene geloescht und 0
// zurueckgegeben. Allerdings kann nach dieser Funktion sehr wohl die
// access_cl 0 sein, wenn der SV keine oeffentliche definiert! In diesem Fall
// wird access() den Zugriff immer erlauben.
private int assert_supervisor(struct channel_s ch)
{
  // Wenn der supervisor nicht mehr existiert, muss ein neuer gesucht werden.
  if (!ch.supervisor)
  {
    // Wenn der Wechsel des SV verboten ist, wird versucht, den
    // urspruenglichen Ersteller neuzuladen und zum neuen, alten Supervisor zu
    // machen.
    if (ch.flags & CHF_FIXED_SUPERVISOR)
    {
      object sv;
      string err=catch(sv=load_object(ch.creator);publish);
      if (!err)
      {
        // Juchu, die richtige SV ist wieder da. Sie muss noch auf die Ebene
        // und kann dann wieder SV werden.
        add_member(ch, sv);
        if (!change_sv_object(ch, sv))
        {
          // ich wuesste nicht, was in change_sv_object in diesem Fall
          // schiefgehen kann, daher einfach ein raise_error.
          raise_error(sprintf("Supervisor von Channel %s konnte nicht "
                "reaktiviert werden: %O\n",ch.name,sv));
        }
      }
      // wenns nicht geklappt hat, wird die Ebene deaktiviert.
      else
      {
        // Die inaktive Ebene kann wegen CHF_FIXED_SUPERVISOR nur vom
        // urspruenglichen Ersteller reaktiviert/neu erstellt werden. Und
        // solange der das nicht tut, ist weder die History zugaenglich, noch
        // kann jemand sonst was damit machen. Wenn die inaktive Ebene
        // irgendwann inkl. History  expired wird, kann jemand anderes dann
        // den Namen wieder verwenden und ein komplett neue Ebene erstellen.
        deactivate_channel(lower_case(ch.name), 1);
        log_file("CHANNEL",
            sprintf("[%s] Channel %s deaktiviert. SV-Fehler: %O -> %O\n",
                  dtime(time()), ch.name, ch.supervisor, err));
        return 0;
      }
    }
    // Der normalfall ist aber, dass wir einfach einen supervisor aus dem
    // Kreise der Zuhoerer bestimmen und zwar den aeltesten. Das macht
    // change_sv_object().
    else
    {
      if (!change_sv_object(ch, 0))
      {
        // wenn das nicht klappt, Ebene deaktivieren, vermutlich hat sie keine
        // Zuhoerer.
        deactivate_channel(lower_case(ch.name), 1);
        log_file("CHANNEL",
            sprintf("[%s] Kein SV, deaktiviere channel %s.\n",
                  dtime(time()), ch.name));
        return 0;
      }
    }
  }
  return 1;
}

// access() - check access by looking for the right argument types and
//            calling access closures respectively
// SEE: new, join, leave, send, list, users
// Wertebereich: 0 fuer Zugriff verweigert, 1 fuer Zugriff erlaubt, 2 fuer
// Zugriff erlaubt fuer privilegierte Objekte, die senden duerfen ohne
// Zuhoerer zu sein. (Die Aufrufer akzeptieren aber auch alle negativen Werte
// als Erfolg und alles ueber >2 als privilegiert.)
varargs private int access(struct channel_s ch, object user, string cmd,
                           string txt)
{
  if (!ch || !user)
    return 0;

  // Dieses Objekt und Root-Objekte duerfen auf der Ebene senden, ohne
  // Mitglied zu sein. Das ist die Folge der zurueckgegebenen 2.
  // Ausserdem duerfen sie auch alles andere machen unter Umgehung aller
  // Supervisoren. (z.B. kann dieses Objekt sogar Meldungen im Namen anderer
  // Objekte faken)
  // Die Pruefung erfolgt absichtlich vor assert_supervisor(), damit der
  // CHANNELD auch in temporaeren SV-losen Zustaenden was machen kann.
  if ( !previous_object(1) || !extern_call() ||
       previous_object(1) == this_object() ||
       getuid(previous_object(1)) == ROOTID)
    return 2;

  // Objekte duerfen keine Meldungen im Namen anderer Objekte faken, d.h. der
  // vermeintliche <user> muss auch der Aufrufer sein. Ausser darf auch sonst
  // kein Objekt was fuer ein anderes Objekt duerfen, sonst kann jemand z.B.
  // eine History abfragen indem einfach ein anderes Objekt uebergeben wird.
  if (previous_object(1) != user)
    return 0;

  if (IsBanned(user, cmd))
    return 0;

  // Wenn kein SV-Objekt mehr existiert und kein neues bestimmt werden konnte,
  // wurde die Ebene ausfgeloest. In diesem Fall auch den Zugriff verweigern.
  if (!assert_supervisor(ch))
    return 0;
  // Wenn closure jetzt dennoch 0, wird der Zugriff erlaubt.
  if (!ch.access_cl)
    return 1;

  // Das SV-Objekt wird gefragt, ob der Zugriff erlaubt ist. Dieses erfolgt
  // fuer EM+ aber nur, wenn es das Default-SV-Objekt ist, damit
  // nicht beliebige SV-Objekt EMs den Zugriff verweigern koennen. Ebenen mit
  // Default-SV koennen aber auch EM+ Zugriff verweigern.
  if (IS_ARCH(previous_object(1))
      && ch.supervisor != find_object(DEFAULTSV))
    return 1;

  return funcall(ch.access_cl, lower_case(ch.name), user, cmd, &txt);
}

// Neue Ebene <ch> erstellen mit <owner> als Ebenenbesitzer.
// <desc> kann die statische Beschreibung der Ebene sein oder eine Closure,
// die dynamisch aktualisierte Infos ausgibt.
// Das Objekt <owner> sollte eine Funktion ch_check_access() definieren, die
// gerufen wird, wenn eine Ebenenaktion vom Typ join/leave/send/list/users
// eingeht.
#define IGNORE  "^/xx"
public varargs int new(string ch_name, object owner, string|closure desc,
                       int channel_flags)
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

  // Zunaechst pruefen, ob eine alte, inaktive Ebene mit dem Namen noch
  // existiert.
  struct channel_base_s cbase = channelC[lower_case(ch_name)];
  struct channel_s ch;
  if (cbase)
  {
    // Wenn bei Reaktivierung von Ebenen (auch mit neuer Beschreibung *g*) der
    // neue owner != dem urspruenglichen Ersteller der Ebene ist und das Flag
    // CHF_FIXED_SUPERVISOR gesetzt ist, wird die Reaktivierung abgebrochen,
    // damit niemand inaktive Ebenen und deren History auf diesem Wege
    // uebernehmen kann, d.h. den Supervisor ersetzen kann.
    if ((cbase.flags & CHF_FIXED_SUPERVISOR)
        && object_name(owner) != cbase.creator)
      return E_ACCESS_DENIED;
    // Alte Daten der Ebene uebernehmen
#if __VERSION_MINOR__ == 6 && __VERSION_MICRO__ < 4
    // Workaround fuer Bug in to_struct: erst in array wandeln, dann in die
    // richtige struct.
    ch = to_struct(to_array(cbase), (<channel_s>));
#else
    ch = to_struct(cbase, (<channel_s>));
#endif
    // Wenn eine Beschreibung uebergeben, dann ersetzt sie jetzt die alte
    if (desc)
      ch.desc = desc;
    // creator bleibt natuerlich bestehen. Die Flags auch. Wir behalten auch
    // die Schreibweise (Gross-/Kleinschreibung) des Namens aus
    // Konsistenzgruenden bei.
  }
  else
  {
    // Wenn keine Beschreibung und keine inaktive Ebene da ist, wirds nen
    // Fehler...
    if (!desc)
      return E_ACCESS_DENIED;
    // prima, alles da. Dann wird ein ganz frische neue Ebenenstruktur
    // erzeugt.
    ch = (<channel_s> name: ch_name, desc: desc, creator: object_name(owner),
          flags: channel_flags);
  }

  ch_name = lower_case(ch_name);

  ch.members = ({ owner });
  ch.supervisor = owner;
  // ch_check_access() dient der Zugriffskontrolle und entscheidet, ob die
  // Nachricht gesendet werden darf oder nicht.
  ch.access_cl = symbol_function("ch_check_access", owner);

  m_add(channels, ch_name, ch);

  // History fuer eine Ebene nur dann initialisieren, wenn es sie noch
  // nicht gibt.
  if (!pointerp(channelH[ch_name]))
    channelH[ch_name] = ({});

  // Datenstruktur einer ggf. inaktiven Ebene mit dem Namen in channelC kann
  // jetzt auch weg.
  if (cbase)
    m_delete(channelC, ch_name);

  // Erstellen neuer Ebenen loggen, wenn wir nicht selbst der Ersteller sind.
  if (owner != this_object())
    log_file("CHANNEL.new", sprintf("[%s] Neue Ebene %s: %O %O\n",
        dtime(time()), ch.name, owner, desc));

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
      "laesst die Ebene '" + ch.name + "' entstehen.", MSG_EMOTE);
  }

  stats["new"]++;
  return (0);
}

// Objekt <pl> betritt Ebene <ch>. Dies wird zugelassen, wenn <pl> die
// Berechtigung hat und noch nicht Mitglied ist. (Man kann einer Ebene nicht
// zweimal beitreten.)
public int join(string chname, object joining)
{
  struct channel_s ch = channels[lower_case(chname)];
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  if (!funcall(#'access, ch, joining, C_JOIN))
    return E_ACCESS_DENIED;
  //TODO: Sollte der creator das Recht auf join haben, auch wenn der aktuelle
  //SV es verweigert? (s.u.)
  int res = add_member(ch, joining);
  if (res != 1)
    return res;

  // Wenn <joining> der urspruengliche Ersteller der Ebene und kein
  // Spieler ist, wird es automatisch wieder zum Supervisor.
  if (!query_once_interactive(joining)
      && object_name(joining) == ch.creator)
    change_sv_object(ch, joining);

  return 0;
}

// Objekt <pl> verlaesst Ebene <ch>.
// Zugriffsrechte werden nur der Vollstaendigkeit halber geprueft; es duerfte
// normalerweise keinen Grund geben, das Verlassen einer Ebene zu verbieten.
// Dies ist in ch_check_access() so geregelt, allerdings koennte dem Objekt
// <pl> das Verlassen auf Grund eines Banns verboten sein.
// Wenn kein Zuhoerer mehr auf der Ebene ist, loest sie sich auf.
public int leave(string chname, object leaving)
{
  struct channel_s ch = channels[lower_case(chname)];
  // Nicht-existierenden Ebenen soll das Spielerobjekt austragen, also tun wir
  // so, als sei das erfolgreich gewesen.
  if (!ch)
    return 0;

  ch.members -= ({0}); // kaputte Objekte erstmal raus

  if (!IsChannelMember(ch, leaving))
    return E_NOT_MEMBER;

  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  if (!funcall(#'access, ch, leaving, C_LEAVE))
    return E_ACCESS_DENIED;

  // Dann mal den Zuhoerer raus.
  ch.members -= ({leaving});

  // Wenn auf der Ebene jetzt noch Objekte zuhoeren, muss ggf. der SV
  // wechseln.
  if (sizeof(ch.members))
  {
    // Kontrolle an jemand anderen uebergeben, wenn der Ebenensupervisor
    // diese verlassen hat. change_sv_object() waehlt per Default den
    // aeltesten Zuhoerer.
    if (leaving == ch.supervisor)
    {
      change_sv_object(ch, 0);
    }
  }
  // ansonsten Ebene loeschen, wenn keiner zuhoert.
  // Kommentar: Supervisoren sind auch Zuhoerer auf der Ebene. Wenn keine
  // Zuhoerer mehr, folglich auch kein Supervisor mehr da.
  else
  {
    // Der Letzte macht das Licht aus, aber nur, wenn er nicht unsichtbar ist.
    // Wenn Spieler, NPC, Clone oder Channeld als letztes die Ebene verlassen,
    // wird diese zerstoert, mit Meldung.
    if (!leaving->QueryProp(P_INVIS))
    {
      // Die Zugriffskontrolle auf die Ebenen wird von der Funktion access()
      // erledigt. Weil sowohl externe Aufrufe aus dem Spielerobjekt, als auch
      // interne Aufrufe aus diesem Objekt vorkommen koennen, wird hier ein
      // explizites call_other() auf this_object() gemacht, damit der
      // Caller-Stack bei dem internen Aufruf denselben Aufbau hat wie bei
      // einem externen.
      this_object()->send(CMNAME, leaving,
        "verlaesst als "+
        (leaving->QueryProp(P_GENDER) == 1 ? "Letzter" : "Letzte")+
        " die Ebene '" + ch.name + "', worauf diese sich in "
        "einem Blitz oktarinen Lichts aufloest.", MSG_EMOTE);
    }
    deactivate_channel(lower_case(ch.name),0);
  }
  return (0);
}

// Nachricht <msg> vom Typ <type> mit Absender <pl> auf der Ebene <ch> posten,
// sofern <pl> dort senden darf.
public varargs int send(string chname, object sender, string msg, int type)
{
  chname = lower_case(chname);
  struct channel_s ch = channels[chname];
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  int a = funcall(#'access, ch, sender, C_SEND, msg);
  if (!a)
    return E_ACCESS_DENIED;

  // a<2 bedeutet effektiv a==1 (weil a==0 oben rausfaellt), was dem
  // Rueckgabewert von ch_check_access() entspricht, wenn die Aktion zugelassen
  // wird. access() liefert allerdings 2 fuer "privilegierte" Objekte (z.B.
  // ROOT-Objekte oder den channeld selber). Der Effekt ist, dass diese
  // Objekte auf Ebenen senden duerfen, auf denen sie nicht zuhoeren.
  if (a < 2 && !IsChannelMember(ch, sender))
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
  (ch.members)->ChannelMessage(
                            ({ ch.name, sender, msg, type}));

  if (sizeof(channelH[chname]) > MAX_HIST_SIZE)
    channelH[chname] = channelH[chname][1..];

  channelH[chname] +=
    ({ ({ ch.name,
          (sender->QueryProp(P_INVIS)
                    ? "/(" + capitalize(getuid(sender)) + ")$"
                    : "")
                  + (sender->Name(WER, 2) || "<Unbekannt>"),
          msg + " <" + strftime("%a, %H:%M:%S") + ">\n",
          type }) });
  return (0);
}

// Gibt ein Mapping mit allen Ebenen aus, die das Objekt <pl> lesen kann,
// oder einen Integer-Fehlercode
public int|mapping list(object pl)
{
  mapping chs = ([]);
  foreach(string chname, struct channel_s ch : channels)
  {
    /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
       zu erzeugen, weil access() mit extern_call() und previous_object()
       arbeitet und sichergestellt sein muss, dass das in jedem Fall das
       richtige ist. */
    if(funcall(#'access, ch, pl, C_LIST))
    {
      ch.members = filter(ch.members, #'objectp);
      m_add(chs, chname, ({ch.members, ch.access_cl, ch.desc,
                           ch.supervisor, ch.name }) );
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
public string|string* find(string chname, object pl)
{
  chname = lower_case(chname);

  // Suchstring <ch> muss Formatanforderung erfuellen;
  // TODO: soll das ein Check auf gueltigen Ebenennamen als Input sein?
  // Wenn ja, muesste laut Manpage mehr geprueft werden:
  // "Gueltige Namen setzen sich zusammen aus den Buchstaben a-z, A-Z sowie
  // #$%&@<>-." Es wuerden also $%&@ fehlen.
  if (!regmatch(chname, "^[<>a-z0-9#-]+$"))
    return 0;

  // Der Anfang des Ebenennamens muss dem Suchstring entsprechen und das
  // Objekt <pl> muss auf dieser Ebene senden duerfen, damit der Ebenenname
  // in das Suchergebnis aufgenommen wird.
  string* chs = filter(m_indices(channels), function int (string ch_n) {
                 /* funcall() auf Closure-Operator, um einen neuen Eintrag
                    im Caller Stack zu erzeugen, weil access() mit
                    extern_call() und previous_object() arbeitet und
                    sichergestellt sein muss, dass das in jedem Fall das
                    richtige ist. */
                  return ( stringp(regmatch(ch_n, "^"+chname)) &&
                           funcall(#'access, channels[ch_n], pl, C_SEND) );
                });

  int num_channels = sizeof(chs);
  if (num_channels > 1)
    return chs;
  else if (num_channels == 1)
    return chs[0];
  else
    return 0;
}

// Ebenen-History abfragen.
public int|<int|string>** history(string chname, object pl)
{
  struct channel_s ch = channels[lower_case(chname)];
  /* funcall() auf Closure-Operator, um einen neuen Eintrag im Caller Stack
     zu erzeugen, weil access() mit extern_call() und previous_object()
     arbeitet und sichergestellt sein muss, dass das in jedem Fall das
     richtige ist. */
  if (!funcall(#'access, ch, pl, C_JOIN))
    return E_ACCESS_DENIED;
  else
    return channelH[chname];
}

// Wird aus der Shell gerufen, fuer das Erzmagier-Kommando "kill".
public int remove_channel(string chname, object pl)
{
  if (!member(channels, chname))
    return E_ACCESS_DENIED;
  //TODO: integrieren in access()?
  if (previous_object() != this_object())
  {
    if (!stringp(chname) ||
        pl != this_player() || this_player() != this_interactive() ||
        this_interactive() != previous_object() ||
        !IS_ARCH(this_interactive()))
      return E_ACCESS_DENIED;
  }

  delete_channel(lower_case(chname), 1);

  return (0);
}

// Wird aus der Shell aufgerufen, fuer das Erzmagier-Kommando "clear".
public int clear_history(string chname)
{
  if (!member(channelH, chname))
    return E_ACCESS_DENIED;
  //TODO: mit access() vereinigen?
  // Sicherheitsabfragen
  if (previous_object() != this_object())
  {
    if (!stringp(chname) ||
        this_player() != this_interactive() ||
        this_interactive() != previous_object() ||
        !IS_ARCH(this_interactive()))
      return E_ACCESS_DENIED;
  }
  chname=lower_case(chname);
  // History des Channels loeschen (ohne die ebene als ganzes, daher Key nicht
  // aus dem mapping loeschen.)
  if (pointerp(channelH[chname]))
    channelH[chname] = ({});

  return 0;
}

// Aendert den Ersteller/Besitzer der Ebene.
// Achtung: das ist nicht das gleiche wie der aktuelle Supervisor!
public int transfer_ownership(string chname, object new_owner)
{
  struct channel_s ch = channels[lower_case(chname)];
  if (!ch)
    return E_ACCESS_DENIED;
  // Nur der aktuelle Besitzer oder EM+ darf die Ebene verschenken
  if (ch.creator != object_name(previous_object())
      && !IS_ARCH(previous_object()))
    return E_ACCESS_DENIED;

  ch.creator = object_name(new_owner);
  return 1;
}

// Aendert den Flags der Ebene.
public int change_channel_flags(string chname, int newflags)
{
  struct channel_s ch = channels[lower_case(chname)];
  if (!ch)
    return E_ACCESS_DENIED;
  // Nur der aktuelle Besitzer, Supervisor oder EM+ darf die Flags aendern.
  if (ch.supervisor != previous_object()
      && ch.creator != object_name(previous_object())
      && !IS_ARCH(previous_object()))
    return E_ACCESS_DENIED;

  ch.flags = newflags;
  return 1;
}


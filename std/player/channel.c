// MorgenGrauen MUDlib
//
// channel.c -- channel client
//
// $Id: channel.c 9404 2015-12-13 00:21:44Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone

#define NEED_PROTOTYPES
#include <util.h>
#include <thing/properties.h>
#include <living/comm.h>
#include <player.h>
#include <player/comm.h>
#include <daemon.h>
#include <player/gmcp.h>
#undef NEED_PROTOTYPES

#include <wizlevels.h>
#include <defines.h>
#include <properties.h>
#include <sys_debug.h>
#include <regexp.h>

#define P_CHANNEL_SHORT  "short_channels"

#define CHANNELCMDS      "[#@%$&()<>a-zA-Z0-9\\-]"

#define DEFAULT_CHANNELS ({"Abenteuer", "Anfaenger", "Grats", "Tod", "ZT"})
#define DEFAULT_SHORTCUTS \
([                     \
 "b":"Abenteuer",      \
 "a":"Allgemein",      \
 "B":"Beileid",        \
 "q":"D-chat",         \
 "G":"Grats",          \
 "M":"Moerder",        \
 "h":"Seher",          \
 "T":"Tod",            \
])

#define WIZARD_SHORTCUTS \
([                     \
 "P":"D-code",         \
 "D":"Debug",          \
 "O":"Intercode",      \
 "I":"Intermud",       \
 "m":"Magier",         \
])

// TODO: <shortcut> wird eigentlich nur in getChannel() sinnvoll verwendet
// Koennte man an sich komplett einsparen und dort wie ueberall sonst auch
// einfach nur mit P_CHANNEL_SHORT arbeiten.
private nosave mapping shortcut;
private nosave int c_status;

void create()
{
  Set(P_CHANNELS, SAVE, F_MODE);
  Set(P_CHANNELS, DEFAULT_CHANNELS);
  Set(P_SWAP_CHANNELS, SAVE, F_MODE);
  Set(P_STD_CHANNEL, "Allgemein");
  Set(P_STD_CHANNEL, SAVE, F_MODE);
  Set(P_CHANNEL_SHORT, SAVE, F_MODE);
  Set(P_CHANNEL_SHORT, DEFAULT_SHORTCUTS
      + (IS_LEARNER(ME) ? WIZARD_SHORTCUTS : ([])));
}

static <int|string>** _query_localcmds()
{
  return ({({"-","ChannelParser", 1, 0}),
           ({"ebene", "ChannelAdmin", 0, 0}),
           ({"ebenen", "ChannelAdmin", 1, 0}),
         });
}

string* RegisterChannels()
{
  if (extern_call() &&
      previous_object() != find_object(CHMASTER))
    return 0;

  c_status = 0;
  shortcut = QueryProp(P_CHANNEL_SHORT);
  SetProp(P_CHANNELS, map(QueryProp(P_CHANNELS) || ({}), #'lower_case));

  closure cl = symbol_function("join", CHMASTER);
  string* err;
  if (closurep(cl))
  {
    err = filter(QueryProp(P_CHANNELS), cl, ME);
  }
  if (QueryProp(P_LEVEL) < 5)
    return err;

  // CHMASTER->new() gibt bei Erfolg 0 zurueck, d.h. es bleiben
  // alle Elemente erhalten, deren Channel nicht erstellt werden konnten.
  // Die werden an die Aufrufer zurueckgegeben.
  return filter(err, "new", CHMASTER, ME);
}

string* RemoveChannels()
{
  if (extern_call() &&
      previous_object() != find_object(CHMASTER))
    return 0;

  string* err = ({});

  if (!c_status)
    c_status = 1;
  else
    return err;

  closure cl = symbol_function("leave", CHMASTER);
  if (closurep(cl))
  {
    err = filter(QueryProp(P_CHANNELS), cl, ME);
    SetProp(P_CHANNELS, QueryProp(P_CHANNELS) - err);
  }
  return err;
}

varargs private string getName(string|object|closure who, int fall) {
  // Objekt zur Closure raussuchen. <o> ist danach entweder jenes Objekt,
  // oder aber <who> selbst, das Objekt oder String sein kann.
  //
  // query_closure_object() KANN auch 0 oder -1 zurueckgeben. In beiden
  // Faellen fuehrt das hier zu einem Laufzeit-Typfehler, weil ein Integer
  // bzw. die urspruengliche Closure zugewiesen wird. Diesen Fall abzufangen
  // und separat zu behandeln, hilft nicht, weil es auch keine alternative
  // Moeglichkeit gibt, <who> in verwendbare Daten umzuwandeln, denn das ist
  // ja offenbar eine kaputte Closure. Dieser Fall ist ausserdem anscheinend
  // so exotisch, dass wir vorerst auf Absicherungen mittels catch()
  // verzichten.
  string|object o = closurep(who) ? query_closure_object(who) : who;

  // Ist es ein String, pruefen wir, ob es vielleicht ein Objektname ist,
  // indem wir es in ein Objekt umzuwandeln versuchen. Schlaegt das fehl,
  // schreiben wir <who> wieder zurueck, so dass <o> wieder der urspruengliche
  // String ist.
  if (stringp(o) && sizeof(o))
    o = find_object(o) || who;

  // Objekte
  if (objectp(o))
  {
    // Magier sehen unsichtbare nicht nur als "Jemand"
    if (o->QueryProp(P_INVIS) && IS_LEARNING(ME))
      return "("+capitalize(getuid(o))+")";
    // Froesche mit Namen versorgen.
    if (o->QueryProp(P_FROG))
      return "Frosch "+capitalize(getuid(o));
    // Default (Unsichtbare als "Jemand" (s. Name()))
    return o->Name(fall, 2)||"<Unbekannt>";
  }
  // Strings
  else if (stringp(o) && sizeof(o))
  {
    if (o[0] == '/')
    {
      // unsichtbare Objekte...
      int p = strstr(o, "$");
      if (p != -1)
      {
        // Magier im Magiermodus kriegen den Realnamen, andere nicht.
        if (IS_LEARNING(ME))
          return o[1..p-1];
        else
          return o[p+1..];
      }
      else // doch nicht unsichtbar
        return (fall == WESSEN ? o+"s" : o);
    }
    else
      // nicht unsichtbar
      return (fall == WESSEN ? o+"s" : o);
  }
  // Fall-through
  return "<Unbekannt>";
}

// <nonint> unterdrueckt die Ausgabe an den Spieler und liefert den Text
// zurueck. Wird nur fuer die Ebenenhistory benutzt.
string ChannelMessage(<string|object|int>* msg, int nonint)
{
  string channel_message;
  string channel = msg[0];

  // Wenn eine Ebenenmeldung ausgegeben werden soll, ist msg[1] ein Objekt,
  // im Fall der History aber ein String. Daher wird <sender> als Union
  // deklariert. Das ist unproblematisch, weil die beiden Datentypen
  // komplett getrennte Wege nehmen: ein Objekt wird an ReceiveMsg()
  // durchgereicht (Ebenenmeldung). Ein String wird direkt in die Meldung
  // (History) eingebaut, diese an den ChannelParser() zurueckgegeben, der
  // sie via More() ausgibt.
  string|object sender = msg[1];
  string message = msg[2];
  int msg_type = msg[3];

  if (previous_object() != find_object(CHMASTER) &&
      previous_object() != ME )
    return 0;

  string sender_name = getName(sender, msg_type == MSG_GEMOTE ? WESSEN : WER);
  int prepend_indent_flag =
        QueryProp(P_MESSAGE_PREPEND) ? BS_PREPEND_INDENT : 0;

  switch (msg_type)
  {
    case MSG_EMPTY:
      channel_message= message+"\n";
      break;
    case MSG_GEMOTE:
    case MSG_EMOTE:
      channel_message = break_string(sender_name + " "+ message+"]", 78,
                          sprintf("[%s:", channel),
                          BS_INDENT_ONCE|prepend_indent_flag);
      break;
    case MSG_SAY:
    default:
      string presay = sprintf("[%s:%s] ", channel, sender_name);
      channel_message = break_string(message, max(78,sizeof(presay)+10),
                          presay, prepend_indent_flag);
      break;
  }

  if (nonint)
    return channel_message;

  // Wenn GMCP sich um Uebertragung der Nachricht kuemmert, wird ReceiveMsg()
  // nicht mehr aufgerufen. getName leider nochmal aufrufen, weil GMCP den
  // Namen im Nominativ braucht.
  if (msg_type == MSG_GEMOTE)
    sender_name = getName(sender, WER);
  if (GMCP_Channel(channel_message, channel, sender_name) != 1)
  {
    // Der Ebenenname muss in Kleinbuchstaben uebergeben werden, damit die
    // Ignorierepruefung funktioniert. Die ignorierestrings sind naemlich alle
    // kleingeschrieben.
    ReceiveMsg(channel_message,
               MT_COMM|MT_FAR|MSG_DONT_STORE|MSG_DONT_WRAP,
               MA_CHANNEL"." + lower_case(channel), 0, sender);
  }
  return 0;
}

// Defines fuer den Zugriff auf die Channeldaten in der vom CHANNELD
// erhaltenen Kanalliste.
#define I_MEMBER          0
#define I_ACCESS          1
#define I_INFO            2
#define I_SUPERVISOR      3
#define I_NAME            4

private void createList(mapping ch_list, string* p_channels,
                           int show_only_subscribed) {
  // Kopfzeile der Tabelle erstellen.
  string listhead =
            sprintf("%-12.12' 's  [A] %|12' 's (%-3' 's) %-42.42s\n",
                    "Name", "Eigner", "Sp", "Beschreibung");
  // Linie drunter.
  listhead += ("-"*78 + "\n");

  // Rest der Daten erstmal in einem Array zusammenstellen, um es
  // anschliessend sortieren zu koennen.
  string* entries = ({});

  object* ch_members;
  string|object ch_master;
  string ch_real_name, ch_description;
  closure|string ch_access;
  closure|string ch_info;
  string sh;

  foreach(string chname, <object*|closure|string|object>* chdata : ch_list)
  {
    // Wenn nur abonnierte Ebenen aufgelistet werden sollen, dann alle
    // ueberspringen, die nicht in P_CHANNELS stehen.
    int is_subscribed = (member(p_channels, chname) > -1);
    if (show_only_subscribed && !is_subscribed)
      continue;

    ch_members =   chdata[I_MEMBER];
    ch_master =    chdata[I_SUPERVISOR];
    ch_access =    chdata[I_ACCESS];
    ch_real_name = chdata[I_NAME];
    ch_info =      chdata[I_INFO];
    sh = "";

    // Ist eine Closure als I_INFO eingetragen, zu der es auch ein Objekt
    // gibt, tragen wir deren Rueckgabewert als Beschreibung ein.
    if (closurep(ch_info) && objectp(query_closure_object(ch_info))) {
      ch_description = funcall(ch_info);
    }
    // Ist es ein String, wird er unveraendert uebernommen.
    else if (stringp(ch_info)) {
      ch_description = ch_info;
    }
    // Sollte nirgends etwas eingetragen sein, oder die Closure 0 zurueck-
    // gegeben haben, gibt es eine Defaultbeschreibung. Man haette die
    // Variable auch schon mit dem Default initialisieren koennen, der kann
    // aber bei Rueckgabe von 0 aus der Closure wieder ueberschrieben werden.
    // Daher passiert das erst hier.
    ch_description ||= "- Keine Beschreibung -";

    // Wir brauchen noch das vom Spieler festgelegte Kuerzel fuer die aktuell
    // bearbeitete Ebene, falls vorhanden, um es in die Liste eintragen
    // zu koennen.
    foreach(string _sh_cut, string _chan_name : shortcut) {
      if ( lower_case(_chan_name) == chname ) {
        sh = _sh_cut;
        break;
      }
    }

    // Jetzt haben wir endlich alle Infos beisammen und koennen die
    // Eintraege zusammenbauen.
    entries += ({
      sprintf("%-12.12'.'s %c[%-1.1s] %|12.12' 's (%-|3' 'd) %-42.42s\n",
          ch_real_name,
          is_subscribed ? '*' : ' ',
          sh,
          ch_master ? getName(ch_master) : getName(ch_access),
          sizeof(ch_members - ({ find_object(CHMASTER) })),
          ch_description
      ) });
  }

   // alphabetisch sortieren
  entries = sort_array(entries, #'>);

  More( listhead +             // Listenkopf
        implode(entries, "") + // Eintraege
        "-"*78+"\n" );         // Linie drunter
}

private string|string* getChannel(string ch)
{
  if (!sizeof(ch))
    ch = QueryProp(P_STD_CHANNEL);
  if (shortcut && shortcut[ch])
    ch = shortcut[ch];
  return CHMASTER->find(ch, ME);
}

int ChannelParser(string args)
{
  string|string* ch;
  int pos, type, err;
  string tmp;
  notify_fail("Benutzung: -<Ebene>[ ]['|:|;]<Text>\n"
              "           -<Ebene>[+|-|?|!|*]\n"
              "           -?\n");

  args = _unparsed_args();
  string|string* cmd = query_verb();
  if (!cmd && !args)
    return 0;

  if (!args)
    args = "";

  cmd = cmd[1..];
  cmd = regexplode(cmd, "^" CHANNELCMDS "*" "([+-]|\\!|\\?|\\*)*");
  if (sizeof(cmd) > 1)
  {
    //z.B. cmd= ({"","allgemein",":testet"})
    if (sizeof(cmd[1]) > 1 && strstr("+-?!*", cmd[1][<1..<1]) > -1)
      tmp = cmd[1][0..<2];
    else
      tmp = cmd[1];

    if (cmd[1] != "?" && cmd[1] != "!")
    {
      ch = getChannel(tmp);
      if (pointerp(ch))
      {
        notify_fail("Diese Angabe war nicht eindeutig! "
                    "Folgende Ebenen passen:\n"+
                    implode(ch, ", ")+"\n");
        return 0;
      }
      else if (!ch)
      {
        notify_fail("Die Ebene '"+tmp+ "' gibt es nicht!\n");
        return 0;
      }
    }

    if (sizeof(cmd[1])) {
      switch (cmd[1][<1]) {
        case '+':
          switch (CHMASTER->join(ch, ME))
          {
            case E_ACCESS_DENIED:
              notify_fail("Du darfst an die Ebene '"+ch+"' nicht heran.\n");
              return 0;
            case E_ALREADY_JOINED:
              notify_fail("Du hast diese Ebene schon betreten!\n");
              return 0;
            default:
              break;
          }
          tell_object(ME,"Du betrittst die Ebene '"+ch+"'.\n");
          if (member(QueryProp(P_CHANNELS), ch = lower_case(ch)) == -1)
            SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({ ch }));
          return 1;

        case '-':
          switch (CHMASTER->leave(ch, ME))
          {
            case E_ACCESS_DENIED:
              tell_object(ME,"Du kannst die Ebene '"+ch+"' nicht "
                "verlassen.\n");
              break;
            case E_NOT_MEMBER:
              tell_object(ME,"Wie willst Du eine Ebene verlassen, welche Du "
                "nicht betreten hast?\n");
              break;
            default:
              tell_object(ME,"Du verlaesst die Ebene '"+ch+"'.\n");
              SetProp(P_CHANNELS,
                      QueryProp(P_CHANNELS) - ({ lower_case(ch), ch }));
              break;
          }
          return 1;

        case '!':
        case '?':
          mapping l = CHMASTER->list(ME);
          if (mappingp(l))
          {
            // Es wurde ein Channel angegeben, dessen Infos gefragt sind.
            if (stringp(ch) && sizeof(ch) && pointerp(l[ch = lower_case(ch)]))
            {
              <object*|closure|string|object>* chdata = l[ch];
              string* m =
                sort_array(map(chdata[I_MEMBER], #'getName, WER), #'>) -
                ({ CMNAME });

              string wen;
              switch(sizeof(m)) {
                case 1:  wen = "ein Gesicht"; break;
                case 0:  wen = "niemanden";   break;
                // TODO: mittels Zahlwoerter-Objekt die Zahl als Text
                // ausgeben?
                default: wen = sprintf("%d Gesichter", sizeof(m)); break;
              }

              tell_object(ME,
                chdata[I_NAME]+", "+funcall(chdata[I_INFO])+".\n" +
                "Du siehst "+wen+" auf der Ebene '"+chdata[I_NAME]+"':\n"+
                break_string(CountUp(m), 78) + getName(chdata[I_SUPERVISOR])
                + " hat das Sagen auf dieser Ebene.\n");
            }
            // kein Channel angegeben, dann Gesamtliste erzeugen
            else
            {
              // Wenn nur die abonnierten angezeigt werden sollen
              // (Kommando -!), wird die Bedingung im 3. Argument == 1, so
              // dass createList() die reduzierte Tabelle erzeugt.
              createList(l, QueryProp(P_CHANNELS), (cmd[1][<1] == '!'));
            }
          }
          return 1;

        case '*':
          int|<int|string>** hist = CHMASTER->history(ch, ME);
          if (!pointerp(hist) || !sizeof(hist))
          {
            tell_object(ME,"Es ist keine Geschichte fuer '"+ch+
              "' verfuegbar.\n");
            return 1;
          }

          int amount = to_int(cmd[2]);
          if (amount <= 0 || amount >= sizeof(hist))
            amount = sizeof(hist);

          string txt = "Folgendes ist auf '"+ch+"' passiert:\n" +
                       implode(map(hist[<amount..], #'ChannelMessage, 1), "");
          More(txt);
          return 1;

        default:
          break;
      }
    }
  }

  cmd = implode(cmd[2..], "");
  if (sizeof(cmd))
    args = cmd + (sizeof(args) ? " " : "") + args;

  // KOntrollchars ausfiltern.
  args = regreplace(args, "[[:cntrl:]]", "", RE_PCRE|RE_GLOBAL);
  if (!sizeof(args))
    return 0;

  //Wenn cmd leer ist: MSG_SAY
  if (!sizeof(cmd))
  {
    type = MSG_SAY;
  }
  else
  {
    switch (cmd[0])
    {
      case ':' :
        type = MSG_EMOTE;
        args = args[1..];
        break;
      case ';' :
        type = MSG_GEMOTE;
        args = args[1..];
        break;
      case '\'':
        args = args[1..];
        // Der Fallthrough in default ist hier Absicht.
      default  :
        type = MSG_SAY;
        break;
    }
  }

  if (!ch || !sizeof(ch))
    ch = QueryProp(P_STD_CHANNEL);

  err = CHMASTER->send(ch, ME, args, type);
  if (err < 0)
  {
    err = CHMASTER->join(ch, ME);
    if (!err)
    {
      ch = lower_case(ch);
      if (member(QueryProp(P_CHANNELS), ch) == -1)
        SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({ ch }));
      err = CHMASTER->send(ch, ME, args, type);
    }
  }

  switch (err)
  {
    case E_ACCESS_DENIED:
      notify_fail("Auf der Ebene '"+ch+"' darfst Du nichts sagen.\n");
      return 0;
    case E_NOT_MEMBER:
      notify_fail("Du hast die Ebene '"+ch+"' nicht betreten!\n");
      return 0;
  }
  return 1;
}

int ChannelAdmin(string args)
{
  args = _unparsed_args();

  string target_channel, descr, _sh_cut;
  string|string* chans;
  notify_fail("Benutzung: ebene <Abkuerzung>=<Ebene>\n"
              "           ebene <Abkuerzung>=\n"
              "           ebene abkuerzungen [standard]\n"
              "           ebene standard <Ebene>\n"
              "           ebene an|ein|aus\n"
              +(QueryProp(P_LEVEL) >= 5 ?
                "           ebene neu <Name> <Bezeichnung>\n"
                "           ebene beschreibung <Name> <Beschreibung>\n" : "")
              +(IS_ARCH(ME) ?
                "           ebene kill <Name>\n"
                "           ebene clear <Name>\n": ""));

  if (!args || !sizeof(args))
    return 0;

  if (sscanf(args, "kill %s", target_channel) && IS_ARCH(ME))
  {
    chans = CHMASTER->find(target_channel, ME);

    if (!chans)
    {
      notify_fail("Ebene '"+target_channel+"' nicht gefunden!\n");
      return 0;
    }
    else if (pointerp(chans))
    {
      notify_fail(
        "Das war keine eindeutige Angabe! Folgende Ebenen passen:\n"+
        break_string(CountUp(chans), 78));
      return 0;
    }
    else if (CHMASTER->remove_channel(target_channel, ME))
    {
      notify_fail("Die Ebene '"+target_channel+"' lies sich nicht "
        "entfernen!\n");
      return 0;
    }

    tell_object(ME,"Du entfernst die Ebene '"+target_channel+"'.\n");
    return 1;
  }

  if (sscanf(args, "clear %s", target_channel) && IS_ARCH(ME))
  {
    chans = CHMASTER->find(target_channel, ME);

    if (!chans)
    {
      notify_fail("Ebene '"+target_channel+"' nicht gefunden!\n");
      return 0;
    }
    else if (pointerp(chans))
    {
      notify_fail(
        "Das war keine eindeutige Angabe! Folgende Ebenen passen:\n"+
        break_string(CountUp(chans), 78));
      return 0;
    }
    else if (CHMASTER->clear_history(target_channel, ME) == E_ACCESS_DENIED)
    {
      notify_fail("Der Verlauf zur Ebene '"+target_channel+"' liess sich "
                  "nicht entfernen!\n");
      return 0;
    }

    tell_object(ME,"Du entfernst den Verlauf zur Ebene '"+target_channel+
      "'.\n");
    return 1;
  }

  if (sscanf(args, "neu %s %s", target_channel, descr) == 2)
  {
    notify_fail(break_string("Neue Ebenen kannst Du erst erstellen, wenn "
      "Du Spielerstufe 5 erreicht hast.", 78));
    if (QueryProp(P_LEVEL) < 5)
      return 0;

    notify_fail("Der Name '"+target_channel+"' ist nicht konform!\n");
    if (!sizeof(regexp(({target_channel}), "^" CHANNELCMDS CHANNELCMDS "*")))
      return 0;

    notify_fail("Der Name '"+target_channel+"' ist zu lang.\n");
    if (sizeof(target_channel) > 20)
      return 0;

    notify_fail("Diese Ebene darfst du nicht erschaffen!\n");
    if (CHMASTER->new(target_channel, ME, descr) == E_ACCESS_DENIED)
      return 0;

    tell_object(ME,"Du erschaffst die Ebene '"+target_channel+"'.\n");
    SetProp(P_CHANNELS, QueryProp(P_CHANNELS) +
                        ({lower_case(target_channel)}));
    return 1;
  }

  if (sscanf(args, "beschreibung %s %s", target_channel, descr) == 2)
  {
    chans = CHMASTER->find(target_channel, ME);

    if (!chans)
    {
      notify_fail("Ebene '"+target_channel+"' nicht gefunden!\n");
      return 0;
    }
    else if (pointerp(chans))
    {
      notify_fail(
        "Das war keine eindeutige Angabe! Folgende Ebenen passen:\n"+
        break_string(CountUp(chans), 78));
      return 0;
    }

    mapping ch = CHMASTER->list(ME);
    notify_fail("Du bist nicht berechtigt, die Beschreibung der Ebene"
      " '"+target_channel+"' zu aendern.\n");
    if (ch[lower_case(chans)][I_SUPERVISOR] != ME)
      return 0;

    ch[lower_case(target_channel)][I_INFO] = descr;
    tell_object(ME,"Die Ebene '"+target_channel+"' hat ab sofort die "
      "Beschreibung:\n"+descr+"\n");
    return 1;
  }

  if (sscanf(args, "%s=%s", _sh_cut, target_channel) == 2 &&
      sizeof(target_channel))
  {
    chans = CHMASTER->find(target_channel, ME);

    if (!chans)
    {
      notify_fail("Ebene '"+target_channel+"' nicht gefunden!\n");
      return 0;
    }
    else if (pointerp(chans))
    {
      notify_fail(
        "Das war keine eindeutige Angabe! Folgende Ebenen passen:\n"+
        break_string(CountUp(chans), 78));
      return 0;
    }

    mapping shortcut_list = QueryProp(P_CHANNEL_SHORT) || ([]);
    m_add(shortcut_list, _sh_cut, chans);
    SetProp(P_CHANNEL_SHORT, shortcut_list);

    shortcut = QueryProp(P_CHANNEL_SHORT);

    tell_object(ME,"'"+_sh_cut+"' wird jetzt als Abkuerzung fuer '"+chans+
      "' anerkannt.\n");
    return 1;
  }

  if (sscanf(args, "%s=", _sh_cut))
  {
    SetProp(P_CHANNEL_SHORT,
      m_delete(QueryProp(P_CHANNEL_SHORT) || ([]), _sh_cut));
    shortcut = QueryProp(P_CHANNEL_SHORT);
    tell_object(ME,"Du loeschst die Abkuerzung '"+_sh_cut+"'.\n");
    return 1;
  }

  if (args == "an" || args == "ein")
  {
    if (pointerp(QueryProp(P_SWAP_CHANNELS)))
      SetProp(P_CHANNELS, QueryProp(P_SWAP_CHANNELS));
    else
      SetProp(P_CHANNELS, m_indices(CHMASTER->list(ME)));

    // <excl> enthaelt die Channelnamen, deren Channel nicht erstellt wurden.
    string* excl = RegisterChannels();
    tell_object(ME,"Du schaltest folgende Ebenen ein:\n"+
          break_string(CountUp(QueryProp(P_CHANNELS) - excl), 78));
    SetProp(P_SWAP_CHANNELS, 0);
    return 1;
  }

  if (args == "aus")
  {
    SetProp(P_SWAP_CHANNELS, QueryProp(P_CHANNELS));
    RemoveChannels();
    SetProp(P_CHANNELS, ({}));
    tell_object(ME,"Du stellst die Ebenen ab.\n");
    return 1;
  }

  string* pa = old_explode(args, " ");
  if (strstr("abkuerzungen", pa[0]) == 0)
  {
    string txt = "";
    if (sizeof(pa) > 1 && strstr("standard", pa[1]) == 0)
    {
      tell_object(ME,"Die Standardabkuerzungen werden gesetzt.\n");
      SetProp(P_CHANNEL_SHORT,
        DEFAULT_SHORTCUTS + (IS_LEARNER(ME) ? WIZARD_SHORTCUTS : ([])));
    }

    mapping scut = QueryProp(P_CHANNEL_SHORT);
    string* abbreviations = m_indices(scut);
    foreach (string abk : sort_array(abbreviations, #'>))
    {
      txt += sprintf("%5.5s = %s\n", abk, scut[abk]);
    }
    More( "Folgende Abkuerzungen sind definiert:\n"+
          sprintf("%-78#s\n", txt) );
    return 1;
  }

  if (strstr("standard", pa[0]) == 0)
  {
    if (sizeof(pa) < 2)
    {
      notify_fail("Benutzung: ebene standard <Ebene>\n"+
        (QueryProp(P_STD_CHANNEL)
          ? "Momentan ist '"+QueryProp(P_STD_CHANNEL)+"' eingestellt.\n"
          : "Es ist keine Standardebene eingestellt.\n"));
      return 0;
    }

    chans = CHMASTER->find(pa[1], ME);
    if (!chans)
    {
      notify_fail("Ebene '"+pa[1]+"' nicht gefunden!\n");
      return 0;
    }
    else if (pointerp(chans))
    {
      notify_fail(
        "Das war keine eindeutige Angabe! Folgende Ebenen passen:\n"+
        break_string(CountUp(chans), 78));
      return 0;
    }

    tell_object(ME,"'"+chans+"' ist jetzt die Standardebene.\n");
    SetProp(P_STD_CHANNEL, chans);
    return 1;
  }
  return(0);
}

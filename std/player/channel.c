// MorgenGrauen MUDlib
//
// channel.c -- channel client
//
// $Id: channel.c 9404 2015-12-13 00:21:44Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

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

#define P_SWAP_CHANNELS  "swap_channels"
#define P_CHANNEL_SHORT  "short_channels"

#define CHANNELCMDS      "[#@%$&()<>a-zA-Z0-9\\-]"

#define DEFAULT_CHANNELS ({"Abenteuer", "Anfaenger","Grats","Tod", "ZT"})
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
      + (IS_LEARNER(this_object()) ? WIZARD_SHORTCUTS : ([])));
}

static mixed _query_localcmds()
{
  return ({({"-","ChannelParser", 1, 0}),
           ({"ebene", "ChannelAdmin", 0, 0}),
           ({"ebenen", "ChannelAdmin", 1, 0}),
         });
}

mixed RegisterChannels()
{
  mixed err;
  if(extern_call() &&
     previous_object() != find_object(CHMASTER)) return;
  c_status = 0;
  shortcut = QueryProp(P_CHANNEL_SHORT);
  SetProp(P_CHANNELS, map(QueryProp(P_CHANNELS) || ({}),
    #'lower_case));
  err = filter(QueryProp(P_CHANNELS),
         symbol_function("join", CHMASTER),
         this_object());
  if(QueryProp(P_LEVEL) < 5) return err;
  while(sizeof(err)) {
    CHMASTER->new(err[0], this_object());
    err[0..0] = ({});
  }
  return err;
}

mixed RemoveChannels()
{
  closure cl;
  mixed err=({});
  if(extern_call() &&
     previous_object() != find_object(CHMASTER)) return;
  if(!c_status) c_status = 1;
  else return ({});
  cl=symbol_function("leave", CHMASTER);
  if (closurep(cl)) {
      err = filter(QueryProp(P_CHANNELS), cl, this_object());
      SetProp(P_CHANNELS, QueryProp(P_CHANNELS) - err);
  }
  return err;
}

varargs private string getName(mixed x, int fall) {
  
  mixed o = closurep(x) ? query_closure_object(x) : x;
  if(stringp(o) && sizeof(o) && (x = find_object(o))) 
    o = x;
  
  // Objekte
  if (objectp(o)) {
    // Magier sehen unsichtbare nicht nur als "Jemand"
    if (o->QueryProp(P_INVIS) && IS_LEARNING(this_object()))
      return "("+capitalize(getuid(o))+")";
    // Froesche mit Namen versorgen.
    if (o->QueryProp(P_FROG))
      return "Frosch "+capitalize(getuid(o));
    // Default (Unsichtbare als "Jemand" (s. Name()))
    return o->Name(fall, 2)||"<Unbekannt>";
  }
  // Strings
  else if (stringp(o) && sizeof(o)) {
    if (o[0] == '/') {
      // unsichtbare Objekte...
      int p = strstr(o, "$");
      if (p != -1) {
    // Magier im Magiermodus kriegen den Realnamen, andere nicht.
    if (IS_LEARNING(this_object()))
      return o[1..p-1];
    else
      return o[p+1..];
      }
      else
    // doch nicht unsichtbar
    return (fall == WESSEN ? o+"s" : o);
    }
    else
      // nicht unsichtbar
      return (fall == WESSEN ? o+"s" : o);
  }
  // Fall-through
  return "<Unbekannt>";
}

// <nonint> unterdrueckt die AUsgabe an den Spieler und liefert den Text
// zurueck. Wird nur fuer die Ebenenhistory benutzt. 
string ChannelMessage(mixed* msg, int nonint)
{
  string channel_message;
  string channel=msg[0];
  object sender=msg[1];
  string message=msg[2];
  int msg_type = msg[3];

  if ( previous_object() != find_object(CHMASTER) &&
       previous_object() != ME )
      return 0;

  string sender_name = getName(sender, msg_type == MSG_GEMOTE ? WESSEN : WER);
  int prepend_indent_flag=QueryProp(P_MESSAGE_PREPEND) ? BS_PREPEND_INDENT : 0;
  switch(msg_type) {
  case MSG_EMPTY:
    channel_message= message+"\n";
    break;
  case MSG_GEMOTE:
  case MSG_EMOTE:
    channel_message = break_string(sender_name + " "+ message+"]",
                   78, sprintf("[%s:", channel),
                   BS_INDENT_ONCE|prepend_indent_flag);
    break;
  case MSG_SAY:
  default:
    string presay=sprintf("[%s:%s] ", channel, sender_name);
    channel_message = break_string(message, max(78,sizeof(presay)+10),
                   presay, prepend_indent_flag);
    break;
  }
  if(nonint)
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

private void createList(string n, mixed a, mixed m, mixed l)
{
  int pos; string sh, *mem;
  if((pos = member(map(m_values(shortcut), #'lower_case/*'*/), n)) != -1)
    sh = m_indices(shortcut)[pos];
  else sh = "";
  mem=map(a[I_MEMBER],#'getName/*'*/, WER);
  mem-=({"<MasteR>"});
  l += ({ sprintf("%-12.12'.'s %c[%-1.1s] %|12.12' 's (%-|3' 'd) %-42.42s\n",
                  a[I_NAME], (member(m, n) != -1 ? '*' : ' '), sh,
                  a[I_MASTER] ?
                  getName(a[I_MASTER]) : getName(a[I_ACCESS]),
                  sizeof(mem),
            (closurep(a[I_INFO]) && objectp(query_closure_object(a[I_INFO]))) ?
                  funcall(a[I_INFO]) || "- Keine Beschreibung -" :
                  (stringp(a[I_INFO]) ? a[I_INFO] : "- Keine Beschreibung -")
        ) });
}

private mixed getChannel(string ch)
{
  mixed ff;
  if(!sizeof(ch)) ch = QueryProp(P_STD_CHANNEL);
  if(shortcut && shortcut[ch]) ch = shortcut[ch];
  return CHMASTER->find(ch, this_object());
}

#ifndef DEBUG
#define DEBUG(x)        if (funcall(symbol_function('find_player),"zesstra"))\
          tell_object(funcall(symbol_function('find_player),"zesstra"),\
                  "MDBG: "+x+"\n")
#endif
int ChannelParser(string args)
{
  mixed ch, cmd, tmp;
  int pos, type, err;
  string txt;
  cmd = query_verb();
  args = _unparsed_args();
  notify_fail("Benutzung: -<Ebene>[ ]['|:|;]<Text>\n"
              "           -<Ebene>[+|-|?|!|*]\n"
              "           -?\n");
  if(!cmd && !args) return 0;
  if(!args) args = "";
  cmd = cmd[1..];
  if(sizeof(cmd = regexplode(cmd,
                             "^" CHANNELCMDS "*"
                             "([+-]|\\!|\\?|\\*)*")) > 1)
  {
    //z.B. cmd= ({"","allgemein",":testet"})
    if(sizeof(cmd[1]) > 1 &&
       strstr("+-?!*", cmd[1][<1..<1]) > -1)
      tmp = cmd[1][0..<2];
    else
      tmp = cmd[1];
    if(cmd[1] != "?" && cmd[1] != "!")
    {
      ch = getChannel(tmp);
      if(pointerp(ch))
      {
        notify_fail("Diese Angabe war nicht eindeutig! "
                    "Folgende Ebenen passen:\n"
                    +implode(ch, ", ")+"\n");
        return 0;
      }
      else if(!ch)
        return (notify_fail("Die Ebene '"+tmp
                            + "' gibt es nicht!\n"), 0);
    }
    //DEBUG(sprintf("ChanCmd: %O\n",cmd));
    if (sizeof(cmd[1])) {
      switch(cmd[1][<1]) {
    case '+':
      switch(CHMASTER->join(ch, this_object()))
      {
      case E_ACCESS_DENIED:
        notify_fail("Du darfst an die Ebene '"+ch+"' nicht heran.\n");
        return 0;
      case E_ALREADY_JOINED:
        notify_fail("Du hast diese Ebene schon betreten!\n");
        return 0;
      default: break;
      }
      write("Du betrittst die Ebene '"+ch+"'.\n");
      if(member(QueryProp(P_CHANNELS), ch = lower_case(ch)) == -1)
        SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({ ch }));
      return 1;
    case '-':
      switch(CHMASTER->leave(ch, this_object()))
      {
      case E_ACCESS_DENIED:
        write("Du kannst die Ebene '"+ch+"' nicht verlassen.\n");
        break;
      case E_NOT_MEMBER:
        write("Wie willst Du eine Ebene verlassen, welche Du nicht "
              "betreten hast?\n");
        break;
      default:
        write("Du verlaesst die Ebene '"+ch+"'.\n");
        SetProp(P_CHANNELS, QueryProp(P_CHANNELS) - ({ lower_case(ch), ch }));
        break;
      }
      return 1;
    case '!':
    case '?':
    {
      mapping l;
      if(mappingp(l = CHMASTER->list(this_object())))
      {
        if(stringp(ch) && sizeof(ch) && pointerp(l[ch = lower_case(ch)]))
        {
          int c; object o; string n; string *m;
          m=sort_array(map(l[ch][I_MEMBER],#'getName/*'*/, WER),#'>/*'*/);
          m-=({"<MasteR>"});
          write(l[ch][I_NAME]+", "+funcall(l[ch][I_INFO])+".\n");
          write("Du siehst "+((c = sizeof(m)) > 0
                              ? (c == 1 ? "ein Gesicht" : c+" Gesichter")
                              : "niemanden")+" auf der Ebene '"
                                +l[ch][I_NAME]+"':\n");
          write(break_string(implode(m,", "), 78));
          write((l[ch][I_MASTER] ?
                 getName(l[ch][I_MASTER]) : getName(l[ch][I_ACCESS], WER))
                +" hat das Sagen auf dieser Ebene.\n");
        }
        else
        {
          mixed list; list = ({});
          if(cmd[1][<1] == '!')
            l -= mkmapping(m_indices(l) - QueryProp(P_CHANNELS));
          walk_mapping(l, #'createList/*'*/, QueryProp(P_CHANNELS), &list);
          list = sort_array(list, #'>/*'*/);
          txt = sprintf("%-12.12' 's  [A] %|12' 's (%-3' 's) %-42.42s\n",
                        "Name", "Eigner", "Sp", "Beschreibung")
              + "-------------------------------------------------------"
              + "-----------------------\n"
              + implode(list, "");
          More(txt);
        }
      }
      return 1;

    }
    case '*':
    {
      mixed hist; int amount;
      if(!pointerp(hist = CHMASTER->history(ch, this_object())) 
              || !sizeof(hist))
      {
        write("Es ist keine Geschichte fuer '"+ch+"' verfuegbar.\n");
        return 1;
      }
      
      //(Zesstra) cmd hat offenbar immer 3 Elemente...
      //bei -all* ({"","all*",""})
      //bei -all*10 ({"","all*,"10"})
      //also ist bei -all* amount immer == 0 und es funktioniert eher zufaellig.
      /*if(sizeof(cmd) > 2) 
        amount = to_int(cmd[2]);
      else 
        amount=sizeof(hist);*/
      amount=to_int(cmd[2]);
      if (amount <= 0 || amount >= sizeof(hist))
        amount=sizeof(hist);

      txt = "Folgendes ist auf '"+ch+"' passiert:\n"
          + implode(map(hist[<amount..], #'ChannelMessage/*'*/, 1), "");
      More(txt);
      return 1;
    }
    default:
      break;
    }
    }
  }
  if(sizeof(cmd = implode(cmd[2..], "")))
     args = cmd + (sizeof(args) ? " " : "") + args;

  // KOntrollchars ausfiltern.
  args = regreplace(args,"[[:cntrl:]]","",RE_PCRE|RE_GLOBAL);
  if(!sizeof(args)) return 0;

  //Wenn cmd leer ist: MSG_SAY
  if (!sizeof(cmd)) type=MSG_SAY;
  else {
    switch(cmd[0])
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
    default  : type = MSG_SAY; break;
    }
  }
  if(!ch || !sizeof(ch)) ch = QueryProp(P_STD_CHANNEL);
  if((err = CHMASTER->send(ch, this_object(), args, type)) < 0)
    if(!(err = CHMASTER->join(ch, this_object())))
    {
      if(member(QueryProp(P_CHANNELS), ch = lower_case(ch)) == -1)
        SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({ ch }));
      err = CHMASTER->send(ch, this_object(), args, type);
    }

  switch(err)
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
  string n, descr, sh, cn;
  mixed pa, tmp;
  args = _unparsed_args();
  notify_fail("Benutzung: ebene <Abkuerzung>=<Ebene>\n"
              "           ebene <Abkuerzung>=\n"
              "           ebene abkuerzungen [standard]\n"
              "           ebene standard <Ebene>\n"
              "           ebene an|ein|aus\n"
              +(QueryProp(P_LEVEL) >= 5 ?
                "           ebene neu <Name> <Bezeichnung>\n"
    "           ebene beschreibung <Name> <Beschreibung>\n" : "")
        +(IS_ARCH(this_object()) ?
          "           ebene kill <Name>\n"
      "           ebene clear <Name>\n": ""));
  if(!args || !sizeof(args)) return 0;
  if(sscanf(args, "kill %s", n) && IS_ARCH(this_object()))
  {
    if(!(cn = CHMASTER->find(n, this_object()))) cn = n;
    switch(CHMASTER->remove(cn, this_object()))
    {
    case E_ACCESS_DENIED:
     notify_fail("Die Ebene '"+cn+"' lies sich nicht entfernen!\n");
     return 0;
    }
    write("Du entfernst die Ebene '"+cn+"'.\n");
    return 1;
  }
  if(sscanf(args, "clear %s", n) && IS_ARCH(this_object()))
  {
    if(!(cn = CHMASTER->find(n, this_object()))) cn = n;
    switch(CHMASTER->clear_history(cn, this_object()))
    {
    case E_ACCESS_DENIED:
     notify_fail("Der Verlauf zur Ebene '"+cn+"' lies sich nicht entfernen!\n");
     return 0;
    }
    write("Du entfernst den Verlauf zur Ebene '"+cn+"'.\n");
    return 1;
  }
  if(sscanf(args, "neu %s %s", n, descr) == 2)
  {
    mixed x;
    if(QueryProp(P_LEVEL) < 5)
      return (notify_fail("Neue Ebenen zu erstellen ist dir verwehrt.\n"), 0);
    if(!sizeof(regexp(({ n }), "^" CHANNELCMDS CHANNELCMDS "*")))
      return (notify_fail("Der Name '"+n+"' ist nicht konform!\n"), 0);
    if (sizeof(n) > 20 )
      return(notify_fail("Der Name '"+n+"' ist zu lang.\n"), 0);
    switch(x = CHMASTER->new(n, this_object(), descr))
    {
    case E_ACCESS_DENIED:
      notify_fail("Diese Ebene darfst du nicht erschaffen!\n"); break;
    default:
      write("Du erschaffst die Ebene '"+n+"'.\n");
      SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({ lower_case(n) }));
      return 1;
    }
  }
  if(sscanf(args, "beschreibung %s %s", n, descr) == 2)
  {
    mixed ch;
    cn = CHMASTER->find(n, this_object());
    if(!cn || pointerp(cn))
      return (notify_fail("Die Ebene '"+n+"' existiert nicht oder die Angabe "
        "war nicht eindeutig.\n"), 0);
    ch = CHMASTER->list(this_object());
    if(ch[lower_case(cn)][I_MASTER] != this_object())
      return (notify_fail("Du bist nicht berechtigt die Beschreibung der Ebene"
        " '"+cn+"' zu aendern.\n"), 0);
    ch[lower_case(cn)][I_INFO] = descr;
    write("Die Ebene '"+cn+"' hat ab sofort die Beschreibung:\n"+descr+"\n");
    return 1;
  }
  if(sscanf(args, "%s=%s", sh, n) == 2 && sizeof(n))
  {
    mapping sc;
    if(pointerp(tmp = CHMASTER->find(n, this_object())) || !tmp)
      return (notify_fail("Benutzung: ebene <Abkuerzung>=<Ebene>\n"
                          +(pointerp(tmp) ? implode(tmp, ", ") + "\n" :
                            "Ebene '"+n+"' nicht gefunden!\n")), 0);
    sc = QueryProp(P_CHANNEL_SHORT);
    if(!sc) sc = ([]);
    sc[sh] = tmp;
    SetProp(P_CHANNEL_SHORT, sc);
    shortcut = QueryProp(P_CHANNEL_SHORT);
    write("'"+sh+"' wird jetzt als Abkuerzung fuer '"+tmp+"' anerkannt.\n");
    return 1;
  }
  if(sscanf(args, "%s=", sh))
  {
    SetProp(P_CHANNEL_SHORT, m_copy_delete(QueryProp(P_CHANNEL_SHORT) || ([]), sh));
    shortcut = QueryProp(P_CHANNEL_SHORT);
    write("Du loeschst die Abkuerzung '"+sh+"'.\n");
    return 1;
  }
  if(args == "an" || args == "ein")
  {
    mixed excl;
    if(pointerp(QueryProp(P_SWAP_CHANNELS)))
      SetProp(P_CHANNELS, QueryProp(P_SWAP_CHANNELS));
    else
      SetProp(P_CHANNELS, m_indices(CHMASTER->list(this_object())));
    excl = RegisterChannels();
    write("Du schaltest folgende Ebenen ein:\n"
          +break_string(implode(QueryProp(P_CHANNELS) - excl, ", "), 78));
    SetProp(P_SWAP_CHANNELS, 0);
    return 1;
  }
  if(args == "aus")
  {
    SetProp(P_SWAP_CHANNELS, QueryProp(P_CHANNELS));
    RemoveChannels();
    SetProp(P_CHANNELS, ({}));
    write("Du stellst die Ebenen ab.\n");
    return 1;
  }
  pa = old_explode(args, " ");
  if(!strstr("abkuerzungen", pa[0]))
  {
    string txt; txt = "";
    if(sizeof(pa) > 1 && !strstr("standard", pa[1]))
    {
      write("Die Standard Abkuerzungen werden gesetzt.\n");
      SetProp(P_CHANNEL_SHORT, DEFAULT_SHORTCUTS
              + (IS_LEARNER(this_object()) ? WIZARD_SHORTCUTS : ([])));
    }
    foreach(string abk, string ch_name : QueryProp(P_CHANNEL_SHORT)) {
      txt += sprintf("%5.5s = %s\n", abk, ch_name);
    }
    txt = sprintf("Folgende Abkuerzungen sind definiert:\n%-78#s\n",
                  implode(sort_array(old_explode(txt, "\n"), #'>/*'*/), "\n"));
    More(txt);
    return 1;
  }
  if(!strstr("standard", pa[0]))
    if(sizeof(pa) < 2)
      return (notify_fail("Benutzung: ebene standard <Ebene>\n"
                          +(QueryProp(P_STD_CHANNEL)
                            ? "Momentan ist '"+QueryProp(P_STD_CHANNEL)
                            +"' eingestellt.\n"
                            : "Es ist keine Standardebene eingestellt.\n")),0);
    else
      if(pointerp(tmp = CHMASTER->find(pa[1], this_object())))
        return (notify_fail("Das war keine eindeutige Angabe! "
                            "Folgende Ebenen passen:\n"
                            +break_string(implode(tmp, ", "), 78)), 0);
      else
        if(!tmp) return (notify_fail("Ebene '"+pa[1]+"' nicht gefunden!\n"),0);
        else
        {
          write("'"+tmp+"' ist jetzt die Standardebene.\n");
          SetProp(P_STD_CHANNEL, tmp);
          return 1;
        }
  return(0);
}


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

#include <properties.h>
#include <config.h>
#include <language.h>

#define NEED_PROTOTYPES
#include "channel.h"

#define CMNAME  "<MasteR>"
#define CHANNEL_SAVE    "/p/daemon/save/channeld"

#define MEMORY "/secure/memory"

// channels - contains the simple channel information
//            ([ channelname : ({ I_MEMBER, I_ACCESS, I_INFO, I_MASTER })...])
private nosave mapping lowerch;
private nosave mapping channels;
private nosave mapping channelH;
private nosave mapping stats;

private mapping channelC;
private mapping channelB;

private int save_me_soon;

// BEGIN OF THE CHANNEL MASTER ADMINISTRATIVE PART

#define RECV    0
#define SEND    1
#define FLAG    2

// Channel flags
// Levelbeschraenkungen gegen Magierlevel (query_wiz_level) pruefen, nicht
// P_LEVEL.
#define F_WIZARD 1
// Keine Gaeste. ;-)
#define F_NOGUEST 2

private nosave mapping admin = m_allocate(0, 3);

int check(string ch, object pl, string cmd)
{
  int level;
  
  if((admin[ch, FLAG] & F_NOGUEST) && pl->QueryGuest()) return 0;

  if((admin[ch, FLAG] & F_WIZARD) && query_wiz_level(pl) < SEER_LVL) return 0;
  level = (admin[ch, FLAG] & F_WIZARD
           ? query_wiz_level(pl)
           : pl->QueryProp(P_LEVEL));

  switch(cmd)
  {
  case C_FIND:
  case C_LIST:
  case C_JOIN:
    if(admin[ch, RECV] == -1) return 0;
    if(admin[ch, RECV] <= level) return 1;
    break;
  case C_SEND:
    if(admin[ch, SEND] == -1) return 0;
    if(admin[ch, SEND] <= level) return 1;
    break;
  case C_LEAVE:
    return 1;
  default: break;
  }
  return(0);
}

private int CountUser(mapping l)
{
  mapping n;
  n = ([]);
  walk_mapping(l, lambda(({'i/*'*/, 'a/*'*/, 'n/*'*/}),
                         ({#'+=/*'*/, 'n/*'*/,
                              ({#'mkmapping/*'*/,
                                   ({#'[/*'*/, 'a/*'*/, 0})})})),
               &n);
  return sizeof(n);
}

private void banned(string n, mixed cmds, string res)
{
  res += sprintf("%s [%s], ", capitalize(n), implode(cmds, ","));
}

private mapping Tcmd = ([]);
#define TIMEOUT (time() - 60)

void ChannelMessage(mixed msg)
{
  string ret, mesg;
  mixed lag;
  int max, rekord;
  string tmp;
  if(msg[1] == this_object() || !stringp(msg[2]) ||
     msg[0] != CMNAME || previous_object() != this_object()) return;


  mesg = lower_case(msg[2]);

  if(!strstr("hilfe", mesg) && sizeof(mesg) <= 5)
    ret = "Folgende Kommandos gibt es: hilfe, lag, up[time], statistik, bann";
  else
  if(!strstr("lag", mesg) && sizeof(mesg) <= 3)
  {
    if(Tcmd["lag"] > TIMEOUT) return;
    Tcmd["lag"] = time();
    lag = "/p/daemon/lag-o-daemon"->read_ext_lag_data();
    ret = sprintf("Lag: %.1f%%/60, %.1f%%/15, %.1f%%/5, %.1f%%/1, "
                       "%.1f%%/20s, %.1f%%/2s",
                  lag[5], lag[4], lag[3], lag[2], lag[1], lag[0]);
    call_out(#'send/*'*/, 2, CMNAME, this_object(), ret);
    ret = query_load_average();
  } else
  if(!strstr("uptime", mesg) && sizeof(mesg) <= 6)
  {
    if(Tcmd["uptime"] > TIMEOUT) return;
    Tcmd["uptime"] = time();
    if(file_size("/etc/maxusers") <= 0) {
      ret = "Diese Information liegt nicht vor.";
    } else {
      sscanf(read_file("/etc/maxusers"), "%d %s", max, tmp);
      sscanf(read_file("/etc/maxusers.ever"), "%d %s", rekord, tmp);
      ret = sprintf("Das MUD laeuft jetzt %s. Es sind momentan %d Spieler "
                  "eingeloggt; das Maximum lag heute bei %d und der Rekord "
                  "bisher ist %d.", uptime(), sizeof(users()), max, rekord);
    }
  } else
  if(!strstr("statistik", mesg) && sizeof(mesg) <= 9)
  {
    if(Tcmd["statistik"] > TIMEOUT) return;
    Tcmd["statistik"] = time();
    ret = sprintf(
    "Im Moment sind insgesamt %d Ebenen mit %d Teilnehmern aktiv.\n"
    "Der %s wurde das letzte mal am %s von %s neu gestartet.\n"
    "Seitdem wurden %d Ebenen neu erzeugt und %d zerstoert.\n",
    sizeof(channels), CountUser(channels), CMNAME,
    dtime(stats["time"]), stats["boot"], stats["new"], stats["dispose"]);
  } else
  if(!strstr(mesg, "bann"))
  {
    string pl, cmd;
    if(mesg == "bann")
      if(sizeof(channelB))
      {
        ret = "";
        walk_mapping(channelB, #'banned/*'*/, &ret);
        ret = "Fuer folgende Spieler besteht ein Bann: " + ret;
      } else ret = "Zur Zeit ist kein Bann aktiv.";
    else
    {
      if(sscanf(mesg, "bann %s %s", pl, cmd) == 2 &&
         IS_DEPUTY(msg[1]))
      {
#     define CMDS ({C_FIND, C_LIST, C_JOIN, C_LEAVE, C_SEND, C_NEW})
        pl = lower_case(pl); cmd = lower_case(cmd);
        if(member(CMDS, cmd) != -1)
        {
          if(!pointerp(channelB[pl]))
            channelB[pl] = ({});
          if(member(channelB[pl], cmd) != -1)
            channelB[pl] -= ({ cmd });
          else
            channelB[pl] += ({ cmd });
          ret = "Fuer '"+capitalize(pl)+"' besteht "
            + (sizeof(channelB[pl]) ?
               "folgender Bann: "+implode(channelB[pl], ", ") :
               "kein Bann mehr.");
          if(!sizeof(channelB[pl]))
            channelB = m_copy_delete(channelB, pl);
          save_object(CHANNEL_SAVE);
        }
        else ret = "Das Kommando '"+cmd+"' ist unbekannt. Erlaubte Kommandos: "
               + implode(CMDS, ", ");
      }
      else
      {
        if(!IS_ARCH(msg[1])) return;
        else ret = "Syntax: bann <name> <kommando>";
      }
    }
  }
  else if(mesg == "lust")
  {
    mixed t, up;
    if(Tcmd["lag"] > TIMEOUT ||
       Tcmd["statistik"] > TIMEOUT ||
       Tcmd["uptime"] > TIMEOUT) return;
    Tcmd["lag"] = time();
    Tcmd["statistik"] = time();
    Tcmd["uptime"] = time();
    lag = "/p/daemon/lag-o-daemon"->read_lag_data();

    sscanf(read_file("/etc/maxusers"), "%d %s", max, tmp);
    sscanf(read_file("/etc/maxusers.ever"), "%d %s", rekord, tmp);

    t=time()-last_reboot_time();
    up="";
    if(t >= 86400)
      up += sprintf("%dT", t/86400);
    if(t >= 3600)
      up += sprintf("%dh", (t=t%86400)/3600);
    if(t > 60)
      up += sprintf("%dm", (t=t%3600)/60);
    up += sprintf("%ds", t%60);

    ret = sprintf("%.1f%%/15 %.1f%%/1 %s %d:%d:%d E:%d T:%d",
                  lag[1], lag[2], up, sizeof(users()), max, rekord, 
                  sizeof(channels), CountUser(channels));
  } else return;

  call_out(#'send/*'*/, 2, CMNAME, this_object(), ret);
}

// setup() -- set up a channel and register it
//            arguments are stored in the following order:
//            ({ channel name,
//               receive level, send level,
//               flags,
//               description,
//               master obj
//            })
private void setup(mixed c)
{
  closure cl;
  object m;
  string d;
  d = "- Keine Beschreibung -";
  m = this_object();
  if(sizeof(c) && sizeof(c[0]) > 1 && c[0][0] == '\\')
    c[0] = c[0][1..];

  switch(sizeof(c))
  {
  case 6:
    if(!stringp(c[5]) || !sizeof(c[5]) 
        || (catch(m=load_object(c[5]);publish) || !objectp(m) ))
      m = this_object();
  case 5: d = stringp(c[4]) || closurep(c[4]) ? c[4] : d;
  case 4: admin[c[0], FLAG] = to_int(c[3]);
  case 3: admin[c[0], SEND] = to_int(c[2]);
  case 2: admin[c[0], RECV] = to_int(c[1]);
    break;
  case 0:
  default:
    return;
  }
  switch(new(c[0], m, d))
  {
  case E_ACCESS_DENIED:
    log_file("CHANNEL", sprintf("[%s] %s: %O: error, access denied\n",
                           dtime(time()), c[0], m));
    break;
  default:
    break;
  }
  return;
}

void initialize()
{
  mixed tmp;
  tmp = read_file(object_name(this_object())+".init");
  tmp = regexp(old_explode(tmp, "\n"), "^[^#]");
  tmp = map(tmp, #'regexplode/*'*/, "[^:][^:]*$|[ \\t]*:[ \\t]*");
  tmp = map(tmp, #'regexp/*'*/, "^[^: \\t]");
  map(tmp, #'setup/*'*/);
}

// BEGIN OF THE CHANNEL MASTER IMPLEMENTATION

void create()
{
  seteuid(getuid());
  restore_object(CHANNEL_SAVE);
  if(!channelC) channelC = ([]);
  if(!channelB) channelB = ([]);
  channels = ([]);

  /* Die Channel-History wird nicht nur lokal sondern auch noch im Memory 
     gespeichert, dadurch bleibt sie auch ueber ein Reload erhalten. 
     Der folgende Code versucht, den Zeiger aus dem Memory zu holen. Falls
     das nicht moeglich ist, wird ein neuer erzeugt und gegebenenfalls im
     Memory abgelegt. */

  // Hab ich die noetigen Rechte im Memory?
  if (call_other(MEMORY,"HaveRights")) {
    // Objektpointer laden
    channelH = (mixed) call_other(MEMORY,"Load","History");

    // Wenns nich geklappt hat, hat der Memory noch keinen Zeiger, dann
    if (!mappingp(channelH)){
      // Zeiger erzeugen
      channelH = ([]);
      // und in den Memory schreiben
      call_other(MEMORY,"Save","History",channelH);
    }
  } else {
    // Keine Rechte im Memory, dann wird mit einem lokalen Zeiger gearbeitet.
    channelH = ([]);
  }

  stats = (["time": time(),
            "boot": capitalize(getuid(previous_object())||"<Unbekannt>")]);
  new(CMNAME, this_object(), "Zentrale Informationen zu den Ebenen");
  initialize();
  map_objects(efun::users(), "RegisterChannels");
  this_object()->send(CMNAME, this_object(),
                      sprintf("%d Ebenen mit %d Teilnehmern initialisiert.",
                              sizeof(channels),
                              CountUser(channels)));
}

// reset() and cache_to() - Cache Timeout, remove timed out cached channels
// SEE: new, send
private int cache_to(string key, mapping m, int t)
{
  if(!pointerp(m[key]) || m[key][2] + 43200 > t) return 1;
  return(0);
}

varargs void reset(int nonstd)
{
  channelC = filter_indices(channelC, #'cache_to/*'*/, channelC, time());
  if (save_me_soon)
  {
    save_me_soon=0;
    save_object(CHANNEL_SAVE);
  }
}

// name() - define the name of this object.
string name() { return CMNAME; }
string Name() { return CMNAME; }

// access() - check access by looking for the right argument types and
//            calling access closures respectively
// SEE: new, join, leave, send, list, users
// Note: <pl> is usually an object, only the master supplies a string during
//       runtime error handling.
varargs private int access(mixed ch, mixed pl, string cmd, string txt)
{
  mixed co, m;

  if(!stringp(ch) || !sizeof(ch = lower_case(ch)) || !channels[ch])
    return 0;
  if(!channels[ch][I_ACCESS]||!previous_object(1)||!extern_call()||
     previous_object(1)==this_object()||
     (stringp(channels[ch][I_MASTER])&&
      previous_object(1)==find_object(channels[ch][I_MASTER]))||
     getuid(previous_object(1)) == ROOTID)
    return 2;
  if(!objectp(pl) || 
     ((previous_object(1)!=pl) &&(previous_object(1)!=this_object()))) 
     return 0;
   if(pointerp(channelB[getuid(pl)]) &&
     member(channelB[getuid(pl)], cmd) != -1)
    return 0;
   if(stringp(channels[ch][I_MASTER]) &&
     (!(m = find_object(channels[ch][I_MASTER])) ||
      (!to_object(channels[ch][I_ACCESS]) ||
       get_type_info(channels[ch][I_ACCESS])[1])))
  {
    string err;
    if(!objectp(m)) err = catch(load_object(channels[ch][I_MASTER]);publish);
    if(!err &&
       ((!to_object(channels[ch][I_ACCESS]) ||
         get_type_info(channels[ch][I_ACCESS])[1]) &&
        !closurep(channels[ch][I_ACCESS] =
                  symbol_function("check",
                                  find_object(channels[ch][I_MASTER])))))
    {
      log_file("CHANNEL", sprintf("[%s] %O -> %O\n",
                                  dtime(time()), channels[ch][I_MASTER],
                                  err));
      channels = m_copy_delete(channels, ch);
      return 0;
    }
    this_object()->join(ch, find_object(channels[ch][I_MASTER]));
  }
  if(closurep(channels[ch][I_ACCESS]))
      return funcall(channels[ch][I_ACCESS],
                     channels[ch][I_NAME], pl, cmd, &txt);
}

// new() - create a new channel
//         a channel with name 'ch' is created, the player is the master
//         info may contain a string which describes the channel or a closure
//         to display up-to-date information, check may contain a closure
//         called when a join/leave/send/list/users message is received
// SEE: access

#define IGNORE  "^/xx"

varargs int new(string ch, object pl, mixed info)
{
  mixed pls;

  if(!objectp(pl) || !stringp(ch) || !sizeof(ch) || channels[lower_case(ch)] ||
     (pl == this_object() && extern_call()) ||
     sizeof(channels) >= MAX_CHANNELS ||
     sizeof(regexp(({ object_name(pl) }), IGNORE)) ||
     (pointerp(channelB[getuid(pl)]) &&
      member(channelB[getuid(pl)], C_NEW) != -1))
    return E_ACCESS_DENIED;

  if(!info) {
    if(channelC[lower_case(ch)]) {
      ch = channelC[lower_case(ch)][0];
      info = channelC[lower_case(ch)][1];
    }
    else return E_ACCESS_DENIED;
  }
  else channelC[lower_case(ch)] = ({ ch, info, time() });

  pls = ({ pl });

  channels[lower_case(ch)] = ({ pls,
                                symbol_function("check", pl) ||
                                #'check/*'*/, info,
                                (!living(pl) &&
                                 !clonep(pl) &&
                                 pl != this_object()
                                 ? object_name(pl)
                                 : pl),
                                 ch,
                             });

  // ChannelH fuer einen Kanal nur dann initialisieren, wenn es sie noch nich gibt.
  if ( !pointerp(channelH[lower_case(ch)]) )
         channelH[lower_case(ch)] = ({});

  if(pl != this_object())
    log_file("CHANNEL.new", sprintf("[%s] %O: %O %O\n",
                                    dtime(time()), ch, pl, info));
  if(!pl->QueryProp(P_INVIS))
    this_object()->send(CMNAME, pl,
                        "laesst die Ebene '"+ch+"' entstehen.", MSG_EMOTE);
  stats["new"]++;
 
  save_me_soon=1;
  return(0);
}

// join() - join a channel
//          this function checks whether the player 'pl' is allowed to join
//          the channel 'ch' and add if successful, one cannot join a channel
//          twice
// SEE: leave, access
int join(string ch, object pl)
{
  if(!funcall(#'access,&ch, pl, C_JOIN)) return E_ACCESS_DENIED;
  if(member(channels[ch][I_MEMBER], pl) != -1) return E_ALREADY_JOINED;
  channels[ch][I_MEMBER] += ({ pl });
  return(0);
}

// leave() - leave a channel
//           the access check in this function is just there for completeness
//           one should always be allowed to leave a channel.
//           if there are no players left on the channel it will vanish, unless
//           its master is this object.
// SEE: join, access
int leave(string ch, object pl)
{
  int pos;
  if(!funcall(#'access,&ch, pl, C_LEAVE)) return E_ACCESS_DENIED;
  channels[ch][I_MEMBER] -= ({0}); // kaputte Objekte erstmal raus
  if((pos = member(channels[ch][I_MEMBER], pl)) == -1) return E_NOT_MEMBER;
  if(pl == channels[ch][I_MASTER] && sizeof(channels[ch][I_MEMBER]) > 1)
  {
    channels[ch][I_MASTER] = channels[ch][I_MEMBER][1];
    if(!pl->QueryProp(P_INVIS))
      this_object()->send(ch, pl, "uebergibt die Ebene an "
                          +channels[ch][I_MASTER]->name(WEN)+".", MSG_EMOTE);
  }
  channels[ch][I_MEMBER][pos..pos] = ({ });


  if(!sizeof(channels[ch][I_MEMBER]) &&
     !stringp(channels[ch][I_MASTER]))
  {
    // delete the channel that has no members
    if(!pl->QueryProp(P_INVIS))
      this_object()->send(CMNAME, pl,
                          "verlaesst als "
                          +(pl->QueryProp(P_GENDER) == 1 ? "Letzter" :
                            "Letzte")
                          +" die Ebene '"
                          +channels[ch][I_NAME]
                          +"', worauf diese sich in einem Blitz oktarinen "
                          +"Lichts aufloest.", MSG_EMOTE);
    channelC[lower_case(ch)] = ({ channels[ch][I_NAME],
                                  channels[ch][I_INFO], time() });
    m_delete(channels, lower_case(ch));

    // Wird ein Channel entfernt, wird auch seine History geloescht
    channelH = m_copy_delete(channelH, lower_case(ch));

    stats["dispose"]++;
    save_me_soon=1;
  }
  return(0);
}

// send() - send a message to all recipients of the specified channel 'ch'
//          checks if 'pl' is allowed to send a message and sends if success-
//          ful a message with type 'type'
//          'pl' must be an object, the message is attributed to it. e.g.
//            ignore checks use it. It can be != previous_object()
// SEE: access, ch.h
varargs int send(string ch, object pl, string msg, int type)
{
  int a;

  if(!(a = funcall(#'access,&ch, pl, C_SEND, &msg))) return E_ACCESS_DENIED;
  if(a < 2 && member(channels[ch][I_MEMBER], pl) == -1) return E_NOT_MEMBER;
  if(!msg || !stringp(msg) || !sizeof(msg)) return E_EMPTY_MESSAGE;
  map_objects(channels[ch][I_MEMBER],
              "ChannelMessage", ({ channels[ch][I_NAME], pl, msg, type }));
  if(sizeof(channelH[ch]) > MAX_HIST_SIZE)
    channelH[ch] = channelH[ch][1..];
  channelH[ch] += ({ ({ channels[ch][I_NAME],
                        (stringp(pl)
                         ? pl
                         : (pl->QueryProp(P_INVIS)
                            ? "/("+capitalize(getuid(pl))+")$" : "")
                         + (pl->Name(WER, 2) || "<Unbekannt>")),
                         msg+" <"+strftime("%a, %H:%M:%S")+">\n",
                         type }) });
  return(0);
}

// list() - list all channels, that are at least receivable by 'pl'
//          returns a mapping,
// SEE: access, channels
private void clean(string n, mixed a) { a[0] -= ({ 0 }); }
mixed list(object pl)
{
  mapping chs;
  
  chs = filter_indices(channels, #'access/*'*/, pl, C_LIST);
  walk_mapping(chs, #'clean/*'*/);
  if(!sizeof(chs)) return E_ACCESS_DENIED;
  return deep_copy(chs);
}

// find() - find a channel by its name (may be partial)
//          returns an array for multiple results and 0 for no matching name
// SEE: access
mixed find(string ch, object pl)
{
  mixed chs, s;
  if(stringp(ch)) ch = lower_case(ch);
  if( !sizeof(regexp(({ch}),"^[<>a-z0-9#-]*$")) ) return 0; // RUM
  if(!sizeof(chs = regexp(m_indices(channels), "^"+ch+"$")))
    chs = regexp(m_indices(channels), "^"+ch);
  if((s = sizeof(chs)) > 1)
    if(sizeof(chs = filter(chs, #'access/*'*/, pl, C_FIND)) == 1)
      return channels[chs[0]][I_NAME];
    else return chs;
  return ((s && funcall(#'access,chs[0], pl, C_FIND)) ? channels[chs[0]][I_NAME] : 0);
}

// history() - get the history of a channel
// SEE: access
mixed history(string ch, object pl)
{
  if(!funcall(#'access,&ch, pl, C_JOIN))
    return E_ACCESS_DENIED;
  return deep_copy(channelH[ch]);
}

// remove - remove a channel (wird aus der Shell aufgerufen)
// SEE: new
mixed remove(string ch, object pl)
{
  mixed members;

  if(previous_object() != this_object())
    if(!stringp(ch) ||
       pl != this_player() || this_player() != this_interactive() ||
       this_interactive() != previous_object() ||
       !IS_ARCH(this_interactive()))
      return E_ACCESS_DENIED;

  if(channels[lower_case(ch)]) {
    channels[lower_case(ch)][I_MEMBER] =
        filter_objects(channels[lower_case(ch)][I_MEMBER],
                       "QueryProp", P_CHANNELS);
    map(channels[lower_case(ch)][I_MEMBER],
        lambda(({'u/*'*/}), ({#'call_other/*'*/, 'u, /*'*/
                                   "SetProp", P_CHANNELS,
                                   ({#'-/*'*/,
                                          ({#'call_other/*'*/, 'u, /*'*/
                                                 "QueryProp", P_CHANNELS}),
                                          '({ lower_case(ch) })/*'*/,})
                                   })));
    channels = m_copy_delete(channels, lower_case(ch));

    // Wird ein Channel entfernt, wird auch seine History geloescht
    if( pointerp(channelH[lower_case(ch)] ))
     channelH = m_copy_delete(channelH, lower_case(ch));

    stats["dispose"]++;
  }
  if(!channelC[lower_case(ch)])
    return E_ACCESS_DENIED;
  channelC = m_copy_delete(channelC, lower_case(ch));
  save_me_soon=1;
  return(0);
}

// Wird aus der Shell aufgerufen 
mixed clear_history(string ch)
{
  mixed members;
  // Sicherheitsabfragen
  if(previous_object() != this_object())
    if(!stringp(ch) ||
       this_player() != this_interactive() ||
       this_interactive() != previous_object() ||
       !IS_ARCH(this_interactive()))
      return E_ACCESS_DENIED;

  // History des Channels loeschen
  if( pointerp(channelH[lower_case(ch)] ))
    channelH[lower_case(ch)]=({});

  return 0;
}

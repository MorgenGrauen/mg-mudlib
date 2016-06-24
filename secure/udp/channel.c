// MorgenGrauen MUDlib
//
// channel.c
//
// $Id: channel.c 9142 2015-02-04 22:17:29Z Zesstra $

#include <udp.h>

#include <properties.h>
#include <daemon.h>

#ifdef ZEBEDEE
inherit "/sys/format";
#endif

#define COMMAND                "cmd"
#define CHANNEL                "channel"

private nosave mixed _name_;

int filter_listeners(object ob, string channel) {
    return ob->QueryProp(P_INTERMUD);
}

int udp_channel(mapping data) {
  object *list;
  string msg;
  int i, type;

  /* Compatability with older systems. */
  if (!data[CHANNEL])
    data[CHANNEL] = data["CHANNEL"];
  if (!data[COMMAND])
    data[COMMAND] = data["CMD"];
  if (!data[DATA])
    data[DATA]="";

  if (!stringp(data[CHANNEL]) || !sizeof(data[CHANNEL])
      || !stringp(data[DATA]) || !sizeof(data[DATA])
      || !stringp(data[NAME]) || !sizeof(data[NAME])
      || !stringp(data[SENDER]) || !sizeof(data[SENDER]))
    return 0;

  data[DATA]=
    implode(filter(explode(data[DATA], ""), #'>=, " "), "");//'))
  data[NAME]=
    implode(filter(explode(data[NAME], ""), #'>=, " "), "");//'))
  switch(data[COMMAND]) {
  case "list":
    /* Request for a list of people listening to a certain channel. */
    list = filter(users(), "filter_listeners",
                        this_object(), data[CHANNEL]);
    if (i = sizeof(list)) {
      msg = "[" + capitalize(data[CHANNEL]) + "@" +
        LOCAL_NAME + "] Listening:\n";
      while(i--)
        msg +=
          "    " + capitalize(list[i]->query_real_name()) + "\n";
    }
    else
      msg = "[" + capitalize(data[CHANNEL]) + "@" + LOCAL_NAME 
      + "] Nobody Listening.\n";
    INETD->_send_udp(data[NAME], ([
                                  REQUEST: REPLY,
                                  RECIPIENT: data[SENDER],
                                  ID: data[ID],
                                  DATA: msg
                                ]));
    return 1;
  case "emote": /* A channel emote. */
      type = MSG_EMOTE;
      break;
  default: /* A regular channel message. */
    type = MSG_SAY;
    break;
  }
  _name_ = capitalize(data[SENDER])+"@"+capitalize(data[NAME]);
  CHMASTER->send(capitalize(data[CHANNEL]), this_object(), 
                 data[DATA], type);
  _name_ = 0;
  return 1;
}

string name() { return _name_ || "<Intermud>"; }
string Name() {return capitalize(_name_ || "<Intermud>");}

private void _send(string mud, mixed data, mapping request)
{
  if(member(data[HOST_COMMANDS], "channel") != -1 ||
     member(data[HOST_COMMANDS], "*") != -1)
    INETD->_send_udp(data[HOST_NAME], request);
}

void ChannelMessage(mixed m)
{
  mapping request;
  if(m[1] == this_object()) return;
  request = ([
              REQUEST : "channel",
              SENDER  : m[1]->name() || capitalize(getuid(m[1])),
              "CHANNEL": lower_case(m[0]),
              DATA    : implode(old_explode(m[2], "\n"), " ")]);
  if(m[3] == MSG_GEMOTE || m[3] == MSG_EMOTE)
  { 
    request["EMOTE"] = 1;
    request["CMD"] = "emote";
  }
  walk_mapping(INETD->query("hosts") - ([lower_case(MUDNAME)]), 
               #'_send/*'*/, request);
}

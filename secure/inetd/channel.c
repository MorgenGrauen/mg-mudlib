#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

inherit __DIR__"i_service";

#include <intermud.h>
#include <regexp.h>

#define CHMASTER "/p/daemon/channeld"
#define COMMAND  "cmd"
#define CHANNEL  "channel"

public string service_name()
{
  return "channel";
}

public string name(int egal)
{
  return currentname || "<Intermud>";
}

varargs private string getName(mixed x, int fall) {

  mixed o = closurep(x) ? query_closure_object(x) : x;
  if(stringp(o) && sizeof(o) && (x = find_object(o)))
    o = x;

  // Objekte
  if (objectp(o))
  {
    // Froesche mit Namen versorgen.
    if (o->QueryProp(P_FROG))
      return "Frosch "+capitalize(getuid(o));
    // Default (Unsichtbare als "Jemand" (s. Name()))
    return o->Name(fall, 2)||"<Unbekannt>";
  }
  // Strings
  else if (stringp(o) && sizeof(o)) {
    if (o[0] == '/')
    {
      // unsichtbare Objekte...
      int p = strstr(o, "$");
      if (p != -1)
        return o[p+1..];
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

// NOT USED we received a reply to one of our messages.
//protected void recv_channel_reply(int id, mapping request, mapping response)
//{
//}

// we received an intermud channel message
protected void recv_request(mapping data)
{
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

  data[DATA]= regreplace(data[DATA],"[:^print:]|\n","",1);

  int type;
  switch(data[COMMAND])
  {
  case "list":
    /* Request for a list of people listening to a certain channel. */
    mapping l = CHMASTER->list(this_object());
    string channel = lower_case(data[CHANNEL]);
    string *members;
    string ret;
    if(mappingp(l))
    {
        if(stringp(channel) && sizeof(channel) && pointerp(l[channel]))
        {
          members=sort_array(map(l[channel][I_MEMBER],#'getName, WER),#'>);
        }
    }
    if (members && sizeof(members))
      ret = "[" + data[CHANNEL] + "@" + INETD->PeerName() + "] Listening:\n"
            + implode(members, "\n");
    else
      ret = "Nobody listening / channel unknown.\n";

    INETD->send(data[NAME], ([ REQUEST: REPLY, ID: data[ID],
                               RECIPIENT: data[SENDER],
                               DATA: ret ]),
                0);
    return 0;
  case "emote": /* A channel emote. */
    if (data["EMOTE"] == 1)
      type = MSG_EMOTE;
    else
      type = MSG_GEMOTE;
      break;
  default: /* A regular channel message. */
    type = MSG_SAY;
    break;
  }

  currentname = capitalize(data[SENDER])+"@"+capitalize(data[NAME]);
  CHMASTER->send(capitalize(data[CHANNEL]), this_object(),
                 data[DATA], type);
  currentname = 0;
}

public string ChannelMessage(mixed* msg, int nonint)
{
  if ( previous_object() != find_object(CHMASTER))
      return 0;

  string channel=msg[0];
  object sender=msg[1];
  string channel_message=msg[2];
  int msg_type = msg[3];

  if(sender == this_object()) return 0;

  string sender_name = getName(sender, msg_type == MSG_GEMOTE ? WESSEN : WER);

  mapping request = ([REQUEST: "channel", SENDER: sender,
                      "CHANNEL": lower_case(channel),
                      DATA: channel_message ]);

  switch(msg_type)
  {
  case MSG_GEMOTE:
    request["EMOTE"] = 2;
    request["CMD"] = "emote";
    break;
  case MSG_EMOTE:
    request["EMOTE"] = 1;
    request["CMD"] = "emote";
    break;
  case MSG_SAY:
  default:
    request["CMD"] = "";
    break;
  }

  INETD->broadcast("channel", request, 0);

  return 0;
}

public int request(string mudname, string|int data)
{
  raise_error("request should not be used, use ChannelMessage()!\n");
}


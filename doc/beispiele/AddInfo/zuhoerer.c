#pragma strong_types, save_types, rtt_checks, pedantic
inherit "std/npc";

#include <properties.h>

protected void create() {
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_SHORT, "Ein Beobachter");
  SetProp(P_LONG, break_string(
    "Der Beobachter hoert und sieht zu.", 78));
  SetProp(P_NAME, "Beobachter");
  SetProp(P_GENDER, MALE);
  AddId("beobachter");

  SetProp(P_RACE, "Beobachter");
  SetProp(P_XP, 0);
  SetProp(P_BODY, 50);
}

private string buf = "";

void empty_buffer() {
  tell_room(environment(),
    "\nDer Zuhoerer murmelt: 'Folgendes habe ich gesehen'\n***\n"+
    buf+
    "***\n",
    ({this_object()}));
  buf = "";
}

public varargs int ReceiveMsg(string msg, int msg_typ, string msg_action,
  string msg_prefix, mixed origin) {
  if (msg_typ & MSG_DONT_WRAP)
    buf+=sprintf("%s%s", msg_prefix||"", msg);
  else
    buf+=sprintf("%s%s\n", msg_prefix||"", msg);
  if(find_call_out("empty_buffer")<0)
    call_out("empty_buffer", 1);
}

void catch_tell(string msg) {
  buf+=msg;
  if(find_call_out("empty_buffer")<0)
    call_out("empty_buffer", 1);
}

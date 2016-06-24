/*
 * mudlink tool, offers several commands as an interface to the
 * MUDLINK system.
 *
 * Commands offered:
 * rpeers
 * rwho <remotemud>
 * rtell <player>@<remotemud>
 *
 * Deepthought, 19-Jan-93
 */

#include <config.h>
#include <properties.h>

inherit "/std/thing";

create() {
  string name, foo;

  if (sizeof(old_explode(object_name(this_object()),"#")) != 2) return;

  thing::create();
  SetProp (P_SHORT,"a MUDLINK tool");
  SetProp (P_LONG,
 "This tool is used for communicating with MUDLINK. Commands are:\n"
+"rpeers                          Get a list of muds connected to MUDLINK\n"
+"rwho <mud>                      Show a list of players on the remote mud\n"
+"rtell <player>@<mud> <message>  Tell something to a player on another mud\n"
          );
  AddId ("tool");
  AddId ("mudlink");
  AddAdjective("mudlink");
  SetInfo ("Mudlink Tool V0.1 by Deepthought");
}

_query_read_msg() { return 0; }

/*-------------------------------------------------------------------------
** Add and decode our commands.
*/

init() {
  thing::init();
  add_action("rpeers","rpeers");
  add_action("rwho","rwho");
  add_action("rtell","rtell");
}

rpeers() {
  string u;
  u = geteuid(this_player());
  if (stringp(u))
    tell_object(find_player("mudlink"),"rpeers "+u+"\n");
  return 1;
}

rwho(str) {
  string u;
  if (!str || str == "") {
    write("Usage: rwho <mud>\n");
    return 1;
  }
  if (stringp(u = geteuid(this_player())))
    tell_object(find_player("mudlink"),"rwho "+u+"="+str+"\n");
  return 1;
}

rtell(str) {
  string u;
  string a, b, c, d;

  if (!str || str == "") {
    write("Usage: rtell <player>@<mud> <message>\n");
    return 1;
  }
  if (sscanf(str, "%s@%s %s", a, b, c) != 3) {
    write("Usage: rtell <player>@<mud> <message>\n");
    return 1;
  }
  if (stringp(u = geteuid(this_player())))
    tell_object(find_player("mudlink"),"rpage "+u+"@"+a+"@"+b+"="+c+"\n");
  return 1;
}

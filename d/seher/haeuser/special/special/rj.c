inherit "/std/thing";

#include <properties.h>
#include "../haus.h"

create()
{
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_SHORT,0);
  SetProp(P_NOGET, 1);
  AddId("r&j");
  AddCmd("lies", "lies");
}

static int lies(string str)
{
  int nr;

  if (!str || sscanf(str, "seite %d", nr) != 1)
    return notify_fail("Syntax: lies seite <nr>\n"), 0;

  this_player()->More(SPECIALPATH+"rom_jul/seite"+nr, 1);
  return 1;
}

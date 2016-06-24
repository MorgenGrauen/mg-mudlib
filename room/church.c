inherit "/std/room";

#include <properties.h>

create()
{
  ::create();
  SetProp(P_LIGHT,1);
  SetProp(P_INT_SHORT,"Village church");
  SetProp(P_INT_LONG,
   "You are in the local village church.\nThere is a huge pit in the center,\n" +
	 "and a door in the west wall. There is a button beside the door.\n"+
   "This church has the service of reviving ghosts. Dead people come\n"+
   "to the church and pray.\n"+
   "There is a clock on the wall.\n"+
   "There is an exit to south.\n");
  AddDetail("pit","In the middle of the church is a deep pit.\n"+
        "It was used for sacrifice in the old times, but nowadays\n" +
        "it is only left for tourists to look at.\n");
  AddDetail(({"elevator","door","button"}),
            "The elevator doesn't work any more. It must be a relict from another time,\n"+
            "a long-gone time, as this whole building.\n");
  AddDetail("clock","The clock shows this game hasn't been rebooted for centuries. Time is\n"+
                    "standing still in this strange room.\n");
  AddCmd("pray","pray");
  AddCmd("open","open");
  AddCmd("push","open");
}

open()
{
  notify_fail("The elevator doesn't work anymore, sorry.\n");
  return 0;
}

pray()
{
  write("The gods this church was built for have died an extremly long time ago.\n");
  return 1;
}


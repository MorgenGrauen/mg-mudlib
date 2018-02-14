// MorgenGrauen MUDlib
//
// gilde.c -- Standardgilde
//
// $Id: gilde.c 8388 2013-02-16 17:28:31Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/room";
inherit "/std/room/gilde";

protected void create()
{
  room::create();
  gilde::create();
}

void init()
{
  room::init();
  gilde::init();
}

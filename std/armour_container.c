// MorgenGrauen MUDlib
//,
// armour_container.c -- Ruestung, in die man was reinstecken kann
//
// $Id: armour_container.c 7804 2011-07-10 20:37:52Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/properties";
inherit "/std/thing/commands";
inherit "/std/thing/language";
inherit "/std/thing/envchk";
inherit "/std/container/light";
inherit "/std/container/restrictions";
inherit "/std/container/inventory";
inherit "/std/container/items";
inherit "/std/clothing/moving";
inherit "/std/armour/wear";
inherit "/std/armour/combat";
inherit "/std/armour/container_description";

//#define NEED_PROTOTYPES

#include <container.h>

protected void create() {
  properties::create();
  commands::create();
  light::create();
  container_description::create();
  restrictions::create();
  items::create();
  wear::create();
  combat::create();
  envchk::create();
  SetProp(P_CONTAINER,1);
  SetProp(P_PREPOSITION, "in");
  SetProp(P_SOURCE_PREPOSITION, "aus");
  SetProp(P_DEST_PREPOSITION, "in");
  AddId("Ding");
}

protected void create_super() {
  set_next_reset(-1);
}

void reset()  {
  items::reset();
}


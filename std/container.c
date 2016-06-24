// MorgenGrauen MUDlib
//
// container.c -- container standard object
//
// $Id: container.c 7804 2011-07-10 20:37:52Z Zesstra $

// The most general object class. It defines the really basic functions.
//
// This object should understand the properties short, long, info, read_msg,
// value, weight.
//
// weight is given in grams, 1 kilogram (kg) = 1000 grams (g) = 1 old weight
// unit

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/properties";
inherit "/std/thing/moving";
inherit "/std/thing/commands";
inherit "/std/thing/language";
inherit "/std/container/description";
inherit "/std/container/light";
inherit "/std/container/restrictions";
inherit "/std/container/inventory";
inherit "/std/container/items";
inherit "/std/thing/envchk";

#include <properties.h>
#include <wizlevels.h>
#include <defines.h>

protected void create()
{
  properties::create();
  commands::create();
  light::create();
  description::create();
  restrictions::create();
  items::create();
  envchk::create();
  SetProp(P_CONTAINER,1);
  SetProp(P_PREPOSITION, "in");
  SetProp(P_SOURCE_PREPOSITION, "aus");
  SetProp(P_DEST_PREPOSITION, "in");
}

protected void create_super() {
  set_next_reset(-1);
}

/*
void init() 
{
  commands::init();
  description::init();
}
*/

void reset()
{
  items::reset();
}


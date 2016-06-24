// MorgenGrauen MUDlib               
//
// tray.c -- container standard object
//
// $Id: tray.c 7804 2011-07-10 20:37:52Z Zesstra $

// The most general object class. It defines the really basic functions.
//
// This object should understand the properties short, long, info, read_msg,
// value, weight.
//
// weight is given in grams, 1 kilogram (kg) = 1000 grams (g) = 1 old weight
// unit
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/properties";
inherit "/std/thing/moving";
inherit "/std/thing/commands";
inherit "/std/thing/language";
inherit "/std/container/light";
inherit "/std/container/restrictions";
inherit "/std/tray/description";
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
  envchk::create();
  SetProp(P_TRAY,1);
  SetProp(P_PREPOSITION, "auf");
  SetProp(P_SOURCE_PREPOSITION, "von");
  SetProp(P_DEST_PREPOSITION, "auf");
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
{}

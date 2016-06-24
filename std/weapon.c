// MorgenGrauen MUDlib
//
// weapon.c -- weapon standard object
//
// $Id: weapon.c 7804 2011-07-10 20:37:52Z Zesstra $

#pragma strict_types
#pragma save_types
//#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/properties";
inherit "/std/thing/language";
inherit "/std/thing/commands";
inherit "/std/thing/restrictions";
inherit "/std/thing/light";
inherit "/std/weapon/moving";
inherit "/std/weapon/combat";
inherit "/std/weapon/description";
inherit "/std/thing/envchk";

//#define NEED_PROTOTYPES

#define <thing/properties.h>

#include <properties.h>
#include <moving.h>
#include <defines.h>

void create()
{
  seteuid(getuid());
  properties::create();
  commands::create();
  light::create();
  restrictions::create();
  combat::create();
  description::create();
  envchk::create();
  AddId(({"Ding","waffe"}));
}

/*
void init()
{
  commands::init();
  description::init();
}
*/

void reset()  // Man kann in ALLEN Standardobjekten ::reset aufrufen!
{ }


// MorgenGrauen MUDlib
//
// inpc.c -- Intelligenter NPC
//
// $Id: inpc.c 6571 2007-10-21 14:41:10Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/npc";
inherit "/std/inpc/nobank";
inherit "/std/inpc/select";
inherit "/std/inpc/boozing";
inherit "/std/inpc/items";
inherit "/std/inpc/eval";

#include <moving.h>
#include <inpc.h>
#define ME this_object()
#define ENV environment()
#define TP this_player()

#pragma strong_types

protected void create() {
  npc::create();
  items::create();
  add_select_commands();
}

protected void create_super() {
  set_next_reset(-1);
}

void reset() {
  npc::reset();
  items::reset();
}

void init() {
  npc::init();
  if (TP && query_once_interactive(TP))
    SetProp(P_INPC_LAST_PLAYER_CONTACT,time());
}

varargs int move(mixed dest, int meth, string dir, string tin, string tout) {
  int res;
  object env;

  env=ENV;
  if (!(meth&M_NOCHECK)
      && !ENV
      && QueryProp(P_INPC_WALK_MODE)
      && !may_enter_room(dest))
    return ME_CANT_TPORT_IN;
  res=::move(dest,meth,dir,tin,tout);
  if (env!=ENV)
    SetProp(P_INPC_LAST_ENVIRONMENT,ENV);
  return res;
}

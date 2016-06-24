// MorgenGrauen MUDlib
//
// tray/description.c -- standard description for containers
//
// $Id: description.c 6371 2007-07-17 22:46:50Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "std/container/description";

//#define NEED_PROTOTYPES

#include <container.h>
#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <language.h>

void create()
{ 
  ::create();
  AddId("tray");
}

varargs string long(int mode) {
  string descr, inv_descr;

  descr = process_string(QueryProp(P_LONG));
  inv_descr = make_invlist(PL, all_inventory(ME), mode );
  if ( inv_descr != "" )
    descr += "Auf " + QueryPronoun(WEM) + " liegt:\n" + inv_descr;
  return descr;
}

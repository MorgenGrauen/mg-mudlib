// MorgenGrauen MUDlib
//
// armour/description.c -- armour description handling
//
// $Id: description.c,v 1.7 2002/08/19 14:21:04 Rikus Exp $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check
#pragma warn_empty_casts

inherit "/std/thing/description";

//#define NEED_PROTOTYPES

#include <properties.h>

string dam_descr() {
  return "";
}

string short()
{   string s;
    if(!(s=process_string(QueryProp(P_SHORT))))
        return 0;
    return s+(QueryProp(P_WORN)?" (angezogen).\n":".\n");
}

varargs string long()
{   
    return (process_string(QueryProp(P_LONG)||"") + (dam_descr()||""));
}

mapping _query_material() {
  mixed res,at;

  if (mappingp(res=Query(P_MATERIAL)))
   return res;
  at=QueryProp(P_ARMOUR_TYPE);
  switch(at) {
    case AT_ARMOUR:
    case AT_HELMET:
    case AT_RING:
    case AT_AMULET:
    case AT_SHIELD:
    return ([MAT_MISC_METAL:100]);
    case AT_CLOAK:
    case AT_TROUSERS:
    return ([MAT_CLOTH:100]);
    case AT_GLOVE:
    case AT_BOOT:
    return ([MAT_LEATHER:100]);
  }
  return ([MAT_CLOTH:100]);
}


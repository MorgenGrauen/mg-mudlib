// MorgenGrauen MUDlib
//
// armour/description.c -- armour description handling
//
// $Id: description.c 6306 2007-05-20 11:32:03Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/clothing/description";

//#define NEED_PROTOTYPES

#include <properties.h>

string dam_descr()
{   string re;
    mixed desc;
    int maximum,dam,pos;

    if (!QueryProp(P_NAME) || !QueryProp(P_DAMAGED) || !QueryProp(P_SHORT) ||
        !(desc=QueryProp(P_DAM_DESC)) || (!stringp(desc) && !pointerp(desc)))
        return "";
    re = capitalize(name(WER,2))+" ";
    maximum = QueryProp(P_AC)+(dam=QueryProp(P_DAMAGED));
    if (stringp(desc))
        return (dam>(maximum/2))?(re+desc+".\n"):"";
    if (maximum==dam)
        pos=sizeof(desc)-1;
    else
        pos = (sizeof(desc)*dam/maximum);
    if (stringp(desc[pos]))
        return (re+desc[pos]+".\n");
    return "";
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
  return ([MAT_LEATHER:100]);
}

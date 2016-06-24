// MorgenGrauen MUDlib
//
// clothing/container_description.c -- clothing_container description handling
//
// $Id: container_description.c 6198 2007-02-13 23:39:43Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/description";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/description.h>
#include <thing/language.h>
#include <container.h>
#include <combat.h>
#include <thing/material.h>
#include <defines.h>
#include <wizlevels.h>
#include <player/base.h>

void create()
{
  ::create();
  SetProp(P_TRANSPARENT, 1);
  AddId("Container");
}

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
    pos = (sizeof(desc)*dam/maximum);
    if (stringp(desc[pos]))
        return (re+desc[pos]+".\n");
    return "";
}

string short()
{   string s;
    if(!(s=process_string(QueryProp(P_SHORT))))
        return 0;
    return s+(QueryProp(P_WORN)?" (angezogen).\n":".\n");
}

varargs string long(int mode)
{   string descr, inv_descr;

    descr=(process_string(QueryProp(P_LONG)||"") + (dam_descr()||""));
    if (!QueryProp(P_TRANSPARENT))
      return descr;

    inv_descr = make_invlist(PL, all_inventory(ME), mode );
    if ( inv_descr != "" )
        descr += capitalize(QueryPronoun(WER)) + " enthaelt:\n" + inv_descr;
    return descr;
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
    return ([MAT_CLOTH:100]);
  }
  return ([MAT_CLOTH:100]);
}

// flags: 1 - wizard, 2 - don't collect equal objects '
// flags: 4 - don't append infos for wizards
private void stringenize(mixed obj, int flags, mixed objs, mixed info)
{
  string id, tmp;
  int idx;
  tmp = capitalize(obj->short()||"")[0..<2]
      + (!(flags & 4) && (flags & 1) ? " ["+object_name(obj)+"]" : "");
  if(flags & 3 || living(obj)) id = object_name(obj);
  else
    id = explode(object_name(obj), "#")[0] + tmp;
  if((idx = member(objs, id)) == -1)
  {
    objs += ({ id });
    info += ({ ({ tmp, 1, obj}) });
  }
  else
    info[idx][1]++;
}

private string collect(mixed obj)
{
  if(!sizeof(obj[0])) return 0;
  return obj[0] + (obj[1] > 1 ? " ("+obj[1]+")" : "");
}

// flags: 1 - return array, 2 - don't collect equal objects '
// flags: 4 - don't append infos for wizards
varargs mixed make_invlist(object viewer, mixed inv, int flags)
{
  int iswiz;
  mixed objs, info;
  string descr;

  iswiz = IS_LEARNER( viewer ) && viewer->QueryProp(P_WANTS_TO_LEARN);
  descr = ""; objs = ({}); info = ({});
  map(inv, #'stringenize/*'*/, iswiz | (flags & 2) | (flags & 4), &objs, &info);
  if(flags & 1) return info;
  inv = map(info, #'collect/*'*/) - ({ 0 });
  if(!sizeof(inv)) return "";
  return sprintf("%"+(sizeof(inv) > 6 ? "#" : "=")+"-78s",
                 implode(inv, "\n")) + "\n";
}

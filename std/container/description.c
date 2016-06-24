// MorgenGrauen MUDlib
//
// container/description.c -- standard description for containers
//
// $Id: description.c 8755 2014-04-26 13:13:40Z Zesstra $

inherit "/std/thing/description";

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include <container.h>
#include <properties.h>
#include <defines.h>
#include <wizlevels.h>


void create()
{
  ::create();
  SetProp(P_TRANSPARENT, 1);
  AddId("Container");
}

varargs string long(int mode) {
  string descr, inv_descr;

  descr = process_string(QueryProp(P_LONG));
  if(!QueryProp(P_TRANSPARENT)) return descr;

  inv_descr = make_invlist(PL, all_inventory(ME), mode );
  if ( inv_descr != "" )
    descr += capitalize(QueryPronoun(WER)) + " enthaelt:\n" + inv_descr;
  return descr;
}

// flags: 1 - wizard, 2 - don't collect equal objects '
// flags: 4 - don't append infos for wizards
private void stringenize(mixed obj, int flags, mixed objs, mixed info)
{
  string id, tmp;
  int idx;
  
  if ( (!(flags & 4) && (flags & 1))) {
    //wenn Magier und Magierinfos angehaengt werden sollen:
    tmp = capitalize(obj->short()||"")[0..<2]
        + " ["+object_name(obj)+"]";
  }
  else if (obj->QueryProp(P_INVIS)) {
    //keine Ausgabe bzw. leerer String, wird dann von collect rausgeschmissen
    tmp="";
  }
  else {
    //Spieler, Magier ohne P_WANTS_TO_LEARN etc.
    tmp = capitalize(obj->short()||"")[0..<2];
  }
  //wenn wizard und 'dont collect equal objects': id ist object_name()
  if(flags & 3 || living(obj)) 
    id = object_name(obj);
  //sonst nehmen wir den Namen der Blueprint ->zusammenfassen der Objekte
  else
    id = BLUE_NAME(obj) + tmp;
  // Anzahl des jeweiligen Objekts (anhand von id) zaehlen.
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
  //bekommt ein Array ({name,anzahl,objekt})
  //Objekte ohne Namen (P_SHORT==0 oder P_INVIS) wegwerfen
  if(!sizeof(obj[0])) return 0;
  // Objektname + (Anzahl) zurueckgeben.
  return obj[0] + (obj[1] > 1 ? " ("+obj[1]+")" : "");
}

// flags: 1 - return array, 2 - don't collect equal objects '
// flags: 4 - don't append infos for wizards
varargs mixed make_invlist(object viewer, mixed inv, int flags)
{
  int iswiz;
  mixed objs, info;

  iswiz = IS_LEARNER( viewer ) && viewer->QueryProp(P_WANTS_TO_LEARN);
  objs = ({}); info = ({});
  map(inv, #'stringenize/*'*/, iswiz | (flags & 2) | (flags & 4), &objs, &info);
  if(flags & 1) return info;
  inv = map(info, #'collect/*'*/) - ({ 0 });
  if(!sizeof(inv)) return "";
  return sprintf("%"+(sizeof(inv) > 6 ? "#" : "=")+"-78s",
                 implode(inv, "\n")) + "\n";
}


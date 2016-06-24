// MorgenGrauen MUDlib
//
// weapon/description.c -- weapon description handling
//
// $Id: description.c 6475 2007-09-19 20:56:40Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

//#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/description.h>
#include <properties.h>
#include <combat.h>
#include <thing/material.h>

inherit "/std/thing/description";

void create()
{
  ::create();
  SetProp(P_DAM_DESC,DFLT_DAM_DESC);
}

string dam_descr()
{   string re;
    mixed desc;
    int max,dam,pos;

    if (!QueryProp(P_NAME) || !QueryProp(P_DAMAGED) || !QueryProp(P_SHORT) ||
        !(desc=QueryProp(P_DAM_DESC)) || (!stringp(desc) && !pointerp(desc)))
        return "";
    re = capitalize(name(WER,2))+" ";
    max = QueryProp(P_WC)+(dam=QueryProp(P_DAMAGED));
    // Bei reinen Parierwaffen den AC als max nehmen!
    if (QueryProp(P_PARRY)==PARRY_ONLY)
    {
	    max=QueryProp(P_AC)+dam;
    }
    if (stringp(desc))
        return (dam>(max/2))?(re+desc+".\n"):"";
    pos = (sizeof(desc)*dam/max);
    // Sonst koennen Parierwaffen, die Schrott sind, buggen
    if (pos==sizeof(desc)) pos--;
    if (stringp(desc[pos]))
        return (re+desc[pos]+".\n");
    return "";
}

string short()
{   string s;

    if (!(s=QueryProp(P_SHORT)))
        return 0;
    return s + (QueryProp(P_WIELDED)?" (gezueckt).\n":".\n");
}

varargs string long()
{   
    return (process_string(QueryProp(P_LONG)||"") + (dam_descr()||""));
}

mixed _query_size() {
  mixed res, wt;
  if (intp(res=Query(P_SIZE)) && (res>0))
    return res;
  wt=QueryProp(P_WEAPON_TYPE);
  switch (wt) {
    case WT_SWORD  : return 100;  // default: Langschwert
    case WT_AXE    : return  80;
    case WT_CLUB   : return  80;
    case WT_SPEAR  : return 180;
    case WT_KNIFE  : return  20;
    case WT_WHIP   : return 200;
    case WT_STAFF  : return 150;
  }
  return 10;  // alles andere
}

mapping _query_material() {
  mixed res,wt;

  if (mappingp(res=Query(P_MATERIAL)))
    return res;
  wt=QueryProp(P_WEAPON_TYPE);
  switch(wt) {
    case WT_SWORD:
    return ([MAT_MISC_METAL:100]);
    case WT_KNIFE:
    return ([MAT_MISC_METAL:80,MAT_MISC_WOOD:20]);
    case WT_AXE:
    return ([MAT_MISC_METAL:50,MAT_MISC_WOOD:50]);
    case WT_SPEAR:
    return ([MAT_MISC_METAL:20,MAT_MISC_WOOD:80]);
    case WT_STAFF:
    case WT_CLUB:
    return ([MAT_MISC_WOOD:100]);
  }
  return ([MAT_MISC_METAL:100]);
}

// P_BALANCED_WEAPON und P_TECHNIQUE sind mangels umgesetztem Konzept durch
// EM-Beschluss fuer obsolet erklaert worden. Zesstra. 26.06.2007


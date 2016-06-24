// MorgenGrauen MUDlib
//
// inpc/items.c -- Die richtige Ausruestung tragen
//
// $Id: items.c 6571 2007-10-21 14:41:10Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include <thing.h>
#include <inpc.h>
#include <properties.h>
#include <moving.h>

protected void create() {
  SetProp(P_ITEMS,({0}));
}

protected void create_super() {
  set_next_reset(-1);
}

void AddWeapon(string nm, string path) {
  object ob;
  mixed *it;
  
  if (!path || !nm) return;
  if (ob=clone_object(path)) {
    ob->move(this_object(),M_NOCHECK);
    command("zuecke "+nm);
  }
  it=QueryProp(P_ITEMS);
  if (!pointerp(it) || !sizeof(it)) it=({0});
  it[0]=({ob,path,nm});
  SetProp(P_ITEMS,it);
}

void AddArmour(string nm, string path) {
  object ob;
  mixed *it;
  
  if (!path || !nm) return;
  if (ob=clone_object(path)) {
    ob->move(this_object(),M_NOCHECK);
    command("zieh "+nm+" an");
  }
  it=QueryProp(P_ITEMS);
  if (!pointerp(it) || !sizeof(it)) it=({0});
  it+=({ob,path,nm});
  SetProp(P_ITEMS,it);
}

void reset() {
  mixed *it,x;
  int i;
  object ob;
  
  if (!pointerp(it=QueryProp(P_ITEMS))) return;
  for (i=sizeof(it)-1;i>=0;i--) {
    x=it[i];
    if (!pointerp(x) || sizeof(x)<3) continue;
    if (!objectp(x[0])) {
      if (ob=clone_object(x[1]))
	ob->move(this_object(),M_NOCHECK);
      x[0]=ob;
    }
    if (objectp(x[0])) {
      if (i)
	command("zieh "+x[2]+" an");
      else
	command("zuecke "+x[2]);
    }
  }
}

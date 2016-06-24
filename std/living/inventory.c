// MorgenGrauen MUDlib
//
// container/inventory.c -- Umgang mit besonderen Objekten im Inventory
//
// $Id: inventory.c 6554 2007-10-17 22:45:53Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/container/inventory";

#define NEED_PROTOTYPES

#include <properties.h>
#include <sensitive.h>
#include <attributes.h>

#define ME this_object()

public void RemoveSensitiveObject(object ob) {
  ::RemoveSensitiveObject(ob);
  RemoveSensitiveObjectFromList(ob,SENSITIVE_ATTACK);
  if (objectp(ob) && (ob->QueryProp(P_X_ATTR_MOD)   ||
                      ob->QueryProp(P_X_HEALTH_MOD) ))
  {
    deregister_modifier(ob);
    // Erst wenn das Objekt den Spieler verlassen konnte, die Attribute
    // neu berechnen.
    if (find_call_out("UpdateAttributes")==-1)
      call_out("UpdateAttributes",0); 
  }
}

public void InsertSensitiveObject(object ob, mixed arg) {
  ::InsertSensitiveObject(ob,arg);
  if (objectp(ob) && (ob->QueryProp(P_X_ATTR_MOD)   ||
                      ob->QueryProp(P_X_HEALTH_MOD) ))
  {
    register_modifier(ob);
    UpdateAttributes();
  }
}

public void CheckSensitiveAttack(int dam, mixed dam_type, mixed spell, 
                                 object enemy) {
  mixed a,x;
  int i;
  
  if (!pointerp(a=QueryProp(P_SENSITIVE_ATTACK)))
    return;
  if (!pointerp(dam_type))
    dam_type=({dam_type});
  for (i=sizeof(a)-1;i>=0;i--)
    if (pointerp(x=a[i]) &&
	dam>x[SENS_THRESHOLD] &&
	member(dam_type,x[SENS_KEY])>=0 &&
	objectp(x[SENS_OBJECT]) &&
	environment(x[SENS_OBJECT])==ME &&
	closurep(x[SENS_CLOSURE]))
      funcall(x[SENS_CLOSURE],
	      enemy,x[SENS_KEY],dam,
	      spell,x[SENS_OPT]);
}


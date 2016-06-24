// MorgenGrauen MUDlib
//
// container/inventory.c -- Umgang mit besonderen Objekten im Inventory
//
// $Id: inventory.c 6379 2007-07-20 22:32:02Z Zesstra $

#pragma strict_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include <thing.h>
#include <properties.h>
#include <sensitive.h>

#define ME this_object()

void RemoveSensitiveObjectFromList(object ob, string list) {
  mixed a,b,c;
  int i,f;

  if (!pointerp(a=QueryProp(SENS_PROP_PREFIX+list)))
    return;
  f=1;
  for (i=sizeof(a)-1;i>=0;i--)
    if (!pointerp(b=a[i]) ||
	!sizeof(b) ||
	!objectp(c=b[0]) ||
	environment(c)!=ME ||
	c==ob)
      a[i]=f=0;
  if (f)
    return;
  a-=({0});
  if (!sizeof(a))
    a=0;
  SetProp(SENS_PROP_PREFIX+list,a);
}

void RemoveSensitiveObject(object ob) {
  RemoveSensitiveObjectFromList(ob,SENSITIVE_INVENTORY);
  RemoveSensitiveObjectFromList(ob,SENSITIVE_INVENTORY_TRIGGER);
}

varargs void InsertSensitiveObjectToList(object ob, string list, string key,
					 int threshold, mixed *opt) {
  mixed a;
  int i;
  
  if (!pointerp(a=QueryProp(SENS_PROP_PREFIX+list)))
    a=({});
  for (i=sizeof(a)-1;i>=0;i--)
    if (a[i][SENS_OBJECT]==ob && a[i][SENS_KEY]==key)
      return;
  a+=({({ob,symbol_function(SENS_TRIGGER_PREFIX+list,ob),
         key,threshold,opt})});
  SetProp(SENS_PROP_PREFIX+list,a);
}

void InsertSensitiveObject(object ob, mixed arg) {
  int i;
  mixed x;
  mapping map_ldfied;
  
  if (!pointerp(arg))
    return;
  for (i=sizeof(arg)-1;i>=0;i--)
    if (pointerp(x=arg[i]) &&
	sizeof(x)>=3 &&
	stringp(x[0]) &&
	stringp(x[1]) &&
	intp(x[2])) {
      InsertSensitiveObjectToList(ob,x[0],x[1],x[2],x[3..]);
      call_other(ME,SENS_INSERT_PREFIX+x[0],ob,x[1],x[2],x[3..]);
    }
}

varargs void insert_sensitive_inv(object ob, string key,
				 int threshold, mixed *opt) {
  // Diese Funktion sucht, ob beim Hinzufuegen eines sensitiven Objekts
  // schon ein Objekt da ist, dass dieses ausloest.
  // z.B. (dynamit, feuer, 100, opt_dynamit) sorgt fuer
  // dynamit->trigger_sensitive_inv(fackel,feuer,120,opt_fackel,opt_dynamit)
  // wenn eine Fackel vorher mit Wert 120 eingetragen war.
  mixed a,x;
  int i;
  
  if (!pointerp(a=QueryProp(P_SENSITIVE_INVENTORY_TRIGGER)) ||
      !objectp(ob))
    return;
  for (i=sizeof(a)-1;i>=0;i--)
    if (pointerp(x=a[i]) &&
	x[SENS_KEY]==key &&
	x[SENS_THRESHOLD]>threshold && // Ist der Ausloeser gross genug?
	objectp(x[SENS_OBJECT]) &&
	environment(x[SENS_OBJECT])==environment(ob))
      ob->trigger_sensitive_inv(x[SENS_OBJECT],x[SENS_KEY],x[SENS_THRESHOLD],
				x[SENS_OPT],opt);
  // Zuerst Trigger-Optionen, dann Optionen des sensitiven Objekts
}

varargs void insert_sensitive_inv_trigger(object ob, string key,
					 int threshold, mixed *opt) {
  // Diese Funktion sucht, ob ein sensitives Objekt im Inventory ist,
  // das durch dieses Objekt ausgeloest wird.
  // z.B. (fackel, feuer, 120, opt_fackel) sorgt fuer
  // dynamit->trigger_sensitive_inv(fackel,feuer,120,opt_fackel,opt_dynamit)
  // wenn Dynamit mit Wert<120 eingetragen war
  mixed a,x;
  int i;
  
  if (!pointerp(a=QueryProp(P_SENSITIVE_INVENTORY)) ||
      !objectp(ob))
    return;
  for (i=sizeof(a)-1;i>=0;i--)
    if (pointerp(x=a[i]) &&
	x[SENS_KEY]==key &&
	x[SENS_THRESHOLD]<threshold && // Ausloeser gross genug?
	objectp(x[SENS_OBJECT]) &&
	environment(x[SENS_OBJECT])==environment(ob) &&
	closurep(x[SENS_CLOSURE]))
      funcall(x[SENS_CLOSURE],
	      ob,key,threshold,opt,x[SENS_OPT]);
  // Zuerst Trigger-Optionen, dann Optionen des sensitiven Objekts
}

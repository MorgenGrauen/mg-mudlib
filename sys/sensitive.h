// MorgenGrauen MUDlib
//
// sensitive.h -- Definitionen fuer sensitive Objekte.
//                Siehe auch /std/container/inventory.c und
//                /std/living/inventory.c
//
// $Id: sensitive.h,v 3.1 1997/02/12 13:04:59 Wargon Exp $

#ifndef __SENSITIVE_H__
#define __SENSITIVE_H__

#define P_SENSITIVE_INVENTORY "sensitive_inv"
#define P_SENSITIVE_INVENTORY_TRIGGER "sensitive_inv_trigger"
#define P_SENSITIVE_ATTACK "sensitive_attack"

#define SENSITIVE_INVENTORY "inv"
#define SENSITIVE_INVENTORY_TRIGGER "inv_trigger"
#define SENSITIVE_ATTACK "attack"

#define SENS_OBJECT 0
#define SENS_CLOSURE 1
#define SENS_KEY 2
#define SENS_THRESHOLD 3
#define SENS_OPT 4

#define SENS_PROP_PREFIX "sensitive_"
#define SENS_INSERT_PREFIX "insert_sensitive_"
#define SENS_TRIGGER_PREFIX "trigger_sensitive_"

#endif

#ifdef NEED_PROTOTYPES
#ifndef __SENSITIVE_H_PROTO__
#define __SENSITIVE_H_PROTO__

void RemoveSensitiveObjectFromList(object ob, string list);
void RemoveSensitiveObject(object ob);
varargs void InsertSensitiveObjectToList(object ob, string list, string key,
					 int threshold, mixed *opt);
void InsertSensitiveObject(object ob, mixed arg);
void CheckSensitiveAttack(int dam, mixed dam_type, mixed spell, object enemy);

#endif
#endif

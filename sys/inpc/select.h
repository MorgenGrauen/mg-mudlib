// MorgenGrauen MUDlib
//
// inpc/select.h -- Definitionen zur Objektauswahl
//
// $Id: select.h,v 3.3 2003/08/06 18:40:05 Vanion Exp $

#ifndef __INPC_SELECT_H__
#define __INPC_SELECT_H__

#define OT_WEAPON "weapon"
#define OT_COMBAT_OBJECT "combat_object"
#define OT_MISC "misc_objects"
#define INPC_BEST_WEAPON_ID "inpc_best_weapon"
#define INPC_BEST_SHIELD_ID "inpc_best_shield"

// if npc does not like weapons, i.e. Karateorc
#define INPC_DONT_WIELD_WEAPONS "inpc_dont_wield_weapons"

#endif

#ifdef NEED_PROTOTYPES
#ifndef __INPC_SELECT_PROTO__
#define __INPC_SELECT_PROTO__

int eval_wepon(object ob);
int eval_armour(object ob);
int eval_combat_object(object ob, mapping vals, object enemy);

object find_best_weapon(mixed from);
object find_best_armour(mixed from, mixed typ);
object *find_best_armours(mixed from);
varargs string find_best_combat_command(mixed from, object enemy, mapping pref);

varargs int wield_best_weapon(mixed from);
varargs int wear_best_armours(mixed from);
varargs int use_best_combat_command(mixed enemy, mixed from, mapping pref);


#endif
#endif

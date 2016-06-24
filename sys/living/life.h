// MorgenGrauen MUDlib
//
// living/life.h -- living life header
//
// $Id: life.h 9048 2015-01-11 18:21:32Z Zesstra $

#ifndef __LIVING_LIFE_H__
#define __LIVING_LIFE_H__

// properties

#define P_AGE                     "age"
#define P_ALIGN                   "align"

#define P_DEADS                   "deads"
#define P_GHOST                   "ghost"
#define P_FROG                    "frog"

#define P_FOOD                    "food"
#define P_MAX_FOOD                "max_food"
#define P_DRINK                   "drink"
#define P_MAX_DRINK               "max_drink"
#define P_ALCOHOL                 "alcohol"
#define P_MAX_ALCOHOL             "max_alcohol"

#define P_HP                      "hp"
#define P_MAX_HP                  "max_hp"
#define P_SP                      "sp"
#define P_MAX_SP                  "max_sp"
#define P_XP                      "xp"
#define P_NO_XP                   "no_xp"
#define P_LAST_XP                 "last_xp"

#define P_FOOD_DELAY              "food_delay"
#define P_DRINK_DELAY             "drink_delay"
#define P_ALCOHOL_DELAY           "alcohol_delay"
#define P_HP_DELAY                "hp_delay"
#define P_SP_DELAY                "sp_delay"
#define P_POISON_DELAY            "poison_delay"

#define P_POISON                  "poison"
#define P_MAX_POISON              "max_poison"

#define P_NO_REGENERATION         "no_regeneration"

#define P_ENEMY_DAMAGE            "enemy_damage"
#define P_TIMING_MAP              "timing_map"
#define P_LAST_DEATH_TIME         "last_death_time"
#define P_LAST_DEATH_PROPS        "last_death_props"
#define P_CORPSE_DECAY_TIME       "corpse_decay"

// ----- Definitionen fuer Beeinflussung der Gesundheit durch consume -----

// H_EFFECTS - Zu aendernde Properties fuer den Effekt
#define H_EFFECTS "effects"
// H_CONDITIONS - Zu pruefende Properties für den Effekt (Saettigung, ...)
#define H_CONDITIONS "conditions"
// H_DISTRIBUTION - Wie die Aenderung an HP/SP austeilen, sofort, zeitlich
// versetzt, ...
#define H_DISTRIBUTION "distribution"
// H_INGREDIENTS - Bestandteile, die die Aenderung verursachen
#define H_INGREDIENTS "ingredients"

// Verteilungen,
// 0    Sofortwirkung
// 1-50 gleichverteilte Pufferung mit angegebener Rate fuer SP/HP
// >= 51 spezielle Verteilungen (TODO)
#define HD_INSTANT      0
#define HD_STANDARD     5

#define H_ALLOWED_CONDITIONS ({P_FOOD, P_DRINK, P_ALCOHOL})
#define H_ALLOWED_EFFECTS    ({P_SP, P_HP, P_POISON})

#define HC_MAX_FOOD_REACHED        1
#define HC_MAX_DRINK_REACHED       2
#define HC_MAX_ALCOHOL_REACHED     4
#define HC_HOOK_CANCELLATION       8

// -------------------------------------------------------------------------

#endif // __LIVING_LIFE_H__

#ifdef NEED_PROTOTYPES

#ifndef __LIVING_LIFE_H_PROTO__
#define __LIVING_LIFE_H_PROTO__         

// prototypes

// ----- zentrale Funktion, um die Gesundheit des Lebewesens zu beeinflussen -----
public varargs int consume(mapping cinfo, int testonly);
// -------------------------------------------------------------------------

public int do_damage(int dam, object enemy);
public int reduce_hit_points(int dam);
public int restore_hit_points(int heal);

public int reduce_hit_point(int dam);  //Nur als Uebergansloesung (Rikus)

public void restore_spell_points(int h);
public void reduce_spell_points(int h);

public void heal_self(int h);

public varargs void die( int poisondeath, int extern );

public varargs void transfer_all_to( string|object dest, int check );

public varargs int drink_alcohol(int strength, int testonly, string mytext);
public varargs int drink_soft(int strength, int testonly, string mytext);
public varargs int eat_food(int strength, int testonly, string mytext);

public int buffer_hp(int val,int rate);
public int buffer_sp(int val,int rate);

public void show_age();

public int AddExp(int e);

// internal
protected void ResetEnemyDamage();

#endif // __LIVING_LIFE_H_PROTO__

#endif // NEED_PROTOYPES

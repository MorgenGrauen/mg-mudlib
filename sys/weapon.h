// MorgenGrauen MUDlib
//
// weapon.h -- Properties fuer Waffen (Schdenstypen siehe <combat.h>)
//
// $Id: weapon.h,v 3.8 2002/09/20 07:14:11 Tilly Exp $
 
#include "/sys/weapon/description.h"

#ifndef __WEAPON_H__
#define __WEAPON_H__

#include <combat.h>

/* ---------- Properties ---------- */

#ifndef P_NR_HANDS
#define P_NR_HANDS      "nr_hands"     // benoetigte Haende zum zuecken
#endif

#define P_WC            "wc"           // Waffenklasse (weapon class)
#define P_MAX_WC        "max_wc"       // obsolet, nicht mehr in Benutzung
#define P_WEAPON_TYPE   "weapon_type"  // Waffentyp (Schwert, Keule usw.)
#define P_DAM_TYPE      "dam_type"     // Schadensart der Waffe
#define P_WIELDED       "wielded"      // Objekt mit dem Traeger der Waffe

#define P_HIT_FUNC      "hit_func"     // Objekt das eine HitFunc definiert
#define P_WIELD_FUNC    "wield_func"   // Objekt das eine WieldFunc definiert
#define P_UNWIELD_FUNC  "unwield_func" // Objekt das eine UnwieldFunc definiert

#define P_WIELD_MSG     "wield_msg"    // Meldungen beim Zuecken
#define P_UNWIELD_MSG   "unwield_msg"  // Meldungen beim Wegstecken

// Wann der Spieler das letzte Mal eine Waffe weggesteckt hat
#define P_UNWIELD_TIME  "unwield_time"

// Mit was fuer Techniken kann die Waffe eingesetzt werden?
#define P_TECHNIQUE       "technique"

// Ist die Waffe balanciert oder nicht?
#define P_BALANCED_WEAPON "balanced_weapon"

/* ---------- Zugehoerige Defines ---------- */

// Techniken fuer P_TECHNIQUE
#define TQ_THRASH         "Schlagtechnik"
#define TQ_THRUST         "Stosstechnik"
#define TQ_STROKE         "Streichtechnik"
#define TQ_WHIP           "Peitschtechnik"

// Defines fuer balanciert/unbalanciert
#define WP_BALANCED       1
#define WP_UNBALANCED     -1

/* ---------- Parierwaffen ----------- */

#define P_PARRY         "parry"        // definiert eine Parierwaffe
#define P_PARRY_WEAPON  "parry_weapon" // Welche Parierwaffe wird benutzt?

/* -------------------- Defines fuer Parierwaffen -------------------- */

#define PARRY_NOT  0
#define PARRY_TOO  1
#define PARRY_ONLY 2

#endif // __WEAPON_H__

#ifdef NEED_PROTOTYPES

#ifndef __WEAPON_H_PROTO__
#define __WEAPON_H_PROTO__

// prototypes
// no prototypes

#endif // __WEAPON_H_PROTO__

#endif	// NEED_PROTOYPES

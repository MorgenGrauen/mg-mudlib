// MorgenGrauen MUDlib
//
// living.h -- Header fuer Lebewesen
//
// $Id: living.h 6513 2007-10-06 15:55:27Z Zesstra $

#include "/sys/living/life.h"
#include "/sys/living/attributes.h"
#include "/sys/living/put_and_get.h"
#include "/sys/living/combat.h"
#include "/sys/living/team.h"
#include "/sys/living/skills.h"
//#include "/sys/living/moving.h"

#ifndef __LIVING_H__
#define __LIVING_H__

#define INTERVAL_BETWEEN_HEALING	10
#define WEAPON_CLASS_OF_HANDS		(3)
#define ARMOUR_CLASS_OF_BARE		0
#define KILL_NEUTRAL_ALIGNMENT		10
#define ADJ_ALIGNMENT(al)		((-al - KILL_NEUTRAL_ALIGNMENT)/4)
#define MAX_LIST			20
#define NAME_OF_GHOST			"some mist"

/* Hands */
#define HAND_SHORT 0
#define HAND_WEAPON 1
#define HAND_WC 2

#endif // __LIVING_H__

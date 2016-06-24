// MorgenGrauen MUDlib
//
// health.h -- Heilstaerken und -geschwindigkeiten
//
// $Id: health.h 8368 2013-02-12 21:03:07Z Zesstra $

#ifndef _HEALTH_
#define _HEALTH_

/*
 * health.h
 *
 * A configuration file for all food and drink stuff
 */

/* These are tune macros to determine the actual effect of food, */
/* drink and alcohol */
#define ALOCHOL_VALUE(strength) strength
#define DRINK_VALUE(strength) strength
#define FOOD_VALUE(strength) strength

/* How fast alcohol wears off and drink and food are reduced */
#define ALCOHOL_DELAY 5 /* quite fast */
#define DRINK_DELAY 7 /* quite medium */
#define FOOD_DELAY 9 /* quite slow */
#define HEAL_DELAY 4 /* quite fast */

#define POISON_DELAY 4
#define POISON_MERCY_DELAY 3

#define ALC_EFFECT_HICK 0
#define ALC_EFFECT_RUELPS 1
#define ALC_EFFECT_LOOKDRUNK 2
#define ALC_EFFECT_STUMBLE 3

#define ALC_EFFECT_AREA_GUILD 0
#define ALC_EFFECT_AREA_ENV 1

/* Any regeneration prohibited ? */
#define NO_REG_HP         0x01
#define NO_REG_BUFFER_HP  0x02
#define NO_REG_SP         0x04
#define NO_REG_BUFFER_SP  0x08
#define NO_REG_ALCOHOL    0x10
#define NO_REG_DRINK      0x20
#define NO_REG_FOOD       0x40
#define NO_REG   (NO_REG_HP|NO_REG_BUFFER_HP|NO_REG_SP|NO_REG_BUFFER_SP| \
                  NO_REG_ALCOHOL|NO_REG_DRINK|NO_REG_FOOD)
                  
#endif

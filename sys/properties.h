// MorgenGrauen MUDlib
//
// properties.h -- properties header file
//
// $Id: properties.h 7169 2009-03-08 18:09:53Z Zesstra $
 
#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#ifdef NEED_PROTOTYPES
#undef NEED_PROTOTYPES		// we need only the properties here!!
#define TMP_NEED_PROTOTYPES
#endif

#include <snooping.h>
#include <thing.h>
#include <clothing.h>
#include <armour.h>
#include <container.h>
#include <living.h>
#include <npc.h>
#include <player.h>
#include <rooms.h>
#include <shells.h>
#include <weapon.h>
#include <transport.h>
#include <lightsource.h>
#include <quest_items.h>
#include <moving.h>

#ifdef TMP_NEED_PROTOTYPES
#undef TMP_NEED_PROTOTYPES
#define NEED_PROTOTYPES
#endif

#define P_ORIG_NAME      "original_name"
#define P_ORIG_FILE_NAME "original_file_name"

#define P_CAP_NAME "cap_name"
#define P_EARMUFFS "earmuffs"
#define P_VALUE_PER_UNIT "value_per_unit"
#define P_WEIGHT_PER_UNIT "weight_per_unit"
#define P_FW_UNDERSTAND "fw_understand" /* kann die sprache aus fernwest */
#define P_TRAY "tray" /* man kann was auf uns drauflegen */
#define P_AMOUNT "amount"
#define P_HP_HOOKS "hp_hooks"

#define P_INFORMME "informme"

#endif // __PROPERTIES_H__

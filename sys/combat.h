// MorgenGrauen MUDlib
//
// combat.h -- Schadenstypen, Waffen- und Ruestungsklassen und ein
//             paar Properties fuer das Kampfsystem
//
// $Id: combat.h 7178 2009-03-09 20:42:49Z Zesstra $

#ifndef _COMBAT_H_
#define _COMBAT_H_

#include <weapon.h>
#include <armour.h>
#include <ranged_weapon.h>

/* -------------------- Properties -------------------- */

/* ---------- Kampf allgemein ---------- */

#define P_ENABLE_IN_ATTACK_OUT "enable_in_attack_out"
#define P_NO_GLOBAL_ATTACK     "no_global_attack"
#define P_FRIEND               "friend"
#define P_LAST_COMBAT_TIME     "last_combat_time"

/* ---------- Befrieden ---------------- */

#define P_ACCEPT_PEACE         "accept_peace"

/* ---------- Waffen und Ruestungen ---------- */

#define P_LAST_USE             "last_object_use"
#define P_DAMAGED              "item_damaged"
#define P_EFFECTIVE_WC         "effective_wc"
#define P_EFFECTIVE_AC         "effective_ac"
#define P_QUALITY              "quality"
#define P_RESTRICTIONS         "restrictions"
#define P_EQUIP_TIME           "equip_time"

/* ---------- Artillerie ------------*/

#define P_IS_ARTILLERY         "artillery"

/* ---------- Fernkampfwaffen ---------- */

#define P_SHOOTING_WC          "shooting_wc"
#define P_AMMUNITION           "munition"
#define P_STRETCH_TIME         "stretch_time"
#define P_RANGE                "range"
#define P_SHOOTING_AREA        "shooting_area"
#define P_TARGET_AREA          "target_area"

/* ----- Defines fuer Array-Positionen ----- */

#define RW_SIZE   5
#define RW_ENEMY  0
#define RW_AMMU   1
#define RW_WRITE  2
#define RW_VICTIM 3
#define RW_SAY    4

/* -------------------- Waffentypen -------------------- */

#define WT_AMMU          "ammunition"
#define WT_AXE           "axe"
#define WT_CLUB          "club"
#define WT_HANDS         "hands"
#define WT_KNIFE         "knife"
#define WT_MAGIC         "magic"
#define WT_RANGED_WEAPON "fernwaffe"
#define WT_SPEAR         "spear"
#define WT_STAFF         "staff"
#define WT_SWORD         "sword"
#define WT_WHIP          "whip"

// Fuer alles, was sich unter den o.g. Typen nicht einordnen laesst
#define WT_MISC          "misc"

/* ---------- Liste aller erlaubten Waffentypen ---------- */

#define VALID_WEAPON_TYPE(t) (member(({\
    WT_SWORD, WT_AXE,  WT_CLUB, WT_SPEAR, WT_KNIFE, WT_AMMU, WT_MAGIC,\
    WT_STAFF, WT_WHIP,  WT_MISC, WT_RANGED_WEAPON }), t ) != -1)

/* ---------- Maximal erlaubte Waffenstaerke ---------- */

#define MAX_WEAPON_CLASS 200

#define MAX_TWOHANDED_WC MAX_WEAPON_CLASS
/* Die Einhaltung der Einhaender-Grenze wird bisher nicht garantiert */
#define MAX_ONEHANDED_WC 150

/* ---------- Mindest-Waffenstaerke, die eine Waffe
              bei einer Beschaedigung behaelt       ---------- */

#define MIN_WEAPON_CLASS 30

/* ---------- Maximal erlaubte Parierwaffenstaerke ---------- */

#define MAX_PARRY_CLASS 40

/* ---------- Auch fuer Paruerwaffen gibt 
              es eine Minimalstaerke      ---------- */

#define MIN_PARRY_CLASS 0

/* ---------- Dito fuer Ruestungen ---------- */

#define MIN_ARMOUR_CLASS 0

/* -------------------- Munition fuer Fernkampfwaffen -------------------- */

// als x bitte _immer_ den Namen der Munitionsart im Plural angeben,
// dieses ermoeglicht ein auswerten via MUN_NAME() z.b. in gilden

#define MUN_MISC(x) "Munition:"+x

// ueber dieses define und bitte _nur_ ueber dieses define koennen
// Gilden oder aehnliches den Namen einer Munitionsart ermitteln. Dieses
// ist wichtig wenn naehere Infos ueber eine Waffe ausgegeben werden sollen.
// Bsp.  "Ein Bogen wiegt 1500 Gramm und erlaubt es Dir Pfeile mit einer
//        Reichweite von 50 m zu verschiessen."
// sprintf("%s wiegt %d Gramm und erlaubt es Dir %s mit einer Reichweite "
//        +"von %d m zu verschiessen.", ob->Name(WER), ob->QueryProp(P_WEIGHT),
//        MUN_NAME(ob->QueryProp(P_AMMUNITION)), ob->QueryProp(P_RANGE))

#define MUN_NAME(x) x[9..]

#define MUN_ARROW   MUN_MISC("Pfeile")
#define MUN_STONE   MUN_MISC("Steine")
#define MUN_BOLT    MUN_MISC("Bolzen")
#define MUN_DART    MUN_MISC("kleine Blasrohrpfeile")

// Fuer die hier definierten Munitionstypen gibt es auch einen Schuss-Skill

#define VALID_SHOOT_SKILL ([\
  MUN_ARROW : "arrow",\
  MUN_STONE : "stone",\
  MUN_BOLT  : "bolt",\
  MUN_DART  : "dart" ])

/* -------------------- Schadensarten -------------------- */

/* ---------- physikalisch --------- */

// Schlagschaden
#define DT_BLUDGEON "bludgeon"

#define DT_EXPLOSION "explosion"

// Stichschaden
#define DT_PIERCE "pierce"

// reissender Schaden/Krallen
#define DT_RIP "rip"

// Schnittschaden
#define DT_SLASH "slash"

// Quetschungsschaden
#define DT_SQUEEZE "squeeze"

// peitschender Schaden
#define DT_WHIP "whip"

/* ---------- Mapping aller physikalischen Schadensarten ---------- */

#define PHYSICAL_DAMAGE_TYPES ([\
    DT_BLUDGEON  : 1,\
    DT_EXPLOSION : 1,\
    DT_PIERCE    : 1,\
    DT_RIP       : 1,\
    DT_SLASH     : 1,\
    DT_SQUEEZE   : 1,\
    DT_WHIP      : 1 ])

/* ----------- magisch ---------- */

// Saeure
#define DT_ACID "acid"

// Luft(mangel)
#define DT_AIR "air"

// Kaelte oder Eis
#define DT_COLD "cold"

// Feuer
#define DT_FIRE "fire"

// heiliger Schaden
#define DT_HOLY "holy"

// Blitz oder Elektrizitaet
#define DT_LIGHTNING "lightning"

// (allg.) Magie
#define DT_MAGIC "magic"

// Gift
#define DT_POISON "poison"

// Laerm bzw. Schall
#define DT_SOUND "sound"

// Angst
#define DT_TERROR "terror"

// satanischer Schaden
#define DT_UNHOLY "unholy"

// Wasser
#define DT_WATER "water"

/* ---------- Mapping aller magischen Schadensarten ---------- */

#define MAGICAL_DAMAGE_TYPES ([\
    DT_ACID      : 1,\
    DT_AIR       : 1,\
    DT_COLD      : 1,\
    DT_FIRE      : 1,\
    DT_HOLY      : 1,\
    DT_LIGHTNING : 1,\
    DT_MAGIC     : 1,\
    DT_POISON    : 1,\
    DT_SOUND     : 1,\
    DT_TERROR    : 1,\
    DT_UNHOLY    : 1,\
    DT_WATER     : 1 ])


/* ---------- Liste aller Schadensarten ---------- */

#define ALL_DAMAGE_TYPES   ({ DT_BLUDGEON, DT_SLASH, DT_PIERCE, DT_MAGIC, \
                              DT_FIRE, DT_COLD, DT_ACID, DT_WATER, \
                              DT_LIGHTNING, DT_AIR, DT_POISON, DT_EXPLOSION, \
                              DT_SQUEEZE, DT_SOUND, DT_RIP, DT_WHIP, \
                              DT_HOLY, DT_UNHOLY, DT_TERROR })

/* Ist x ein gueltiger Schadenstyp der Mudlib? */
#define VALID_DAMAGE_TYPE(x) (member(ALL_DAMAGE_TYPES, x) > -1)

/* -------------------- Ruestungstypen -------------------- */

#define AT_AMULET   "Amulett"
#define AT_ARMOUR   "Ruestung"
#define AT_BELT     "Guertel"
#define AT_BOOT     "Schuhe"
#define AT_CLOAK    "Umhang"
#define AT_GLOVE    "Handschuh"
#define AT_HELMET   "Helm"
#define AT_QUIVER   "Koecher"
#define AT_RING     "Ring"
#define AT_SHIELD   "Schild"
#define AT_TROUSERS "Hosen"

// Fuer alles, was sich unter den o.g. Typen nicht einordnen laesst
#define AT_MISC     "Misc"

// Ruestungen, die nicht angezogen werden koennen/duerfen
#define AT_ILLEGAL  "ILLEGAL"

/* ---------- Liste aller erlaubten Ruestungstypen ---------- */

#define VALID_ARMOUR_TYPE(t) (member(({\
    AT_ARMOUR, AT_HELMET, AT_RING, AT_GLOVE, AT_CLOAK, AT_BOOT,\
    AT_TROUSERS, AT_SHIELD, AT_AMULET, AT_MISC, AT_BELT,\
    AT_QUIVER}), t ) != -1)

/* ---------- Mapping mit den maximal erlaubten 
              Ruestungswerten der einzelnen Ruestungstypen ---------- */

#define VALID_ARMOUR_CLASS ([ \
    AT_AMULET   :  2 ,\
    AT_ARMOUR   : 50 ,\
    AT_BELT     :  2 ,\
    AT_BOOT     :  6 ,\
    AT_CLOAK    : 10 ,\
    AT_GLOVE    :  5 ,\
    AT_HELMET   : 15 ,\
    AT_QUIVER   :  0 ,\
    AT_RING     :  2 ,\
    AT_SHIELD   : 40 ,\
    AT_TROUSERS : 15 ,\
    AT_MISC     :  0 ])

/* ---------- Mapping mit den Ruestungswerten, ab denen
              die Ruestung im Laden einbehalten werden  ---------- */

#define KEEP_ARMOUR_CLASS ([ \
    AT_AMULET   :  2 ,\
    AT_ARMOUR   : 35 ,\
    AT_BELT     :  2 ,\
    AT_BOOT     :  5 ,\
    AT_CLOAK    :  7 ,\
    AT_GLOVE    :  5 ,\
    AT_HELMET   : 12 ,\
    AT_QUIVER   :  1 ,\
    AT_RING     :  2 ,\
    AT_SHIELD   : 26 ,\
    AT_TROUSERS : 12 ,\
    AT_MISC     :  1 ])

/* -------------------- Defines fuer Kampfkommandos -------------------- */

#define C_AVG "avg"
#define C_MAX "max"
#define C_MIN "min"
#define C_DTYPES "dtypes"
#define C_HEAL "heal"


/* -------------------- Defines fuer erweiterte Defendinformationen -- */
#define EINFO_DEFEND		"einfo_defend"
#define ORIGINAL_DAM		"original_dam"
#define ORIGINAL_DAMTYPE	"original_damtype"
#define CURRENT_DAM		"current_dam"
#define CURRENT_DAMTYPE		"current_damtype"
#define ORIGINAL_AINFO  	"original_ainfo"
#define ENEMY_INSERTED		"enemy_inserted"
#define RFR_REDUCE		"rfr_reduce"
#define PRESENT_DEFENDERS  	"present_defenders"
#define DEFENDING_DEFENDER	"defending_defender"
#define DEFEND_HOOK		"defend_hook"
#define DEFEND_ARMOURS		"defend_armours"
#define DEFEND_GUILD		"defend_guild"
#define DEFEND_RESI		"defend_resi"
#define DEFEND_BODY		"defend_body"
#define DEFEND_LOSTLP		"defend_lostlp"
#define DEFEND_CUR_ARMOUR_PROT	"defend_cur_armour_prot"

#define DEF_DEFENDER		0
#define DEF_DAM			1
#define DEF_DAMTYPE		2
#define DEF_SPELL		3

#define HOOK_DAM		0
#define HOOK_DAMTYPE		1
#define HOOK_SPELL		2
#define DI_NOHOOK		0
#define DI_HOOK			1
#define DI_HOOKINTERRUPT	2

#define DEF_ARMOUR_DAM		0
#define DEF_ARMOUR_PROT		1

#define GUILD_DAM		0
#define GUILD_DAMTYPE		1

#endif

/* -------------------- Prototypen-Deklarationen -------------------- */

#ifndef __COMBAT_H_PROTOTYPES__
#define __COMBAT_H_PROTOTYPES__

#ifdef NEED_PROTOTYPES

#endif // NEED_PROTOTYPES

#endif // __COMBAT_H_PROTOTYPES__


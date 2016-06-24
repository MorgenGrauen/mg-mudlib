// MorgenGrauen MUDlib
//
// living/combat.h -- combat header
//
// $Id: combat.h 9008 2015-01-06 17:20:17Z Zesstra $

#ifndef __LIVING_COMBAT_H__
#define __LIVING_COMBAT_H__

// properties
#define P_DISABLE_ATTACK          "disable_attack"
#define P_NEXT_DISABLE_ATTACK     "next_disable_attack"

#define P_CORPSE                  "corpse"
#define P_NOCORPSE                "nocorpse"

// bitte P_ARMOURS auch in /secure/master.h aendern, falls das mal geaendert
// wird.
#define P_ARMOURS                 "armours"
#define P_BODY                    "body"

#define P_RESISTANCE_STRENGTHS    "resistance_strengths"
#define P_RESISTANCE_MODIFIER     "rstr:mod"
#define P_RESISTANCE              "resistance"
#define P_VULNERABILITY           "vulnerability"
#define P_TOTAL_AC 	          "total_ac"
#define P_TOTAL_WC                "total_wc"

#define P_HANDS                   "hands"
#define P_MAX_HANDS               "max_hands"
#define P_USED_HANDS              "used_hands"
#define P_FREE_HANDS              "free_hands"
#define P_HANDS_USED_BY           "hands_used_by"

#define P_ATTACK_BUSY	          "attack_busy"
#define P_NO_ATTACK               "no_attack"

#define P_PREFERED_ENEMY          "pref_enemy"
#define P_SHOW_ATTACK_MSG         "show_attack_msg"

#define P_LAST_DAMAGE             "last_damage"
#define P_LAST_DAMTYPES           "last_damtypes"
#define P_LAST_DAMTIME            "last_damtime"

#define P_DEATH_SPONSORED_BY      "responsible_wizard_for_death"
#define P_KILLER                  "killer"
#define P_MURDER_MSG              "murder_msg"
#define P_FORCE_MURDER_MSG        "force_murder_msg"
#define P_ENEMY_DEATH_SEQUENCE    "enemy_death_sequence"
#define P_NEXT_DEATH_SEQUENCE     "p_lib_next_death_sequence"
#define P_DEATH_MSG               "death_msg"

#define P_HUNTTIME                "p_lib_hunttime"

// Befriede: Gildenzaehler
#define P_PEACE_HISTORY           "_peace_history"

#define DISABLE_TOO_EARLY 	  -100111

#define ENEMY_HUNTTIME		0

// Helfer-NPC
// a) NPC: welchem Spieler ist dieser NPC zugeordnet, d.h. fuer wen kaempft
//         er? Array: ({ spielerobject, flags })
// b) Spieler: welche NPC sind diesem Spieler zugeordnet. Mapping mit Objekten
//             und Flags.
#define P_HELPER_NPC              "std:helper_npc"
// Flags:
// 2 Klassen von Helfern momentan: Gilden-NPC und sonstige.
#define GUILD_HELPER     0x1
#define MISC_HELPER      0x2
// Wenn gesetzt, ist der Helfer exklusiv und laesst keinen anderen in der
// derselben Klasse zu.
#define EXCLUSIVE_HELPER 0x40000000
// Wenn gesetzt, ist der Helfer 'aktiv', d.h. nicht nur Schlagfaenger
#define ACTIVE_HELPER    0x20000000

// individuelle Schadensmeldungen eines Lebewesens
#define P_DAMAGE_MSG     "std_p_dam_msg"

#endif // __LIVING_COMBAT_H__

#ifdef NEED_PROTOTYPES

#ifndef __LIVING_COMBAT_H_PROTO__
#define __LIVING_COMBAT_H_PROTO__

// prototypes
public int Kill(object ob);
public void Attack2(object enemy);
public void Attack(object enemy);
public void AddDefender(object friend);
public void RemoveDefender(object friend);
public void InformDefend(object enemy);
public mixed DefendOther(int dam, string|string* dam_type, int|mapping spell, object enemy);
public int Defend(int dam, string|string* dam_type, int|mapping spell, object enemy);
public void CheckWimpyAndFlee();
public varargs void Flee(object oldenv, int force);

public varargs int StopHuntFor(object arg, int silent);
public varargs mapping StopHuntingMode(int silent);

public void UpdateResistanceStrengths();
public varargs int AddResistanceModifier(mapping mod, string add);
public varargs void RemoveResistanceModifier(string add);
public float CheckResistance(string* dam_type);

public int InsertSingleEnemy(object ob);
public int InsertEnemy(object ob);
public int IsEnemy(object wer);
public object *PresentEnemies();
public object QueryPreferedEnemy();
public varargs object SelectEnemy(object *here);
public <object*|int*>* QueryEnemies();
public mapping GetEnemies();
#if __VERSION_MAJOR__>=3 && __VERSION_MINOR__>=5 && (__VERSION_MICRO__>0 || (__VERSION_MICRO__==0 && __VERSION_PATCH__>=2))
public mapping SetEnemies(<object*|int*>* myenemies);
#else
public mapping SetEnemies(mixed *myenemies);
#endif
public mixed EnemyPresent();
public mixed InFight();
public int CheckEnemy(object ob);

public int SpellDefend(object caster, mapping sinfo);

public object|object* QueryArmourByType(string type);

public varargs int UseHands(object ob, int num);
public varargs int FreeHands(object ob);

public void InitAttack();
public void ExitAttack();

public int Pacify(object caster);

public varargs int StopHuntID(string str, int silent);
public void StopHuntText(object arg);
public varargs void ExecuteMissingAttacks(object *remove_attackers);

// Query- und Set-Methoden
static int     _query_attack_busy();
static int     _query_free_hands();
static mixed   _query_hands();
static mixed  *_query_hands_used_by();
static mapping _query_resistance_strengths();
static int     _query_total_ac();
static int     _query_total_wc();
static int     _query_used_hands();
static int     _set_attack_busy(mixed val);
static int     _set_disable_attack(int val);
static mixed   _set_hands(mixed h);
static mixed   _set_resistance(mixed arg);
static int     _set_used_hands(int new_num);
static mixed   _set_vulnerability(mixed arg);
static int     _set_wimpy(int i);

// Internal
protected void SkillResTransfer(mapping from_M, mapping to_M);
protected void InternalModifyAttack(mapping ainfo);
protected void InternalModifyDefend(int dam, string* dt, mapping spell, object enemy);
protected string mess(string msg,object me,object enemy);
 
#endif // __LIVING_COMBAT_H_PROTO__

#endif	// NEED_PROTOYPES

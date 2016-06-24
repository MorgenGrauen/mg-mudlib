// MorgenGrauen MUDlib
//
// new_skills.h -- Alle noetigen Defintionen fuer Skills (Faehigkeiten),
//                 Spells (Zaubersprueche) und Gilden.
//
// $Id: new_skills.h 8610 2014-01-06 20:59:13Z Zesstra $

#ifndef _NEW_SKILLS_H_
#define _NEW_SKILLS_H_

#include <combat.h>
#include <living/skill_attributes.h>

// ===== Aktuelle Version der Skills ====
#define CURRENT_SKILL_VERSION       2

// ===== Allgemeine Defines: =====

#define DEFAULT_GUILD               "abenteurer"
#define GUILD_DIR                   "/gilden/"
#define GUILD_SAVEFILE              "/etc/gildenmaster"
#define GUILDMASTER                 "/secure/gildenmaster"
#define SPELLBOOK_DIR               "/spellbooks/"

#define MAX_ABILITY                 10000
#define MAX_SKILLEARN               200

// ===== Moegliche Resultate eine Spells: =====

#define ERFOLG                      1
#define MISSERFOLG                  -1
#define ABGEWEHRT                   2

// ===== Properties des Gilden-Masters: =====

#define P_VALID_GUILDS              "valid_guilds"

// ===== Properties der Gilde: ======

#define P_GUILD_SKILLS              "guild_skills"
#define P_GUILD_RESTRICTIONS        "guild_rest"
//bitte dieses Define auch in /scure/master/userinfo.c aendern...
#define P_GUILD_DEFAULT_SPELLBOOK   "guild_sb"
#define P_GUILD_MALE_TITLES         "guild_male_titles"
#define P_GUILD_FEMALE_TITLES       "guild_female_titles"
#define P_GUILD_LEVELS              "guild_levels"
#define P_GUILD_PREPAREBLOCK        "guild_prepareblock"
#define P_GUILD_DEACTIVATE_SKILLS   "guild_deactivate_skills"

// ===== Properties des Spellbooks: =====

#define P_SB_SPELLS                 "sb_spells"

// ===== Properties der Gilde UND des Spellbooks: =====
#define P_GLOBAL_SKILLPROPS         "sm_global"

// ===== Properties des Spielers: =====

// #define P_GUILD                     "guild"
#define P_DEFAULT_GUILD             "default_guild"
#define P_DEFENDERS                 "defenders"
#define P_GUILD_LEVEL               "guild_level"
#define P_GUILD_RATING              "guild_rating"
#define P_GUILD_TITLE               "guild_title"
#define P_SUBGUILD_TITLE            "subguild_title"
#define P_VISIBLE_SUBGUILD_TITLE    "visible_subguild_title"
#define P_MAGIC_RESISTANCE_OFFSET   "mag_res_offset"
#define P_NEWSKILLS                 "newskills"
#define P_NEXT_SPELL_TIME           "next_spell"
#define P_PREPARED_SPELL            "prepared_spell"
#define P_TMP_ATTACK_HOOK           "attack_hook"
#define P_TMP_ATTACK_MOD            "attack_mod"
#define P_TMP_DEFEND_HOOK           "defend_hook"
#define P_TMP_DIE_HOOK              "die_hook"
#define P_TMP_MOVE_HOOK             "move_hook"
#define P_WEAPON_TEACHER            "weapon_teacher"
#define P_SKILLSVERSION             "p_lib_skillsversion"

// ===== Standard-Skills: =====

#define FIGHT(x)                    ("Fight_"+x)
#define SHOOT(x)                    ("Shoot_"+x)
#define SK_BOOZE                    "Booze"
#define SK_CARRY                    "CarrySkill"
#define SK_CASTING                  "Cast"
#define SK_DEFEND_OTHER             "DefendOther"
#define SK_FIGHT                    "Fight"
#define SK_INFORM_DEFEND            "InformDefend"
#define SK_MAGIC_ATTACK             "MagicAttack"
#define SK_MAGIC_DEFENSE            "MagicDefense"
#define SK_NIGHTVISION              "Nightvision"
#define SK_SHOOT                    "Shoot"
#define SK_SPELL_DEFEND             "SpellDefend"
#define SK_SWORDFIGHTING            FIGHT(WT_SWORD)
#define SK_TWOHANDED                "Bihand"
#define SK_WEAPONLESS               FIGHT(WT_HANDS)

// ===== Skill Infos: =====

#define FACTOR(x)                   ("fac_"+x)
#define OFFSET(x)                   ("off_"+x)

#define SI_SKILLFUNC                "si_func"
  // Funktion, die aufgerufen wird

#define SI_CLOSURE                  "si_closure"
  // Nur fuer den internen Gebrauch
  // (Geschwindigkeitsoptimierung)

#define SI_SPELLBOOK                "si_spellbook"
  // Spellbook, in dem der Spell
  // steht

#define SI_SPELLCOST                "si_cost"
  // Kosten des Spells

#define SI_TIME_MSG                 "time_msg"
  // Die Meldung wird anstelle von
  // "Du bist noch zu erschoepft
  //  von Deinem letzten Spruch."
  // ausgegeben.

#define SI_SP_LOW_MSG               "sp_low_msg"
  // Die Meldung wird anstelle von
  // "Du hast zu wenig Zauberpunkte
  //  fuer diesen Spruch."
  // ausgegeben.

#define SI_PREPARE_MSG              "si_prepare_msg"
#define SI_PREPARE_ABORT_MSG        "si_prepare_abort_msg"
#define SI_PREPARE_BUSY_MSG         "si_prepare_busy_msg"
  // Meldungen fuer Spruch-
  // vorbereitung, Abbruch und
  // schon beschaeftigt

#define SI_NOMAGIC                  "si_nomagic"
  // Prozentwert, mit dem P_NOMAGIC
  // umgangen werden kann

#define SI_NOMAGIC_MSG              "si_nomagic_msg"
  // Meldung, die Anstelle von
  // "Dein Zauberspruch verpufft
  //  im Nichts" ausgegeben werden
  // soll.

#define SI_SPELLFATIGUE             "si_fatigue"
  // Erschoepfung - Zeit, in der
  // keine weiteren Spells
  // aufgerufen werden koennen
#define SI_X_SPELLFATIGUE           "std:si:xfatigue"

#define SI_SKILLLEARN               "si_learn"
  // Lerngeschwindigkeit in 0.01%
  // pro A_INT/2

#define SI_LEARN_ATTRIBUTE          "si_learn_attribute"
  // Wenn man nicht will, dass man in Abhaengigkeit von
  // INT lernt

#define SI_NO_LEARN                 "si_no_learn"
  // Wenn man (temporaer!) nicht will, dass dieser Skill gelernt wird.
  // Muss von den Spellbooks beachtet werden.
  // Sollte niemals im Spieler abgespeichert werden. Oder permanent in
  // Gilde/Spellbook gesetzt sein. Sondern im Laufe einesr Nutzung in der jew.
  // Kopie von sinfo gesetzt werden, die gerade genutzt wird.

#define SI_SKILLABILITY             "si_abil"
  // Faehigkeit, diesen Spruch zu
  // benutzen

#define SI_SKILLARG                 "si_arg"
  // Argumente, die uebergeben
  // wurden

#define SI_SKILLRESTR_USE           "si_restr_u"
  // Beschraenkungen beim Gebrauch

#define SI_SKILLRESTR_LEARN         "si_restr_l"
  // Beschraenkungen beim Lernen

#define SI_SKILLINFO                "si_info"
  // Kurzer Informationstext

#define SI_SKILLINFO_LONG           "si_info_l"
  // Langer Informationstext

#define SI_SKILLDAMAGE              "si_damage"
  // Schaden

#define SI_SKILLDAMAGE_BY_ROW       "si_damage_by_row"
  // Schaden durch Teamreihen

#define SI_SKILLDAMAGE_TYPE         "si_damtyp"
  // Art des Schadens

#define SI_SKILLDAMAGE_MSG          "si_dammsg"
  // Meldung die anstelle einer
  // Waffe kommen soll

#define SI_SKILLDAMAGE_MSG2         "si_dammsg2"
  // dito fuer den Text fuer
  // andere im Raum

#define SI_SPELL                    "si_spell"
  // Spell, mit dem angegriffen
  // wurde

#define SI_COLLATERAL_DAMAGE        "si_collateral_damage"
  // Schadensanteil bei Flaechen-
  // spells, den Freunde abbekommen,
  // in Prozent

#define SI_NUMBER_ENEMIES           "si_num_enemies"
  // Anzahl Feinde bei
  // Flaechenspruch

#define SI_NUMBER_FRIENDS           "si_num_friends"
  // Anzahl Freunde bei
  // Flaechenspruch

#define SI_DISTANCE                 "si_distance"
  // Entfernung

#define SI_WIDTH                    "si_width"
  // Breite des Flaechenangriffs

#define SI_DEPTH                    "si_depth"
  // Tiefe des Flaechenangriffs

#define SI_INHERIT                  "si_inherit"
  // Skill, von dem etwas
  // uebernommen werden soll

#define SI_DIFFICULTY               "si_difficulty"
  // Wert, der die Obergrenze der
  // Faehigkeit abgrenzt

#define SI_LASTLIGHT                "si_lastlight"
  // Fuer Nachtsicht: Wann hat
  // der Spieler zum letzten mal
  // Licht gesehen.

#define SI_SKILLHEAL                "si_heal"
  // Heilung

#define SI_USR                      "si_usr"
  // selbst definierte Infos

#define SI_TESTFLAG                 "si_testflag"
  // Testflag

#define SI_GUILD                    "si_guild"
  // Gilde, falls Auswahl aus
  // mehreren moeglich

#define SI_ENEMY                    "si_enemy"
  // Feind bei Kampfspruechen

#define SI_FRIEND                   "si_friend"
  // Der zu verteidigende Freund
  // bei DefendOther und
  // InformDefend

#define SI_MAGIC_TYPE               "si_magic_type"
  // Von was fuer einer Art ist
  // die Magie (s.u.)

#define SI_PREPARE_TIME             "si_prepare_time"
  // Zeit die zur Vorbereitung
  // benoetigt wird.

#define SI_ATTACK_BUSY_MSG          "si_attack_busy_msg"
  // Meldung, wenn der Spieler zu
  // beschaeftigt ist

#define SI_NO_ATTACK_BUSY           "si_no_attack_busy"
  // Wenn der Spell nicht als
  // Taetigkeit zaehlen/gezaehlt
  // werden soll, kann man hier
  // NO_ATTACK_BUSY[_SET|_QUERY]
  // (s.u.) setzen

#define SI_ATTACK_BUSY_AMOUNT       "si_attack_busy_amount"
  // Der Spell hat ein gewisses
  // AttackBusy, aber das weicht
  // vom Standardwert 1 ab. Der
  // Wert kann hier gesetzt
  // werden.

#define SI_LAST_USE                 "si_last_use"
  // Letzte benutzung eines
  // Skills (fuer Rassenskills)

#define SI_LEARN_PROB               "si_learn_prob"
  // Lernwahrscheinlichkeit
  // (fuer Rassenskills)

#define SI_SKILLDURATION            "si_duration"
  // Wie soll wird der Skill anhalten?
  // (fuer Spells mit einer Dauer)

// ===== Parameter fuer Spell-Mapping: =====

#define SP_NAME                     "sp_name"
  // Name des Spells

#define SP_SHOW_DAMAGE              "sp_showdamage"
  // Treffermeldung soll gezeigt
  // werden.

#define SP_REDUCE_ARMOUR            "sp_reduce_ac"
  // AC soll Typabhaengig
  // reduziert werden.

#define SP_PHYSICAL_ATTACK          "sp_physical"
  // Koerperlicher Angriff

#define SP_RECURSIVE                "sp_recursive"
  // Rekursionen

#define SP_NO_ENEMY                 "no_enemy"
  // Ist kein richtiger Angriff

#define SP_NO_ACTIVE_DEFENSE        "sp_no_active_defense"
  // Es ist keine aktive Abwehr erwuenscht

#define SP_GLOBAL_ATTACK            "sp_global_attack"
  // Ist Flaechenangriff

// ===== Skill Restrictions: =====

#define SR_FUN                      "restr_fun"
  // Funktion, die weitere
  // Einschraenkungen prueft

#define SR_EXCLUDE_RACE             "restr_race_ex"
  // Ausgeschlossene Rassen

#define SR_INCLUDE_RACE             "restr_race_in"
  // Eingeschlossene Rassen

#define SR_EXCLUDE_GUILD            "restr_guild_ex"
  // Ausgeschlossene Gilden

#define SR_INCLUDE_GUILD            "restr_guild_in"
  // Eingeschlossene Gilden

#define SR_GOOD                     "restr_good"
  // Align <

#define SR_BAD                      "restr_bad"
  // Align >

#define SR_FREE_HANDS               "restr_hands"
  // Benoetigte freie Haende

#define SR_SEER                     "restr_seer"
  // Muss Seher sein

#define SR_MIN_SIZE                 "min_size"
  // Mindestgroesse

#define SR_MAX_SIZE                 "max_size"
  // Maximalgroesse

#define SR_PROP                     "restr_properties"
  // Werte beliebiger Properties

#define SR_QUEST                    "restr_quests"
  // Quest bestanden

#define SR_MINIQUEST                "restr_miniquests"
  // Miniquest geloest

// ===== Skill Modifier: =====

#define SM_RACE                     "modify_race"
  // Rassenspezifische
  // Besonderheiten

// ===== Fuer das Attack-Busy-Handling: =====

#define NO_ATTACK_BUSY_SET          0x001
#define NO_ATTACK_BUSY_QUERY        0x002
#define NO_ATTACK_BUSY              0x003

// ===== Magie-Arten: =====

#define MT_ANGRIFF                  "mt_angriff"
#define MT_BEHERRSCHUNG             "mt_beherrschung"
#define MT_BESCHWOERUNG             "mt_beschwoerung"
#define MT_BEWEGUNG                 "mt_bewegung"
#define MT_CREATION                 "mt_creation"
#define MT_HEILUNG                  "mt_heilung"
#define MT_HELLSICHT                "mt_hellsicht"
#define MT_ILLUSION                 "mt_illusion"
#define MT_PSYCHO                   "mt_psycho"
#define MT_SAKRAL                   "mt_sakral"
#define MT_SCHUTZ                   "mt_schutz"
#define MT_VERWANDLUNG              "mt_verwandlung"

#define MT_MISC                     "mt_misc"

// ===== Defines fuer FindGroup() =====

#define FG_ENEMIES                  -1
#define FG_FRIENDS                  1
#define FG_ALL                      0

// ======== OBSOLETE Properties / Defines =============
// Prop wird nicht mehr genutzt, ist aber noch fuer /std/player/base.c noetig,
// um den SAVE-Status in den Spieler-Props zu loeschen.
#define P_GUILD_PREVENTS_RACESKILL  "guild_prevents_raceskill"

#endif

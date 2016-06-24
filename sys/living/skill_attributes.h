// MorgenGrauen MUDlib
//
// FileName.c -- Beschreibung
//
// $Id$

#ifndef __SKILL_ATTRIBUTES_H__
#define __SKILL_ATTRIBUTES_H__

// ===== Properties fuer Skill-Attribute: =====

#define P_SKILL_ATTRIBUTES          "skill_attr"
#define P_SKILL_ATTRIBUTE_OFFSETS   "skill_attr_offsets"

// ===== Skill Attribute: =====  
// Allgemeine Qualitaet
#define SA_QUALITY                  "sa_qual"
  
// Schaden
#define SA_DAMAGE                   "sa_dam"
  
// Geschwindigkeit
#define SA_SPEED                    "sa_speed"

// Dauer
#define SA_DURATION                 "sa_dur"

// Ausdehnung
#define SA_EXTENSION                "sa_ext"

// Reichweite
#define SA_RANGE                    "sa_range"

// Je hoeher, desto groesser die Chance, dass nen vom Lebewesen gecasteter
// Spell durch das gegnerische P_NOMAGIC durchschlaegt.
// (SA_ENEMY_SAVE obsolet!)
#define SA_ENEMY_SAVE               "sa_save"
#define SA_SPELL_PENETRATION        "sa_save"

// welche Skill-Attribute gibt es?
#define VALID_SKILL_ATTRIBUTES ({ SA_QUALITY, SA_DAMAGE, SA_SPEED,\
                                  SA_DURATION, SA_EXTENSION, SA_RANGE,\
                                  SA_ENEMY_SAVE })

// Indizes fuer die Verwaltung der Skill-Attribute
#define SAM_CACHE              0
#define SAM_STATIC             1
#define SAM_DYNAMIC            2

#define SAM_SUM                0
#define SAM_CACHE_TIMEOUT      1
#define SAM_COUNT              2

#define SAM_DURATION           0
#define SAM_VALUE              1


// einige Konstanten
#define SAM_MAX_MODS           5
#define SAM_MAX_CACHE_TIMEOUT  60

// rueckgabewerte der Verwaltungsfunktionen:
#define SA_MOD_OK              1
#define SA_MOD_REMOVED         1
#define SA_TOO_MANY_MODS       -1
#define SA_MOD_TOO_SMALL       -2
#define SA_MOD_TOO_BIG         -3
#define SA_MOD_INVALID_ATTR    -4
#define SA_MOD_INVALID_OBJECT  -5
#define SA_MOD_NOT_FOUND       -6
#define SA_MOD_INVALID_VALUE   -7

// Statistik?
#define SASTATD "/p/daemon/sastatd"

#endif // __SKILL_ATTRIBUTES_H__


#ifdef NEED_PROTOTYPES

#ifndef __SKILL_ATTRIBUTES_H_PROTO__
#define __SKILL_ATTRIBUTES_H_PROTO__

// prototypes

public int    ModifySkillAttribute(string atrname, mixed value,
                                    int duration);
public int    RemoveSkillAttributeModifier(object caster, string attrname);
public int    QuerySkillAttribute(string attr);
public varargs mapping QuerySkillAttributeModifier(object caster,
                           string *attr);

// Kompatibilitaetsfunktion mit altem Interface.
public int    ModifySkillAttributeOld(object caster, string atrname, int value,
                          int duration, mixed fun);

#endif // __SKILL_ATTRIBUTES_H_PROTO__

#endif // NEED_PROTOYPES


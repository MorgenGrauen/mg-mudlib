#ifndef __KRAEUTER_TRANKATTRIBUTE_H__
#define __KRAEUTER_TRANKATTRIBUTE_H__

#define T_CARRY                 "attr_tragen"
#define T_DAMAGE_ANIMALS        "attr_dam_animals"
#define T_DAMAGE_MAGIC          "attr_dam_magical"
#define T_DAMAGE_UNDEAD         "attr_dam_undead"
#define T_PROTECTION_ANIMALS    "attr_prot_animals"
#define T_PROTECTION_MAGIC      "attr_prot_magical"
#define T_PROTECTION_UNDEAD     "attr_prot_undead"
#define T_FROG                  "attr_defrog"
#define T_FLEE_TPORT            "attr_flee_tport"
#define T_HEAL_LP               "attr_heal_lp"
#define T_HEAL_SP               "attr_heal_sp"
#define T_HEAL_POISON           "attr_heal_poison"
#define T_HEAL_DISEASE          "attr_heal_disease"
#define T_CHANGE_DIMENSION      "attr_change_dimension"
#define T_SA_SPEED              "attr_change_sa_speed"
#define T_SA_DURATION           "attr_change_sa_duration"
#define T_SA_SPELL_PENETRATION  "attr_change_sa_spell_penetration"
#define T_EFFECT_DURATION       "attr_effect_duration"
#define T_SUPPORTER             "attr_supporter"
#define T_BLOCKING              "attr_blocking"
#define T_ABUNDANCE             "attr_abundance"
#define T_EXPIRE                "best_before"

#define T_KRAUT_EFFECTS ({ \
    T_CARRY, T_DAMAGE_ANIMALS, T_DAMAGE_MAGIC, \
    T_DAMAGE_UNDEAD, T_PROTECTION_ANIMALS, T_PROTECTION_MAGIC, \
    T_PROTECTION_UNDEAD, T_FROG, T_FLEE_TPORT, T_HEAL_LP, T_HEAL_SP, \
    T_HEAL_POISON, T_HEAL_DISEASE, T_CHANGE_DIMENSION, \
    T_SA_SPEED, T_SA_DURATION, T_SA_SPELL_PENETRATION, })

#define T_KRAUT_MAP \
  (["car": T_CARRY, \
    "da":  T_DAMAGE_ANIMALS, \
    "dm":  T_DAMAGE_MAGIC, \
    "du":  T_DAMAGE_UNDEAD, \
    "dn":  T_EFFECT_DURATION, \
    "flt": T_FLEE_TPORT, \
    "fro": T_FROG, \
    "hI":  T_HEAL_DISEASE, \
    "hP":  T_HEAL_POISON, \
    "hK":  T_HEAL_SP, \
    "hL":  T_HEAL_LP, \
    "pa":  T_PROTECTION_ANIMALS, \
    "pm":  T_PROTECTION_MAGIC, \
    "pu":  T_PROTECTION_UNDEAD, \
    "ss":  T_SA_SPEED, \
    "sp":  T_SA_SPELL_PENETRATION, \
    "sd":  T_SA_DURATION ])

// negative Effekte muesssen abs. groesser als dies sein, damit sie bei der Wirkung
// beruecksichtigt werden.
#define T_MINIMUM_THRESHOLD	150
// Dies ist der max. Effekt. Alles, was hierueber gebt, wird gedeckelt.
#define T_MAXIMUM_THRESHOLD	2000

#endif // __KRAEUTER_TRANKATTRIBUTE_H__

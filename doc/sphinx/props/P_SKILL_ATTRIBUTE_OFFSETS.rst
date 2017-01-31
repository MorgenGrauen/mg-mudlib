P_SKILL_ATTRIBUTE_OFFSETS
=========================

NAME
----
::

    P_SKILL_ATTRIBUTE_OFFSETS       "skill_attr_offsets"                        

DEFINIERT IN
------------
::

    /sys/living/skill_attributes.h

BESHREIBUNG
-----------
::

    Der Wert der Property ist ein Mapping: ([Attributname: Wert])
    In dieser Property stehen permanente Abweichungen der Skillattribute
    vom Standardwert 100.

    Zu den Moeglichen Attributwerten, siehe P_SKILL_ATTRIBUTES.

    Die Werte duerfen zwischen 10 und 1000 liegen.

BEMERKUNG
---------
::

    Diese Property sollte AUF GAR KEINEN FALL in einem Spieler gesetzt
    werden, ohne Ruecksprachen mit allerhoechsten Stellen!

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill, QuerySkillAbility
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    QuerySkillAttributeModifier, RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung, skill_info_liste
    * Properties:   P_NEWSKILLS

31.12.2013, Zesstra


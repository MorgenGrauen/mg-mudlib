P_SKILL_ATTRIBUTE_OFFSETS
=========================

NAME
----

  P_SKILL_ATTRIBUTE_OFFSETS       "skill_attr_offsets"                        

DEFINIERT IN
------------

  /sys/living/skill_attributes.h

BESHREIBUNG
-----------

  Der Wert der Property ist ein Mapping: ([Attributname: Wert])
  In dieser Property stehen permanente Abweichungen der Skillattribute
  vom Standardwert 100.

  Zu den Moeglichen Attributwerten, siehe P_SKILL_ATTRIBUTES.

  Die Werte duerfen zwischen 10 und 1000 liegen.

BEMERKUNG
---------

  Diese Property sollte AUF GAR KEINEN FALL in einem Spieler gesetzt
  werden, ohne Ruecksprachen mit allerhoechsten Stellen!

SIEHE AUCH
----------

  Skills Lernen:  :doc:`../lfun/LearnSkill`, :doc:`../lfun/ModifySkill`, :doc:`../lfun/LimitAbility`
  Nutzung:      :doc:`../lfun/UseSpell`, :doc:`../lfun/UseSkill`
  Abfragen:     :doc:`../lfun/QuerySkill`, :doc:`../lfun/QuerySkillAbility`
  Modifikation: :doc:`../lfun/ModifySkillAttribute`, :doc:`../lfun/QuerySkillAttribute`,
  :doc:`../lfun/QuerySkillAttributeModifier`, :doc:`../lfun/RemoveSkillAttributeModifier`
  Properties: :doc:`P_SKILL_ATTRIBUTES`, :doc:`P_SKILL_ATTRIBUTE_OFFSETS`
  sonstig:      spruchermuedung, :doc:`skill_info_liste`
  Properties:   :doc:`P_NEWSKILLS`

31.12.2013, Zesstra

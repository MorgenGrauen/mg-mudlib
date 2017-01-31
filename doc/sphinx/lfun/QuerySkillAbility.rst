QuerySkillAbility()
===================

FUNKTION
--------
::

    public varargs int QuerySkillAbility(string sname, string gilde)

DEFINIERT IN
------------
::

    /std/living/skills.c

    

ARGUMENTE
---------
::

    string sname        Name des abzufragenden Skill
    string gilde         Gilde, unter der der Skill gesucht werden soll

BESCHREIBUNG
------------
::

    Diese Funktion liefert einen Wert zurueck, der aussagt, wie gut das
    Lebewesen den Skill 'sname' beherrscht (Key SI_SKILLABILITY im
    Skillmapping).
    Dieser Wert kann zwischen -MAX_ABILITY und MAX_ABILITY liegen,
    normal ist 0 bis MAX_ABILITY (10000 - Skill perfektioniert).

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    QuerySkillAttributeModifier, RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung, skill_info_liste
    * Properties:   P_NEWSKILLS

5. Okt 2011 Gloinson


LearnSkill()
============

FUNKTION
--------
::

    public varargs void LearnSkill(string sname, int add, int diff)

DEFINIERT IN
------------
::

    /std/living/skills.c

ARGUMENTE
---------
::

    string sname     der zu lernende Skill
    string add       Anzahl zu lernender Skillpunkte
    int diff         Schwierigkeit

BESCHREIBUNG
------------
::

    Die Methode laesst einen interaktiven (eingeloggten) Spieler den
    Skill 'sname' um 'add' Punkte lernen.

    Dabei wird sichergestellt, dass 'add' den Wert MAX_SKILLEARN nicht
    ueberschreitet, der Skill nicht verschwindet und fuer uebergeordnete
    Skills (SI_INHERIT) dieser uebergeordnete Skill auch einen Lerneffekt
    erfaehrt.

    Wird zB von Learn (spellbook) und SpellSuccess (spellbook) gerufen.

SIEHE AUCH
----------
::

    Skills Lernen:  ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill, QuerySkillAbility
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    QuerySkillAttributeModifier, RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung
    * Properties:   P_NEWSKILLS
    Spellbook:      Learn, SpellSuccess

3. Okt 2011 Gloinson


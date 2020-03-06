P_GLOBAL_SKILLPROPS
===================

NAME
----
::

    P_GLOBAL_SKILLPROPS        "sm_global"                   

DEFINIERT IN
------------
::

    /sys/new_skills.h

BESCHREIBUNG
------------
::

    Diese Gilden- und Spellbookproperty enthaelt Eigenschaften, die
    alle Spells eines Spellbooks bzw. alle Skills und Spells einer Gilde
    haben sollen.

BEISPIELE
---------
::

    SetProp(P_GLOBAL_SKILLPROPS,
      ([SI_SKILLRESTR_USE:     ([SR_FUN: #'spellTest]),
        OFFSET(SI_SKILLLEARN): #'learnOffset,
        OFFSET(SI_SPELLCOST):  #'costOffset,
        FACTOR(SI_SPELLCOST):  #'costFactor]));

BEMERKUNG
---------

  Diese Property wird in AddSpell() ausgewertet, nicht in UseSpell(). Sie
  muss also vor dem Hinzufuegen von Spells mitels AddSpell gesetzt werden.

SIEHE AUCH
----------
::

    GObj Verwalten:   LearnSkill, LearnSpell, InitialSkillAbility
    * Properties:     P_GUILD_SKILLS
    Spellbook Lernen: Learn, SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      AddSpell, QuerySpell
    * Properties:     P_SB_SPELLS
    Skills:           P_NEWSKILLS, spruchermuedung, skill_info_liste

Letzte Aenderung: 06.03.2020, Bugfix

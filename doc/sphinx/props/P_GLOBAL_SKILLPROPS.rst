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

SIEHE AUCH
----------
::

    GObj Verwalten:   LearnSkill, LearnSpell, InitialSkillAbility
    * Properties:     P_GUILD_SKILLS
    Spellbook Lernen: Learn, SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      AddSpell, QuerySpell
    * Properties:     P_SB_SPELLS
    Skills:           P_NEWSKILLS, spruchermuedung, skill_info_liste

3. Okt 2011 Gloinson


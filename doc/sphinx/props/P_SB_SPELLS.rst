P_SB_SPELLS
===========

NAME
----
::

    P_SB_SPELLS            "sb_spells"                   

DEFINIERT IN
------------
::

    /sys/new_skills.h

BESCHREIBUNG
------------
::

    In dieser Spellbookproperty sind saemtliche Sprueche des Spellbooks
    vermerkt. Veraendert wird sie durch AddSpell().

BEMERKUNGEN
-----------
::

    Man sollte diese Property nicht per Hand veraendern, sondern die
    Funktion AddSpell() nutzen.

SIEHE AUCH
----------
::

    GObj Verwalten:   LearnSkill, LearnSpell, InitialSkillAbility
    * Properties:     P_GUILD_SKILLS
    Spellbook Lernen: Learn, SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      AddSpell, QuerySpell
    * Properties:     P_GLOBAL_SKILLPROPS
    Skills:           P_NEWSKILLS, spruchermuedung, skill_info_liste

Last modified: Wed Jan 14 19:17:06 1998 by Patryn


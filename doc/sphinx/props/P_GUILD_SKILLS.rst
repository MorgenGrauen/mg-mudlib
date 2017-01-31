P_GUILD_SKILLS
==============

NAME
----
::

    P_GUILD_SKILLS            "guild_skills"                

DEFINIERT IN
------------
::

    /sys/new_skills.h

BESCHREIBUNG
------------
::

    Diese Gildenproperty enthaelt ein Mapping mit allen Skills und
    Spells der Gilde.

BEMERKUNGEN
-----------
::

    Man sollte diese Property sollte nicht per Hand veraendern, sondern
    die Funktionen AddSkill() bzw. AddSpell() nutzen.

SIEHE AUCH
----------
::

    GObj Verwalten:   LearnSkill, LearnSpell, InitialSkillAbility
    * Sonstiges:      GuildRating
    Spellbook Lernen: Learn, SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      AddSpell, QuerySpell
    * Properties:     P_SB_SPELLS, P_GLOBAL_SKILLPROPS, P_GUILD_RATING
    Skills:           P_NEWSKILLS, spruchermuedung, skill_info_liste

3. Okt 2011 Gloinson


SkillListe()
============

FUNKTION
--------
::

    int SkillListe(int what)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    int what        Rueckgabeoption:

BESCHREIBUNG
------------
::

    Gibt eine Text mit Liste mit lernbaren Skills/Spells zurueck:

    

    Fuer 'what': 0 - alle; 1 - Spells; 2 - Skills.

    

    Zeigt Eintraege aus SI_SKILLINFO.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten
    Skills:         skill_info_liste

3. Okt 2011 Gloinson


AddSkill()
==========

FUNKTION
--------
::

    varargs int AddSkill(string sname, mapping ski)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    string sname       Name des Skills
    mapping ski        Skill-Mapping

BESCHREIBUNG
------------
::

    Fuegt den Skill zur Liste der in dieser Gilde lernbaren Skills
    hinzu. Das Mapping enthaelt Informationen, die der Gildenraum
    bzw. das Gildenobjekt zum Skill herausgeben und die das Lernen
    des Skills beeinflussen.

RUECKGABWERT
------------
::

    1 fuer Erfolg

BEISPIEL
--------
::

    AddSkill( FIGHT(WT_CLUB), ([ OFFSET(SI_SKILLLEARN) : 1 ]) );

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

3. Okt 2011 Gloinson


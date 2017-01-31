LearnSkill()
============

FUNKTION
--------
::

    int LearnSkill(string skill)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    string skill     Der zu lernende Skill

BESCHREIBUNG
------------
::

    Diese Funktion ueberprueft den Spieler auf Gildenzugehoerigkeit
    und ruft, falls kein 'skill' angegeben ist die SkillListe()
    fuer Skills auf.

    Falls ein Argument angegeben wird, wird bei dem Spieler dieser Skill
    initialisiert, sofern er die notwendigen Anforderungen erfuellt hat.

RUECKGABEWERT
-------------
::

    0 bei Fehler, 1 bei Erfolg.

BEMERKUNGEN
-----------
::

    Fuer die Lebewesen-Skills gibt es eine gleichnamige Funktion.
    Siehe dafuer LearnSkill.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

3. Okt 2011 Gloinson


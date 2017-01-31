LearnSpell()
============

FUNKTION
--------
::

    varargs int LearnSpell(string spell, object pl)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    string spell     Der zu lernende Spell
    object pl        lernender Spieler, wenn 0, wird this_player() genommen

BESCHREIBUNG
------------
::

    Diese Funktion ueberprueft den Spieler auf Gildenzugehoerigkeit
    und ruft, falls kein 'spell' angegeben ist die SkillListe()
    fuer Spells auf.

    Falls ein Argument angegeben wird, wird bei dem Spieler dieser Spell
    initialisiert, sofern er die notwendigen Anforderungen erfuellt hat.

RUECKGABEWERT
-------------
::

    0 bei Fehler, 1 bei Erfolg.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

3. Okt 2011 Gloinson


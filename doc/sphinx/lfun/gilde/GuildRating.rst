GuildRating()
=============

FUNKTION
--------
::

    int GuildRating(object pl)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    object pl          Spieler, der geratet werden soll

BESCHREIBUNG
------------
::

    Gibt das Guild-Rating des Spielers im Bereich 0-MAX_ABILITY zurueck
    und schreibt sie in P_GUILD_RATING. Wichtig fuer Levelbestimmung!

    

    Normalerweise ist das der Mittelwert der Skill-Abilities aller Skills
    und Spells, das Verhalten kann aber ueberschrieben werden.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS, P_GUILD_RATING
    Gildenfkt.:
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

10. Okt 2011 Gloinson


austreten()
===========

FUNKTION
--------
::

    varargs int austreten(int loss)

ARGUMENTE
---------
::

    int loss       Prozentsatz, um den sich die Gildenskills verschlechtern

DEFINIERT IN
------------
::

    /std/gilden_ob.c

BESCHREIBUNG
------------
::

    Austrittsfunktion der Gilde. Prueft die Restriktionen der Gilde und
    laesst this_player() ggf austreten. Das Austreten aus der Standard-
    gilde ist dabei nicht moeglich.

    Der Gildenmaster loest ggf ein EVT_GUILD_CHANGE aus. Dabei werden
    E_OBJECT, E_GUILDNAME, E_LAST_GUILDNAME entsprechend gesetzt.

    Der Gildenmaster senkt auch die Skill/Spell-Faehigkeiten um 'loss' bzw.
    normalerweise mindestens 20%.

    Durch Ueberschreiben koennen hier zB Abschiedsmeldungen gesendet werden.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:
    * Ein/Austritt: beitreten, bei_oder_aus_treten
    * Props dafuer: P_GUILD_RESTRICTIONS

3. Okt 2011 Gloinson


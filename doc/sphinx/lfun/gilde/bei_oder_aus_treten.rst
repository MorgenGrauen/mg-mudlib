bei_oder_aus_treten()
=====================

FUNKTION
--------
::

    int bei_oder_aus_treten(string str)

ARGUMENTE
---------
::

    string str       Spielerparameter

DEFINIERT IN
------------
::

    /std/gilden_ob.c

BESCHREIBUNG
------------
::

    Aus- oder Eintrittsfunktion, ruft beitreten() bzw. austreten() auf.
    Wird im Std-Gildenraum per AddCmd zur Verfuegung gestellt.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:
    * Ein/Austritt: beitreten, austreten
    * Props dafuer: P_GUILD_RESTRICTIONS

3. Okt 2011 Gloinson


UseSpell()
==========

FUNKTION
--------
::

    varargs int UseSpell(object caster, string spell, mapping sinfo)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    object caster      Spieler, der Spell nutzt
    string spell       Spell-Name
    mapping sinfo      Spell-Informationen

BESCHREIBUNG
------------
::

    Prueft, ob der Spell in der Gilde definiert ist und ruft ihn dann
    ggf in aus dem Spell-Mapping gelesenen Gilden-SI_SPELLBOOK auf.

    Wird von living/skills::UseSpell gerufen, wenn dieses die SI_CLOSURE,
    also die Funktion eines Spells sucht, fuer den kein SI_SPELLBOOK
    explizit angegeben wurde.

RUECKGABWERT
------------
::

    Rueckgabewert des Spells aus dem Spellbook oder 0.

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSpell (gilde), AddSkill, QuerySpell, QuerySkill
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

3. Okt 2011 Gloinson


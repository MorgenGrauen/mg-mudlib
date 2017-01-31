QuerySpell()
============

FUNKTION
--------
::

    mapping QuerySpell(string spell)

DEFINIERT IN
------------
::

    /std/spellbook.c

ARGUMENTE
---------
::

    string spell    Name des Spells

BESCHREIBUNG
------------
::

    Gibt das Spellmapping aus P_SB_SPELLS fuer diesen Spell zurueck.

    

    Hier enthaelt QuerySpell nur die Spellbook-Informationen.

SIEHE AUCH
----------
::

    Spellbook Lernen: Learn, SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      AddSpell
    * Angriff:        TryAttackSpell, TryDefaultAttackSpell,
                      TryGlobalAttackSpell
    * Properties:     P_GLOBAL_SKILLPROPS, P_SB_SPELLS
    Skills Lernen:    LearnSkill, ModifySkill, LimitAbility
    * Nutzung:        UseSpell, UseSkill
    * sonstig:        spruchermuedung, skill_info_liste

5. Okt 2011 Gloinson


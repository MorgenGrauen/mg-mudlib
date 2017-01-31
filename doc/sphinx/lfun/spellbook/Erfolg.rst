Erfolg()
========

FUNKTION
--------
::

    void Erfolg(object caster, string spell, mapping sinfo)

DEFINIERT IN
------------
::

    /std/spellbook.c

ARGUMENTE
---------
::

    object caster    Spell sprechender Spieler
    string spell     Spellname
    mapping sinfo    Spell-Info-Mapping mit allen Informationen

BESCHREIBUNG
------------
::

    Wird bei Erfolg eines Spells gerufen. Ruft SpellInform() am
    Environment.

SIEHE AUCH
----------
::

    Sonstiges:        SpellInform
    Spellbook Lernen: Learn, SpellSuccess, Misserfolg
    * Verwalten:      AddSpell, QuerySpell
    * Angriff:        TryAttackSpell, TryDefaultAttackSpell,
                      TryGlobalAttackSpell
    * Properties:     P_GLOBAL_SKILLPROPS, P_SB_SPELLS
    Skills Lernen:    LearnSkill, ModifySkill, LimitAbility
    * Nutzung:        UseSpell, UseSkill
    * sonstig:        spruchermuedung, skill_info_liste

5. Okt 2011 Gloinson


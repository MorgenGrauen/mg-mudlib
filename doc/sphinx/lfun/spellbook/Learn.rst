Learn()
=======

FUNKTION
--------
::

    void Learn(object caster, string spell, mapping sinfo)

DEFINIERT IN
------------
::

    /std/spellbook.c

ARGUMENTE
---------
::

    object caster   Derjenige, der den Spruch spricht.
    string spell    Der gesprochene Spell
    mapping sinfo   Mapping mit allen moeglichen Informationen zum Spell

BESCHREIBUNG
------------
::

    Diese Funktion wird von der Funktion "Misserfolg" aus dem 
    Spellbook aufgerufen. Hier lernt der Spieler den Spruch, der 
    nicht geglueckt ist.

BEMERKUNGEN
-----------
::

    Kann auch ueberschrieben werden, wenn man komplexe Lern-Aenderungen
    vornehmen will. Andere Attribute sind ueber SI_LEARN_ATTRIBUTE
    setzbar.

SIEHE AUCH
----------
::

    Spellbook Lernen: SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      AddSpell, QuerySpell
    * Angriff:        TryAttackSpell, TryDefaultAttackSpell,
                      TryGlobalAttackSpell
    * Properties:     P_GLOBAL_SKILLPROPS, P_SB_SPELLS
    Skills Lernen:    LearnSkill, ModifySkill, LimitAbility
    * Nutzung:        UseSpell, UseSkill
    * sonstig:        spruchermuedung, skill_info_liste

5. Okt 2011 Gloinson


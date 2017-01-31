Misserfolg()
============

FUNKTION
--------
::

    void Misserfolg(object caster, string spell, mapping sinfo) 

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

    Wird bei Misserfolg eines Spells im Spellbook aufgerufen und
    ruft die Lernfunktion Learn() nach einer Fehlermeldung.

    

    Kann ueberschrieben werden, um die Meldungen anzupassen.

BEISPIEL
--------
::

    // Misserfolge im Klerus mit angepassten Meldungen
    void Misserfolg(object caster, string spell, mapping sinfo) {
      switch(spell) {
        case "begrabe":
          tell_object(caster, BS(
            "Du begraebst Deine Hoffnungen, dass Du diese Anrufung jemals "
            "perfekt beherrschen wirst."));
          tell_room(environment(caster),
            caster->Name(WER)+" tritt die Leiche lustlos.\n", ({caster}));
          break;
        case "blitz":
        [...]
      }

        

      int old_abil = sinfo[SI_SKILLABILITY];
      Learn(caster, spell, sinfo);
      int new_abil = caster->QuerySkillAbility(spell);
      if (old_abil < new_abil)
        tell_object(caster, "Die Goetter schenken Dir eine Erleuchtung.\n");
      else
        tell_object(caster, "Leider lernst Du nicht aus Deinem Fehler.\n"); 
    }

SIEHE AUCH
----------
::

    Spellbook Lernen: Learn, SpellSuccess, Erfolg
    * Verwalten:      AddSpell, QuerySpell
    * Angriff:        TryAttackSpell, TryDefaultAttackSpell,
                      TryGlobalAttackSpell
    * Properties:     P_GLOBAL_SKILLPROPS, P_SB_SPELLS
    Skills Lernen:    LearnSkill, ModifySkill, LimitAbility
    * Nutzung:        UseSpell, UseSkill
    * sonstig:        spruchermuedung, skill_info_liste

5. Okt 2011 Gloinson


AddSpell()
==========

FUNKTION
--------
::

    varargs int AddSpell(string verb, mapping ski)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    string verb        Name des Spells
    mapping ski        Skill-Mapping

BESCHREIBUNG
------------
::

    Fuegt den Spell zur Liste der in dieser Gilde lernbaren Spells
    hinzu. Das Mapping enthaelt Informationen, die der Gildenraum
    bzw. das Gildenobjekt zum Spell herausgeben und die das Lernen
    des Spells beeinflussen.

RUECKGABWERT
------------
::

    1 fuer Erfolg

BEMERKUNGEN
-----------
::

    Siehe das Verhalten von QuerySpell (gilde) zum Zusammenfuegen
    der AddSpell-Informationen aus Gilde und Spellbook. Relevant
    zB fuer Lernrestriktionen.

BEISPIEL
--------
::

    AddSpell("entfluche",
      ([SI_SKILLRESTR_LEARN :
        ([P_GUILD_LEVEL: LVL_WANDER,
          SR_FUN:        #'glaubensTest]),
        SI_DIFFICULTY: 100,
        SI_SKILLINFO:  "Wanderprediger ab Stufe 7",
        SI_SKILLINFO_LONG: break_string(
          "Um jemanden von einem laestigen Sprachfluch zu befreien, "
          "sollte man diese Anrufung benutzen [...]", 78),
        SI_GOD:        LEMBOLD]));

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSkill, QuerySpell, QuerySkill, QuerySkill
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

3. Okt 2011 Gloinson


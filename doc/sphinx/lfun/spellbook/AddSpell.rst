AddSpell()
==========

FUNKTION
--------
::

    varargs int AddSpell(string verb, int kosten, mixed ski) 

DEFINIERT IN
------------
::

    /std/spellbook.c

ARGUMENTE
---------
::

    string verb     Name des Spells
    int kosten      normale Kosten (Grundkosten)
    mixed ski       Skillmapping mit allen Eintraegen zum Spell

BESCHREIBUNG
------------
::

    Addiert einen Eintrag fuer den Spell im Spellbook. Wenn dieser
    Spell direkt vom Spieler (SI_SPELLBOOK) oder (normalerweise)
    ueber ein Gildenobjekt aufgerufen wird, wird das Mapping auf
    die Skillinformationen aus Spieler und Gilde addiert und
    beeinflusst damit den Aufruf der letztlichen Spellfunktion.

BEMERKUNGEN
-----------
::

    Siehe das Verhalten von QuerySpell (gilde) zum Zusammenfuegen
    der AddSpell-Informationen aus Gilde und Spellbook. Relevant
    zB fuer Lernrestriktionen.

BEISPIEL
--------
::

    AddSpell("kampfschrei", 30,
         ([SI_SKILLRESTR_LEARN:([P_LEVEL:13]),
           SI_MAGIC_TYPE: ({MT_PSYCHO}),
           SI_SPELL:      ([
             SP_NAME: "Kampfschrei",
             SP_SHOW_DAMAGE:
               ({({ -1, "Dir geschieht jedoch nichts.",
                   "@WEM1 geschieht jedoch nichts.",
                   "@WEM1 geschieht jedoch nichts." }),
                 ({  0, "Du kannst darueber aber nur lachen.",
                   "@WER1 kann darueber aber nur lachen.",
                   "@WER1 kann darueber aber nur lachen." }),
                 ({ 10, "Dir droehnen die Ohren.",
                   "@WEM1 droehnen die Ohren.",
                   "@WEM1 droehnen die Ohren." })
               })])
         ]));

SIEHE AUCH
----------
::

    Spellbook Lernen: Learn, SpellSuccess, Erfolg, Misserfolg
    * Verwalten:      QuerySpell
    * Angriff:        TryAttackSpell, TryDefaultAttackSpell,
                      TryGlobalAttackSpell
    * Properties:     P_GLOBAL_SKILLPROPS, P_SB_SPELLS
    Skills Lernen:    LearnSkill, ModifySkill, LimitAbility
    * Nutzung:        UseSpell, UseSkill
    * sonstig:        spruchermuedung, skill_info_liste

5. Okt 2011 Gloinson


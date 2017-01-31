UseSpell()
==========

FUNKTION
--------
::

    public varargs int UseSpell(string str, string spell)

DEFINIERT IN
------------
::

    /std/living/skills.c

ARGUMENTE
---------
::

    string str       Spell-Optionen
    string spell     optionaler Spellname

BESCHREIBUNG
------------
::

    Benutzt einen Spell, dessen Spellname 'spell' ggf ueber query_verb()
    ermittelt wird. Dieser Spell sollte (als kleingeschriebener
    Skill/Spell) im Living vorliegen.

    

    Die Argumente 'str' werden als SI_SKILLARG temporaer in das
    Skillmapping eingetragen (also nur fuer diesen Aufruf gueltig).

    

    Eine ausfuehrbare Spell-Funktion zum Spell wird in folgender
    Reihenfolge bestimmt:
    - eine gesetzte SI_CLOSURE nutzen
    - "UseSpell" an einem gesetzten SI_SPELLBOOK nutzen
    - "UseSpell" an der gesetzten P_GUILD nutzen
      - [UseSpell der Gilde sucht iA ebenfalls nur den Spell am Spellbook]
    - eine Closure mit Rueckgabewert 0 erstellen
    Die so bestimmte Spell-Funktion wird dann als SI_CLOSURE im Spieler
    gesetzt und ist bis zur Zerstoerung der entsprechenden Objekte gueltig.
    Die Methode wird dann gerufen (der Spell also angewandt).

    Standardmaessig gibt ein UseSpell() also 0 zurueck, es sei denn, eine
    Funktion wurde fuer den Spell an einer der oben genannten Stellen
    implementiert.

    SI_INHERIT ist fuer Spells beim Aufruf wirkungslos (gilt aber bei
    LearnSkill normal).

    Ein Durchlauf von UseSpell durch den Spieler sieht in etwa so aus:
      1) Die Methode wird als Empfaenger fuer Kommandos bekannt gemacht.
         Das passiert mit der Anweisung
         'add_action("UseSpell", "", 1);' in den Dateien player/base.c und
         in living/npc.c.

      2) /std/living/skills::UseSpell wird durch Kommando oder Heartbeat
         gerufen.

      

      3) UseSpell() ermittelt eine SI_CLOSURE oder delegiert diesen Aufruf
         an die gueltige Gilde/das Spellbook weiter.

      

      4) Eine gueltige Closure wird ausgefuehrt. UseSpell() uebergibt dabei
         die Spell/Skill-Informationen und SI_SKILLARG.

      

      4.1.) Im Normalfall einer Gilde/Spellbook landet der Aufruf ueber
            /std/gilden_ob::UseSpell() in /std/spellbook::UseSpell() und
            dieses ruft eine Spellfunktion im Spellbook auf.
            Die Spellfunktion arbeitet mit den Spell-Informationen und
            gibt ein ERFOLG, MISSERFOLG oder 0 dafuer zurueck, ob das
            Spellbook Lernen/Fehlermeldung oder nichts machen soll.
            Dementsprechend werden P_SP, P_ATTACK_BUSY und
            P_NEXT_SPELL_TIME im Spellbook geaendert.

      5.) Der Aufruf der Closure kehrt zurueck und wird zurueckgegeben.
          Damit ist der 0-Rueckgabewert der Spellfunktion im Spellbook
          aequivalent einem nicht ausgefuehrten Kommando.

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSkill
    * Abfragen:     QuerySkill, QuerySkillAbility
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    QuerySkillAttributeModifier, RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung, skill_info_liste
    * Properties:   P_NEWSKILLS
    Spellbook:      UseSpell (spellbook), Learn, SpellSuccess

5. Okt 2011 Gloinson


AddSpell()
==========

FUNKTION
--------
::

  varargs int AddSpell(int rate, int damage,
    string|<int|spell>* TextForEnemy, string|<int|string>* TextForOthers,
    string|string* dam_type, string|closure func, int|mapping spellarg)

DEFINIERT IN
------------
::

    /std/npc/combat.c

ARGUMENTE
---------
::

    rate          - Relative Haeufigkeit der Anwendung (*),
                    muss >= 0 sein
    damage        - Der Schadenswert fuer Defend(),
                    muss >= 0 sein. Bei Schaden von 0 MUSS eine Funktion
                    eingetragen werden.
    TextForEnemy  - Text, den der Feind erhalten soll
                    String <text> oder
                    Array der Form ({<text>,<messagetype>})
                    (Siehe ReceiveMsg())
    TextForOthers - Text, den andere im Raum erhalten sollen
                    String <text> oder
                    Array der Form ({<text>,<messagetype>})
                    (Siehe ReceiveMsg())
    dam_type      - Schadenstyp(en) fuer Defend(),
                    (Default: ({DT_MAGIC}) )
    func          - Funktionsname oder Closure, die nach Anwendung
                    aufgerufen werden soll
                    (Optional, bekommt als Argumente object enemy, 
                    int real_damage, string* dam_type)
    spellarg      - Spell-Argument fuer Defend(), Default ist "1"

BESCHREIBUNG
------------
::

    Ermoeglicht einfache Angriffs-Zaubersprueche fuer NPCs. Das Ausfuehren von
    Spells verursacht bei dem NPC keine KP-Kosten.

    Mit P_SPELLRATE wird die generelle Wahrscheinlichkeit der Ausfuehrung
    solcher Spells im Heartbeat angegeben, mit 'rate' kann man die relative
    Wahrscheinlichkeit der Spells untereinander steuern.

    (*) Relative Haeufigkeit heisst, dass sich alle 'rate' der Spells
    aufaddieren und ein einzelnes 'rate' dann in Relation zur Gesamtsumme
    steht. D.h. drei Spells mit 80, 10, 10 (Summe 100) haben die selben
    Aufruf-Wahrscheinlichkeiten wie drei Spells mit 120, 15, 15 oder drei
    Spells mit 160, 20, 20.

    Ein Spell wird immer in folgender Reihenfolge abgearbeitet:
     1. Die Texte werden an die Beteiligten ausgegeben.
     2. Es wird ggf. SpellDefend gerufen (wenn kein SP_PHYSICAL_ATTACK).
        Abbruch bei Schutz.
     3. Im Opfer wird Defend() mit den angegebenen Werten aufgerufen.
        Abbruch bei Tod/Zerstoerung des Opfers.
     4. Eine Funktion, so definiert, wird ausgefuehrt.

BEMERKUNGEN
-----------
::

    TextForOthers wird vor der Ausgabe der Meldung durch replace_personal()
    geschickt, d.h. es koennen Platzhalter wie @WER1, @WEMQP1 und aehnliche
    verwendet werden (siehe auch die Manpage von replace_personal()).
    Da Ersetzungen nur fuer das Gegnerobjekt beruecksichtigt werden, koennen
    nur Platzhalter mit Endziffer 1 verwendet werden. Die Ersetzungen werden
    so gesteuert, dass die eingefuegten Namen nach Satzanfaengen automatisch
    gross geschrieben werden.
    Frueher wurden statt replace_personal() die Platzhalter @WER, @WESSEN, 
    @WEM, @WEN verwendet. Diese funktionieren weiterhin, sollten aber nicht 
    mehr in neuem Code benutzt werden.

    In der von AddSpell gerufenen Methode "func" koennen speziellere
    Sachen mit dem aktuellen Feind angestellt werden koennen. Die Methode
    muss im selben Objekt definiert sein, sofern der Funktionsname und
    keine Closure uebergeben wird.

    Will man einen physischen Angriff ausloesen, MUSS <spellarg> ein Mapping
    mit ([SP_PHYSICAL_ATTACK: 1]) sein. Bei Uebergeben einer 0 oder Weglassen
    des Werts wird an Defend das Default '1' (da es Spells sind) uebergeben.

    Wenn damage<=0 oder rate<0 oder keine Meldungen uebergeben werden, wird
    der Spell NICHT eingetragen, sondern die Funktion bricht mit Rueckgabe
    von 0 ab.

BEISPIELE
---------
::

    // #1 Einfacher NPC mit drei Spells, Gesamtrate = 100, also sind die
    //    Raten direkt als Prozent Aufrufwahrscheinlichkeit lesbar.
    AddSpell(80, 400,
             "Der Hexer greift Dich mit einem kleinen Feuerball an.\n",
             "Der Hexer greift @WEN mit einem kleinen Feuerball an.\n",
             ({DT_FIRE, DT_MAGIC}));
    AddSpell(10, 800,
             "Der Hexer greift Dich mit einem riesigen Feuerball an.\n",
             "Der Hexer greift @WEN mit einem riesigen Feuerball an.\n",
             ({DT_FIRE, DT_MAGIC}));
    AddSpell(8, 100,
             "Der Hexer piekst Dir in die Augen!",
             "Der Hexer piekst @WEM in die Augen!", ({DT_PIERCE}),
             "augen_stechen");
    AddSpell(2, 5, (string)0, (string)0, (string*)0, "salto_mortalis");

    (Kleiner Feuerball mit 80% Wahrscheinlichkeit, riesiger mit 10%,
     "augen_stechen" mit 8%, "salto_mortalis" mit 2%)

    // Die Funktion "augen_stechen" kann dann so aussehen:
    void augen_stechen(object enemy, int damage, mixed dam_types ) {
      if (damage>10 && !enemy->QueryProp(P_BLIND)) {
        enemy->SetProp(P_BLIND, 1);
        if(enemy->QueryProp(P_BLIND))
          tell_object(enemy, "Du bist nun blind!\n");
      }
    }

    // Zur Funktion "salto_mortalis" gibt es keine Meldungen, dennoch
    // wird Defend mit: enemy->Defend(5, ({DT_MAGIC}), 1, this_object())
    // gerufen!
    void salto_mortalis(object enemy, int damage, mixed dam_types ) {
      // dem geneigten Leser ueberlassen, den Gegner zu toeten
    }

    // #2 Physische Angriffe: die Ruestungen sollen beruecksichtigt werden!
    //    SP_PHYSICAL_ATTACK muss in einem Mapping auf 1 gesetzt werden,
    //    damit Ruestungen physisch wirken (ansonsten werden nur ihre
    //    DefendFuncs() ausgewertet). Es muss auch eine physische Schadensart
    //    enthalten sein!
    //    SpellDefend() wird bei diesem Flag nicht mehr am Gegner gerufen.
    AddSpell(100, 200+random(200),
      "Die kleine Ratte beisst Dich!\n",
      "@WER wird von einer kleinen Ratte gebissen!\n",
      ({DT_PIERCE, DT_POISON}), (string)0,
      ([SP_PHYSICAL_ATTACK:1]));

    // #3 Selektive physische Angriffe (siehe auch man Defend_bsp):
    //    Will man erreichen, dass einige Ruestungen wirken, andere aber
    //    nicht oder nur teilweise, kann man das ueber die Spellparameter
    //    ausfuehrlich steuern:

    // erstmal fuer alle Ruestungsarten einen Schutz von 0% einstellen:
    mapping armours = map_indices(VALID_ARMOUR_CLASS, #'!);
    armours[AT_TROUSERS] = 120;  // 120% Schutz durch Hosen
    armours[AT_BOOT] = 30;       //  30% Schutz durch Stiefel

    AddSpell(20,200+random(200),
      "Die kleine Ratte beisst Dir blitzschnell in die Wade!\n",
      "@WER wird von einer kleinen Ratte in die Wade gebissen!\n",
      ({DT_PIERCE, DT_POISON}), (string)0,
      ([SP_PHYSICAL_ATTACK:1, SP_NO_ACTIVE_DEFENSE:1,
        SP_REDUCE_ARMOUR: armours]));

    // SP_NO_ACTIVE_DEFENSE = 1 schaltet aktive Abwehr (Karate/Klerus) ab
    // SP_REDUCE_ARMOUR enthaelt eine Liste von Ruestungstypen mit ihren
    // neuen Wirkungsgraden in Prozent. Nicht enthaltene Ruestungen haben
    // weiterhin 100% Schutzwirkung.

SIEHE AUCH
----------
::

     Sonstiges:  SpellAttack, SpellDefend, Defend, QueryDefend, SelectEnemy
                 replace_personal
     Properties: P_DISABLE_ATTACK, P_SPELLRATE, P_AGGRESSIVE
     Abwehr:     Defend, Defend_bsp, SpellDefend
     Methoden:   modifiers

Letzte Aenderung: 15.02.2018, Bugfix

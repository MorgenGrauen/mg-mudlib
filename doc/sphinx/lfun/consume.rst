consume()
=========

public varargs int consume(mapping cinfo, int testonly)

FUNKTION
--------
::

    public varargs int consume(mapping cinfo, int testonly);

    Aenderung der Gesundheit eines Lebewesens durch etwas Konsumierbares.

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    cinfo
      Mapping mit Informationen ueber die Gesundheitsaenderung. Folgende
      Schluessel koennen verwendet werden:
      H_EFFECTS - Mapping der zu aendernden Properties mit dem Umfang der
                  Aenderung, erlaubte Properties siehe H_ALLOWED_EFFECTS
                  (P_HP, P_SP, P_POISON).
      H_CONDITIONS - Mapping der zu pruefenden Properties mit dem Umfang der
                     Aenderung, erlaubte Properties siehe 
                     H_ALLOWED_CONDITIONS (P_FOOD, P_DRINK, P_ALCOHOL)
      H_DISTRIBUTION - Verteilung der Aenderung fuer P_HP, P_SP
                       HD_INSTANT:  sofortige Heilung
                       HD_STANDARD: 5 pro Heartbeat
                       1 - 50:      angebene Zahl pro Heartbeat
      Aenderungen koennen sowohl positiv als auch negativ sein.
    
    testonly
      Gibt an, ob nur die Bedingungen abgetestet werden sollen, oder 
      auch die Wirkung eintreten soll. 

RUECKGABEWERT
-------------
::

    1 erfolgreich konsumiert
    0 keine oder falsche Aenderungsdaten in cinfo (nicht benutzbar)
   <0 Bedingung fuer konsumieren nicht erfuellt.
      Der Rueckgabewert ist dann ein Bitset aus:
      HC_MAX_FOOD_REACHED    - Kann nichts mehr essen
      HC_MAX_DRINK_REACHED   - Kann nichts mehr trinken
      HC_MAX_ALCOHOL_REACHED - Kann nichts mehr saufen
      HC_HOOK_CANCELLATION   - durch H_HOOK_CONSUME abgebrochen
    
    Hinweis: Wenn eine der Bedingungen nicht erfuellt ist, wird gar keine
    Heilung ausgefuehrt, aber es sind stets alle Fehlercodes im
    Rueckgabewert eingetragen.

BESCHREIBUNG
------------
::

    Die Funktion stellt eine Moeglichkeit zur Verfuegung, die Aenderung
    der Gesundheit eines Lebewesens beim Konsumieren von irgendetwas (z.B in
    einer Kneipe, durch eine Heilstellte oder tragbare Tanke, ...) zentral zu
    erledigen. Sie vereint in sich die Pruefung auf Durchfuerbarkeit der
    Aenderung und Anwendung der Aenderung.

    Das erste Argument gibt die Eigenschaften der Aenderung an, der zweite 
    ob ausschliesslich die Pruefung auf Anwendbarkeit erfolgen soll.

BEMERKUNGEN
-----------
::

    Hierbei bitte beachten, dass Tanken/Entanken sowie Heilungen ggf. von
    der (Heilungs-)Balance genehmigt werden muessen!

    Historischer Code enthaelt u.U. auch eine einfache Angabe der
    betroffenen Properties als Argument enthalten. Dies in neuem Code
    nicht mehr machen, dort ein Mapping wie oben beschrieben nutzen.

    H_ALLOWED_CONDITIONS und H_ALLOWED_EFFECTS sind in /sys/living/life.h
    definiert.

BEISPIELE
---------
::

    Heilung um 100 KP, 50 LP, aber nur wenn 30 P_FOOD gegessen werden kann:

    consume( ([H_EFFECTS:    ([P_HP:50, P_SP:100]),
               H_CONDITIONS: ([P_FOOD:30]) ]) );

    Heilung um 100 KP und Vergiftung um 2, wenn 15 Alkohol getrunken werden
    koennen. Die SP werden zeitverzoegert mit 10 pro Heartbeat zugefuehrt.

    consume( ([H_EFFECTS:      ([P_SP: 100, P_POISON: 2]),
               H_CONDITIONS:   ([P_ALCOHOL: 15]),
               H_DISTRIBUTION: 10]) );

SIEHE AUCH
----------
::

     Aehnlich:  drink_alcohol, eat_food, drink_soft
     Heilung:   heal_self, restore_spell_points, restore_hit_points, 
                buffer_hp, buffer_sp
     Timing:    check_and_update_timed_key
     Enttanken: defuel_drink, defuel_food
     Props:     P_DRINK, P_FOOD, P_ALCOHOL, P_SP, P_HP,
                P_DEFUEL_TIME_DRINK
     Konzepte:  heilung, enttanken, food

LETZTE AeNDERUNG
----------------
::

    29.05.2015, Boing


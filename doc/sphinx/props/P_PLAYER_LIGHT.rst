P_PLAYER_LIGHT
==============

NAME
----
::

    P_PLAYER_LIGHT                       "player_light"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Gibt den Lichtlevel an, mit dem ein Lebewesen sieht, ein Abfragen dieser
    Property kann z.B. sinnvoll sein wenn man abfragen will ob ein Spieler
    genug Licht dabei hat um ein bestimmtes Detail untersuchen zu koennen.

    Bitte _nur_ ueber QueryProp auf diese Property zugreifen,
    da das Lichtlevel ggf. neu berechnet werden muss!

    Ein direktes setzen dieser Property ist NICHT moeglich. Es ist jedoch
    moeglich entweder eine Closure zu benutzen oder den Wert ueber einen
    P_LIGHT_MODIFIER zu veraendern.

    Um zu erreichen, das ein NPC Nachtsicht bekommt, gibt es nun 3 Varianten.
    - eine closure:
        Set(P_PLAYER_LIGHT, function int () {return 1;} , F_QUERY_METHOD) 
      dieses bedeutet, dass der NPC in jeder Dunkelheit perfekt sehen kann.
    - das setzen von einem P_LIGHT_MODIFIER
    - das benutzen des stdskills. Hierzu schreibt man in das create() des
      NPCs einfach ein: ModifySkill(SK_NIGHTVISION, 10000);

SIEHE AUCH
----------
::

    P_LIGHT_MODIFIER, P_LIGHT, P_TOTAL_LIGHT, P_INT_LIGHT, CannotSee()


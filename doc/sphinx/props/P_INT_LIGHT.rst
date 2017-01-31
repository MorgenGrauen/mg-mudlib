P_INT_LIGHT
===========

NAME
----
::

    P_INT_LIGHT                       "int_light"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Gibt den Lichtlevel an, der _in_ einem Objekt ist. Ein Abfragen dieser
    Property kann z.B. in Raeumen sinnvoll sein, wenn es z.B. um das
    aufwachen einer Eule oder einer Fledermaus geht. Zum Abfragen ob ein
    Spieler etwas sehen kann, bitte auf jeden Fall P_PLAYER_LIGHT benutzen!

    Bitte _nur_ ueber QueryProp auf diese Property zugreifen,
    da das Lichtlevel ggf. neu berechnet werden muss!

    Ein direktes setzen dieser Property ist NICHT moeglich, hierzu bitte
    P_LIGHT benutzen!

SIEHE AUCH
----------
::

    P_LIGHT, P_TOTAL_LIGHT, P_PLAYER_LIGHT, P_LIGHT_MODIFIER, CannotSee()


P_TOTAL_LIGHT
=============

NAME
----
::

    P_TOTAL_LIGHT                 "total_light"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Gibt das Lichtlevel an, das von einem Objekt ausgeht. Hierzu wird das
    eigene Lichtlevel P_LIGHT mit dem gesamten Inhalt eines Containers
    verrechnet.

    Bitte _nur_ ueber QueryProp auf diese Property zugreifen,
    da das Lichtlevel ggf. neu berechnet werden muss!

    Ein direktes setzen dieser Property ist NICHT moeglich, hierzu bitte
    P_LIGHT benutzen!

BEMERKUNGEN
-----------
::

    Das ist die VON einem Objekt ausgehende Lichtstaerke. Fuer das IN einem
    Raum herrschende Licht bitte P_INT_LIGHT abfragen!

SIEHE AUCH
----------
::

    P_LIGHT, P_INT_LIGHT, P_PLAYER_LIGHT, P_LIGHT_MODIFIER, CannotSee()


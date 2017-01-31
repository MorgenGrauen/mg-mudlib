P_LIGHT_TRANSPARENCY
====================

NAME
----
::

    P_LIGHT_TRANSPARENCY             "light_transparency"

DEFINIERT IN
------------
::

    /sys/container.h

BESCHREIBUNG
------------
::

    Wieviel Licht schluckt ein Container, d.h. wieviel Licht dringt aus
    einem Behaelter noch nach draussen. Bei Containern die _nicht_
    transparent sind, liefert eine _query_method hier immer 999 zurueck.
    Defaultmaessig steht diese Property auf 2.

SIEHE AUCH
----------
::

    P_TOTAL_LIGHT, P_INT_LIGHT, P_PLAYER_LIGHT, P_LIGHT_MODIFIER, CannotSee()


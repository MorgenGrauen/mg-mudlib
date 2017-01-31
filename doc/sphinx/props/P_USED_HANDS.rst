P_USED_HANDS
============

NAME
----
::

    P_USED_HANDS                  "used_hands"

DEFINIERT IN
------------
::

    /sys/living/combat.h

BESCHREIBUNG
------------
::

    Anzahl der Haende in Benutzung.
    Effektiv nur ein sizeof(P_HANDS_USED_BY).

BEMERKUNGEN
-----------
::

    Keine echte Property. Die Methode /std/living/combat::_query_used_hands
    stellt die Daten zusammen. Nicht setzen!

SIEHE AUCH
----------
::

    P_HANDS, P_HANDS_USED_BY
    P_MAX_HANDS, P_FREE_HANDS
    UseHands, FreeHands

1. Okt 2012, Gloinson


P_PREVENT_PILE
==============

NAME
----
::

    P_PREVENT_PILE                   "prevent_pile"

DEFINIERT IN
------------
::

    /sys/container/properties.h

BESCHREIBUNG
------------
::

    Wenn in einem Raum diese Property gesetzt ist, endet das Verwesen einer
    Leiche damit, dass ihr Inventar in dem Raum verteilt wird. Ist diese
    Property nicht gesetzt, entsteht ein "Haufen", der das Inventar
    enthaelt.

    Diese Property sollte nur in Ausnahmefaellen benutzt werden!

    Diese Property ist vor allem fuer "Store-Rooms" gedacht, in denen die
    Magier die Leiche eines Spieler ablegen und in denen nachher die
    gesammelten Sachen von einer anderen Stelle aus gesucht werden. In
    diesem Fall wuerden Spieler sonst die Moeglichkeit haben, einen Haufen
    als Ganzes zu bekommen, das soll aber *NIE* passieren.


Last modified: Tue May 15 13:56:18 CEST 2007 by Rumata


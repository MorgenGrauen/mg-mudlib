P_KNOWN_POTIONROOMS
===================

NAME
----
::

    P_KNOWN_POTIONROOMS                 "known_potionrooms"                 

DEFINIERT IN
------------
::

    /sys/player/potion.h

BESCHREIBUNG
------------
::

    Array mit den Nummern der Raeume, in denen der Spieler Zauber-
    traenke finden kann. Die Zuordnung der Raeume und Nummern
    geschieht im Potionmaster.

    Nur lesbare _query - Property.

SIEHE AUCH
----------
::

    Sonstiges: zaubertraenke, /secure/potionmaster.c, /room/orakel.c
    Verwandt:  FindPotion(), AddKnownPotion(), RemoveKnownPotion(), InList()
    Props:     P_POTIONROOMS

6.Feb 2016 Gloinson


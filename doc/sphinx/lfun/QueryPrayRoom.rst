QueryPrayRoom()
===============

FUNKTION
--------
::

     public string QueryPrayRoom()

DEFINIERT IN
------------
::

     /std/player/base.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

    Dies ist der Raum, in den der Spieler nach dem Ende der Todessequenz
    bewegt wird, d.h. ein Raum, wo er beten kann, um einen neuen Koerper zu
    erhalten.
    Der Raum wird als String angegeben (kein Objekt).

    Jede Rasse hat einen Default fuer diese Funktion, welcher mit
    SetDefaultPrayRoom() gesetzt wird. Dieser Default kann mit der Property
    P_PRAY_ROOM ueberlagert werden. Wird die Property auf 0 dgesetzt, wird
    dieser Default aktiv.

RUeCKGABEWERT
-------------
::

    Der Objektname des Betraums (string)

SIEHE AUCH
----------
::

     P_PRAY_ROOM
     SetDefaultPrayRoom


21.05.2013, Zesstra


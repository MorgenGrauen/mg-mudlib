AddKnownPotion()
================

FUNKTION
--------
::

     int AddKnownPotion(int nr)

DEFINIERT IN
------------
::

     /std/player/potion.c

ARGUMENTE
---------
::

     int nr       Nummer eines ZTs

BESCHREIBUNG
------------
::

     Addiert einen ZT als bekannt in einem Spieler. Nur vom Orakel rufbar.

RUeCKGABEWERT
-------------
::

     1  Erfolg
     -1 fehlende Berechtigung
     -2 Nummer bereits eingetragen

SIEHE AUCH
----------
::

     Sonstiges: zaubertraenke, /secure/potionmaster.c, /room/orakel.c
     Verwandt:  FindPotion(), RemoveKnownPotion(), InList()
     Props:     P_POTIONROOMS, P_KNOWN_POTIONROOMS

6.Feb 2016 Gloinson


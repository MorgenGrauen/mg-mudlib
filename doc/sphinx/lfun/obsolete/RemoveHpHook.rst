RemoveHpHook()
==============

********************* OBSOLETE LFUN ***********************************
* Diese Efun bitte nicht mehr benutzen, sondern stattdessen die       *
* Hooks (s. /doc/std/hooks).                                          *
***********************************************************************
RemoveHpHook()

FUNKTION
--------
::

     int RemoveHpHook(object ob)

DEFINIERT IN
------------
::

     /std/player/life.c

ARGUMENTE
---------
::

     ob - das Objekt, das sich austragen moechte.

BESCHREIBUNG
------------
::

     Entfernt den Eintrag fuer dieses Objekt in P_HP_HOOKS.

RUECKGABEWERT
-------------
::

     1, wenn Erfolg, 0 sonst

SIEHE AUCH
----------
::

     Gegenpart:	AddHpHook()
     Props:	P_HP_HOOKS, P_HP

23.Feb.2004 Gloinson


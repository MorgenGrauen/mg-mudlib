QueryDisguise()
===============

FUNKTION
--------
::

     mixed QueryDisguise();

DEFINIERT IN
------------
::

     ???

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Prueft, ob der Spieler durch einen Shadow (meistens der Tarnhelm) 
     'manipuliert' ist.

RUeCKGABEWERT
-------------
::

     0, wenn dies nicht der Fall ist, ansonsten die Beschreibung des Shadow
     vom Typ string.

BEMERKUNGEN
-----------
::

     In Waffen / Ruestungen u.ae. die P_RESTRICTIONS gesetzt haben,
     eruebrigt sich eine Abfrage auf QueryDisguise(), da dies bereits im
     restriction_checker erledigt wird.

SIEHE AUCH
----------
::

     P_RESTRICTIONS, /std/restriction_checker.c


Last modified: Mon Mar 26 14:48:20 2001 von Tilly


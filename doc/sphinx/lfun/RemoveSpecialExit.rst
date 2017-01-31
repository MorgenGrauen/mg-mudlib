RemoveSpecialExit()
===================

FUNKTION
--------
::

     void RemoveSpecialExit(string|string* cmd);

DEFINIERT IN
------------
::

     /std/room/exits

ARGUMENTE
---------
::

     string/string* cmd
          Richtung(en), die entfernt werden sollen.

BESCHREIBUNG
------------
::

     Die in cmd angegebenen Ausgaenge werden wieder entfernt.

     Ist cmd = 0, so werden alle Ausgaenge entfernt.

BEMERKUNGEN
-----------
::

     Ausgaenge werden an der gleichen Stelle gespeichert:
     - RemoveSpecialExit() greift auf die gleichen Daten wie RemoveExit()
       zu, entfernt also auch normale Ausgaenge!

SIEHE AUCH
----------
::

     AddExit(), AddSpecialExit(), AddGuardedExit(), GetExits()
     RemoveExit(),
     P_EXITS, P_HIDE_EXITS, /std/room/exits.c
     ausgaenge

31.01.2015, Zesstra


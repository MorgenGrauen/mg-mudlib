P_SPECIAL_EXITS
===============

NAME
----
::

    P_SPECIAL_EXITS               "special_exits"               

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Enthaelt ein Mapping (der Breite 2) mit den Ausgaengen, der Funktion,
    die jeweils gerufen wird, wenn der Ausgang benutztwird und einer
    Bewegungsmeldung.

    

    Die Bewegungsmeldung wird bei SpecialExits nicht ausgewertet, sondern
    ist nur aufgrund der gemeinsamen Datenstruktur vorhanden. Im Normalfall
    ist sie 0.

BEMERKUNG
---------
::

    Keine echte Property, wird bei der Abfrage aus P_EXITS erstellt.

    

    Zugriff nur mit den dafuer vorgesehenen Funktionen, nicht aendern!

SIEHE AUCH
----------
::

     AddExit(), AddSpecialExit(), GetExits(),
     RemoveExit(), RemoveSpecialExit(),
     GuardExit(),
     H_HOOK_EXIT_USE, P_EXITS, P_HIDE_EXITS, /std/room/exits.c
     ausgaenge

Letzte Aenderung: 22.12.2016, Bugfix


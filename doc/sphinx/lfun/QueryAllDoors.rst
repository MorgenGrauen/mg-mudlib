QueryAllDoors()
===============

FUNKTION
--------
::

     mapping QueryAllDoors();

DEFINIERT IN
------------
::

     /obj/doormaster.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Es werden die Zustaende ALLER Tueren im MorgenGrauen ermittelt.

RUECKGABEWERT
-------------
::

     Ein Mapping mit den Zustaenden aller Tueren im MorgenGrauen. Als
     Schluessel dienen die Dateinamen der Start- und Zielraeume in
     lexikographischer (alphabetischer) Reihenfolge, getrennt durch ":",
     der Wert des Keys ist der aktuelle Zustand dieser Tuer, z.B.:

    ([ "/d/inseln/schiffe/jolle:/d/inseln/schiffe/jolle/masch" : -1,
       ...
     ]);

     Es gibt also eine Tuer zwischen Jolle und Maschinenraum, die
     momenten geschlossen ist (-1 = D_STATUS_CLOSED).

SIEHE AUCH
----------
::

    NewDoor(), QueryDoorKey(), QueryDoorStatus(), SetDoorStatus(),
    /std/room/doors.c, /obj/doormaster.c, GetPhiolenInfos(), P_DOOR_INFOS


Letzte Aenderung: Don, 08.05.2014, Gabylon


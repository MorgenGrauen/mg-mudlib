QueryDoorStatus()
=================

FUNKTION
--------
::

     int QueryDoorStatus(string dest);

DEFINIERT IN
------------
::

     /obj/doormaster.c

ARGUMENTE
---------
::

     <dest> = Zielraum der Tuer.

BESCHREIBUNG
------------
::

     Es wird der Zustand der Tuer, die von diesem Raum nach <dest> fuehrt,
     ermittelt.

RUeCKGABEWERT
-------------
::

     0 bei nicht vorhandene Tuer, ansonsten einer der folgenden Zustaende (aus
       <doorroom.h>):

       D_STATUS_LOCKED - Tuer abgeschlossen
       D_STATUS_CLOSED - Tuer geschlossen
       D_STATUS_OPEN   - Tuer geoeffnet

SIEHE AUCH
----------
::

    NewDoor(), QueryDoorKey(), SetDoorStatus(), P_DOOR_INFOS,
    /std/room/doors.c, /obj/doormaster.c, GetPhiolenInfos(), QueryAllDoors()


Letzte Aenderung: Don, 08.05.2014, Gabylon


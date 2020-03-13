P_DOOR_INFOS
============

NAME
----
::

    P_DOOR_INFOS                  "door_infos"

DEFINIERT IN
------------
::

    /sys/doorroom.h

BESCHREIBUNG
------------
::

    Array mit Informationen ueber eine im Raum per NewDoor() definierte
    Tuer. Diese Infos werden ueber /std/room/doors.c an den /obj/doormaster.c
    weitergegeben und dem Raum, der die Tuer besitzt, als Property gesetzt.
    Werden mehrere Tueren in einem Raum eingebunden, enthaelt das Array
    entsprechend viele Eintraege.

    Dieses Property dient zur internen Verwaltung der Tueren im
    /obj/doormaster.c und sollte nicht per Hand veraendert werden!

    Jeder Eintrag im Array ist erneut ein Array und hat folgende Indices
    (definiert in /sys/doorroom.h):

    D_DEST : Zielraum (string)
    D_CMDS : Befehl(e), um durch die Tuer zu gehen (string oder *string)
    D_IDS  : IDs der Tuer (string oder *string)
    D_FLAGS : Besondere Eigenschaften der Tuer (Tuer braucht Schluessel etc.)
    D_LONG  : Langbeschreibung (string)
    D_SHORT : Kurzbeschreibung (string)
    D_NAME  : Name (string oder *string)
    D_GENDER  : Geschlecht
    D_FUNC    : Funktion, die VOR dem Durchschreiten der Tuer aufgerufen wird
    D_MSGS    : Bewegungsmeldungen
    D_FUNC2   : Funktion, die NACH dem Durchschreiten der Tuer aufgerufen wird
    D_TESTFUNC  : Funktion die im Sartraum testet, ob die Tuer durchschritten
                  werden darf
    D_RESET_MSG : Meldungen beim Tuer-Reset
    D_OPEN_WITH_MOVE : Falls gesetzt, wird die Tuer auch mit dem
                       Bewegungsbefehl geoeffnet und durchschritten, falls
                       Oeffnen erfolgreich

SIEHE AUCH
----------
::

    NewDoor(), QueryDoorKey(), QueryDoorStatus(), SetDoorStatus(),
    /std/room/doors.c, /obj/doormaster.c, GetPhiolenInfos(), QueryAllDoors()


Letzte Aenderung: 13.3.2020 Zesstra


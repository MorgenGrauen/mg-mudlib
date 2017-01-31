P_PRAY_ROOM
===========

NAME
----
::

    P_PRAY_ROOM                      "_lib_p_pray_room"                  

DEFINIERT IN
------------
::

    /sys/player/base.h

BESCHREIBUNG
------------
::

    Dies ist der Raum, in den der Spieler nach dem Ende der Todessequenz
    bewegt wird, d.h. ein Raum, wo er beten kann, um einen neuen Koerper zu
    erhalten.
    Der Raum muss als String angegeben werden (kein Objekt).

    Diese Property wird im Spieler rebootfest gespeichert.

    Der jeweils gueltige Betraum wird im Spieler mittels QueryPrayRoom()
    ermittelt. Jede Rasse hat einen Default fuer diese Funktion. Wird die
    Property auf 0 dgesetzt, wird dieser Default wiederhergestellt.

    Von einer Ueberlagerung mittels Query- oder gar Setmethoden wird
    abgeraten. Ebenso lasst bitte die Modusbits unveraendert.

    Vor einer Aenderung dieser Property sollte geprueft werden, ob sie 0 ist.
    Wenn nicht, sollte von einem Setzen der Property abgesehen werden, da dann
    schon jemand anders den Betraum des Spielers geaendert hat. (Gleiches gilt
    fuers Loeschen.) Bitte niemals den Inhalt der Property woanders speichern
    und spaeter zurueckschreiben.

    Eine dauerhafte Aenderung des Betraums des Spielers muss mit dem EM
    Rassen und dem EM Gilden abgestimmt werden.

SIEHE AUCH
----------
::

    QueryPrayRoom(), SetDefaultPrayRoom()

LETZTE AeNDERUNG
----------------
::

21.05.2013, Zesstra


P_IGNORE
========

NAME
----
::

    P_IGNORE                      "ignore"

DEFINIERT IN
------------
::

    /sys/player/base.h

BESCHREIBUNG
------------
::

    Array mit Spielern, Kommandos, Aktionen u.ae. die ignoriert werden.
    In aller Regel sollte die Ignorierepruefung durch Aufruf von TestIgnore()
    im Spieler erfolgen und nicht selber P_IGNORE durchsucht werden.

BEMERKUNGEN
-----------
::

    Die Daten in dieser Property werden intern als Mapping gespeichert, nicht
    als Array von Strings. Bitte nicht mit Set/Query() an dieser Property
    herumbasteln.

SIEHE AUCH
----------
::

    TestIgnore, /std/player/comm.c


Last modified: 02.10.2011, Zesstra


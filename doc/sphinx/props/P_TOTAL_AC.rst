P_TOTAL_AC
==========

NAME
----
::

    P_TOTAL_AC                    "total_ac"                    

DEFINIERT IN
------------
::

    /sys/living/combat.h

BESCHREIBUNG
------------
::

     Numerischer Wert der Abwehrstaerke des Wesens.
     Dieser wird durch Aufaddieren der P_AC aller getragenen Ruestungen
     bestimmt. Aus diesem Grund ist das Abfragen dieser Property ziemlich
     teuer. Falls das Ergebnis mehrfach kurz hintereinander gebraucht wird,
     sollte die Property auf jeden Fall nur einmal abgefragt und der Wert
     gespeichert werden.

BEMERKUNGEN
-----------
::

    Auf diese Property sollte nicht mittels Query() oder Set() zugegriffen
    werden, das Setzen von Query- oder Setmethoden bitte auf jeden Fall
    unterlassen.

SIEHE AUCH
----------
::

    P_AC

05.09.2008, Zesstra


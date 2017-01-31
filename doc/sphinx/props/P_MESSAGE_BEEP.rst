P_MESSAGE_BEEP
==============

NAME
----
::

    P_MESSAGE_BEEP                        "message_beep"

DEFINIERT IN
------------
::

    /sys/player/comm.h

BESCHREIBUNG
------------
::

     Wertebereich: int=0..3600 (Sekunden)
     Wenn gesetzt wird in der Kommunikation des Spielers in den angegebenen
     Zeitraeumen ein Signalton ausgegeben. Wird in player/comm.c in comm_beep()
     verarbeitet.
     Ausgabe erfolgt nur, wenn P_VISUALBELL nicht gesetzt ist.
     Wird im Spielerobjekt gespeichert!

SIEHE AUCH
----------
::

     klingelton, ton, P_VISUALBELL, P_MESSAGE_LAST_BEEP

LETZTE AENDERUNG
----------------
::

   16. Mai 2007  Ennox


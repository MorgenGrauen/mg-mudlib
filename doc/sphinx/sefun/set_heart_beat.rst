set_heart_beat()
================

SYNOPSIS
--------
::

        int set_heart_beat(int flag);

BESCHREIBUNG
------------
::

        Schaltet den Heartbeat ein (1) oder aus (0). Der Treiber wendet die
        Lfun heart_beat() auf das aktuelle Objekt alle __HEARTBEAT_INTERVAL__
        Sekunden an, wenn der Heartbeat eingeschaltet ist. Ein Shadow
        auf der Lfun wird ignoriert.

        

        Der Heartbeat sollte immer ausgeschaltet werden, wenn er nicht
        gebraucht wird. Das reduziert die Systemauslastung.

        Liefert '1' bei Erfolg, '0' bei Fehler.

        Das Abschalten eines bereits abgeschalteten Heartbeats (und umgekehrt
        das Einschalten eines bereits eingeschalteten Heartbeats) zaehlt
        als Fehler.

BEMERKUNGEN
-----------
::

        __HEARTBEAT_INTERVAL__ ist in MG = 2 Sekunden

SIEHE AUCH
----------
::

        heart_beat(A), call_out(E)


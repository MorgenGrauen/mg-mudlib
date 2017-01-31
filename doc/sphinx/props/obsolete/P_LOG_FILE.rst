P_LOG_FILE
==========

********************* VERALTETE PROPERTY ******************************
* Diese Property wird nicht mehr ausgewertet.                         *
***********************************************************************

NAME
----
::

    P_LOG_FILE                   "log_file"

DEFINIERT IN
------------
::

    /sys/thing/properties.h

BESCHREIBUNG
------------
::

    Enthaelt einen String auf einen Filenamen. 

    Werden zu dem Objekt (Raum, Monster, ...) Fehlermeldungen abgesetzt, 
    werden diese in das angegebene File umgeleitet. Die Eintragung in
    die per Default fuer Fehlermeldungen vorgesehenen Dateien erfolgt
    nicht.

BEMERKUNGEN
-----------
::

    P_LOG_FILE wird ausgewertet wie log_file().

    Das heisst, es wird AUF JEDEN FALL nach /log geschrieben!

    Direkt in /log kann NICHT geschrieben werden, es muss also ein 
    Unterverzeichnis mit Eurem Magiernamen vorhanden sein.

BEISPIELE
---------
::

    SetProp(P_LOG_FILE,"tilly_log");          // FALSCH, es wuerde versucht, 
                                                 das File /log/tilly_log 
                                                 anzulegen
    SetProp(P_LOG_FILE,"/log/tilly_log");     // FALSCH, es wuerde versucht, 
                                                 das File /log/log/tilly_log
                                                 anzulegen
    SetProp(P_LOG_FILE,"/d/ebene/tilly_log"); // FALSCH, s.o.

    SetProp(P_LOG_FILE,"tilly/tilly_log");    // RICHTIG!

    Im letzten Beispiel werden alle Eintraege in das File tilly_log ge-
    schrieben, das sich im Verzeichnis /log/tilly/ befindet.

    Das Unterverzeichnis /tilly in /log muss zuvor angelegt werden.

SIEHE AUCH
----------
::

    P_LOG_INFO, write_file(), log_file(),

Letzte Aenderung: 13.09.04 Tilly@MorgenGrauen


P_TESTPLAYER
============

NAME
----
::

    P_TESTPLAYER                  "testplayer"                  

DEFINIERT IN
------------
::

    /sys/player/base.h

BESCHREIBUNG
------------
::

     Gesetzt, wenn der Spieler ein Testspieler ist. Enthaelt die UID des
     Magiers, dem dieser Testie (momentan) gehoert.

     

     Bei Testspielern duerfen Skills geaendert werden, sie duerfen gezappt
     werden und - ihre eigentliche Aufgabe - nicht angeschlossene Gebiete
     testen.

     AUSNAHMEN: Gildentesties duerfen nur sehr eingeschraenkt manipuliert
                werden werden, da sie im ganzen Mud rumlaufen koennen,
                Spielerkontakt haben und nach Abschluss der Tests ggf. sogar
                die Testiemarkierung entfernt werden kann.

                

     Fuer Spielertesties, die von einem Spieler kontrolliert werden, gelten
     teilweise besondere Regeln, s. 'spielertesties'.

BEMERKUNGEN: 
     P_TESTPLAYER kann nur per SetProp() gesetzt werden und das auch nur ein
     Mal! Geloescht werden kann das Flag nur von EM+.

ZULETZT GEAeNDERT
-----------------
::

05.01.2010, Zesstra


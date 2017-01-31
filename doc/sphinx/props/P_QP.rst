P_QP
====

NAME
----
::

    P_QP                          "questpoints"                 

DEFINIERT IN
------------
::

    /sys/player/quest.h

BESCHREIBUNG
------------
::

     Anzahl der Questpunkte, die ein Spieler hat.

HINWEIS
-------
::

     Nur Abfragen der Property mit QueryProp() liefert den korrekten Wert,
     da eine Quermethode fuer die Auslieferung der Daten sorgt. Query()
     oder gar QueryProperties() enthalten u.U. einen veralteten Wert.


P_KEEP_ON_SELL
==============

NAME
----
::

    P_KEEP_ON_SELL                "keep_on_sell"                

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Die Property enthaelt die UID des Spielers, der "behalte <name>"
     eingegeben hat.

     Bei "verkaufe alles" wird das Objekt behalten, wenn die Property
     die UID des verkaufenden Spielers enthaelt. Es ist also durchaus
     moeglich, auf diese Weise Objekte zu verkaufen, die auf andere
     Spieler markiert wurden.

BEMERKUNGEN
-----------
::

     Es ist nicht noetig, P_KEEP_ON_SELL zusaetzlich zu P_NODROP oder
     P_NOSELL zu setzen, denn jede dieser beiden verhindert bereits
     das Verkaufen.


Letzte Aenderung: 11. Aug. 2020, Arathorn

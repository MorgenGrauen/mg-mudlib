P_WORN
======

NAME
----
::

     P_WORN "worn"

DEFINIERT IN
------------
::

     <armour.h>

BESCHREIBUNG
------------
::

     Mittels dieser Property laesst sich ermitteln, ob eine Ruestung bzw. 
     Kleidung derzeit getragen wird und wenn ja, von wem.

     Entweder enthaelt die Property den Wert 0, oder sie enthaelt den
     Traeger der Ruestung / Kleidung (als Objekt).

BEMERKUNGEN
-----------
::

     Diese Property laesst sich nur abfragen!

BEISPIELE
---------
::

     Eine DefendFunc() koennte dem Traeger der Ruestung wie folgt etwas
     mitteilen:

     // Die Ruestung gibt Schutz gegen Feuer
     int DefendFunc(string *dtyp, mixed spell, object enemy)
     {
       if (member(dtyp, DT_FIRE) != -1) {
         // P_WORN ist auf jeden Fall gesetzt, da sonst die
         // DefendFunc ueberhaupt nicht aufgerufen wuerde!
         tell_object(QueryProp(P_WORN),
           "Die Flammen zuengeln nur leicht ueber die Ruestung.\n");
         return 10;
       }
       return 0;
     }

SIEHE AUCH
----------
::

     clothing, /std/clothing.c, armour, /std/armour.c

LETZTE AeNDERUNG
----------------
::

15.02.2009, Zesstra


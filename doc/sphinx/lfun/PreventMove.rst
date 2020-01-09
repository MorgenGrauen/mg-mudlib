PreventMove()
=============

PreventMove()
---------------


FUNKTION
--------

     protected int PreventMove(object dest, object oldenv, int method);

DEFINIERT IN
------------

     /std/thing/moving.c
     /std/living/moving.c
     /std/player/moving.c

ARGUMENTE
---------

     dest
          Das Ziel des Moves
     oldenv
          Das (Noch-)Environment des Objekts
     method
          Die Move-Methode(n), mit der/denen bewegt werden soll

BESCHREIBUNG
------------

     Mit dieser Funktion prueft ein Objekt, ob es von 'oldenv' nach 'dest'
     bewegt werden mag. Dabei wird 'method' beruecksichtigt (z.B. schaltet
     M_NOCHECK die meisten Pruefungen ab).
     Bei Gegenstaenden wird z.B. P_NODROP, P_NOGET, das Gewicht, ob das Ziel
     das Objekt aufnehmen will (MayAddWeight(), PreventInsert) oder auch
     PreventLeave() geprueft.
     Bei Lebewesen wird z.B. P_NO_TPORT in 'dest' und 'oldenv' und
     PreventLeaveLiving/PreventInsertLiving() ausgewertet.
     Bei Spielern wird auch noch getestet, ob method M_GO oder M_TPORT
     enthaelt und ob das Ziel nur von Testspielern betreten werden kann.

RUeCKGABEWERT
-------------

     0, wenn das Objekt bewegt werden darf.
     Wenn es nicht bewegt werden darf, sind als Rueckgabewerte die in
     /sys/moving.h definierten Move-Fehlerwerte zulaessig (s. move()). Sollte
     hier ein ungueltiger Fehlerwert zurueckgegeben werden, wird das move()
     ebenfalls abgebrochen und ME_DONT_WANT_TO_BE_MOVED zurueckgeben.

BEMERKUNGEN
-----------

     Diese Funktion kann ueberschrieben, um feinere Kontrolle ueber die
     Bewegungen des Objekt zu haben. Dabei aber bitte einige Dinge beachten:
     
     1. Denkt daran, ggf. M_NOCHECK zu beruecksichtigen und und eure
     Pruefungen nur zu machen, wenn es nicht in method vorkommt.
     
     2. *GANZ WICHTIG*: Wenn ihr mit euren Pruefungen fertig sein und das
     Objekt bewegt werden duerfte, die geerbten Pruefungen noch testen,
     also *IMMER* das geerbte PreventMove() noch aufrufen und dessen Wert
     zurueckgeben/beruecksichtigen, da sonst Pruefungen des Gewichts etc. 
     nicht funktionieren oder bei Lebewesen die Prevent\*() im Environment 
     nicht gerufen werden!
     
     3. Die Funktion ist nur objektintern zu verwenden, Call-Other von aussen
     sind nicht moeglich, beim Ueberschreiben 'protected' nicht vergessen.
     
     4. Nochmal: Geerbtes PreventMove() *NICHT VERGESSEN*!

BEISPIELE
---------

     Ein Objekt, was nur im Sternenlicht aufgenommen werden kann (warum
     auch immer):

.. code-block:: pike

     protected int PreventMove(object dest, object oldenv, int method) {
       if ( (method & M_NOCHECK) ) {
           // wenn mit M_NOCHECK bewegt, ist das Sternenlicht egal, nur
           // geerbtes PreventMove() beruecksichten:
           return ::PreventMove(dest, oldenv, method);
       }
       else if ( method & M_GET) {
           // wenn es aufgenommen werden soll: nach Sternenlicht im Raum
           // gucken:
           if (objectp(dest) && 
               (dest->QueryProp(P_LIGHT_TYPE) != LT_STARS))
               return ME_CANT_BE_TAKEN;
       }
       // Fall-through:
       return ::PreventMove(dest, oldenv, method);
     }

SIEHE AUCH
----------

     :doc:`../lfun/PreventLeave`, :doc:`NotifyInsert`,
     :doc:`NotifyLeave`, :doc:`MayAddObject`,
     :doc:`PreventInsertLiving`, :doc:`PreventLeaveLiving`,
     :doc:`NotifyMove`, :doc:`PreventMove`, :doc:`MayAddWeight`,
     :doc:`move`

Last modified: 09.01.2020, Zesstra


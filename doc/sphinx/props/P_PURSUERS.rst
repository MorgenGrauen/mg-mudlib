_PURSUERS
==========

NAME
----

  P_PURSUERS:         "pursuers"                    

DEFINIERT IN
------------
::

  /sys/living/moving.h

BESCHREIBUNG
------------
:: 
  
   Enthaelt ein zweielementiges Array mit folgendem Aufbau:

   0: Objekt, welches verfolgt wird, oder 0.
   1: Array der Objekte, welche verfolgen.

   Die Property ist im Verfolger ebenso wie im Verfolgten gesetzt.


BEMERKUNG
---------
:: 

   Kann auch 0 sein, also auf pointerp() pruefen.
   
   NICHT von Hand, sondern nur mit den dafuer gedachten Funktionen 
   modifizieren.

   Bei der typischen Konstellation, dass ein einzelner NPC einen
   Spieler verfolgt, sehen die Properties so aus:

   - Im Spieler  ({ 0, ({npc_objekt}) }) 
   - Im NPC      ({ spieler_objekt , ({}) })

   Nur bei einer Kette von Verfolgern, d.h. wenn ein Verfolger selbst
   ebenfalls verfolgt wird, sind beide Elemente des Arrays befuellt.


SIEHE AUCH
----------

  Lfuns:  :doc:`../lfun/AddPursuer`, :doc:`../lfun/RemovePursuer`, :doc:`../lfun/PreventFollow`

P_PURSUERS
==========

NAME
----

  P_PURSUERS:
    "pursuers"                    

DEFINIERT IN
------------
::

  /sys/living/moving.h

BESCHREIBUNG
------------

  Enthaelt ein zweielementiges Array mit folgendem Aufbau:
  0: Objekt welches verfolg wird oder 0.
  1: Array der Objekte welche verfolgen.

BEMERKUNG
---------

  Kann auch 0 sein, also auf pointerp() pruefen.
  
  NICHT von Hand, sondern nur mit den dafuer gedachten Funktionen 
  modifizieren.

SIEHE AUCH
----------

  - :doc:`../lfun/AddPursuer`, :doc:`../lfun/RemovePursuer`

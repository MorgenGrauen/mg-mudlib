QueryArrived()
==============

FUNKTION
--------
::

     mixed QueryArrived();

DEFINIERT IN
------------
::

     /std/transport.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Diese Funktion ermittelt, ob sich der Transporter momentan an einer
     Haltestelle befindet (und bestiegen oder verlassen werden kann) oder ob
     er unterwegs ist.

RUeCKGABEWERT
-------------
::

     Null, wenn der Transporter unterwegs ist. Liegt der Transporter an
     einer Haltestelle, so wird der mit AddRoute als name uebergebene String
     zurueckgegeben.

SIEHE AUCH
----------
::

     AddRoute(), /std/transport.c


Last modified: Wed May 8 10:21:47 1996 by Wargon


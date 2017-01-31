Halt()
======

FUNKTION
--------
::

     void Halt();

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

     Die Fahrt des Transporters wird abgebrochen. Sie muss explizit mit
     Start() wieder aufgenommen werden!

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Es gibt keine einfache Moeglichkeit, die Fahrt eines Transporters an
     der Stelle wieder aufzunehmen, an der sie unterbrochen wurde. Man
     muesste die aktuelle Position mit QueryPosition() ermitteln, mit Hilfe
     der AddXXX()-Aufrufe in eine Positionsnummer umwandlen und diese an
     Start() uebergeben.

SIEHE AUCH
----------
::

     Start(), /std/transport.c


Last modified: Wed May 8 10:19:23 1996 by Wargon


trigger_sensitive_inv()
=======================

FUNKTION
--------
::

     varargs void trigger_sensitive_inv(object ob, string key, int wert,
     mixed *optionen1, mixed *optionen2);

DEFINIERT IN
------------
::

     eigenen Objekten, wird aufgerufen von /std/container/inventory.c

ARGUMENTE
---------
::

     ob
          Das ausloesende Objekt
     key
          Der Schluessel, der die Aktion ausloeste.
     wert
          Der Grenzwert des ausloesenden Objektes.
     optionen1
          Die Optionen des ausloesenden Objektes.
     optionen2
          Die Optionen des reagierenden Objektes.

BESCHREIBUNG
------------
::

     Diese Funktion wird in Objekten des sensitiven Typs SENSITIVE_INVENTORY
     aufgerufen, wenn sie in Kontakt mit Objekten des Typs
     SENSITIVE_INVENTORY_TRIGGER treten, die den gleichen Schluessel
     aufweisen und einen hoeheren Grenzwert haben.

     Anhand der Parameter koennen dann die noetigen Aktionen ausgeloest
     werden.

RUeCKGABEWERT
-------------
::

     keiner

BEISPIELE
---------
::

SIEHE AUCH
----------
::

     trigger_sensitive_attack(), sensitive Objekte


Last modified: Wed May 8 10:26:10 1996 by Wargon


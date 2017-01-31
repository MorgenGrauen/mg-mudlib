present_objects()
=================

FUNKTION
--------
::

     object *present_objects(string desc);

DEFINIERT IN
------------
::

     /std/container/restrictions.c

ARGUMENTE
---------
::

     desc
          Umschreibung des gesuchten Objektes oder "alles" oder "alle".

BESCHREIBUNG
------------
::

     Diese Funktion gibt die Objekte im Inneren des Behaelters zurueck, die
     sich mit desc ansprechen lassen. In dem Fall, dass "alle(s)"
     angefordert wird, sind das alle sichtbaren Objekte, ansonsten das erste
     Objekt, das sich mit desc ansprechen laesst.
     Objekte, die P_INVIS gesetzt haben, zaehlen als nicht ansprechbar, im
     Gegensatz zu solchen Objekten, die keine P_SHORT haben.

RUeCKGABEWERT
-------------
::

     Ein Array von Objekten mit den geforderten Eigenschaften.

     Befindet sich kein Objekt im Behaelter, das sich durch desc ansprechen
     laesst, so wird ein leeres Array zurueckgegeben.

SIEHE AUCH
----------
::

     locate_objects(), /std/container/restrictions.c


03.03.2013, Zesstra


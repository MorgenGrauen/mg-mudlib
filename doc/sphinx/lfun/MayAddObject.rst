MayAddObject()
==============

FUNKTION
--------
::

     int MayAddObject(object ob);

DEFINIERT IN
------------
::

     /std/container/restrictions.c

ARGUMENTE
---------
::

     ob - Das Object bei dem geprueft werden soll, ob es noch in den
          Container passt.

BESCHREIBUNG
------------
::

     Wenn ein Objekt in einen Container bewegt wird, prueft move() ueber
     diese Funktion, ob das Objekt ueberhaupt noch in den Behaelter hinein
     passt. Dazu uebergibt move() das Objekt das in den Container bewegt
     werden soll. (In Raeumen ist diese Funktionen ueberschrieben und
     liefert immer True zurueck.)

RUeCKGABEWERT
-------------
::

     1 - wenn das Objekt noch vom Container aufgenommen werden kann.
     0 sonst.

BEMERKUNGEN
-----------
::

     invis-Objekte passen immer in den Container hinein!
     move() ruft diese Funktion nicht auf, wenn in den Flags M_NOCHECK
     gesetzt war!

SIEHE AUCH
----------
::

     MayAddWeight(), PreventInsert(), move(), /std/container/restrictions.c


Last modified: 23.09.2007, Zesstra


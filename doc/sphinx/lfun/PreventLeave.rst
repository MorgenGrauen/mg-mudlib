PreventLeave()
==============

FUNKTION
--------
::

     int PreventLeave(object ob, mixed dest);

DEFINIERT IN
------------
::

     /std/container/restrictions.c

ARGUMENTE
---------
::

     ob
          Das Objekt, das aus dem Behaelter genommen werden soll.
     dest 
          Das Ziel in das das Objekt ob bewegt werden soll.

BESCHREIBUNG
------------
::

     Mit dieser Funktion kann ein Behaelter pruefen, ob er das Objekt ob
     sich bewegen lassen moechte oder nicht.

RUeCKGABEWERT
-------------
::

     0, wenn das Objekt bewegt werden kann; ein Wert groesser als 0
     zeigt an, dass das Objekt nicht bewegt werden soll.

BEMERKUNGEN
-----------
::

     Wenn ob mit dem Flag M_NOCHECK bewegt wird, wird PreventLeave() zwar
     aufgerufen, das Objekt wird jedoch auf jeden Fall aus dem Behaelter
     bewegt, unabhaengig vom Rueckgabewert!

SIEHE AUCH
----------
::

     PreventInsert(), NotifyInsert(), NotifyLeave(),
     MayAddWeight(), move(), /std/container/restrictions.c
     PreventLeaveLiving(), PreventInsertLiving(), PreventMove(),
     NotifyMove(), MayAddObject(), NotifyRemove()


Last modified: 04.08.2007, Zesstra


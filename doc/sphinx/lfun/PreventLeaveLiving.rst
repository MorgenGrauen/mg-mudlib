PreventLeaveLiving()
====================

FUNKTION
--------
::

     int PreventLeaveLiving(object ob, mixed dest);

DEFINIERT IN
------------
::

     /std/container/restrictions.c

ARGUMENTE
---------
::

     ob
          Das Living, das aus dem Behaelter genommen werden soll.
     dest 
          Das Ziel in das das Living ob bewegt werden soll.

BESCHREIBUNG
------------
::

     Mit dieser Funktion kann ein Behaelter pruefen, ob er das Living ob
     sich bewegen lassen moechte oder nicht.

RUeCKGABEWERT
-------------
::

     0, wenn das Living bewegt werden kann; ein Wert groesser als 0
     zeigt an, dass das Living nicht bewegt werden soll.

BEMERKUNGEN
-----------
::

     Wenn ob mit dem Flag M_NOCHECK bewegt wird, wird PreventLeave() zwar
     aufgerufen, das Living wird jedoch auf jeden Fall aus dem Behaelter
     bewegt, unabhaengig vom Rueckgabewert!

SIEHE AUCH
----------
::

     PreventInsertLiving(), /std/container/restrictions.c
     PreventMove(), PreventLeave(), PreventInsert(),
     NotifyMove(), NotifyLeave(), NotifyInsert(), NotifyRemove(),
     move(), exit(), init(),
     InitAttack, ExitAttack()

Last modified: 04.08.2007, Zesstra


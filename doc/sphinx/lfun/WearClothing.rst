WearClothing()
==============

FUNKTION
--------
::

     public int WearClothing(object ob) 

DEFINIERT IN
------------
::

     /std/living/clothing.c

ARGUMENTE
---------
::

     object ob
       Das Kleidungsstuck, das angezogen wird.

BESCHREIBUNG
------------
::

     Das Lebewesen, in dem diese Funktion gerufen wird, zieht das
     Kleidungsstueck <ob> an.
     ABER: 'Anziehen' bedeutet in diesem Kontext lediglich, dass die
     Ruestung/Kleidung in P_CLOTHING eingetragen wird. Es finden zur Zeit
     keine Pruefungen statt, ob das Lebewesen den Gegenstand ueberhaupt
     anziehen kann. Genausowenig werden Funktionen wie InformWear() gerufen.

     Die Funktion ist nur dazu gedacht, im Zuge des Anziehens eines Objekts
     von diesem im Lebewesen gerufen zu werden.

RUeCKGABEWERT
-------------
::

     1, wenn das Anziehen erfolgreich war.
     0 sonst.

BEMERKUNGEN
-----------
::

     Nicht von Hand aufrufen, es sei denn man weiss genau, was man tut. Und am
     besten auch dann nicht.

SIEHE AUCH
----------
::

     Wear(), WearArmour(), Unwear(), UnwearArmour(), UnwearClothing()
     P_CLOTHING, P_ARMOURS
     FilterClothing(), FilterArmours()

ZULETZT GEAeNDERT
-----------------
::

14.03.2009, Zesstra


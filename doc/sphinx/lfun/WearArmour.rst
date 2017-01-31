WearArmour()
============

FUNKTION
--------
::

     public int WearArmour(object ob) 

DEFINIERT IN
------------
::

     /std/living/clothing.c

ARGUMENTE
---------
::

     object ob
       Die Ruestung, die angezogen wird.

BESCHREIBUNG
------------
::

     Das Lebewesen, in dem diese Funktion gerufen wird, zieht die Ruestung
     <ob> an.
     ABER: 'Anziehen' bedeutet in diesem Kontext lediglich, dass die
     Ruestung/Kleidung in P_ARMOURS eingetragen wird. Es finden zur Zeit keine
     Pruefungen statt, ob das Lebewesen den Gegenstand ueberhaupt anziehen
     kann. Genausowenig werden Funktionen wie InformWear() gerufen oder
     etwaige Stat-Boni aktiviert.
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

     Wear(), WearClothing(), Unwear(), UnwearArmour(), UnwearClothing()
     P_CLOTHING, P_ARMOURS
     FilterClothing(), FilterArmours()

ZULETZT GEAeNDERT
-----------------
::

14.03.2009, Zesstra


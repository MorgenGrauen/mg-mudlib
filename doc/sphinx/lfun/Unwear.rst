Unwear()
========

FUNKTION
--------
::

     public int Unwear(object ob) 

DEFINIERT IN
------------
::

     /std/living/clothing.c

ARGUMENTE
---------
::

     object ob
       Die Ruestung oder Kleidung, die ausgezogen wird.

BESCHREIBUNG
------------
::

     Das Lebewesen, in dem diese Funktion gerufen wird, zieht die Ruestung
     oder das Kleidungsstueck <ob> aus.
     ABER: 'Ausziehen' bedeutet in diesem Kontext lediglich, dass die
     Ruestung/Kleidung aus P_ARMOURS bzw. P_CLOTHING ausgetragen wird. Es
     finden zur Zeit keine Pruefungen statt, ob das Lebewesen den Gegenstand
     ueberhaupt ausziehen kann. Genausowenig werden Funktionen wie
     InformUnwear()/RemoveFunc() gerufen oder etwaige Stat-Boni deaktiviert.

     Die Funktion ist nur dazu gedacht, im Zuge des Ausziehens eines Objekts
     von diesem im Lebewesen gerufen zu werden.

RUeCKGABEWERT
-------------
::

     1, wenn das Ausziehen erfolgreich war.
     0 sonst.

BEMERKUNGEN
-----------
::

     Nicht von Hand aufrufen, es sei denn man weiss genau, was man tut. Und am
     besten auch dann nicht.

SIEHE AUCH
----------
::

     Wear(), WearArmour(), WearClothing(), UnwearArmour(), UnwearClothing()
     P_CLOTHING, P_ARMOURS
     FilterClothing(), FilterArmours()

ZULETZT GEAeNDERT
-----------------
::

14.03.2009, Zesstra


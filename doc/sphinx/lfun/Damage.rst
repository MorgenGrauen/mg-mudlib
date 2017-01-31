Damage()
========

FUNKTION
--------
::

     int Damage(int dam);

DEFINIERT IN
------------
::

     /std/armour/combat.c und
     /std/weapon/combat.c

ARGUMENTE
---------
::

     dam  Der Wert, mit dem die Waffe/Ruestung beschaedig werden soll.

BESCHREIBUNG
------------
::

     P_WC bzw. P_AC wird um dam reduziert und P_DAMAGED wird um
     dam erhoeht.
     Bei dam>0 wird das Objekt beschaedigt, bei dam<0 repariert.
     Dabei werden sowohl die Obergrenzen (s. /sys/combat.h) wie auch
     die Untergrenzen (Waffen:30, Ruestungen: 0) fuer P_WC und P_AC
     beachtet. Es kann auch nicht mehr repariert werden, als vorher
     beschaedigt wurde.

RUeCKGABEWERT
-------------
::

     Der Wert der Beschaedigung, die tatsaechlich vorgenommen wurde.

BEMERKUNGEN
-----------
::

     Ist das Objekt in Benutzung, setzt die Funktion Damage automatisch
     die Properties P_TOTAL_WC bzw. P_TOTAL_AC in dem benutzenden Spieler
     auf die richtigen Werte.

SIEHE AUCH
----------
::

     /std/armour/combat.c, /std/weapon/combat.c


Last modified: Thu May 22 10:13:23 1997 by Paracelsus


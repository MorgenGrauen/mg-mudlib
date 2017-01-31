QueryDamage()
=============

FUNKTION
--------
::

     int QueryDamage(object enemy);

DEFINIERT IN
------------
::

     /std/weapon/combat.c

ARGUMENTE
---------
::

     enemy
          Der Gegner, gegen den die Waffe eingesetzt wird.

BESCHREIBUNG
------------
::

     Dies ist die zentrale Funktion der Waffe. Sie wird in jeder Kampfrunde
     von Attack() aus aufgerufen und gibt den Schaden zurueck, den der
     Gegner abwehren muss.

     In den Schaden gehen sowohl die Staerke der Waffe als auch die Staerke
     des Traegers der Waffe ein.

     Wurde eine HitFunc() angegeben, so wird diese mit den gleichen
     Parametern wie QueryDamage() aufgerufen.

RUeCKGABEWERT
-------------
::

     Die Staerke des Schlages fuer diese Kampfrunde. Sie ermittelt sich als
     Zufallszahl zwischen 0 und einem Wert, der sich aus der Staerke der
     Waffe und der Staerke ihres Traegers ergibt. Das Ergebnis des
     HitFunc()-Aufrufs wird zu dieser Zahl hinzugezaehlt.

BEMERKUNGEN
-----------
::

     Auch wenn man eine HitFunc() verwendet, darf der Rueckgabewert
     insgesamt nicht groesser als 200 werden. Im Zweifelsfall sollte 
     man sich mit der zustaendigen Balance besprechen!

SIEHE AUCH
----------
::

     HitFunc(), Attack(), /std/weapon.c, grenzwerte


Last modified: Fre Feb 16.02.01 12:58:00 von Tilly


HitFunc()
=========

FUNKTION
--------
::

     int HitFunc(object enemy);

DEFINIERT IN
------------
::

     eigenen Objekten, fuer /std/weapon/combat.c

ARGUMENTE
---------
::

     enemy
          Der Gegner, gegen den die Waffe eingesetzt wird.

BESCHREIBUNG
------------
::

     Die Waffe kann anhand des Gegners enemy entscheiden, ob ein
     Schadensbonus oder auch ein Schadensmalus wirksam wird. Dieser Bonus
     wird zu dem normalen Schaden der Waffe hinzuaddiert.

RUeCKGABEWERT
-------------
::

     Der Schadensbonus bzw. der Abschlag.

BEMERKUNGEN
-----------
::

     Wenn durch den Bonus die Genehmigungsgrenzen der Balance
     ueberschritten werden koennen, muss man seinen Regionsmagier und
     die Objekt-Balance konsultieren!

     Werte der HitFunc sollten immer ueber ein random() zurueckgegeben 
     werden!

     Diese Funktion sollte die Waffe nicht zerstoeren! Falls ihr im Kampf eine
     Waffe (ggf. in Abhaengigkeit vom Gegner) zerstoeren wollt, nutzt dazu
     bitte TakeFlaw(). 

BEISPIELE
---------
::

     Eine Waffe, die gegen Orks besonders gut wirkt:

     inherit "std/weapon";

     #include <properties.h>
     #include <combat.h>
     #include <class.h>

     create(){
       if(!clonep(this_object())) {
           set_next_reset(-1);
           return;
       }
       ::create();
       /* 
       zig SetProp's, um die Waffe zu konfigurieren
       HitFunc() ist in der Waffe selbst definiert 
       */
       SetProp(P_HIT_FUNC, this_object());
     }

     int HitFunc(object enemy)
     {
       /* laesst sich der Gegner als Ork ansprechen? */
       if (enemy->is_class_member(CL_ORC))
         return random(10+random(50)); /* Ja => Schaden erhoehen */

       return 0;   /* ansonsten keinen zusaetzlichen Schaden anrichten */
     }

SIEHE AUCH
----------
::

     QueryDefend(), /std/weapon.c
     TakeFlaw()

11.08.2007, Zesstra


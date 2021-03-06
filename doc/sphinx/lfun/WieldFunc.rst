WieldFunc()
===========

FUNKTION
--------
::

     int WieldFunc(object weapon, int silent, object user);

DEFINIERT IN
------------
::

     eigenen Objekten (fuer /std/weapon/combat)

ARGUMENTE
---------
::

     weapon (object)
          Die Waffe, die gezueckt werden soll.
     silent (int)
          Ob dabei eine Meldung ausgegeben werden soll.
     user (object)
          Das Lebewesen, welches die Waffe zuecken will.

BESCHREIBUNG
------------
::

     In dieser Funktion kann man zusaetzliche Abfragen vornehmen, ob sich
     die Waffe <weapon> von <user> zuecken laesst oder nicht.

RUeCKGABEWERT
-------------
::

     0, wenn die Waffe nicht gezueckt werden kann, sonst ungleich 0.

BEMERKUNGEN
-----------
::

     Selbst wenn man einen Wert ungleich Null zurueckgibt, ist das noch
     keine Garantie, dass sich die Waffe auch wirklich zuecken laesst! Der
     Spieler koennte zum Beispiel noch eine Waffe gezueckt haben, die sich
     nicht wegstecken laesst, etc.
     Wenn ihr sicher sein wollt, dass der Spieler ein Objekt gezueckt hat,
     benutzt bitte InformWield().
     Bitte nicht drauf verlassen, dass this_player() das Lebewesen ist,
     welches die Waffe zuecke will.
     Die Reihenfolge der Argumente ist etwas unschoen, aber leider wurde <user>
     erheblich spaeter hinzugefuegt und es war unmoeglich, einige hundert
     Objekte zu aendern.

BEISPIELE
---------
::

     Eine Waffe, die sich nicht von Zwergen zuecken laesst:

     inherit "std/weapon";

     #include <properties.h>
     #include <combat.h>

     create()
     {
       ::create();

       ... /* zig SetProp's, um die Waffe zu konfigurieren */

       /* WieldFunc() ist in der Waffe selbst zu finden */
       SetProp(P_WIELD_FUNC, this_object());
     }

     int WieldFunc(object weapon, int silent, object user)
     {
       /* Nicht-Zwerge duerfen die Waffe zuecken */
       if (user->QueryProp(P_RACE) != "Zwerg")
         return 1;

       /* Ansonsten sagen wir evtl., warum das Zuecken nicht klappt... */
       if (!silent)
         write( "Deine kleinen Haendchen koennen den Griff nicht "+
                "umklammern.\n");

       /* ...und brechen das Zuecken ab. */
       return 0;
     }

SIEHE AUCH
----------
::

     P_WIELD_MSG, P_UNWIELD_MSG, P_WEAR_MSG, P_UNWEAR_MSG
     DoWield(), DoUnwield(), InformUnwield(), InformWield() 
     UnwieldFunc, WieldFunc 
     /std/weapon/combat.c


02.02.2009, Zesstra


UnwieldFunc()
=============

FUNKTION
--------
::

     int UnwieldFunc(object weapon, int info, object user);

DEFINIERT IN
------------
::

     eigenen Objekten, fuer /std/weapon/combat.c

ARGUMENTE
---------
::

     weapon (object)
          Die Waffe, die weggesteckt werden soll.
     info (int)
          Bei (info&M_SILENT) wird keine Meldung ueber das Wegstecken
          ausgegeben.
          Bei (info&M_NOCHECK) wird die Waffe auch weggesteckt, wenn
          sie verflucht ist. Die tritt insbesondere dann auf, wenn der
          Spieler, der die Waffe benutzt, stirbt und die Waffe in
          die Leiche bewegt wird.
     user (object)
          Das Lebewesen, welches die Waffe gerade gezueckt hat und sie nun
          ausziehen will.

BESCHREIBUNG
------------
::

     Hier koennen zusaetzliche Abfragen vorgenommen werden, ob sich die
     Waffe <weapon> wegstecken laesst oder nicht.

RUeCKGABEWERT
-------------
::

     0, wenn sich die Waffe nicht wegstecken laesst, ansonsten ungleich 0.

BEMERKUNGEN
-----------
::

     Verfluchte Waffen, die sich erst nach Entfernung des Fluches wegstecken
     lassen, sollte man besser mit P_CURSED realisieren.
     Selbst wenn man einen Wert ungleich Null zurueckgibt, ist das noch
     keine Garantie, dass sich die Waffe auch wirklich zuecken laesst! Der
     Spieler koennte zum Beispiel noch eine Waffe gezueckt haben, die sich
     nicht wegstecken laesst, etc.
     Wenn ihr sicher sein wollt, dass der Spieler ein Objekt gezueckt hat,
     benutzt bitte InformWear().
     Bitte nicht drauf verlassen, dass this_player() das Lebewesen ist,
     welches die Waffe gezueckt und wegstecken will.
     Die Reihenfolge der Argumente ist etwas unschoen, aber leider wurde <user>
     erheblich spaeter hinzugefuegt und es war unmoeglich, einige hundert
     Objekte zu aendern.

SIEHE AUCH
----------
::

     P_WIELD_MSG, P_UNWIELD_MSG, P_WEAR_MSG, P_UNWEAR_MSG
     DoWield(), DoUnwield(), InformWield(), InformUnwield(), 
     UnwieldFunc, WieldFunc() 
     /std/weapon/combat.c


02.02.2009, Zesstra


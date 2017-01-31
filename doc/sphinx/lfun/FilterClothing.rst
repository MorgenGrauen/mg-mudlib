FilterClothing()
================

FUNKTION
--------
::

     public object *FilterClothing(closure filterfun, varargs mixed* extra)

DEFINIERT IN
------------
::

     /std/living/clothing.c

ARGUMENTE
---------
::

     closure filterfun
       Die Closure, die entscheiden soll, ob eine Kleidung im Ergebnisarray
       enthalten sein soll.

BESCHREIBUNG
------------
::

     Diese Funktion ruft <filterfunc> fuer jede getragene Kleidung des
     Lebewesen mit der jeweiligen Kleidung als Argument auf und liefert ein
     Array mit aller Kleidung zurueck, fuer die <filterfun> einen Wert != 0
     zurueckliefert.
     Die <extra> Argumente werden als zusaetzliche Parameter an <filterfun>
     uebergeben und duerfen keine Referenzen sein.

     

     Diese Variante ist zu bevorzugen, wenn man die getrage Kleidung nach
     bestimmten Kriterien durchsuchen will. 

RUeCKGABEWERT
-------------
::

     Ein Array von Objekten mit allen passenden Kleidung.

BEISPIELE
---------
::

     1) Ich moechte alle Kleidung, die groesser als 50cm ist.
     private int _armour_is_bigger(object clothing, int size) {
       return clothing->QueryProp(P_SIZE) > size;
     }
     ...
     object *big_armours = PL->FilterClothing(#'_amour_is_bigger, 50); 

     2) alle Kleidung mit einer speziellen ID.
     private int _has_id(object clothing, string idstr) {
       return clothing->id(idstr);
     }
     object *has_id = PL->FilterClothing(#'_has_id, "\ntollekleidung");

     3) ueberhaupt alle getragene Kleidung
     object *clothing = PL->FilterClothing(#'objectp)

SIEHE AUCH
----------
::

     Wear(), WearArmour(), WearClothing(), Unwear(), UnwearArmour(), 
     UnwearClothing()
     P_CLOTHING, P_ARMOURS
     FilterArmours(), QueryArmourByType()

ZULETZT GEAeNDERT
-----------------
::

14.03.2009, Zesstra


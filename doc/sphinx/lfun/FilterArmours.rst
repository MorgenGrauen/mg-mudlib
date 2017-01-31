FilterArmours()
===============

FUNKTION
--------
::

     public object *FilterArmours(closure filterfun, varargs mixed* extra)

DEFINIERT IN
------------
::

     /std/living/clothing.c

ARGUMENTE
---------
::

     closure filterfun
       Die Closure, die entscheiden soll, ob eine Ruestung im Ergebnisarray
       enthalten sein soll.

     

     mixed extra
       Beliebig viele Extra-Argumente, die <filterfun> uebergeben werden.

BESCHREIBUNG
------------
::

     Diese Funktion ruft <filterfunc> fuer jede getragene Ruestung des
     Lebewesen mit der jeweiligen Ruestung als Argument auf und liefert ein
     Array mit allen Ruestungen zurueck, fuer die <filterfun> einen Wert != 0
     zurueckliefert.
     Die <extra> Argumente werden als zusaetzliche Parameter an <filterfun>
     uebergeben und duerfen keine Referenzen sein.

     

     Diese Variante ist zu bevorzugen, wenn man Ruestungen nach bestimmten
     Kriterien durchsuchen will und QueryArmourByType() nicht ausreichend sein
     sollte.

RUeCKGABEWERT
-------------
::

     Ein Array von Objekten mit allen passenden Ruestungen.

BEISPIELE
---------
::

     1) Ich moechte gerne alle Ruestungen haben, die beschaedigt sind:
     private int _is_damaged(object ruestung) {
         return ruestung->QueryProp(P_DAMAGE);
     }
     ...
     object *damaged_armours = PL->FilterArmours(#'_is_damaged);

     2) Ich moechte alle Ruestungen, die groesser als 50cm sind.
     private int _armour_is_bigger(object ruestung, int size) {
       return ruestung->QueryProp(P_SIZE) > size;
     }
     ...
     object *big_armours = PL->FilterArmours(#'_amour_is_bigger, 50); 

     3) alle Ruestungen mit einer speziellen ID.
     private int _has_id(object ruestung, string idstr) {
       return ruestung->id(idstr);
     }
     object *has_id = PL->FilterArmours(#'_has_id, "\ntolleruestung");

     4) alle Ruestungen mit einer speziellen ID, die groesser als 50cm sind.
     private int _has_id(object ruestung, string idstr, int size) {
       return ruestung->id(idstr) && ruestung->QueryProp(P_SIZE) > size;
     }
     object *has_id = PL->FilterArmours(#'_has_id, "\ntolleruestung", 50);

     5) ueberhaupt alle getragene Ruestung
     object *rue = PL->FilterArmours(#'objectp)

SIEHE AUCH
----------
::

     Wear(), WearArmour(), WearClothing(), Unwear(), UnwearArmour(), 
     UnwearClothing()
     P_CLOTHING, P_ARMOURS
     FilterClothing(), QueryArmourByType()

ZULETZT GEAeNDERT
-----------------
::

14.03.2009, Zesstra


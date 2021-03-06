IsEqual()
=========

FUNKTION
--------
::

    int IsEqual(object ob)

DEFINIERT IN
------------
::

    /std/unit.c

ARGUMENTE
---------
::

    ob      Das Objekt das geprueft werden soll.

BESCHREIBUNG:                                                               
    Diese Funktion prueft ob zwei Objekte vom gleichen Typ sind, also ob
    z.B. ob und this_object() beides Muenzen sind oder beides Edelsteine.
    Bei einem Ergebnis != 0 fasst unit.c diese zwei Objekte automatisch
    zusammen, wenn ob->IsEqual(this_object()) auch einen Wert != 0 ergibt.
    Hierbei wird das IsEqual() von beiden beteiligten Objekten gerufen und sie
    muessen uebereinstimmen, dass sie eigentlich das gleiche Objekt sind.
    Selbstverstaendlich ist diese Funktion nur im Falle von Unitobjekten
    sinnvoll.

RUeCKGABEWERT
-------------
::

    0 - this_object() ist nicht vom selben Typ wie ob
    1 - this_object() ist vom gleichen Typ wie ob

BEISPIELE
---------
::

    o Ein Unitobjekt das verschiedene Beschreibungen haben kann...

       int IsEqual(object ob)
       {
          if (!(int)::IsEqual(ob)) return 0;
          return (QueryProp(P_SHORT)==ob->QueryProp(P_SHORT));
       }

SIEHE AUCH
----------
::

    /std/unit.c

25.01.2015, Zesstra


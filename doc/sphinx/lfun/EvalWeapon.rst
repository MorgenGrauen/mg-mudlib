EvalWeapon()
============

FUNKTION
--------
::

    int EvalWeapon(object ob, closure qp)

 

DEFINIERT IN
------------
::

    /std/room/shop.c

 

ARGUMENTE
---------
::

    ob - Eine Waffe.
    qp - symbol_function("QueryProp",ob)

BESCHREIBUNG
------------
::

    Bewertet die Waffe.

 

RUECKGABEWERT
-------------
::

    Max(P_WC,P_EFFECTIVE_WC);

 

SIEHE AUCH
----------
::

    FindBestWeapon()


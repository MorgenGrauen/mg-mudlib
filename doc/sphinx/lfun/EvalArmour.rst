EvalArmour()
============

FUNKTION
--------
::

    int EvalArmour(object ob, closure qp)

 

DEFINIERT IN
------------
::

    /std/room/shop.c

 

ARGUMENTE
---------
::

    ob - Eine Ruestung.
    qp - symbol_function("QueryProp",ob)

BESCHREIBUNG
------------
::

    Bewertet die Ruestung.

 

RUECKGABEWERT
-------------
::

    Max(P_AC,P_EFFECTIVE_AC);

 

SIEHE AUCH
----------
::

    FindBestArmour()


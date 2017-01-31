IsArmour()
==========

FUNKTION
--------
::

    status IsArmour()

DEFINIERT IN
------------
::

    /std/armour/wear.c

RUeCKGABEWERT
-------------
::

    1 wenn ein Ruestung
    0 sonst

BESCHREIBUNG
------------
::

    Gibt 1 zurueck, wenn entsprechende Klasse irgendwo geerbt wurden, es
    also eine Ruestung ist.

BEMERKUNGEN
-----------
::

    Entsprechende Basisklasse ueberschreibt und gibt fuer IsClothing()
    explizit 0 zurueck.

BEISPIEL
--------
::

    // enthaelt im Gegensatz zu P_ARMOURS auch nichtgetragenen Kram
    // im Inventory L1
    object *allarmours =
      filter_objects(all_inventory(this_player()), "IsArmour")

SIEHE AUCH
----------
::

    Aehnlich: living, interactive
    Aehnlich: IsBottle, IsClothing, IsPlayerCorpse, IsRoom, IsUnit
    Props:    P_ARMOURS

3. Sep 2016, Gloinson


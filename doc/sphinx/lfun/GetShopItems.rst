GetShopItems()
==============

FUNKTION
--------

    protected object* GetShopItems()

DEFINIERT IN
------------
    /std/room/shop.c

BESCHREIBUNG
------------

    Erstellt ein Array mit den zum Verkauf stehenden Objekten.

BEMERKUNG
---------

    Enthaelt nur jeweils einen Clone von jeder Blueprint.

RUECKGABEWERTE
--------------

    Object-Array mit zuerst den Objekten im Store und anschliessend den
    fixed-Objects.

SIEHE AUCH
----------

    :doc:`PrintList`
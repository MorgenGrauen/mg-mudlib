give_obj()
==========

FUNKTION
--------
::

    int give_obj(object ob, object where)

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    ob      Das Objekt, das abgegeben werden soll.
    where   Das Lebewesen, dass das Objekt erhaelt.

BESCHREIBUNG
------------
::

    Das Lebewesen, in dem diese Funktion aufgerufen werden soll, gibt
    den angegebenen Gegenstand (ob) an das angegebene Lebewesen (where).

RUeCKGABEWERT
-------------
::

    1, wenn das Objekt uebergeben wurde oder die Uebergabe nicht moeglich war.
    (in diesem Fall wird auch direkt eine Textausgabe ausgegeben)
    0 sonst, in diesem Fall wird in notify_fail eine passende Ausgabe plaziert.

SIEHE AUCH
----------
::

    pick_obj(), drop_obj(), put_obj(), find_obs(), /std/living/put_and_get.c


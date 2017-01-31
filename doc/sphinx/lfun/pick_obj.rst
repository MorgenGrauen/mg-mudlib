pick_obj()
==========

FUNKTION
--------
::

    int pick_obj(object ob)

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    ob      Das Objekt, das genommen werden soll.

BESCHREIBUNG
------------
::

    Das Lebewesen, in dem diese Funktion aufgerufen werden soll, hebt
    den angegebenen Gegenstand (ob) auf, falls es ihm moeglich ist.

RUeCKGABEWERT
-------------
::

    1, wenn das Objekt genommen wurde oder dies nicht moeglich war. (in diesem
    Fall wird auch direkt eine Textausgabe ausgegeben)
    0 sonst, in diesem Fall wird in notify_fail eine passende Ausgabe
    plaziert

SIEHE AUCH
----------
::

    drop_obj(), find_obs(), give_obj(), put_obj(), /std/living/put_and_get.c


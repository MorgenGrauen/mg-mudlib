buffer_sp()
===========

FUNKTION
--------
::

    int buffer_sp( int val, int rate );

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    val:  Gesamte Heilung.
    rate: KP-Rate.

        

BESCHREIBUNG
------------
::

    Erhoeht die KP eines Spielers automatisch insgesamt um den Wert "val".
    Pro heart_beat() wird ihm dabei der Wert "rate" zugefuehrt.
    Sollen neben P_SP noch weitere Props manipuliert werden - bspw. zur
    P_FOOD - bietet sich die Funktion consume() an.

RUECKGABEWERTE
--------------
::

    Der getankte Wert pro heart_beat().

BEMERKUNG
---------
::

    Sollte von jeder tragbaren Heilung genutzt werden, welche den Spieler
    darauf schliessen lassen kann, auf natuerlichem und nichtmagischem Weg
    (Essen, Trinken) geheilt worden zu sein.

BEISPIEL
--------
::

    s. Bsp. zu "buffer_hp"

SIEHE AUCH
----------
::

     Aehnlich:  heal_self, restore_spell_points, restore_hit_points, 
                buffer_hp
     Tanken:    consume, drink_alcohol, drink_soft, eat_food
     Props:     P_SP, P_HP,
     Konzepte:  heilung

9. August 2015 Gloinson


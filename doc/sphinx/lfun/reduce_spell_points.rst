reduce_spell_points()
=====================

FUNKTION
--------
::

    void reduce_spell_points(int points)

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    points: Anzahl der Konzentrationspunkte die abgezogen werden sollen.

BESCHREIBUNG
------------
::

    Dem Lebewesen werden points Konzentrationspunkte abgezogen. Falls
    das Lebewesen weniger Konzentrationspunkte hat, als abgezogen werden
    sollen, werden sie auf 0 gesetzt.

BEISPIELE
---------
::

    write("Das boese boese Monster schaut Dich grimmig an und labt sich an "
         +"Deiner Konzentration.\n");
    this_player()->reduce_spell_points(50);

SIEHE AUCH
----------
::

    Gegenpart:	restore_spell_points(L)
    Verwandt:	reduce_hit_points(L), buffer_sp(L)
    Props:	P_SP
    Konzept:	heilung

23.Feb.2004 Gloinson


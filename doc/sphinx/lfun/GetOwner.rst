GetOwner()
==========

GetOwner(L)
-----------
::

FUNKTION
--------
::

     string GetOwner();

BESCHREIBUNG
------------
::

     Wird vom Hoerrohr gerufen:
      /d/inseln/shakedbeer/shakyisland/obj/hoerrohr.c
     und kann einen Besitzer der Dateien zurueckgeben, der vom Besitzer
     der Pfade in denen die Datei liegt abweicht.

BEISPIELE
---------
::

     string GetOwner() {
      return "Tiamak";
     }

SIEHE AUCH
----------
::

     P_INFO

11. Mai 2004 Gloinson


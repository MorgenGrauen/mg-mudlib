remove()
========

FUNKTION
--------
::

     varargs int remove(int silent);

DEFINIERT IN
------------
::

     /std/thing/moving.c
     /std/living/moving.c
     /std/room/moving.c

ARGUMENTE
---------
::

     silent
          Falls ungleich 0, so werden beim Zerstoeren keine Meldungen
          ausgegeben.

BESCHREIBUNG
------------
::

     Beim Aufruf dieser Funktion entfernt sich das Objekt selbst. Durch
     Ueberladen dieser Funktion kann man diesen Vorgang noch durch die
     Ausgabe von Meldungen kommentieren, oder irgendwelche Daten
     abspeichern, oder das Zerstoeren ganz verhindern (auf diesem Weg... Mit
     destruct() kann das Objekt immer noch direkt zerstoert werden!)

RUeCKGABEWERT
-------------
::

     1, wenn sich das Objekt erfolgreich selbst zerstoert hat, sonst 0.

BEMERKUNGEN
-----------
::

     Nach einem erfolgreichen ::remove() gelten die selben Einschraenkungen
     wie nach einem destruct()!

SIEHE AUCH
----------
::

     destruct()


Last modified: Wed May 8 10:23:40 1996 by Wargon


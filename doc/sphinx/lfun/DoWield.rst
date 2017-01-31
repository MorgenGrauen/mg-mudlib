DoWield()
=========

FUNKTION
--------
::

     varargs int DoWield(int silent);

DEFINIERT IN
------------
::

     /std/weapon/combat.c

ARGUMENTE
---------
::

     silent
          Ungleich 0, wenn die Waffe ohne Meldungen gezueckt werden soll.

BESCHREIBUNG
------------
::

     Es wird versucht, die Waffe zu zuecken. Hat man schon eine Waffe
     gezueckt, so wird versucht, diese wegzustecken. Klappt das nicht, kann
     die Waffe nicht gezueckt werden.

RUeCKGABEWERT
-------------
::

     0, wenn man die Waffe gar nicht bei sich traegt, ansonsten 1.

BEMERKUNGEN
-----------
::

     Anhand des Rueckgabewertes laesst sich nicht entscheiden, ob die Waffe
     sich erfolgreich zuecken liess!

     Gruende, warum sich eine Waffe nicht zuecken lassen kann, sind
     folgende:
        o Man traegt sie nicht bei sich (oder sie steckt in einem Beutel
          o.ae.).
        o Man hat sie schon gezueckt.
        o Falls definiert: WieldFunc() gibt 0 zurueck.
        o Man ist nicht geschickt genug (das haengt von der Waffenklasse
          ab).
        o Eine schon gezueckte Waffe laesst sich nicht wegstecken.
        o Die Waffenklasse ist hoeher als erlaubt.
        o Man hat nicht genug Haende frei.

SIEHE AUCH
----------
::

     WieldFunc(), InformWield(), P_EQUIP_TIME, /std/weapon.c


Last modified: Wed Apr 08 10:25:00 2004 by Muadib


do_unwear()
===========

FUNKTION
--------
::

     varargs int do_unwear(string str, int silent);

DEFINIERT IN
------------
::

     /std/armour/combat.c

ARGUMENTE
---------
::

     str
          String der normalerweise dem Parameter des "ziehe"-Kommandos
          entspricht.

     silent
          1, wenn das Ausziehen ohne Meldungen durchgefuehrt werden soll.

BESCHREIBUNG
------------
::

     Diese Funktion stellt fest, ob die Ruestung sich durch str
     identifizieren laesst und angezogen ist und ruft im Erfolgsfall
     UnWear(silent) auf.

RUeCKGABEWERT
-------------
::

     1, wenn alles gut ging, ansonsten 0.

BEMERKUNGEN
-----------
::

     Wenn man Ruestungen "von aussen" ausziehen will, sollte man direkt
     UnWear() verwenden!

SIEHE AUCH
----------
::

     do_wear(), UnWear(), RemoveFunc(), InformUnwear(),
     /std/armour/combat.c


Last modified: Sun Jun 27 22:29:00 1999 by Paracelsus


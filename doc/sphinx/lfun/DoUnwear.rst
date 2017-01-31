DoUnwear()
==========

FUNKTION
--------
::

     varargs int DoUnwear(int silent, int all);

DEFINIERT IN
------------
::

     /std/clothing/wear.c

ARGUMENTE
---------
::

     silent
          Falls ungleich 0, so werden keine Meldungen ausgegeben.
          Falls (silent&M_NOCHECK) werden auch verfluchte Ruestungen
          ausgezogen
     all
          Ungleich 0, wenn DoUnwear() aus einem "ziehe alles aus" heraus
          aufgerufen wurde.

BESCHREIBUNG
------------
::

     Es wird versucht, die Ruestung auszuziehen. Dabei werden eine eventuell
     vorhandene RemoveFunc() und Flueche mit beruecksichtigt.

RUeCKGABEWERT
-------------
::

     0, wenn die Ruestung gar nicht getragen war, ansonsten 1.

BEMERKUNGEN
-----------
::

     Auch wenn eine 1 zurueckgegeben wird, muss das nicht heissen, dass die
     Ruestung erfolgreich ausgezogen wurde!

SIEHE AUCH
----------
::

     DoWear(), RemoveFunc(), InformUnwear(), /std/armour/combat.c


Last modified: Sun Jun 27 22:22:00 1999 by Paracelsus


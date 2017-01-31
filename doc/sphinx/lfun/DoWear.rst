DoWear()
========

FUNKTION
--------
::

     varargs int DoWear(int silent, int all);

DEFINIERT IN
------------
::

     /std/armour/combat.c

ARGUMENTE
---------
::

     silent
          Falls ungleich 0, so werden keine Meldungen ausgegeben.
     all
          Ungleich 0, wenn DoWear() aus einem "ziehe alles an" heraus
          aufgerufen wurde.

BESCHREIBUNG
------------
::

     Es wird versucht, die Ruestung anzuziehen. Dabei wird eine eventuell
     vorhandene WearFunc() mit beruecksichtigt.

RUeCKGABEWERT
-------------
::

     0, wenn man die Ruestung gar nicht bei sich hat oder sie schon an hat,
     ansonsten 1.

BEMERKUNGEN
-----------
::

     Auch wenn eine 1 zurueckgegeben wird, muss das nicht heissen, dass die
     Ruestung erfolgreich angezogen wurde!

     Gruende fuer ein Fehlschlagen des Anziehens koennen sein:
        o Man hat die Ruestung nicht bei sich.
        o Man hat die Ruestung schon an.
        o Man hat schon eine Ruestung des gleichen Typs an.
        o Der Typ der Ruestung oder die Ruestungsklasse ist illegal.
        o Falls definiert: WearFunc() gab 0 zurueck.
        o Falls es sich um einen Schild handelt: Man hat keine Hand mehr
          frei.

SIEHE AUCH
----------
::

     DoUnwear(), WearFunc(), InformWear(), P_EQUIP_TIME, 
     /std/armour/combat.c, P_UNWEAR_MSG, P_WEAR_MSG


Last modified: Sun Jun 27 22:22:00 1999 by Paracelsus


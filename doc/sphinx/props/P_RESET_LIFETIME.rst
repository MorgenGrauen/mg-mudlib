P_RESET_LIFETIME
================

NAME
----
::

     P_RESET_LIFETIME              "std_food_lifetime_reset"

DEFINIERT IN
------------
::

     /sys/food.h

BESCHREIBUNG
------------
::

     Zeit in Resets, die die Speise haltbar ist.
     Jeder einzelne Reset wird in seiner Laenge zufaellig festgelegt und
     zu einer Gesamtanzahl von Sekunden zusammengefasst. Diese Zeit in
     Sekunden wird dann in P_LIFETIME gespeichert.
     Nach dieser Zeit wird diese Speise schlecht und je nach Konfiguration
     der Speise eventuell zerstoert. Sichergestellt wird dies durch den
     Aufruf von reset() nach dieser Anzahl "Resets".
     Moechte man eine Speise haben, die niemals verdirbt
     (genehmigungspflichtig!), nutzt man die Property P_NO_BAD

     

BEMERKUNGEN
-----------
::

     Sobald der Countdown zum Schlechtwerden der Speise laeuft, also ein
     Spieler damit in Beruehrung kam, zeigt SetProp auf diese Property keine
     Wirkung mehr.

DEFAULT
-------
::

     Ohne Setzen von P_LIFETIME ,P_RESET_LIFETIME und P_NO_BAD verdirbt die
     Speise nach einem Reset, also zwischen 30 und 60 Minuten

SIEHE AUCH
----------
::

     wiz/food, P_LIFETIME, P_NO_BAD


Last modified: Thu Oct 28 12:15:00 2010 by Caldra


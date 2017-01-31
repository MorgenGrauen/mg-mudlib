DoDecayMessage()
================

FUNKTION
--------
::

      protected void DoDecayMessage(int oldamount, int zerfall);

      

DEFINIERT IN
------------
::

      /std/unit.c

ARGUMENTE
---------
::

     oldamount (int)
        Menge vor dem Zerfall
     zerfall (int)
        jetzt zerfallende Menge

BESCHREIBUNG
------------
::

     Diese Funktion wird von DoDecay() gerufen und gibt die Standardmeldungen
     beim Zerfall von Unitobjekten aus.
     Hierbei ist an der Unit noch alles unveraendert, wenn diese Funktion
     gerufen wird, die Reduktion von P_AMOUNT erfolgt direkt im Anschluss.
     Die Funktion wird nicht gerufen, wenn DoDecay() mit silent!=0 gerufen
     wird.

BEMERKUNGEN
-----------
::

     Will man nicht die Standardzerfallsmeldungen (wovon ich meist ausgehe),
     kann man diese Funktion ueberschreiben und eigene Meldungen erzeugen.

SIEHE AUCH
----------
::

     unit
     P_UNIT_DECAY_INTERVAL, P_UNIT_DECAY_FLAGS, P_UNIT_DECAY_QUOTA,
     P_UNIT_DECAY_MIN
     DoDecay()
     /std/unit.c

14.10.2007, Zesstra


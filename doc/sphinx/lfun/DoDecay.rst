DoDecay()
=========

FUNKTION
--------
::

      public  int    DoDecay(int silent) 

DEFINIERT IN
------------
::

      /std/unit.c

ARGUMENTE
---------
::

     silent (int)
       Falls != 0, erfolgt beim Zerfall keine Meldung, d.h. doDecayMessaage()
       wird nicht gerufen.

RUeCKGABEWERT
-------------
::

     Die Funktion gibt die nach dem Zerfall noch uebrig gebliebene Menge
     zurueck (int).

BESCHREIBUNG
------------
::

     Diese Funktion wird in Clones von Unitobjekten aus der Blueprint gerufen,
     wenn ein Zerfallsintervall abgelaufen ist (natuerlich nur, wenn in der BP
     der Zerfall konfiguriert ist).
     Die Funktion prueft normalerweise via P_UNIT_DECAY_FLAGS, ob der Zerfall
     stattfinden soll, bestimmt aus P_UNIT_DECAY_QUOTA die zu zerfallende
     Menge, ruft DoDecayMessage() und reduziert P_AMOUNT.

     

     Sie kann auch von Hand gerufen werden, um einen Zerfall auszuloesen, auch
     wenn mir gerade nicht einfaellt, in welchen Situationen das sinnvoll
     waere (vielleicht als Spruchmisserfolg. *g*)

BEMERKUNGEN
-----------
::

     Wenn man einen anderen Zerfallsmechanismus haben, will muss man diese
     Funktion wohl ueberschreiben. In fast allen Faellen sollte dies jedoch
     unnoetig sein. Hat jemand das Verlangen, diese Funktion zu
     ueberschreiben, ist vielleicht vorher eine Diskussion mit dem Mudlib-EM
     angebracht.

SIEHE AUCH
----------
::

     unit
     P_UNIT_DECAY_INTERVAL, P_UNIT_DECAY_FLAGS, P_UNIT_DECAY_QUOTA,
     P_UNIT_DECAY_MIN
     DoDecayMessage()
     /std/unit.c

14.10.2007, Zesstra


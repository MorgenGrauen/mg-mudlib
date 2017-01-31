SetBuyFact()
============

FUNKTION
--------
::

     void SetBuyFact(int fact);

DEFINIERT IN
------------
::

     /std/laden.c

ARGUMENTE
---------
::

     fact
          Der Einkaufspreismultiplikator.

BESCHREIBUNG
------------
::

     Der Preis, der beim Kauf eines Objekts im Laden zu entrichten ist,
     errechnet sich aus seinem Wert ( P_VALUE), multipliziert mit dem Faktor
     fact. Dieser Preis ist konstant und nicht von der aktuellen Marktlage
     abhaengig.

     Der Defaultwert ist 3. Ein hoeherer Wert entspricht einem teuren Laden,
     waehrend ein kleinerer Wert zu kleinen Preisen fuehrt.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Normalerweise kommt man ohne diese Funktion aus: in teuren Laeden wird
     nicht viel eingekauft (es sei denn, es liegen standardmaessig gute
     Objekte im Speicher), einem Billigladen wird bald das Geld ausgehen mit
     der Folge, dass der Ladenbesitzer nichts mehr ankaufen kann und sein
     Lager gaehnend leer ist.

SIEHE AUCH
----------
::

     QueryBuyFact(), /std/laden.c


Last modified: Wed May 8 10:24:52 1996 by Wargon


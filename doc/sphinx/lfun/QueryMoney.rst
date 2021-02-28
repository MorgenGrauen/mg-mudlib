QueryMoney()
============

FUNKTION
--------
::

     int QueryMoney()

DEFINIERT IN
------------
::

     /std/player/moneyhandler.c
     /std/container/moneyhandler.c

BESCHREIBUNG
------------
::

     Gibt den Geldbetrag zurueck, den ein Spieler, Gegenstand, Raum oder NPC
     besitzt.

RUECKGABEWERT
-------------
::

     Geldmenge im Besitz des abgefragten Spielers (Integer).

HINWEIS
-------
:: 

     Bei Spielern (bzw. allgemein Objekten, die /std/player/moneyhandler
     erben) beruecksichtigt die Funktion auch das Guthaben auf der
     Seherkarte.

     Nicht-Spieler-Objekte sollten /std/container/moneyhandler erben.

     Zur Ermittlung des Geldobjekts, des Geldboersenobjekts oder des 
     Seherkartenobjekts z.B. mittels present() oder present_clone() stehen in
     <money.h> einige Defines zur Verfuegung, die hierfuer bevorzugt zu
     benutzen sind.

BEISPIELE
---------
::

     int i = 50+random(10);
     int money = this_player()->QueryMoney();
     if(!money)
       write("Du besitzt keine Muenzen!\n");
     else if(money < i)
       write("Du besitzt nicht die erforderlichen "+i+" Muenzen.\n");

SIEHE AUCH
----------

   Geldhandling: :doc:`AddMoney`

   Zentralbank:  :doc:`PayIn`, :doc:`WithDraw`, :doc:`_query_current_money`

   Sonstiges:    /items/money.c

Last modified: 2021-02-28, Arathorn

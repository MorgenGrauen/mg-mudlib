AddMsg()
========

FUNKTION
--------
::

     void AddMsg(string msg, int next);

DEFINIERT IN
------------
::

     /std/transport.c

ARGUMENTE
---------
::

     msg
          Die auszugebende Meldung.
     next
          Zeit bis zur naechsten Fahrplanstation.

BESCHREIBUNG
------------
::

     Dem Fahrplan wird die Ausgabe einer Meldung an den Transporter
     hinzugefuegt. Diese Meldung koennte zum Beispiel das Nahen der
     naechsten Haltestelle ankuendigen o.ae. Nach Ausgabe der Meldung
     vergehen next Sekunden, bis die naechste Station angefahren wird.

     Um das Umbrechen der Meldung (normalerweise auf 78 Zeichen pro Zeile)
     muss sich der Aufrufer selber kuemmern.

RUeCKGABEWERT
-------------
::

     keiner

BEISPIELE
---------
::

     AddMsg("In der Ferne taucht eine kleine Inseln auf.\n", 10);
     AddMsg("Das Schiff steuert einen kleinen Steg an.\n");
     AddRoute(...);

     Nach der Ankuendigung der Insel vergehen 10 Sekunden, bis die naechste
     Meldung ausgegeben wird. Da bei der zweiten Meldung keine Zeit
     angegeben war, legt das Schiff direkt nach der Ausgabe der Meldung an.

SIEHE AUCH
----------
::

     AddRoute(), AddFun(), /std/transport.c


25.01.2015, Zesstra


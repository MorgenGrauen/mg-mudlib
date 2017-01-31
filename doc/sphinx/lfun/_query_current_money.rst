_query_current_money()
======================

FUNKTION
--------
::

     int _query_current_money()

DEFINIERT IN
------------
::

     /p/daemon/zentralbank.c

BESCHREIBUNG
------------
::

     Es wird zurueckgegeben, wieviel Geld die Zentralbank besitzt.

BEISPIELE
---------
::

     #include <bank.h>
     ...
     if(ZENTRALBANK->_query_current_money()<30000) {
      write(break_string(
       "Leider koennen wir ihren Bausparvertrag derzeit nicht einloesen.",
       "Der Beamte sagt: ",78));
      say(break_string(
       "Leider koennen wir ihren Bausparvertrag derzeit nicht einloesen.",
       "Der Beamte sagt zu "+this_player()->name(WEM)+": ",78));
     }

BEMERKUNGEN
-----------
::

     Unsere Zentralbank ist korrupt, vor allem dadurch, dass in Laeden und
     an anderen Stellen Geld erzeugt wird.

SIEHE AUCH
----------
::

     Geldhandling:	AddMoney(L), QueryMoney(L)
     Zentralbank:	WithDraw(L), PayIn(L)
     Sonstiges:		/items/money.c, /sys/bank.h

27. Apr 2004 Gloinson


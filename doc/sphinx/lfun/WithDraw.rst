WithDraw()
==========

FUNKTION
--------
::

     int WithDraw(int amount);

DEFINIERT IN
------------
::

     /p/daemon/zentralbank.c

ARGUMENTE
---------
::

     int amount	- angeforderte Geldmenge

BESCHREIBUNG
------------
::

     Damit wird bei der Zentralbank eine bestimmte Menge Geld angefordert.
     Der Rueckgabewert haengt vom Kassenstand der Zentralbank ab und ist
     entweder amount oder amount/3.

RUeCKGABEWERT
-------------
::

     Menge des bei der Zentralbank "abgehobenen" Geldes.

BEISPIELE
---------
::

     #include <bank.h>
     ...
     if(ZENTRALBANK->WithDraw(50000)<50000)
      write(break_string(
       "Leider koennen wir ihnen keinen vollen Zuschuss zu ihrem Hotelbau "
       "geben!",
       "Der Beamte sagt: ",78));
      say(break_string(
       "Leider koennen wir ihnen keinen vollen Zuschuss zu ihrem Hotelbau "
       "geben!",
       "Der Beamte sagt zu "+this_player()->name(WEM)+": ",78));
      ...

BEMERKUNGEN
-----------
::

     Unsere Zentralbank ist korrupt, vor allem dadurch, dass in Laeden und
     an anderen Stellen Geld erzeugt wird.

SIEHE AUCH
----------
::

     Geldhandling:	AddMoney(L), QueryMoney(L)
     Zentralbank:	PayIn(L), _query_current_money(L)
     Sonstiges:		/items/money.c, /sys/bank.h

27. Apr 2004 Gloinson


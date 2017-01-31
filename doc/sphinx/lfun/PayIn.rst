PayIn()
=======

FUNKTION
--------
::

     varargs void PayIn(int amount, int percent);

DEFINIERT IN
------------
::

     /p/daemon/zentralbank.c

ARGUMENTE
---------
::

     int amount	-	einzuzahlender Betrag
     int percent -	Bewertungsprozentsatz

BESCHREIBUNG
------------
::

     Es wird Brutto amount Geld in die Bank eingezahlt. Der Prozentsatz legt
     fest, wieviel tatsaechlich gutgeschrieben wird:
     Gutschrift = amount*percent/100

     Wird percent nicht angegeben, dann wird der derzeitige Bankbewertungs-
     massstab fuer Geld angenommen.

BEISPIELE
---------
::

     #include <bank.h>
     ...
     AddCmd("spende",#'action_spende,
	    "Was willst du spenden?");
     ...
     int action_spende(string str, extra *o) {
      int i;
      if(sscanf("%d muenze",i)==1 && i>0)
       if(this_player()->QueryMoney(i) && this_player()->AddMoney(-i)) {
        write("Du spendest "+i+" Muenzen.\n");
	say(this_player()->Name(WER)+" spendet "+i+" Muenzen.\n");
	ZENTRALBANK->PayIn(i);

	

       } else
        write("Soviel hast du nicht dabei!\n");
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
     Zentralbank:	WithDraw(L), _query_current_money(L)
     Sonstiges:		/items/money.c, /sys/bank.h

27. Apr 2004 Gloinson


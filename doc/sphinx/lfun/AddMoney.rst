AddMoney()
==========

AddMoney(L)
-----------
::

FUNKTION
--------
::

     public int AddMoney(int amount);

DEFINIERT IN
------------
::

     /std/container/moneyhandler.c
     /std/living/moneyhandler.c
     /std/player/moneyhandler.c

ARGUMENTE
---------
::

     int amount
         Die zufuehrende oder abziehende Geldmenge

BESCHREIBUNG
------------
::

     Dem Spieler wird die in <amount> festgelegte Geldmenge abgezogen oder
     zugefuehrt.

RUeCKGABEWERT
-------------
::

     Technisch gesehen wird Geld mit entsprechendem <amount> erzeugt
     ("/items/money.c") und mittels "move" in den Spieler bewegt.  Das Ergebnis
     dieses "move"-Aufrufes wird hier uebergeben, z.B. 1 fuer OK.
     Die moeglichen Fehler-Konstanten sind in /sys/moving.h definiert, siehe
     auch Dokumentation zu "move".

BEMERKUNGEN
-----------
::

     <amount> kann sowohl positiv als auch negativ sein. Welche Auswirkungen
     beide Faelle haben, sollte klar sein. Doch sollte bei einem negativen
     <amount> vorher mittels QueryMoney() abgefragt werden, ob der Spieler
     auch ueber ausreichend Geld verfuegt.
     Wird dem Spieler Geld abgezogen, ist darauf zu achten, dieses in der
     Zentralbank einzuzahlen (s.a.:PayIn() ). 
     Verschafft man dem Spieler Geld aus dem Nichts, muss es vorher bei der
     Zentralbank abgebucht (WithDraw()) werden.

     Achtung: Kann der Spieler die in <amount> angebene Geldmenge nicht
	      tragen, werden ihm keine Muenzen in sein Inventar bewegt.  Die
	      Fehlermeldung erkennt man an dem Rueckgabewert ME_TOO_HEAVY.

     Im Gegensatz zu Spielern haben alle anderen Objekte (Raeume, NPC, etc.)
     standardmaessig keinen Moneyhandler. In diesem Fall muss in Lebewesen
     "/std/living/moneyhandler"
     und in nicht-Lebewesen
     "/std/container/moneyhandler"
     geerbt werden.

BEISPIELE
---------
::

     // gib ihm Geld
     this_player()->AddMoney(50);

     // nimm ihm Geld
     if(this_player()->AddMoney(-50)==1)
      write("Der Ork beklaut dich!\n");

SIEHE AUCH
----------
::

     Geldhandling:	QueryMoney(L)
     Zentralbank:	PayIn(L), WithDraw(L)
     Sonstiges:		move(L),
			/items/money.c, /sys/moving.h, /sys/money.h, /sys/bank.h
			/std/container/moneyhandler.c

18.02.2013, Zesstra


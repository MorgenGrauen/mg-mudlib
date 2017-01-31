P_WEAR_MSG
==========

NAME
----
::

    P_WEAR_MSG                       "wear_msg"                       

DEFINIERT IN
------------
::

    /sys/armour.h

BESCHREIBUNG
------------
::

     Zweiteiliges Array mit Meldungen, die beim Anziehen einer Ruestung oder
     Kleidung an den Spieler und die Umgebung ausgegeben werden.
     Der erste Eintrag geht an den Spieler, der zweite Eintrag an die
     Umgebung. Zeilenumbrueche werden automatisch gemacht, existierende
     jedoch beruecksichtigt.

     Platzhalter fuer Spieler ist @WExxx1, fuer die Waffe @WExxx2 (siehe
     man replace_personal()).

     [Wegen Abwaertskompatibilitaet ist auch noch der Platzhalter %s
      moeglich, wobei in der eigenen Meldung %s fuer den Waffennamen steht,
      in der an den Raum das erste %s fuer den Spielernamen, das zweite fuer
      den Waffennamen.]

BEISPIELE
---------
::

    SetProp(P_NAME, "Helm");
    SetProp(P_WEAR_MSG,
     ({"Du stuelpst die @WEN2 ueber.", 
       "@WER1 stuelpt sich @WENU2 ueber."}));

    -> beim Anziehe durch Urk:
       Urk bekommt: Du stuelpst dir den Helm ueber.
       Der Raum:    Urk stuelpt sich einen Helm ueber.

    SetProp(P_WEAR_MSG,
     ({"Als Du Dir den langen Mantel ueberziehst, steckst Du erstmal "
       "mit Deinem dicken Schaedel fest. Doch nach einem kraeftigen "
       "Ruck bist Du endlich durch und siehst wieder etwas.",
       "@WER1 zieht sich einen langen Mantel ueber und bleibt "
       "prompt mit dem dicken Schaedel stecken. Doch nach einem "
       "kraeftigen Ruck kann @WERQP1 wieder etwas sehen und grinst Dich "
       "verlegen an."}));

    -> beim Anziehen durch Urk:
       Urk bekommt: Als Du Dir den langen Mantel ueberziehst, steckst Du
		    erstmal mit Deinem dicken Schaedel fest. Doch nach einem
		    kraeftigen Ruck bist Du endlich durch und siehst wieder
		    etwas.

       Der Raum:    Urk zieht sich einen langen Mantel ueber und bleibt
		    prompt mit dem dicken Schaedel stecken. Doch nach
		    einem kraeftigen Ruck kann er wieder etwas sehen und
		    grinst Dich verlegen an.

SIEHE AUCH
----------
::

     Aehnliches: P_UNWEAR_MSG, P_WIELD_MSG, P_UNWIELD_MSG
                 P_DROP_MSG, P_PUT_MSG, P_GIVE_MSG, P_PICK_MSG
     Funktionen: WearFunc, UnwearFunc, InformWear()
     Sonstiges:  replace_personal(E), clothing, /std/clothing/wear.c
                 armour, /std/armour/wear.c

LETZTE AeNDERUNG
----------------
::

15.02.2009


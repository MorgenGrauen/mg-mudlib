P_UNWEAR_MSG
============

NAME
----
::

     P_UNWEAR_MSG                       "unwear_msg"     

DEFINIERT IN
------------
::

     /sys/armour.h

BESCHREIBUNG
------------
::

     Zweiteiliges Array mit Meldungen, die beim Ausziehen einer Ruestung 
     oder Kleidung an den Spieler und die Umgebung ausgegeben werden.
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

    SetProp(P_NAME, "Mantel");
    SetProp(P_UNWEAR_MSG,
     ({"Du reisst Dir @WEN2 vom Leib.",
       "@WER1 reisst sich @WENU2 vom Leib." }));

    -> beim Ausziehen durch Urk:
       Urk bekommt: Du reisst dir den Mantel vom Leib.
       Der Raum:    Urk reisst sich einen Mantel vom Leib.

    SetProp(P_UNWEAR_MSG,
     ({"Dir wird furchtbar warm. So eine Hitze aber auch. Schnell "
       "schluepfst Du aus Deiner dicken Ruestung. Aaaah, was fuer "
       "eine Wohltat.",
       "@WEM1 scheint ploetzlich warm zu werden. Schnell schluepft "
       "@WERQP1 aus @WEMQPPFS1 dicken Ruestung. Du hoffst instaendig, "
       "das es noch etwas waermer wird ... "}));

    -> beim Ausziehen durch Urk:
       Urk bekommt: Dir wird furchtbar warm. So eine Hitze aber auch.
		    Schnell schluepfst Du aus Deiner dicken Ruestung.
		    Aaaah, was fuer eine Wohltat.
       Der Raum:    Urk scheint ploetzlich warm zu werden. Schnell
		    schluepft er aus seiner dicken Ruestung. Du hoffst
		    instaendig, das es noch etwas waermer wird ...

SIEHE AUCH
----------
::

     Aehnliches: P_WEAR_MSG, P_WIELD_MSG, P_UNWIELD_MSG
                 P_DROP_MSG, P_PUT_MSG, P_GIVE_MSG, P_PICK_MSG
     Funktionen: WearFunc, UnwearFunc
     Sonstiges:  replace_personal(E), /std/armour/wear.c, armours
                 clothing, /std/clothing.wear.c

LETZTE AeNDERUNG
----------------
::

15.02.2009, Zesstra


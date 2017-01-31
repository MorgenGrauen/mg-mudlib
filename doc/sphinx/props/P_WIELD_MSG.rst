P_WIELD_MSG
===========

NAME
----
::

     P_WIELD_MSG                       "wield_msg" 

DEFINIERT IN
------------
::

     /sys/weapon.h

BESCHREIBUNG
------------
::

     Zweiteiliges Array mit Meldungen, die beim Zuecken einer Waffe an den
     Spieler und die Umgebung ausgegeben werden.
     Der erste Eintrag geht an den Spieler, der zweite Eintrag an die
     Umgebung.  Zeilenumbrueche werden automatisch gemacht, existierende
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

    SetProp(P_NAME, "Streitkolben");
    SetProp(P_WIELD_MSG,
     ({"Du zueckst @WEN2 und stoesst einen markerschuetternden Schrei aus.", 
       "@WER1 zueckt @WENU2 und stoesst einen markerschuetternden Schrei "
       "aus." }));

    -> beim Zuecken durch Urk:
       Urk bekommt: Du zueckst den Streitkolben und stoesst einen
		    markerschuetternden Schrei aus.
       Der Raum:    Urk zueckt einen Streitkolben und stoesst einen
		    markerschuetternden Schrei aus.

    SetProp(P_WIELD_MSG,
     ({"Du zueckst den klobigen Streitkolben und fuchtelst damit "
       "wild vor Deiner Nase herum.",
       "@WER1 zueckt einen klobigen Streitkolben und fuchtelt "
       "damit wild vor der eigenen Nase herum. Hoffentlich verletzt "
       "@WERQP1 sich dabei nicht ..."}));

    -> beim Zuecken durch Urk:
       Urk bekommt: Du zueckst den klobigen Streitkolben und fuchtelst
                    damit wild vor Deiner Nase herum.
       Der Raum:    Urk zueckt einen klobigen Streitkolben und fuchtelt
		    damit wild vor der eigenen Nase herum. Hoffentlich
                    verletzt er sich dabei nicht ...

SIEHE AUCH
----------
::

     Aehnliches: P_UNWIELD_MSG, P_WEAR_MSG, P_UNWEAR_MSG
                 P_DROP_MSG, P_PUT_MSG, P_GIVE_MSG, P_PICK_MSG
     Funktionen: UnwieldFunc, WieldFunc
     Sonstiges:  replace_personal(E), /std/weapon/combat.c

29. Maerz 2004 Gloinson


P_TOO_HEAVY_MSG
===============

NAME
----
::

    P_TOO_HEAVY_MSG                      "too_heavy_msg"                      

DEFINIERT IN
------------
::

    /sys/thing/moving.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt eine Meldung, die ausgegeben wird, wenn jemand
     versucht, ein Objekt in einen Behaelter zu legen, fuer den dieses Objekt
     zu schwer ist.
     Die Property ist im Behaelter zu setzen.
     Ist diese Property nicht oder auf einen nicht-String-Wert gesetzt,
     so wird die Standardmeldung ausgegeben.
     ("<Objekt> passt in <Behaelter> nicht mehr rein.")
     Der String in der Property wird noch durch replace_personal()
     verarbeitet, das zu bewegende Objekt wird als erstes, der Behaelter als
     zweites Objekt angegeben. Danach wird der String auf 78 Zeichen
     umgebrochen.
     Das Setzen eines leeren Strings unterdrueckt die Ausgabe einer Meldung
     ganz.

BEISPIELE
---------
::

     SetProp(P_TOO_HEAVY_MSG, "Wenn du @WEN1 noch in den Beutel stecken"
			      " wuerdest, wuerde er reissen.");

SIEHE AUCH
----------
::

     Aehnliches: P_ENV_TOO_HEAVY_MSG, P_TOO_MANY_MSG, P_NOINSERT_MSG,
                 P_NOLEAVE_MSG, P_NODROP, P_NOGET 
     Erfolg:     P_PICK_MSG, P_DROP_MSG, P_GIVE_MSG, P_PUT_MSG,
                 P_WEAR_MSG, P_WIELD_MSG
     Sonstiges:  replace_personal(E), /std/living/put_and_get.c


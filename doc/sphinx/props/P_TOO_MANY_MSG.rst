P_TOO_MANY_MSG
==============

NAME
----
::

    P_TOO_MANY_MSG                      "too_many_msg"                      

DEFINIERT IN
------------
::

    /sys/thing/moving.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt eine Meldung, die ausgegeben wird, wenn jemand
     versucht, ein Objekt in einen Behaelter zu legen, der schon die maximale
     Anzahl an Objekten enthaelt.
     Die Property ist im Behaelter zu setzen.
     Ist diese Property nicht oder auf einen nicht-String-Wert gesetzt,
     so wird die Standardmeldung ausgegeben.
     ("Dafuer ist nicht mehr genug Platz in <Behaelter>.")
     Der String in der Property wird noch durch replace_personal()
     verarbeitet, das zu bewegende Objekt wird als erstes, der Behaelter als
     zweites Objekt angegeben. Danach wird der String auf 78 Zeichen
     umgebrochen.
     Das Setzen eines leeren Strings unterdrueckt die Ausgabe einer Meldung
     ganz.

BEISPIELE
---------
::

     SetProp(P_TOO_MANY_MSG, "Aber der Korb hat doch nur drei Faecher, die"
			     " sind alle schon voll.");

SIEHE AUCH
----------
::

     Aehnliches: P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_NOINSERT_MSG,
                 P_NOLEAVE_MSG, P_NODROP, P_NOGET 
     Erfolg:     P_PICK_MSG, P_DROP_MSG, P_GIVE_MSG, P_PUT_MSG,
                 P_WEAR_MSG, P_WIELD_MSG
     Sonstiges:  replace_personal(E), /std/living/put_and_get.c


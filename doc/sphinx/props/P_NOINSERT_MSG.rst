P_NOINSERT_MSG
==============

NAME
----
::

    P_NOINSERT_MSG                      "noinsert_msg"                      

DEFINIERT IN
------------
::

    /sys/thing/moving.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt eine Meldung, die ausgegeben wird, wenn
     jemand versucht, ein Objekt in einen Behaelter zu bewegen und der
     Behaelter dieses im PreventInsert() verhindert.
     Die Property ist im Zielbehaelter zu setzen.
     Ist diese Property nicht oder auf einen nicht-String-Wert gesetzt,
     so wird die Standardmeldung ausgegeben.
     ("<Objekt> kannst Du dort nicht hineinstecken.")
     Der String in der Property wird noch durch replace_personal()
     verarbeitet, das zu bewegende Objekt wird als erstes, der Zielbehaelter
     als zweites Objekt angegeben. Danach wird der String auf 78 Zeichen
     umgebrochen.
     Das Setzen eines leeren Strings unterdrueckt die Ausgabe einer Meldung
     ganz.

BEISPIELE
---------
::

     1. Ein Kochtopf laesst im PreventInsert nur bestimmte Objekte zu, die zu
     einer Suppe gehoeren. Fuer eine passende Meldung wird im Topf jetzt die
     Property gesetzt:
     SetProp(P_NOINSERT_MSG, "Du kannst @WEN1 nicht in den Kochtopf tun, da"
	                     " gehoeren doch nur Suppenzutaten rein!");
     Wenn jemand jetzt versucht, eine Muenze reinzustecken, dann wuerde
     folgende Meldung erscheinen:
	Du kannst die Muenze nicht in den Kochtopf tun, da gehoeren doch nur
	Suppenzutaten rein!

     2. Ein Rucksack soll in einer bestimmten Reihenfolge gepackt werden, dazu
     kann im PreventInsert die Meldung je nach Bedarf gesetzt werden:
     if (<objekt noch nicht an der Reihe>)
	SetProp(P_NOINSERT_MSG, "@WEN1 solltest du erst spaeter einpacken.");
     else if (<objekt schon im Rucksack>)
	SetProp(P_NOINSERT_MSG, "Aber @WER1 ist doch schon eingepackt!");
     else ...

SIEHE AUCH
----------
::

     Aehnliches: P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_TOO_MANY_MSG,
                 P_NOLEAVE_MSG, P_NODROP, P_NOGET 
     Erfolg:     P_PICK_MSG, P_DROP_MSG, P_GIVE_MSG, P_PUT_MSG,
                 P_WEAR_MSG, P_WIELD_MSG
     Sonstiges:  replace_personal(E), /std/living/put_and_get.c


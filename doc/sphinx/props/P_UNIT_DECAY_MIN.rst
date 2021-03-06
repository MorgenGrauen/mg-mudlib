P_UNIT_DECAY_MIN
================

NAME
----
::

     P_UNIT_DECAY_MIN					                    "unit_decay_min"

DEFINIERT IN
------------
::

     /sys/unit.h

BESCHREIBUNG
------------
::

     Diese Prop bestimmt, wieviele Einheiten der Unitobjekten mindestens
     uebrig bleiben sollen. 
     Faellt die Menge eines Unitobjekts unter diesen Wert, zerfaellt diese
     Unit solange nicht weiter, bis der Wert wieder ueberschritten wird.
     Die Prop kann in der Blueprint und in den einzelnen Clones gesetzt
     werden.
     Ist die Prop in einem einzelnen Clone nicht explizit gesetzt,
     liefert ein QueryProp(P_UNIT_DECAY_MIN) den in der Blueprint
     eingestellten Wert zurueck und die Unit zerfaellt bis zu dieser
     Mindestmenge..
     D.h. man sollte diese Prop in der Blueprint setzen und in einzelnen
     Clones nur soweit diese abweichende Werte haben sollen.
     Es sind nur Werte zwischen 0 und 100 zulaessig. Auf diese Art laesst sich
     die minidestens uebrig bleibende Menge aller Clones durch Aendern einer
     Prop in der Blueprint aendern.

BEMERKUNGEN
-----------
::

     * Setzt man diese Prop in einem Clone auf 0, wird der Wert aus er
       Blueprint zum Zerfall benutzt.
     * Will man fuer ein bestimmtes Unitobjekt kein Minimum haben, also dass
       dieses Objekt zerfaellt, bis nichts mehr da ist, die Blueprint hat aber
       einen Minimalwert gesetzt, sollte diese Prop im betreffenden Objekt auf
       -1 gesetzt werden.
     * Diese Prop sollte vorsichtig angewandt werden, da Spieler so den
       Zerfall von Units stoppen koennen, indem sie die Units entsprechend
       aufteilen, so dass jedes Einzelobjekt unter dem Minimum liegt.

BEISPIEL
--------
::

     // es soll min. 1 Einheit uebrig bleiben.
     SetProp(P_UNIT_DECAY_MIN, 1);

     // die Blueprint hat ein Minimum von 10 gesetzt, dieser Clone soll
     // aber zerfallen, bis nix mehr da ist.
     klon->SetProp(P_UNIT_DECAY_MIN, -1);

SIEHE AUCH
----------
::

     unit
     P_UNIT_DECAY_INTERVAL, P_UNIT_DECAY_FLAGS, P_UNIT_DECAY_QUOTA
     DoDecay, DoDecayMessage
     /std/unit.c

14.10.2007, Zesstra


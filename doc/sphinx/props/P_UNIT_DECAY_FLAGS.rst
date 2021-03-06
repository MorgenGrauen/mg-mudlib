P_UNIT_DECAY_FLAGS
==================

NAME
----
::

     P_UNIT_DECAY_FLAGS					"unit_decay_flags"

DEFINIERT IN
------------
::

     /sys/unit.h

BESCHREIBUNG
------------
::

     Mit dieser Prop kann das Zerfallsverhalten gesteuert werden, entweder
     fuer alle Clones durch Setzen in der Blueprint oder fuer einzelne Clones.

     In dieser Prop koennen momentan 4 Flags gesetzt werden:
     - NO_DECAY: 
          Zerfall ist abgeschaltet.
     - NO_DECAY_UNTIL_MOVE: 
          Der Zerfall ist solange ausgesetzt, bis dieses Objekt in ein anderes
          Env bewegt wird. Setzt also ein NPC beim AddItem() diese Prop,
          zerfaellt seine Unit nicht, bis sie bewegt wurde (Leiche, Spieler
          etc.). Hierbei zaehlt das move() nicht, wenn das Objekt noch kein
          Env hatte, es zaehlen nur Moves von einem Env in ein anderes Env.
          Dieses Flag sollte nur in Clones gesetzt werden.
     - INACCURATE_DECAY
          Sollen z.b. 45.34 Einheiten zerfallen, wird der Rest von 0.34
          normalerweise als Wahrscheinlichkeit aufgefasst, dass eine
          zusaetzliche Einheit zerfaellt. Dieses Flag sorgt dafuer, dass
          dieser Rest weggeworfen wird und einfach 45 Einheiten zerfallen.
          Gleichzeitig wird bei diesem Flag aber _immer min_ 1 Einheit
          zerstoert!
     - ABSOLUTE_DECAY
          P_UNIT_DECAY_QUOTA wird nicht als prozentualer Anteil aufgefasst,
          sondern als absolute Zahl, d.h. es zerfallen immer einfach
          P_UNIT_DECAY_QUOTA Einheiten.

     Diese Flags koennen z.B. genutzt werden, den Zerfall fuer einzelne
     Objekte temporaer oder dauerhaft abzuschalten, auch wenn alle anderen
     Clones weiterzerfallen.

     Diese Prop kann in der Blueprint gesetzt werden. In diesem Fall wird
     allerdings NO_DECAY_UNTIL_MOVE ignoriert, weil die BP ja nie bewegt
     wuerde. NO_DECAY in der BP schaltet den Zerfallsprozess (temporaer) fuer
     alle Clones aus. Ist nie ein Zerfall gewuenscht, sollte in der Blueprint
     aber besser P_UNIT_DECAY_INTERVAL auf 0 gesetzt werden!

     Ist die Prop in einem einzelnen Clone nicht explizit gesetzt,
     liefert ein klon->QueryProp(P_UNIT_DECAY_FLAGS) den in der Blueprint
     eingestellten Wert zurueck.

     

BEMERKUNGEN
-----------
::

     * Setzt man diese Prop in einem Clone auf 0, wird der Wert aus der
       Blueprint zurueckgeben. Hierbei wird allerdings ein NO_DECAY_UNTIL_MOVE
       ausgefiltert, da dies den Zerfall fuer alle Objekte dauerhaft stoppen
       wuerde, weil BPs nicht bewegt werden.
     * Die Flags koennen "verodert" werden:
       SetProp(P_UNIT_DECAY_FLAGS, NO_DECAY_UNTIL_MOVE | ABSOLUTE_DECAY);

BEISPIEL
--------
::

     // Dieser NPC hat tolle Pfeile, die sollen aber nicht zerfallen, solange
     // sie im Inventar des NPCs sind:
     AddItem("/d/tolleregion/tollermagier/obj/pfeile", REFRESH_NONE,
         ([ P_AMOUNT: 50+random(50),
            P_UNIT_DECAY_FLAGS: NO_DECAY_UNTIL_MOVE ]) );

SIEHE AUCH
----------
::

     unit
     P_UNIT_DECAY_INTERVAL, P_UNIT_DECAY_QUOTA, P_UNIT_DECAY_MIN
     DoDecay, DoDecayMessage
     /std/unit.c

14.10.2007, Zesstra


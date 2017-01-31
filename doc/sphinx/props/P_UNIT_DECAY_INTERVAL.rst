P_UNIT_DECAY_INTERVAL
=====================

NAME
----
::

     P_UNIT_DECAY_INTERVAL					"unit_decay_interval"

DEFINIERT IN
------------
::

     /sys/unit.h

BESCHREIBUNG
------------
::

     Diese Prop bestimmt, wie oft ein Zerfall der entsprechenden Unitobjekte
     durchgefuehrt wird. Das Intervall ist in Sekunden anzugeben (int).
     Die Prop muss in der Blueprint der entsprechenden Unitobjekte gesetzt
     werden, in Clones kann sie nicht gesetzt werden.
     Die Blueprint resettet dann in diesem Intervall und ruft in allen ihren
     Clones (und denen alter Versionen der gleichen BP!) DoDecay() auf,
     woraufhin die Clones den Zerfall durchfuehren.
     Ist die Prop in der Blueprint nicht gesetzt, erfolgt kein Zerfall.

BEMERKUNGEN
-----------
::

     * Ist die Blueprint nicht geladen, erfolgt kein Zerfall der Clones.
     * Ein Setzen dieser Prop beinhaltet immer auch einen Aufruf von
       set_next_reset() auf das ensprechende Intervall.
     * Die Prop kann in den Clones abgefragt werden und liefert das in der
       Blueprint eingestellte Intervall.
     * Von einer Manipulation per Set() wird dringend abgeraten.
     * Die Prop kann nur vom Objekt selber, vom Programmierer des Objekts, vom
       RM der entsprechenden Region, von einem Weisen oder von einem Objekt
       gesetzt werden, welches die gleiche UID hat.

BEISPIEL
--------
::

SIEHE AUCH
----------
::

     unit
     P_UNIT_DECAY_QUOTA, P_UNIT_DECAY_FLAGS, P_UNIT_DECAY_MIN
     DoDecay(), DoDecayMessage()

13.10.2007, Zesstra


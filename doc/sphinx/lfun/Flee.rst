Flee()
======

FUNKTION
--------
::

        public varargs void Flee( object oldenv, int force )

DEFINIERT IN
------------
::

        /sys/living/combat.h
        /std/living/combat.c

ARGUMENTE
---------
::

        oldenv
          Ein Raum oder 0.
          Wird ein Raum angegeben, dann muss sich der Fluechtende in diesem
          Raum befinden, damit er versucht, zu fluechten, es sei denn, das
          optionale Flag "force" ist gesetzt.
        force
          1, wenn der spieler unabhaengig von seiner Vorsicht fluechten soll.
          0 sonst.

BESCHREIBUNG
------------
::

        Flee() wird im heart_beat() oder von CheckWimpyAndFlee() aufgerufen,
        um den Spieler fluechten zu lassen. Man kann die Funktion im Spieler
        auch "von Hand" aufrufen, beispielsweise in einem Spell. Man sollte
        dann force auf 1 setzen, damit der Spieler unabhaengig von seiner
        Vorsicht fluechtet.
        Hierbei kann die Flucht dazu fuehren, dass man die Teamreihe wechselt,
        aber auch, dass man den Raum verlaesst.

        

RUeCKGABEWERT
-------------
::

        keiner

BEMERKUNGEN
-----------
::

BEISPIELE
---------
::

        this_player()->Flee(0, 1);
        // Der Spieler soll fluechten, egal, ob seine Lebenspunkte geringer
        // als seine Vorsicht sind und unabhaengig von seiner Position.

        

        this_player()->Flee( find_object("/gilden/abenteurer") );
        // Der Spieler soll fluechten, wenn er sich in der Abenteurergilde
        // befindet (oder wenn diese nicht existiert)

        

        this_player()->Flee( "/gilden/abenteurer" );
        // Der Spieler wird nicht fluechten, da der Vergleich von Dateiname
        // und dem Raum 0 ergibt.

        this_player()->Flee( find_object("/gilden/abenteurer"), 1);
        // Der Spieler soll auf jeden Fall fluechten, egal ob er sich in der
        // Abenteurergilde befindet oder nicht. Grund: Gesetztes force-Flag.

        

        

        

SIEHE AUCH
----------
::

        CheckWimpyAndFlee(), Defend(), heart_beat(), 


Last modified: Wed Nov 12 14:44:42 2003 by Bambi


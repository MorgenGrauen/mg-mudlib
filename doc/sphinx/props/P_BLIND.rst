P_BLIND
=======

NAME
----
::

    P_BLIND                       "blind"                       

DEFINIERT IN
------------
::

    /sys/player/viewcmd.h

BESCHREIBUNG
------------
::

    1, wenn der Spieler blind ist und nichts mehr sehen kann.

    Wird von CannotSee() bei 'schau' und Betreten von Raeumen 
    u.ae. ausgewertet.

    P_BLIND kann auch auf einen String gesetzt werden, der 
    dann statt des 'Du bist blind' ausgegeben wird.

BEISPIELE
---------
::

    this_player()->SetProp(P_BLIND,1);

    this_player()->SetProp(P_BLIND,"Du hast Dir vorhin so schoen die "
                                  +"Augen ausgekratzt ... deswegen "
                                  +"siehst Du nun nichts mehr.\n");    

BEMERKUNGEN
-----------
::

    Um herauszufinden, ob ein Spieler etwas sehen kann oder nicht,
    sollte man lieber if(this_player()->CannotSee() != 0) pruefen
    statt if(this_player()->QueryProp(P_BLIND)).

    Denn CannotSee() beruecksichtigt auch Nachtsicht (bzw. hier 
    eine nicht aktivierte) und die Lichtmodifikatoren.

SIEHE AUCH
----------
::

    P_LIGHT_MODIFIER, P_PLAYER_LIGHT, CannotSee


Letzte Aenderung: Sa 02.11.02, 00:30:00 Uhr, von Tilly


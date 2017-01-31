P_LEAVEFAIL
===========

NAME
----
::

    P_LEAVEFAIL                   "leavefail"                   

DEFINIERT IN
------------
::

    /sys/transport.h

BESCHREIBUNG
------------
::

     Meldung an den Spieler, wenn er ausserhalb der Anlegezeiten einen 
     Transporter verlassen will. Ist die Propertie ein Array, so wird 
     das erste Element als Meldung an den Spieler, das zweite als 
     Meldung an die Mitspieler im Transporter geschickt. Ist der Eintrag
     dagegen ein simpler String, so wird die Meldung nur an den Spieler
     ausgegeben.

BEISPIEL
--------
::

     SetProp(P_LEAVEFAIL, "Die Wildgaense fliegen viel zu hoch" );

     SetProp(P_LEAVEFAIL, ({ "Die Wildgaense fliegen viel zu hoch",
                             "versucht, vom Ruecken der Wildgans zu "
                            +"klettern und besinnt sich dann doch" }) );

                             

SIEHE AUCH
----------
::

     P_LEAVEMSG, P_ENTERMSG, P_ENTERFAIL, transporter

LETZTE AENDERUNG
----------------
::

    Don, 24.01.2002, 10:15:07h von Tilly


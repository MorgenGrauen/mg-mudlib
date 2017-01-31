P_LEAVEMSG
==========

NAME
----
::

    P_LEAVEMSG                    "leavemsg"                    

DEFINIERT IN
------------
::

    /sys/transport.h

BESCHREIBUNG
------------
::

     Array mit zwei Meldungen. Der erste Eintrag wird an den Transporter
     ausgegeben, der zweite an den Raum in den der Spieler kommt.

BEISPIEL
--------
::

     SetProp(P_LEAVEMSG, ({ "klettert vom Ruecken der Wildgans",
                            "kommt vom Ruecken einer Wildgans herunter" }) );

SIEHE AUCH: 
     P_ENTERMSG, P_LEAVEFAIL, P_ENTERFAIL, transporter

LETZTE AENDERUNG
----------------
::

    Don, 24.01.2002, 10:15:07h von Tilly


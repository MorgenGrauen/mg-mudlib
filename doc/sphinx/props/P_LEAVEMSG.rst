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

     String-Array mit zwei Meldungen, die als Argumente 4 und 5 an move()
     uebergeben werden, d.h. die erste ist fuer den Transporter, den der 
     Spieler verlaesst, und die zweite fuer den Raum, in den er geht.

     Hat das Array keine zwei Elemente, werden Standardmeldungen ausgegeben,
     so als waere die Property auf ({"verlaesst", "kommt herein"}) gesetzt.
     Die Property hat jedoch keinen Default-Wert.

BEISPIEL
--------
::

     SetProp(P_LEAVEMSG, ({ "klettert vom Ruecken der Wildgans",
                            "kommt vom Ruecken einer Wildgans herunter" }) );

SIEHE AUCH
----------
::
     P_ENTERMSG, P_LEAVEFAIL, P_ENTERFAIL, transporter

LETZTE AENDERUNG
----------------
::
     2020-09-01 Arathorn


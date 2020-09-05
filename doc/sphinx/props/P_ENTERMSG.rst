P_ENTERMSG
==========

NAME
----
::

    P_ENTERMSG                    "entermsg"                    

DEFINIERT IN
------------
::

    /sys/transport.h

BESCHREIBUNG
------------
::

     String-Array mit zwei Meldungen, die als Argumente 4 und 5 an move()
     uebergeben werden, d.h. die erste ist fuer den Raum, den der Spieler
     verlaesst, und die zweite fuer den Transporter, in den er geht.

     Hat das Array keine zwei Elemente, werden Standardmeldungen ausgegeben,
     so als waere die Property auf ({"betritt", "kommt herein"}) gesetzt.
     Die Property hat jedoch keinen Default-Wert.

BEISPIEL
--------
::

     SetProp(P_ENTERMSG, ({ "klettert in die Kutsche","klettert herein" }) );

SIEHE AUCH
----------
::

     P_ENTERFAIL, P_LEAVEFAIL, P_LEAVEMSG, transporter

LETZTER AENDERUNG
-----------------
::

     2020-09-01 Arathorn

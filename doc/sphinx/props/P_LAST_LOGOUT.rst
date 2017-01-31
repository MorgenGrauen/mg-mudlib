P_LAST_LOGOUT
=============

NAME
----
::

    P_LAST_LOGOUT                 "last_logout"                 

DEFINIERT IN
------------
::

    /sys/player/base.h

BESCHREIBUNG
------------
::

     Zeitpunkt des letzten Logouts. Anhand dieser Zeit werden die Feindes-
     listen und in Abwesenheit eingefuegte Gegenstaende aktualisiert.

     Dieses Datum wird bei Magiern nicht aktualisiert, wenn sie unsichtbar
     sind/sich unsichtbar ausloggen.

BEMERKUNGEN
-----------
::

     Gegen aeussere Aenderung und Set/QueryMethoden geschuetzt.

SIEHE AUCH
----------
::

     P_LAST_LOGIN, P_INVIS, save_me, init, StopHuntFor

28. Jan 2013 Gloinson


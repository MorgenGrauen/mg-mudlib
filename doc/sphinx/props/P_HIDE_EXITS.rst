P_HIDE_EXITS
============

NAME
----
::

     P_HIDE_EXITS                  "hide_exits"

DEFINIERT IN
------------
::

     /sys/room/exits.h

BESCHREIBUNG
------------
::

     Ist diese Property in einem Raum auf einen Integerwert ungleich 0
     gesetzt, werden einem Spieler fuer diesen Raum keine Ausgaenge angezeigt.
     Auch der Text "Keine sichtbaren Ausgaenge." (oder so) kommt nicht.
     Alternativ kann man auch ein array von strings uebergeben. In diesem
     Fall werden all die Ausgaenge nicht angezeigt, deren Index in P_EXITS
     in dem array vorkommt.
     In diesem Fall wird ggf. der Text "Keine sichtbaren Ausgaenge."
     ausgegeben.

BEISPIEL
--------
::

     AddExit("raus", "/ganz/wo/anders");
     AddExit("weiter", "/in/der/naehe");

     // KEINE Ausgaenge anzeigen. Noch nicht mal, dass man keine sieht.
     SetProp(P_HIDE_EXITS, 1);

     // Der Ausgang weiter wird nicht angezeigt.
     SetProp(P_HIDE_EXITS, ({"weiter"}) );

     // Keinen Ausgang zeigen, aber den Text, dass keiner sichtbar, ausgeben.
     SetProp(P_HIDE_EXITS, ({"weiter", "raus"}) );

SIEHE AUCH
----------
::

     Aehnliches:	P_SHOW_EXITS
     Sonstiges:		AddExit(), GetExits(), int_long(), int_short()

25. Apr 1996 Highlander


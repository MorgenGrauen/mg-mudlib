P_DIE_MSG
=========

NAME
----
::

     P_DIE_MSG			"die_msg"

DEFINIERT IN
------------
::

     /sys/properties.h

BESCHREIBUNG
------------
::

     In dieser Property uebergibt man einen String, der ausgegeben wird, wenn
     das Lebewesen stirbt. Ist die Property nicht gesetzt, so wird als String
     benutzt:
	" faellt tot zu Boden.\n".

     Der Name des Lebewesens wird dem String vor der Ausgabe vorangestellt.
     Der Satzumbruch am Zeilenende und das Leerzeichen nach dem Namen des
     Lebewesens muss man selbst angegeben. Es sollte allerdings beachtet
     werden, dass ein Lebewesen, das durch Gift getoetet wird, eine spezielle
     nicht zu beeinflussende Meldung erhaelt. Es wird dann als String
     benutzt:
	" wird von Gift hinweggerafft und kippt um.\n".

BEISPIELE
---------
::

     Bei einem mitkaempfenden Schatten waere es eher unlogisch, wenn nach
     dessen 'Tod' eine Leiche zurueckbliebe. Eine logische Konsequenz waere
     folgende Meldung:
	SetProp(P_DIE_MSG," loest sich auf.\n");
	SetProp(P_NOCORPSE,1);

     Damit dann auch wirklich keine Leiche zurueckbleibt, wird zusaetzlich
     die Property P_NOCORPSE gesetzt.

SIEHE AUCH
----------
::

     Tod:		die(L)
     Todesmeldungen:	P_KILL_NAME, P_KILL_MSG, P_MURDER_MSG
			P_ZAP_MSG, P_ENEMY_DEATH_SEQUENCE
     Sonstiges:		P_CORPSE, P_NOCORPSE, /std/corpse.c


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


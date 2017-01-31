P_ZAP_MSG
=========

NAME
----
::

      P_ZAP_MSG			"zap_msg"

DEFINIERT IN
------------
::

      /sys/properties.h

BESCHREIBUNG
------------
::

      Die Property enthaelt ein dreielementiges Array mit den folgenden
      Eintraegen:
	  1.) Meldung, die der Magier beim Zappen bekommt.
	  2.) Meldung, die die Spieler im Raum beim Zappen bekommen.
	  3.) Meldung, die das Opfer beim Zappen bekommt.

      Mit @@wer@@, @@wessen@@, ... kann der Name des Opfers und mit @@ich@@
      der Name des Magiers in die Meldung eingewoben werden.

      Die Property ist in Magiern gesetzt und gilt nur dort.

SIEHE AUCH
----------
::

     Tod:		die(L)
     Todesmeldungen:	P_KILL_NAME, P_KILL_MSG, P_DIE_MSG, P_MURDER_MSG
			P_ENEMY_DEATH_SEQUENCE
     Sonstiges:		P_CORPSE, P_NOCORPSE, /room/death/death_room.c


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


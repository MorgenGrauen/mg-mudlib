P_GUILD_LEVELS
==============

NAME
----
::

	P_GUILD_LEVELS			"guild_levels"                

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Gildenproperty enthaelt ein Mapping mit den
	Stufenbeschraenkungen fuer die jeweiligen Gildenstufen. Als
	Schluessel dient der jeweilige Gildenlevel und die entsprechenden
	Eintraege sind wiederum Mappings, in welchen die Beschraenkungen
	angegeben sind.

BEMERKUNGEN
-----------
::

	Die Beschraenkungen fuer Level 1 sollten auch fuer die
	Eintrittsbeschraenkungen der Gilde gelten. Letztere kann man in der
	Property P_GUILD_RESTRICTIONS angeben.

BEISPIELE
---------
::

	Das folgende Beispiel zeigt ein paar moegliche Abfragen:
	  string check(object pl);
	  SetProp(P_GUILD_LEVELS,([1:([P_LEVEL:3,P_QP:100,SR_FUN:#'check]),
	                           2:([A_INT:10,P_LEVEL:25]),
	                           3:([A_INT:20,P_LEVEL:50])]));
	  string check(object pl)
	  { if(pl->QueryProp(P_MALE))
	      return 0;
	    return "Fuer Frauen ist das nichts!\n";
	  }

SIEHE AUCH
----------
::

	P_GUILD_LEVEL, P_GUILD_RESTRICTIONS, /std/restriction_checker.c


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


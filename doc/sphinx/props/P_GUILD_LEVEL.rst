P_GUILD_LEVEL
=============

NAME
----
::

	P_GUILD_LEVEL			"guild_level"                 

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Property enthaelt die Gildenstufe des Lebewesens, welche nicht
	unbedingt seiner Spielerstufe entspricht.

BEMERKUNGEN
-----------
::

	Bei manchen Gilden entspricht die Gildenstufe standardmaessig der
	Spielerstufe (Abenteurer, Katzenkrieger). In der Property selbst
	wird eigentlich ein Mapping eingetragen, welches die Gildennamen als
	Schluessel und die dazugehoerige Gildenstufe als Eintrag enthaelt.
	Bei der Abfrage der Property wird jedoch die Gilde beruecksichtigt
	und die aktuelle Gildenstufe zurueckgeliefert.

SIEHE AUCH
----------
::

	P_GUILD, P_GUILD_TITLE, P_GUILD_MALE_TITLES, P_GUILD_FEMALE_TITLES


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


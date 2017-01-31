P_DEFAULT_GUILD
===============

NAME
----
::

	P_DEFAULT_GUILD			"default_guild"

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Property enthaelt den Namen der Gilde, welcher der Spieler
	standardmaessig angehoert. Der Name wird hierbei in Form eines
	kleingeschriebenen Strings angegeben. Ist P_GUILD gleich Null, so
	wird bei der Abfrage selbiger Property hierfuer der Eintrag von
	P_DEFAULT_GUILD eingesetzt.

BEMERKUNGEN
-----------
::

	Nach dem ersten Einloggen des Spielers wird ebenfalls dieser Eintrag
	genutzt, um die Gildenzugehoerigkeit festzulegen. Dies kann dazu
	genutzt werden, um eine rassenabhaengige Bestimmung der
	Standardgilde zu gewaehrleisten
	 (Felinen -> Katzenkrieger, andere Rassen -> Abenteurer).

SIEHE AUCH
----------
::

	P_GUILD, P_VISIBLE_GUILD


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


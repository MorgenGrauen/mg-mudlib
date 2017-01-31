P_GUILD_FEMALE_TITLES
=====================

NAME
----
::

	P_GUILD_FEMALE_TITLES		"guild_female_titles"         

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Gildenproperty enthaelt die Stufenbezeichnungen fuer
	weibliche Gildenmitglieder. Als Schluessel dienen hierbei die
	Gildenstufen und die entsprechenden Eintraege sind dann die zur
	Stufe gehoerigen Gildentitel.

BEISPIELE
---------
::

	Eine Programmierergilde koennte folgende Stufenbezeichnungen
	beinhalten:
	  SetProp(P_GUILD_FEMALE_TITLES,([1:"die Anfaengerin",
	                                  2:"die Fortgeschrittene",
	                                  3:"die Professionelle ;)"]));

SIEHE AUCH
----------
::

	P_GENDER, P_GUILD_LEVEL, P_GUILD_TITLE, P_GUILD_MALE_TITLES


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


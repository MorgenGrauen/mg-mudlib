P_GUILD_MALE_TITLES
===================

NAME
----
::

	P_GUILD_MALE_TITLES		"guild_male_titles"           

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Gildenproperty enthaelt die Stufenbezeichnungen fuer
	maennliche Gildenmitglieder. Als Schluessel dienen hierbei die
	Gildenstufen und die entsprechenden Eintraege sind dann die zur
	Stufe gehoerigen Gildentitel.

BEISPIELE
---------
::

	Eine Programmierergilde koennte folgende Stufenbezeichnungen
	beinhalten:
	  SetProp(P_GUILD_MALE_TITLES,([1:"der Anfaenger",
	                                2:"der Fortgeschrittene",
	                                3:"der Profi"]));

SIEHE AUCH
----------
::

	P_GENDER, P_GILD_LEVEL, P_GUILD_TITLE, P_GUILD_FEMALE_TITLES


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


P_GUILD_TITLE
=============

NAME
----
::

     P_GUILD_TITLE			"guild_title"                 

DEFINIERT IN
------------
::

     /sys/new_skills.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt den Gildentitel des Lebewesens, welcher der
     Gildenstufe des Lebewesens entspricht und jedoch nur angezeigt wird,
     wenn P_TITLE des Lebewesens gleich Null ist.

BEMERKUNGEN
-----------
::

     In der Property selbst wird eigentlich ein Mapping eingetragen, welches
     die Gildennamen als Schluessel und die dazugehoerigen Gildentitel als
     Eintrag enthaelt. Bei der Abfrage der Property wird jedoch die Gilde
     beruecksichtigt und der aktuelle Gildentitel zurueckgeliefert.

BEISPIELE
---------
::

	Fuer eine Programmierergilde waere folgendes denkbar:
	  SetProp(P_GUILD_MALE_TITLES,([1:"der Anfaenger",
	                                2:"der Fortgeschrittene",
	                                3:"der Profi"]));
	  SetProp(P_GUILD_FEMALE_TITLES,([1:"die Anfaengerin",
	                                  2:"die Fortgeschrittene",
	                                  3:"die Professionelle"]));
	Ein weibliches Gildenmitglied mit der dritten Gildenstufe und ohne
	P_TITLE wuerde dann den Titel "die Professionelle" tragen. Das hat
	zwar nichts mit Programmierern zu tun, aber wie heisst eigentlich
	ein weiblicher "Profi"?! :)

SIEHE AUCH
----------
::

     P_TITLE, P_GUILD, P_GUILD_LEVEL,
     P_GUILD_MALE_TITLES, P_GUILD_FEMALE_TITLES,
     P_SUBGUILD_TITLE

26. Maerz 2004 Gloinson


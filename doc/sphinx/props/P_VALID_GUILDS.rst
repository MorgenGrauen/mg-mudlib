P_VALID_GUILDS
==============

NAME
----
::

	P_VALID_GUILDS			"valid_guilds"                

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Property enthaelt die zugelassenen Gilden in Form von
	kleingeschriebenen Gildennamen, welche in einem Array
	zusammengefasst sind. Sie ist nur fuer den Gildenmaster selbst von
	Bedeutung.

BEISPIELE
---------
::

	Abfrage der zugelassenen Gilden:
	  load_object("/secure/gildenmaster")->QueryProp(P_VALID_GUILDS)
	Das ergibt zum Beispiel:
          ({"abenteurer","zauberer","klerus","kaempfer"})

SIEHE AUCH
----------
::

	P_GUILD, /secure/gildenmaster.c


Last modified: 2022-01-09, Arathorn


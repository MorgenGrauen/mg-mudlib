dtime()
=======

FUNKTION
--------
::

	string dtime(int time)

ARGUMENTE
---------
::

	time - Umzuwandelndes Datum in Sekunden seit 1.1.1970, 0:0:0h

BESCHREIBUNG
------------
::

	Wandelt das Datum time in einen deutschsprachigen String der Form
	"<wtag>, <tag>. <mon> <jahr>, <std>:<min>:<sek>" um.

RUECKGABEWERT
-------------
::

	Der String mit dem umgewandelten Datum.

BEMERKUNGEN
-----------
::

	Als time wird meistens das Ergebnis von time() benutzt.
  strftime() stellt eine wesentlich flexiblere Moeglichkeit der Ausgabe von
  Zeiten dar.

BEISPIELE
---------
::

	datum = dtime(time());
        => datum = "Mon,  6. Mar 1994, 15:00:08"

SIEHE AUCH
----------
::

	ctime(E), strftime(E), time(E)


log_file()
==========

FUNKTION
--------
::

	int log_file(string file, string text)
  int log_file(string file, string text, int size_to_break)

ARGUMENTE
---------
::

	file - Name der Datei, in die geschrieben werden soll
	text - Der Text, der geschrieben werden soll
	size_to_break - Groesse, ab der ein neues File begonnen wird (optional)

BESCHREIBUNG
------------
::

	log_file schreibt den Text text in die Datei /log/file.
	Sollte file schon mit einem /log/ beginnen, wird kein erneutes /log/ davor
	eingefuegt.

RUECKGABEWERT
-------------
::

	1 bei Erfolg oder 0, falls ein Fehler beim Schreiben auftrat.

BEMERKUNGEN
-----------
::

	Wenn die Groesse von file vor dem Schreiben 50000 Bytes ueberschreitet,
	wird sie in file.old umbenannt. Eine schon vorhandene Datei file.old
	wird dabei geloescht. Der Text wird nach dem Umbenennen geschrieben.
	Wird 'size_to_break' angegeben und ist dies > 0, wird dieser Wert (in 
	Bytes) statt der 50000 Bytes zum Rotieren des Logfiles benutzt.

BEISPIELE
---------
::

	log_file( "report/wargon.rep", "TYPO von bla in blubb:\ntest\n");
	In /log/report/wargon.rep finde ich nun die neueste Typomeldung... ;)
	log_file( "/log/report/wargon.rep", "TYPO von bla in blubb:\ntest\n");
	Gleiches Ergebnis. ;-)

SIEHE AUCH
----------
::

	write_file(E)

29.01.2017, Zesstra


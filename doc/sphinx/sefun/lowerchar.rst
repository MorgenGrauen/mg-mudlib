lowerchar()
===========

FUNKTION
--------
::

	int lowerchar(int char)

ARGUMENTE
---------
::

	char - Das umzuwandelnde Zeichen

BESCHREIBUNG
------------
::

	Wenn char ein Grossbuchstabe ist, so wird es in einen Kleinbuchstaben
	umgewandelt. Andere Zeichen werden von dieser Funktion nicht beein-
        flusst.

RUECKGABEWERT
-------------
::

	Das umgewandelte Zeichen.

BEISPIELE
---------
::

	printf("%c\n", lowerchar('A')); => a
	printf("%c\n", lowerchar('1')); => 1

SIEHE AUCH
----------
::

	lower_case(E), lowerstring(E), upperstring(E), capitalize(E)


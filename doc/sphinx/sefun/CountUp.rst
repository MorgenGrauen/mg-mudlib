CountUp()
=========

FUNKTION
--------
::

	public varargs string CountUp( string *s, string sep, string lastsep );

ARGUMENTE
---------
::

	*s
	  Array von Strings mit Woertern.
  sep
    String, der zwischen den Woerten in der Aufzaehlung eingefuegt wird.
    Standard: ", "
  lastsep
    String, der zwischen dem vorletzten und letzten Element eingefuegt wird.
    Standard: " und "

BESCHREIBUNG
------------
::

	Die Woerter Wort_1 bis Wort_n aus dem Stringaray werden als
	Aufzaehlung in der Form
	  Wort_1<sep>Wort_2, ... Wort_n-1<lastsep>Wort_n
	zusammengesetzt. Mit Standardwerten also:
	  Wort_1, Wort_2, ... Wort_n-1 und Wort_n

RUeCKGABEWERT
-------------
::

	String als Aufzaehlung der einzelnen Woerter.


15.03.2008, Zesstra


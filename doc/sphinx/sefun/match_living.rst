match_living()
==============

match_living(sefun)

FUNKTION
--------
::

     varargs mixed match_living( string str, int players_only,
				 string *exclude)

ARGUMENTE
---------
::

     string str		- Kuerzel, nach dem die living_names durchsucht
			  werden soll
     int players_only	- 1, um nur Spieler (Interactives) zu suchen
     string *exlude	- welche Namen sollen ignoriert werden

BESCHREIBUNG
------------
::

     Sucht alle Lebewesen, deren Namen mit str beginnen.

RUECKGABEWERT
-------------
::

     Ein String, falls es ein Lebewesen mit dem Namen str gibt (der Name
     muss genau passen).
     -1, wenn es mehrere Lebewesen gibt, deren Namen mit str beginnen.
     -2, wenn es kein Lebewesen gibt, dessen Name mit str beginnt.

BEISPIELE
---------
::

     match_living("wargon"); => "wargon", wenn Wargon eingeloggt ist.
     match_living("war");    => "wargon", wenn es kein anderes Lebewesen
                                gibt, dessen Name mit "war" beginnt.

SIEHE AUCH
----------
::

     find_living(E), find_player(E), find_netdead(E)

27. Mai 2004 Gloinson


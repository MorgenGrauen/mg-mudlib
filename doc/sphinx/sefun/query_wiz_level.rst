query_wiz_level()
=================

FUNKTION
--------
::

	int query_wiz_level(object ob)
	int query_wiz_level(string ob)

ARGUMENTE
---------
::

	ob - Der Spieler/das Objekt, dessen Magierlevel ermittelt werden soll.
       Es kann auch eine UID (z.B. "zesstra", "d.inseln.zesstra") als String
       uebergeben werden.

BESCHREIBUNG
------------
::

	query_wiz_level() liefert den Magierlevel des Objekts ob zurueck.
	Normale Spieler haben einen Magierlevel von 0, Seher normalerweise
  einen von 1 (auf jeden Fall < 10).
  Objekte bekommen folgende Level:
  /d/           - 25
  /p/           - 21
  /obj/         - 0
  /std/         - 0
  /gilden/      - 30
  /spellbooks/  - 30
  /players/     - entsprechend Magier, 20 - 111
  /secure/      - 100

RUECKGABEWERT
-------------
::

	Der Magierlevel des Spielers/Objekts.

BEMERKUNGEN
-----------
::

	Wird als Parameter ein String mit einem Spielernamen uebergeben, so 
	kann auch der Magierlevel eines nicht eingeloggten Spielers heraus-
	gefunden werden.

BEISPIELE
---------
::

	lv = query_wiz_level(find_player("jof"))
	   => lv = 111, falls Jof eingeloggt ist.
	lv = query_wiz_level("jof")
           => lv = 111 in jedem Fall.

SIEHE AUCH
----------
::

	/secure/wizlevels.h

LETZTE AeNDERUNG
----------------
::

15.10.2007, Zesstra


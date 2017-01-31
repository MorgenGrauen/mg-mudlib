restore_hit_points()
====================

FUNKTION
--------
::

    int restore_hit_points(int heal)

ARGUMENTE
---------
::

    int heal	- der zu heilende Betrag

BESCHREIBUNG
------------
::

    Dem Lebewesen werden heal Lebenspunkte aufgeschlagen. Die HP
    steigen nicht ueber P_MAX_HP.

RUECKGABEWERT
-------------
::

    Die verbleibenden Lebenspunkte.

BEISPIELE
---------
::

    write("Ploetzlich schiesst eine scheussliche Kreatur aus der Pfuetze "+
          "heraus und\nschleimt dich heilend voll, sie verschwindet so, "+
          "wie sie gekommen ist.\n");
    this_player()->restore_hit_points(50);

BEMERKUNGEN
-----------
::

    Bei Heilstellen sollte eine evtl. Heilung des Spielers mit der eigens
    dafuer eingerichteten Funktion check_and_update_timed_key realisiert
    werden.
    Ansonsten bitte buffer_hp() benutzen und die Konzeptseite lesen!

SIEHE AUCH
----------
::

    Gegenpart:	reduce_hit_points()
    Verwandt:	buffer_hp(), heal_self(), restore_spell_points()
    Props:	P_HP
    Konzept:	heilung

23.Feb.2004 Gloinson


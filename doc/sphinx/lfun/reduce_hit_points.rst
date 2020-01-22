reduce_hit_points()
===================

FUNKTION
--------
::

    int reduce_hit_points(int damage)

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    int damage	- der zugefuegte Schaden

BESCHREIBUNG
------------
::

    Dem Lebewesen werden damage Lebenspunkte abgezogen, aber das Lebewesen
    wird hinterher nicht weniger als 1 Lebenspunkt haben und es wird
    dadurch nicht sterben.

RUECKGABEWERT
-------------
::

    Die Lebenspunkte nach der Aenderung.

BEISPIELE
---------
::

    write("Ploetzlich schiesst eine scheussliche Kreatur aus der Pfuetze "+
          "heraus und\nbeisst Dich ins Bein, sie verschwindet so schnell, "+
          "wie sie gekommen ist.\n");
    this_player()->reduce_hit_points(50);
    (Auszug aus /players/boing/friedhof/room/cat1x9)

BEMERKUNGEN
-----------
::

    damage kann auch ein negativer Wert sein, dann werden dem Lebewesen
    diese Lebenspunkte gutgeschrieben und auf die aktuellen Lebenspunkte
    addiert. Da dies eine Form der Heilung ist, ist die Verwendung mit dem
    Regionsmagier abzustimmen und dem EM fuer Heilungsbalance zur
    Genehmigung vorzulegen.

    Heilstellen sollten in neuem Code mit der Funktion restore_hit_points()
    realisiert werden.

    Bei Heilstellen sollte eine evtl. Heilung des Spielers mit der eigens
    dafuer eingerichteten Funktion check_and_update_timed_key() realisiert
    werden.

SIEHE AUCH
----------
::

    Gegenpart: restore_hit_points()
    Verwandt:	 do_damage(), Defend(), reduce_spell_points(), 
               restore_spell_points(), check_and_update_timed_key()
    Props:	   P_HP
    Konzept:   heilung


Last modified: 20. Jan. 2020, Arathorn 


GetDoorsMapping()
=================

FUNKTION
--------
::

    mapping GetDoorsMapping()

BESCHREIBUNG
------------
::

    Mit dieser Funktion erhaelt man das Mapping der vorhandenen
    Seherportale.

    

    Die Struktur ist von der Form: 
	([ Pfad: Portalnummer; Portalbeschreibung, ])
    Es wird eine Kopie dieses Mappings geliefert.

RUECKGABEWERT
-------------
::

    Das Sehertormapping

BEMERKUNGEN
-----------
::

    Diese Funktion wird von /d/seher/portale/sehertormaster definiert.

BEISPIELE
---------
::

    "/d/seher/portale/sehertormaster"->GetDoorsMapping();

SIEHE AUCH
----------
::

    DoorIsKnown, ShowDoors, Teleport, DiscoverDoor


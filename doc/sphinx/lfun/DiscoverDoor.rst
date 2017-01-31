DiscoverDoor()
==============

FUNKTION
--------
::

    varargs int DiscoverDoor(string dname)

ARGUMENTE
---------
::

    dname: Name des Raumes, in dem der Seher das Sehertor kennenlernen soll.
           Default: Die Umgebung des Sehers.

BESCHREIBUNG
------------
::

    Nachdem diese Funktion aufgerufen wurde, kann der Seher (this_player())
    das Tor in dem angegebenen Raum immer verwenden.

RUECKGABEWERT
-------------
::

    1, falls der Seher ein NEUES Tor kennengelernt hat
    0, falls er das Tor schon kannte oder kein Seher war

BEMERKUNGEN
-----------
::

    Von einem Sehertor wird diese Funktion automatisch beim Betreten des
    umgebenden Raumes aufgerufen, falls P_SEERDOOR_DISCOVER gesetzt ist. Wenn
    ein Tor auf diese Art nicht entdeckt werden soll, so darf
    P_SEERDOOR_DISCOVER nicht gesetzt sein und muss DiscoverDoor() separat,
    z.B. von einem Questobjekt, aufgerufen werden.
    Diese Funktion wird von /d/seher/portale/sehertormaster definiert.

BEISPIELE
---------
::

    write("Der Zauberer sagt: Im Nichts wirst Du ein weiteres Tor finden!\n");
    "/d/seher/portale/sehertormaster"->DiscoverDoor("/room/void");

SIEHE AUCH
----------
::

    DoorIsKnown, ShowDoors, Teleport, GetDoorsMapping


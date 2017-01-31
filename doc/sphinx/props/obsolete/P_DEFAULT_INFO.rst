P_DEFAULT_INFO
==============

********************* VERALTETE PROPERTY ******************************
* Diese Property ist veraltet. Bitte nicht mehr in neuem Code nutzen. *
***********************************************************************

NAME
----
::

    P_DEFAULT_INFO                "default_info"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Default-Antwort eines Npc, wenn er auf das Schluesselwort durch den
    Spieler kein AddInfo parat hat.

BEMERKUNG
---------
::

    Diese Property sollte nicht mehr benutzt werden. Stattdessen bitte
    AddInfo(DEFAULT_INFO,...) verwenden.
    Dem in dieser Prop angegeben String wird immer der Name des Npc
    vorausgestellt. Will man dies verhindern, muss man sie ueberschreiben.

BEISPIEL
--------
::

    SetProp(P_DEFAULT_INFO,"bohrt gelangweilt in der Nase.\n");

SIEHE AUCH
----------
::

    AddInfo


17.08.2010, Zesstra


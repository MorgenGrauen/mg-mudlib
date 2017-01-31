P_DETAILS
=========

NAME
----
::

    P_DETAILS            "details"

DEFINIERT IN
------------
::

    /sys/thing/description.h

BESCHREIBUNG
------------
::

    Diese Property enthaelt ein Mapping, in der Details im Objekt
    definiert werden und Beschreibungen, die ausgegeben werden, wenn man
    sich diese Details anschaut.

BEMERKUNGEN
-----------
::

    Man kann diese Property nicht per SetProp() veraendern, sondern muss die
    entsprechenden Funktionen nutzen.
    AddSpecialDetail() und RemoveSpecialDetail() sollten nicht mehr
    verwendet werden.

SIEHE AUCH
----------
::

    Setzen:    AddDetail()
    Loeschen:  RemoveDetail()
    Aehnlich:  P_SPECIAL_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail()
    Sonstiges: GetDetail(), break_string()

27. Jan 2013 Gloinson


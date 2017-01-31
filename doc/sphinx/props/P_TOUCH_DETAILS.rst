P_TOUCH_DETAILS
===============

NAME
----
::

    P_TOUCH_DETAILS            "touch_details"

DEFINIERT IN
------------
::

    /sys/thing/description.h

BESCHREIBUNG
------------
::

    Diese Property entspricht dem P_DETAILS fuer Standarddetails,
    nur werden hierin Gerueche gespeichert:
    Diese Property enthaelt ein Mapping, in der Details im Objekt
    definiert werden und Beschreibungen, die ausgegeben werden, wenn man
    sich diese Details anschaut.

BEMERKUNGEN
-----------
::

    Man sollte diese Property nicht per Hand veraendern, sondern die
    Funktionen nutzen.

SIEHE AUCH
----------
::

    Setzen:    AddTouchDetail()
    Loeschen:  RemoveTouchDetail()
    Aehnlich:  AddDetail(), P_DETAILS
    Sonstiges: GetDetail(), break_string()

27. Jan 2013 Gloinson


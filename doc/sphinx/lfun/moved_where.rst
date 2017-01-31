moved_where()
=============

FUNKTION
--------
::

    public object moved_where(void);

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    keine

BESCHREIBUNG
------------
::

    Mit dieser Funktion laesst sich ermitteln, wohin das Lebewesen zuletzt
    mittels put_objects(L) oder give_objects(L) etwas bewegt oder wem es mit
    show_objects(L) etwas gezeigt hat.

RUECKGABEWERT
-------------
::

    Der Container oder das Lebewesen, wohin die Gegenstaende bewegt wurden.

BEISPIEL
--------
::

    siehe give_objects()

SIEHE AUCH
----------
::

    put_objects(L), give_objects(L), show_objects(L), NotifyInsert(L),
    P_LAST_CONTENT_CHANGE


Last modified: Thu Aug 28 22:16:15 2008 by Amynthor


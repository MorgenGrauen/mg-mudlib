put()
=====

FUNKTION
--------
::

    public varargs int put(object o, object dest, mixed msg);

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    object o
        Das Objekt, das irgendwo hingesteckt werden soll.
    object dest
        Der Behaelter, in den das Objekt gesteckt werden soll.
    mixed msg
        Eine optionale Meldung, die anstelle von P_PUT_MSG oder der
        Standardmeldung verwendet wird, oder -1, um die Meldung zu
        unterdruecken.

BESCHREIBUNG
------------
::

    Der Spieler oder NPC steckt das Objekt in einen Behaelter. Gibt o->move()
    keinen positiven Wert zurueck, beispielsweise weil er das Objekt nicht
    weggeben darf oder der Behaelter schon voll ist, bekommt er eine
    entsprechende Fehlermeldung.

RUECKGABEWERT
-------------
::

    Wenn das Bewegen geklappt hat, 1, ansonsten 0.

BEMERKUNG
---------
::

    Diese Funktion ist dann sinnvoll, wenn man den Spieler ein Objekt irgendwo
    hinstecken lassen und sich nicht selbst um die Fehlerbehandlung kuemmern
    moechte - und da unzaehlige verschiedene Dinge schiefgehen koennen und
    manche Objekte eigene Fehlermeldungen definieren, eigentlich immer.

    Die Funktion prueft nicht, ob sich das Objekt und der Behaelter ueberhaupt
    in der Reichweite des Spielers/NPC befinden, das muss man ggf. selbst
    ermitteln.

SIEHE AUCH
----------
::

    move(L), P_PUT_MSG, put_objects(L), P_NOINSERT_MSG, P_NOLEAVE_MSG,
    P_TOO_MANY_MSG, P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_NOGET, P_NODROP


Last modified: Thu Aug 28 22:21:58 2008 by Amynthor


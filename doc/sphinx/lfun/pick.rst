pick()
======

FUNKTION
--------
::

    public varargs int pick(object o, mixed msg);

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    object o
        Das Objekt, das aufgehoben werden soll.
    mixed msg
        Eine optionale Meldung, die anstelle von P_PICK_MSG oder der
        Standardmeldung verwendet wird, oder -1, um die Meldung zu
        unterdruecken.

BESCHREIBUNG
------------
::

    Der Spieler oder NPC nimmt das Objekt auf. Gibt o->move() keinen positiven
    Wert zurueck, beispielsweise weil das Objekt zu schwer ist oder nicht
    genommen werden darf, bekommt er eine entsprechende Fehlermeldung.

RUECKGABEWERT
-------------
::

    Wenn das Aufnehmen geklappt hat, 1, ansonsten 0.

BEMERKUNG
---------
::

    Diese Funktion ist dann sinnvoll, wenn man den Spieler ein Objekt
    aufnehmen lassen und sich nicht selbst um die Fehlerbehandlung kuemmern
    moechte - und da unzaehlige verschiedene Dinge schiefgehen koennen und
    manche Objekte eigene Fehlermeldungen definieren, eigentlich immer.

    Die Funktion prueft nicht, ob sich das Objekt ueberhaupt in der Reichweite
    des Spielers/NPC befindet, das muss man ggf. selbst ermitteln.

BEISPIEL
--------
::

    ob = clone_object(WEINGUMMI);

    if (this_player()->pick(ob, ({ "Du nimmst @WENU2 aus dem Regal.",
                                   "@WER1 nimmt @WENU2 aus dem Regal." })))
        weingummi--;
    else
        ob->remove();

SIEHE AUCH
----------
::

    move(L), P_PICK_MSG, pick_objects(L), P_NOINSERT_MSG, P_NOLEAVE_MSG,
    P_TOO_MANY_MSG, P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_NOGET


Last modified: Thu Aug 28 22:21:41 2008 by Amynthor


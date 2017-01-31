drop()
======

FUNKTION
--------
::

    public varargs int drop(object o, mixed msg);

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    object o
        Das Objekt, das fallengelassen werden soll.
    mixed msg
        Eine optionale Meldung, die anstelle von P_DROP_MSG oder der
        Standardmeldung verwendet wird, oder -1, um die Meldung zu
        unterdruecken.

BESCHREIBUNG
------------
::

    Der Spieler oder NPC laesst das Objekt fallen. Gibt o->move() keinen
    positiven Wert zurueck, beispielsweise weil das Objekt verflucht ist,
    bekommt er eine entsprechende Fehlermeldung.

RUECKGABEWERT
-------------
::

    Wenn das Fallenlassen geklappt hat, 1, ansonsten 0.

BEMERKUNG
---------
::

    Diese Funktion ist dann sinnvoll, wenn man den Spieler ein Objekt
    fallenlassen lassen und sich nicht selbst um die Fehlerbehandlung kuemmern
    moechte - und da unzaehlige verschiedene Dinge schiefgehen koennen und
    manche Objekte eigene Fehlermeldungen definieren, eigentlich immer.

    Die Funktion prueft nicht, ob der Spieler/NPC das Objekt ueberhaupt hat,
    das muss man ggf. selbst ermitteln.

BEISPIEL
--------
::

    if (this_player()->drop(obj, ({
            "Du wirfst @WEN2 in den Saeureteich.\n",
            "@WER1 wirft @WENU2 in den Saeureteich.\n" })))
        obj->remove();

SIEHE AUCH
----------
::

    move(L), P_DROP_MSG, drop_objects(L), P_NOINSERT_MSG, P_NOLEAVE_MSG,
    P_TOO_MANY_MSG, P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_NODROP


Last modified: Thu Aug 28 22:20:37 2008 by Amynthor


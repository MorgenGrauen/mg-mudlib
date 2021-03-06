pick_objects()
==============

FUNKTION
--------
::

    public varargs int pick_objects(string str, int flag, mixed msg);

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    string str
        Was aufgehoben werden soll.
    int flag
        Muss das Objekt irgendwo drinstecken (flag = 1), oder darf es einfach
        so herumliegen (flag = 0)? Dieses Argument ist hauptsaechlich fuer das
        Kommando "hole" gedacht, in der Regel braucht man es nicht anzugeben.
    mixed msg
        Eine optionale Meldung, die anstelle von P_PICK_MSG oder der
        Standardmeldung verwendet wird, oder -1, um die Meldung zu
        unterdruecken.

BESCHREIBUNG
------------
::

    Der Spieler oder NPC nimmt die in <str> benannten Sachen. Kann er ein
    Objekt nicht nehmen, bekommt er eine entsprechende Fehlermeldung. Wenn
    keine Objekte auf <str> passen, wird per _notify_fail() eine Meldung
    gesetzt, aber noch nicht ausgegeben.

RUECKGABEWERT
-------------
::

    Wenn <str> irgendwelche vorhandenen Sachen sind, 1, sonst 0.

BEMERKUNG
---------
::

    Wenn die Funktion 1 zurueckgibt, heisst das noch nicht, dass der Spieler
    etwas genommen hat! Er hat es nur versucht, d.h. auf jeden Fall eine
    Meldung bekommen. Gibt die Funktion 0 zurueck, hat er noch keine bekommen.

SIEHE AUCH
----------
::

    move(L), pick(L), P_PICK_MSG, find_objects(L), moved_objects(L)


Last modified: Fri Jul 25 10:58:43 2008 by Amynthor


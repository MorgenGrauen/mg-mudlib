getuuid()
=========

SYNOPSIS
--------

    string getuuid(object|string ob)

DESCRIPTION
-----------

    Liefert eine eindeutige (get unique uid) UUID fuer einen Spieler.
    Wird zusammengesetzt aus der UID des Spielers und seinem
    Erstlogin-Datum.

    Nach einer Selbstloeschung und neuem Login erhaelt der Spieler eine
    neue UUID, bei einer Restaurierung behaelt er seine alte UUID.

    Wenn die Funktion ohne Parameter aufgerufen wird, wird per Default
    this_object() genommen.

    Wird in <ob> ein string uebergeben, wird erwartet, dass es eine
    Spieler-UID ist. 

SEE ALSO
--------

        :doc:`../efun/getuid`


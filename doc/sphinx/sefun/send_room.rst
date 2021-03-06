send_room()
===========

FUNKTION
--------

    varargs void send_room(object|string room, string msg, int msg_type,
            string msg_action, string msg_prefix, object \*exclude,
            object origin)

BESCHREIBUNG
------------

    Sendet msg an alle Objekte in room durch Aufruf von ReceiveMsg() mit
    den uebergebenen Argumenten.
    Zur Bedeutung der Argumente siehe Manpage von ReceiveMsg().

    Wenn das Raumobjekt mit seinem Namen angegeben ist, wird das Objekt
    unter diesem Namen gesucht und und geladen, falls notwendig.

    Mit dem Array exclude kann man verhindern, dass die Nachricht an
    die darin enthaltenen Objekte gesendet wird.
    Das ist sinnvoll, wenn beispielsweise ein Spieler Ausloeser einer Meldung
    ist und diese selbst nicht erhalten soll.

    origin gibt an, welches Objekt die Meldung ausloest (muss nicht das
    sendende Objekt selber) und wird vor allem fuer die Ignorierepruefung
    verwendet. Default ist das sendende Objekt.

    Letztendlich ist die sefun vergleichbar zu `tell_room()`.

BEISPIEL
--------

    Oft moechte man einfach nur, dass an den Spieler, der eine Aktion
    oder aehnliches ausloest, eine andere Meldung gegeben wird, als an
    andere im Raum befindliche Spieler. Folgenden Code koente man dann
    verwenden:

    .. code-block:: pike

        /* Zuerst senden wir eine Nachricht nur an den Spieler.
         * Ziel: der Spieler riecht etwas und schuettelt sich fuer alle im
         * Raum sichtbar darauf.
         */
        this_player()->ReceiveMsg("Buah stinkt das. Du schuettelst Dich.\n",
                MT_NOTIFICATION|MT_SMELL, MA_SMELL);
        send_room(environment(this_player()),
                // der Raum in dem der Spieler ist
                 this_player()->Name(WER) + " schuettelt sich. Wieso nur?\n",
                // die eigentliche Nachricht
                MT_LOOK, // man sieht das schuetteln
                MA_UNKNOWN, // MA_SMELL moeglich; im Zweifel immer MA_UNKNOWN
                "", // wir wollen kein Praefix vor der Ausgabe haben
                ({this_player()}));
                // Array nur mit dem Spieler, der das nicht noch
                // mitbekommen soll.

    Fuer weitere Optionen fuer `msg_type` und `msg_action` siehe die manpage
    zu ReceiveMsg. Natuerlich haben `this_player` und `environment` auch
    manpages.



SIEHE AUCH
----------

    :doc:`../lfun/ReceiveMsg`
    tell_object(E), tell_room(E), say(E), write(E)

31.01.2018, Deaddy


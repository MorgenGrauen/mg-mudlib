BecomesNetDead()
================

FUNKTION
--------
::

    void BecomesNetDead(object pl);

GERUFEN VON
-----------
::

    /std/player/base.c

ARGUMENTE
---------
::

    object pl
      Spieler, der Verbindung zum MUD verliert.

BESCHREIBUNG
------------
::

    Spieler, welche die Verbindung zum MUD freiwillig
    (z.B. per 'schlafe ein') oder unfreiwillig verlieren, gehen in den
    Netztotenstatus ueber. Sie verbleiben noch eine definierte Zeit in
    der zuletzt betretenen Umgebung und werden schliesslich automatisch
    in den Netztotenraum ueberfuehrt.
    Um nun einen Ueberblick darueber zu erhalten, wann ein Spieler die
    Verbindung zum MUD verliert, gibt es die Funktion BecomesNetDead().
    Sie wird automatisch in der Umgebung des Spielers, in allen Objekten
    in der Umgebung des Spielers (nicht rekursiv) und in allen Objekten
    im Spieler (rekursiv) aufgerufen. Uebergeben wird hierbei das
    Spielerobjekt.

    Es gibt auch noch die Funktion BecomesNetAlive(), mit der man
    auf aehnliche Weise erwachende Spieler registrieren kann.

BEISPIELE
---------
::

    Es gibt Gebiete, in denen netztote Spieler unerwuenscht sind.
    Folgendermassen kann man sich ihrer wirkungsvoll entledigen:

    

    void BecomesNetDead(object pl) {
      pl->move("eingangsraum zu gebiet", M_TPORT|M_NOCHECK);
    }
    Man schickt diese Spieler wieder zum Eingang des Gebietes.
    Da der letzte Aufenthaltsort eines Spielers, der in den
    Netztotenstatus uebergeht, erst nach Aufruf der Funktion
    BecomesNetDead() abgespeichert wird, wacht der Spieler dann an dem
    Ort auf, wo man Ihn innerhalb dieser Funktion hinteleportiert hat.

SIEHE AUCH
----------
::

    BecomesNetAlive(), PlayerQuit(), /std/player/base.c, /room/netztot.c

24. Aug 2011, Gloinson


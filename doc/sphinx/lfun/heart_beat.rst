heart_beat()
============

FUNKTION
--------
::

     protected void heart_beat();

DEFINIERT IN
------------
::

     /std/living/life.c
     /std/living/combat.c
     und anderen...
     kann auch in beliebigen Objekten selbst definiert werden.

BESCHREIBUNG
------------
::

     Diese Funktion wird alle zwei Sekunden vom GameDriver aufgerufen. Sie
     regelt in der MG-MudLib das Heilen von Spielern und Monstern, den
     Ablauf von Kampfrunden, die Verdauung etc.

     Da heart_beat() ziemlich viele Ressourcen des GameDrivers verbraet,
     sollte man Objekte mit heart_beat() nur so selten wie moeglich
     benutzen! Und selbst dann sollte der heart_beat() nicht die ganze Zeit
     ueber laufen, sondern sich so bald wie moeglich abschalten.

     Das Ein- und Ausschalten des heart_beat()s erfolgt mit
     set_heart_beat().

BEMERKUNGEN
-----------
::

     1. Teuer, teuer, teuer!
     2. Soll euer Viech pro "echtem" Heartbeat mehrere Kampfrunden haben,
        benutzt dafuer SA_SPEED und ruft auf gar keinen Fall mehrfach 
        ::heart_beat() auf. Also _NICHT_
        void heart_beat() {
            ::heart_beat();
            ::heart_beat(); }
        sondern:
        SetProp(P_SKILL_ATTRIBUTE_OFFSETS, ([SA_SPEED: 200]) );

SIEHE AUCH
----------
::

     Efuns:     set_heart_beat(), absolute_hb_count(), set_next_reset()
     Fehler:    make_immortal(L)


22.3.2008, Zesstra


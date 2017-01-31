InformAlcoholEffect()
=====================

FUNKTION
--------
::

	void InformAlcoholEffect(object pl, int msgid, int area)

DEFINIERT IN
------------
::

     eigenen Objekten; fuer /std/living/life.c

 

ARGUMENTE
---------
::

	pl
          Das Lebewesen, das alkoholisiert ist.
        msgid
          Flag fuer die Meldung, die ausgegeben wird.
        area
          Aufruf erfolgt in Gilde (0) oder im Environment (1).

BESCHREIBUNG
------------
::

        Wenn ein Lebewesen alkoholisiert ist, werden in unregelmaessigen
        Abstaenden Meldungen an die Umgebung und ans Lebewesen ausgegeben.
        Gleichzeitig wird die Funktion InformAlcoholEffect in der Gilde
        und im Environment des Lebewesens aufgerufen.

        

        Es gibt vier verschiedene Meldungen (definiert in /sys/health.h):
        ALC_EFFECT_HICK      (0) : "<Hick>! Oh, Tschuldigung.\n"
        ALC_EFFECT_RUELPS    (1) : "Du ruelpst.\n"
        ALC_EFFECT_LOOKDRUNK (2) : "Du fuehlst Dich benommen.\n"
        ALC_EFFECT_STUMBLE   (3) : "Du stolperst.\n"

        

        Wird die Funktion in einem Gildenraum implementiert, so kann man
        anhand des 3. Parameters unterscheiden, ob die Gilde als Gilde
        oder als Raum gemeint ist (die Funktion wird je einmal aufgerufen):
        ALC_EFFECT_AREA_GUILD (0) : der Aufruf betrifft die Gilde,
        ALC_EFFECT_AREA_ENV   (1) : der Aufruf betrifft die Umgebung.

RUeCKGABEWERT
-------------
::

	keiner

SIEHE AUCH
----------
::

        P_ALCOHOL, /sys/health.h


Last modified: Thu May 23 23:08:00 2002 by Mupfel


P_LIGHT_TYPE
============

NAME
----
::

    P_LIGHT_TYPE                       "light_type"

DEFINIERT IN
------------
::

    /sys/thing/description.h

BESCHREIBUNG
------------
::

    Gibt an, was fuer ein Licht in einem Objekt vorherrscht. 

    

    Es sind verschiedene 'atomare' Lichttypen, vordefiniert:
    LT_MISC         Unbestimmt, keine Angaben.

    LT_SUN          Sonnenlicht.
    LT_MOON         Mondlicht 
    LT_STARS        Sternenlicht.

    

    LT_DIFFUSE      Indirektes Tageslicht. (z.B. im Wald)

    LT_CANDLE       Kerzenlicht.
    LT_TORCH        Fackelschein.
    LT_OPEN_FIRE    Sonstiges offenes Feuer. (Lagerfeuer etc.)

    

    LT_MAGIC        Irgendeine magische Lichtquelle.

    LT_GLOWING      Eine selbstleuchtende Lichtquelle.

    LT_DARKNESS     Kein wirkliches Licht, aber auch Dunkelheit soll
                    explizit gesetzt werden koennen.

    In einem Objekt koennen mehrere Lichttypen gesetzt werden. Dies
    geschieht durch logische Oder-Verknuepfungen, siehe man operators.

    Wenn in einem Raum mehr als ein Lichttyp gesetzt ist, bedeutet das, 
    normalerweise, dass mehrere Lichtquellen verschiedenen Typs im Raum 
    sind.

    Es gibt zudem noch Lichttypen, die zusammengesetzt sind:

    LT_DAYLIGHT    Tageslicht (Sonne/Diffuse)
    LT_NATURAL     Natuerliches Licht (Daylight/Sterne/Mond)
    LT_ARTIFICIAL  Kuenstliches Licht (Magie/Feuer/Gluehen)
    LT_FIRE        Feuer (Kerzen/Fackeln/offenes Feuer)

BEISPIELE
---------
::

    Ein Objekt soll ein geheimnisvolles Gluehen von sich geben:

    

    objekt->SetProp( P_LIGHT_TYPE, LT_GLOWING )

    Soll ein Raum beschrieben werden, der durch Sternenlicht erhellt ist,
    in dem aber zusaetzlich noch ein Lagerfeuer brennt, sieht die Syntax
    folgendermassen aus:

    

    raum->SetProp( P_LIGHT_TYPE, LT_STARS|LT_OPEN_FIRE );

    Einer brennenden Hose kann der Lichttyp fuer offenes Feuer mitgegeben 
    werden. Es muss jedoch nicht zwingend der Lichttyp fuer magische
    Lichtquellen benutzt werden. Es ist klar, dass es irgendwas mit Magie
    zu tun hat, wenn brennende Spieler durch die Gegend laufen, ohne zu 
    schreien. P_LIGHT_TYPE sollte jedoch das fassbare Licht beschreiben.
    LT_MAGIC ist also eher eine Notloesung fuer Licht, dessen Ursache man
    nicht erklaeren kann.

ANMERKUNG
---------
::

    P_LIGHT_TYPE dient ausschliesslich der Beschreibung des Lichtes, das 
    vorherrscht. Es ist nicht verbunden mit dem Lichtsystem, und soll es
    auch nicht werden.

    Die Empfindlichkeit der Dunkelelfen gegen Sonnenlicht wird ueber diese
    Property gesteuert. Soll ein Raum mit (P_INDOORS==0) so dunkel sein, dass
    sie nicht in Gefahr sind, sollten nicht LT_MISC oder LT_SUN gesetzt
    sein.

SIEHE AUCH
----------
::

    CheckLightType, /std/thing/description.h, operators


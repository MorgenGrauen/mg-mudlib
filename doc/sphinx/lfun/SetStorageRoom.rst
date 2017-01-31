SetStorageRoom()
================

FUNKTION
--------
::

        void SetStorageRoom(string store);

DEFINIERT IN
------------
::

        /std/room/shop.c

ARGUMENTE
---------
::

        store
          Dateiname des Lagers, in dem die aufgekauften Objekte aufbewahrt
          werden.

RUeCKGABEWERT
-------------
::

        keiner

BESCHREIBUNG
------------
::

        Mit dieser Funktion wird dem Laden bekannt gegeben, welches Objekt
        als Lager fuer angekaufte Objekte dienen soll. Jeder Laden muss
        explizit einen eigenen Lagerraum setzen, ansonsten wird ein 
        Laufzeitfehler ausgeloest und der Laden ist nicht ladbar.

        

        Das Speicherobjekt sollte /std/store.c erben, da dort einige
        Aufraeumfunktionen definiert sind. Entscheidend fuer selbige sind
        die Properties P_MIN_STOCK und P_STORE_CONSUME.

BEISPIELE
---------
::

        Der Raum 'myladen.c' koennte etwa so aussehen:

          

          // myladen.c
          inherit "/std/shop";
          #include <properties.h>

          

          protected void create() {
            ::create();
            SetProp(...); // Beschreibung wie bei normalen Raeumen
            SetStorageRoom("/d/beispiel/mystore");
            AddFixedObject("/items/fackel");
          }

        In diesem Laden wird nun die Standardfackel als mengenmaessig 
        unbegrenzter Artikel verkauft.

        Der zugehoerige Lagerraum 'mystore.c' kann dann im einfachsten Fall
        so aussehen:

          

          // mystore.c
          inherit "/std/store";
          #include <rooms.h>  // Fuer AddItem-Konstanten

          

          protected void create() {
            ::create();
            // KEINE weiteren Beschreibungen!
            // 1. erbt der Speicher keine Beschreibungsmodule,
            // 2. sollen da eh nur Sachen und keine Spieler rein!
            AddItem("/items/schaufel", REFRESH_REMOVE);
          }

        

        Der Laden verkauft nun auch Schaufeln, jedoch nur eine pro Reset.

HINWEISE:        
        Fuer standardmaessig verkaufte Waren beachte man den Unterschied 
        zwischen den Funktionen AddItem() und AddFixedObject().
        Die erstgenannte Funktion ist im Lager zu verwenden, letztere jedoch
        im Laden.

SIEHE AUCH
----------
::

        Allgemeines:  laden
        Funktionen:   AddItem(L), AddFixedObject(L), RemoveFixedObject(L) 
        Basisobjekte: /std/store.c
        Properties:   P_MIN_STOCK, P_STORE_CONSUME


Last modified: 19-Jun-2015, Arathorn


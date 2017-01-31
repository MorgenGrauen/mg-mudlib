paramove()
==========

****************************************************************************
************************* VERALTETE LFUN ***********************************
************************* DO NOT USE!    ***********************************
****************************************************************************
paramove()

FUNKTION
--------
::

     int paramove();

DEFINIERT IN
------------
::

     /std/room/para.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Bewegt den Spieler ggf. in eine Paralleldimension.

RUeCKGABEWERT
-------------
::

     1, wenn der Spieler die Dimension gewechselt hat.

BEISPIEL
--------
::

     init(){
       if(!paramove()) ::init();
      }

BEMERKUNGEN
-----------
::

     DIESE FUNKTION NICHT MEHR BENUTZEN!

     Diese Funktion sollte nur aus einem init() aufgerufen werden!

     Fuer die Entscheidung, in welchem Raum ein Spieler in Abhaengigkeit 
     von P_PARA landet, ist die Funktion move() zustaendig. Als Magier 
     muss man sich darum nicht gesondert kuemmern. Das heisst aber auch, 
     dass beim Anschluss eines Normalweltraumes automatisch alle in dem 
     Verzeichnis mit gleichem Namen vorhandenen Parallelweltraeume mit 
     angeschlossen werden.

     Deswegen ist paramove() veraltet und sollte nicht mehr genutzt werden.

SIEHE AUCH
----------
::

     /std/room/para.c, P_PARA, P_NO_PLAYERS, move


Last modified: 05.08.2007, Zesstra


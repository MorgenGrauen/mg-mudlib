get_killing_player()
====================

FUNKTION
--------
::

     protected object get_killing_player()

DEFINIERT IN
------------
::

     /std/living/life.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Liefert im Tod (nach dem toetenden do_damage()) das Spielerobjekt, was
     den Tod wohl zu verantworten hat, falls es ermittelt werden kann. Es
     werden registrierte Helfer-NPC und einige schadenverursachende Objekte
     beruecksichtigt. Hierbei wird QueryUser() in den Objekten abgefragt.

     Es benoetigt ein gueltiges P_KILLER, d.h. falls das Lebewesen vergiftet
     wurde oder das toetende Objekt aus sonstigen Gruenden nicht in P_KILLER
     steht, funktioniert es nicht.
     Auch gibt es bestimmt Objekte, die fuer Spieler toeten koennen, die die
     diese Funktion nicht kennt.
     (Dies gilt beides ebenso fuer /p/service/mupfel/getkill.c, ist also kein
      Grund, jenes statt dieser Funktion zu nutzen.)

RUeCKGABEWERT
-------------
::

     Das Objekt des Spielers, falls es ermittelt werden konnte, sonst 0.

BEMERKUNGEN
-----------
::

    Der Name des Spieler ist mittel Name() ermittelbar. Will man die Info, 
    womit ein Spieler den Kill ggf. gemacht hat, kann man P_KILLER
    auswerten/nutzen.

SIEHE AUCH
----------
::

     QueryUser
     P_KILLER

11.11.2013, Zesstra


HasAutoloader()
===============

FUNKTION
--------

    int HasAutoloader(string uuid, string loadname);

DEFINIERT IN
------------

    /d/seher/haeuser/moebel/autoloadertruhe.c

ARGUMENTE
---------

    uuid
      UUID des Spielers, dessen Autoloader durchsucht werden sollen

    loadname
      Ladename des gesuchten Autoloaders

BESCHREIBUNG
------------

    Mit dieser Funktion kann man ueberpruefen, ob ein Spieler einen
    bestimmten Autoloader in seiner Autoloadertruhe eingelagert hat.

RUeCKGABEWERT
-------------

    1
      wenn der Spieler den Autoloader eingelagert hat
    0
      wenn nicht
   -1
      wenn Unfug uebergeben wurde, der Spieler keine Truhe besitzt oder die
      Funktion nicht in der Blueprint aufgerufen wurde


HINWEIS
-------

    Die Funktion muss in der Blueprint der Autoloadertruhe gerufen
    werden, weil nur diese ueber alle Daten verfuegt.


Last modified: 2023-08-27, Arathorn


SetDefaultHome()
================

FUNKTION
--------

  protected string SetDefaultHome(string home)

DEFINIERT IN
------------

  /std/player/base.c
  /sys/player/base.h

ARGUMENTE
---------

  string home
    Der Startraum des Spielers

BESCHREIBUNG
------------

  Diese Funktion wird in der Shell verwendet, um den Standard-Startraum zu
  setzen fuer alle Objekte dieser Shell (Rasse) zu setzen.
  Fuer individuelle Spielerobjekte kann dieser ggf. mit der Property
  P_START_HOME geaendert werden.

RUeCKGABEWERT
-------------

  Das neue Default-Home.

BEMERKUNGEN
-----------

  Diese Funktion wird **nicht** verwendet, um Seherhaeuser oder
  Magier-Workrooms zu setzen. Seherhaeuser werden ueber
  /d/seher/haeuser/hausverwalter konfiguriert, Workrooms ueber
  :doc:`../props/P_START_HOME`.

SIEHE AUCH
----------

  :doc:`QueryDefaultHome`, :doc:`../props/P_START_HOME`

Letzte Aenderung: 28.09.2022, Bugfix

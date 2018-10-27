create()
========

FUNKTION
--------

  protected void create();

DEFINIERT IN
------------

  allen Standardobjekten

ARGUMENTE
---------

  keine

BESCHREIBUNG
------------

  Diese Funktion wird aufgerufen, wenn ein Objekt geladen oder geclont
  wird.
  In dieser Funktion wird das Objekt initialisiert und konfiguriert.
  Waehrend des create() kann es einen this_player()/this_interactive()
  geben, muss aber nicht!
  Im create() hat das Objekt noch kein environment().
  create() wird nur gerufen, wenn das Objekte geclont oder explizit geladen
  wird. Wenn es aufgrund eines inherit in einem anderen Objekt vom Driver
  geladen wird, wird das create() nicht ausgefuehrt (s. create_super()).

RUeCKGABEWERT
-------------

  keiner

BEMERKUNGEN
-----------

  Erbt man von anderen Objekten, so besteht die erste Aktion innerhalb
  von create() normalerweise darin, in diesen Objekten create()
  aufzurufen.

  In altem Code wird create() haeufig ohne Sichtbarkeitsmodifikator
  verwendet, es sollte aber immer protected gewaehlt werden, es gibt keinen
  guten Grund create() mehr als einmal zu rufen, im Gegenteil, wird das
  geerbte create() mehrfach gerufen kann das zu schwer zu findenden Bugs
  fuehren.

  Um Speicher zu sparen, kann man bei Blueprints von der Konfigurierung
  absehen (siehe Beispiel). Dies sollte man allerdings nicht bei Objekten
  machen, von denen keine Clones erzeugt werden sollen (zB. bei Raeumen). 

  Man sollte bei Abbruch des create() in BP unbedingt set_next_reset(-1);
  rufen, da sonst die (nicht konfigurierte) BP resetten kann und dann
  buggt.

BEISPIELE
---------

  Ein Gegenstand wuerde wie folgt konfiguriert:

.. code-block:: pike

  inherit "/std/thing";

  #include <properties.h>

  protected void create()
  {
    // Wenn wir die Blueprint sind: NICHT konfigurieren!
    // Bei normalen Raeumen oder Transportern sind diese 
    // Zeilen wegzulassen!!!
    if (!clonep(this_object()))
    {
      set_next_reset(-1);    // wichtig, damit die BP nicht resettet.
      return;
    }

    // Ansonsten die allgemeinen Eigenschaften von /std/thing
    // konfigurieren...
    ::create();

    // ...und nun unsere speziellen Eigenschaften:
    SetProp(P_NAME, "Muell");
    SetProp(P_SHORT, "Muell");
    SetProp(P_LONG, "Voellig unnuetzer Muell!\n");
    SetProp(P_ARTICLE, 0);
    SetProp(P_VALUE, 0);
    SetProp(P_GENDER, MALE);
  }

SIEHE AUCH
----------

  :doc:`create`, :doc:`reset`, :doc:`create_super`
  set_next_reset()

Letzte Aenderung: 27.10.2018, Bugfix

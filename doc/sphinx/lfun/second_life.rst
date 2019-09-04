second_life()
=============

FUNKTION
--------

  protected varargs int second_life(object corpse);

DEFINIERT IN
------------

  /std/player/life.c

ARGUMENTE
---------

  corpse
    Leiche des Lebewesens (sofern es eine hat)

BESCHREIBUNG
------------

  Diese Funktion wird im die() des Lebewesens aufgerufen, wenn sicher
  ist, dass es stirbt. Die Funktion bestimmt dabei, ob ein Lebewesen
  nach dem Tod zerstoert (NPC) oder nur zum Geist wird (Spieler).
  
  Ueblicherweise ist diese Funktion nur im Spieler definiert und regelt
  EP-Verlust und dergleichen. Sie wird aber auch in NPCs gerufen und man
  kann dort z.B. Items clonen oder entsorgen.

  NPC *muessen* 0 zurueckgeben, Spieler geben immer 1 zurueck.

RUeCKGABEWERT
-------------

  0
    wenn das Objekt nach dem Tod zerstoert wird (NPC)
  1
    wenn das Objekt im Tod nicht zerstoert wird (Spieler)

BEMERKUNGEN
-----------

  Das Inventar des Livings wurde bei Aufruf von second_live() normalerweise
  schon in die Leiche bewegt, sofern diese existiert. War das Inventar sehr
  gross, koennen allerdings noch vereinzelte Objekte im Living sein, die
  erst spaeter bewegt werden. Will man noch Gegenstaende hinzufuegen, muss
  man diese direkt in die Leiche bewegen, *nicht* in das gestorbene
  Lebewesen.

BEISPIEL
--------

.. code-block:: pike

  protected varargs int second_life(object corpse)
  {
    // Wenn man sich wirklich sicher ist, dass das Lebewesen eine Leiche
    // hat (weil es der eigene NPC ist), ist es verfuehrerisch, die Pruefung
    // auf die Existenz der Leiche wegzulassen. Aber auch dann koennte es ja
    // passieren, dass diese vom Raum bereits zerstoert wurde.
    if (corpse)
        corpse->AddItem("tolle_trophaehe",REFRESH_NONE);
    return 0;
  }

SIEHE AUCH
----------

  die()

Letzte Aenderung: 04.09.2019, Bugfix


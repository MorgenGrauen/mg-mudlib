RemoveItem()
============

FUNKTION
--------

  void RemoveItem(<string|string*> file);

DEFINIERT IN
------------

  /std/container/items.c

ARGUMENTE
---------

  file
    String oder Array von Strings mit dem Namen des zu entfernenden
    Objekts.

BESCHREIBUNG
------------

  Das mit AddItem(file) dem Raum hinzugefuegte Objekt wird wieder aus
  der Liste der Objekte entfernt.
  Wurde bei AddItem() ein Array von Dateinamen uebergeben, so muss das
  selbe Array auch bei RemoveItem() uebergeben werden!
  Falls das Objekt, das durch den AddItem()-Aufruf erzeugt wurde, sich noch
  innerhalb des Objektes befindet, an dem RemoveItem() aufgerufen wird, wird
  es dadurch zerstoert.

RUECKGABEWERT
-------------

  keiner

BEMERKUNGEN
-----------

  RemoveItem() loest im Gegensatz zu AddItem() bei NPCs keine Meldung aus, da
  diese nur zerstoert und nicht bewegt werden. Wird hier eine Meldung
  gewuenscht, muss diese selbst ausgegeben werden.

BEISPIELE
---------

  Ein muellschluckerfreier Laden laesst sich wie folgt erzeugen:

.. code-block:: pike

  inherit "/std/laden";
  #include <properties.h>

  protected void create()
  {
    ::create();  // Hier wird u.a. der Muellschlucker erzeugt

    RemoveItem("/obj/entsorg"); // und weg damit!

    SetProp(...);  // und die normale Beschreibung...
  }

SIEHE AUCH
----------

  :doc:`AddItem`, /std/room/items.c

Letzte Aenderung: 02.11.2020, Bugfix

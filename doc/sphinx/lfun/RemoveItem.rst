RemoveItem()
============

FUNKTION
--------

  void RemoveItem(mixed file);

DEFINIERT IN
------------

  /std/room/items.c

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
  Falls das Objekt, das durch den AddItem()-Aufruf erzeugt wurde, sich
  noch im Raum befindet, wird es durch den RemoveItem()-Aufruf zerstoert.

RUECKGABEWERT
-------------

  keiner

BEISPIELE
---------

  Ein muellschluckerfreier Laden laesst sich wie folgt erzeugen:

.. code-block:: pike

  inherit "/std/laden";
  #include <properties.h>

  create()
  {
    ::create();  // Hier wird u.a. der Muellschlucker erzeugt

    RemoveItem("/obj/entsorg"); // und weg damit!

    SetProp(...);  // und die normale Beschreibung...
  }

SIEHE AUCH
----------

  :doc:`AddItem`, /std/room/items.c


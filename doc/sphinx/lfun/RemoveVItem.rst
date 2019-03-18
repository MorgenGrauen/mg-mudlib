AddvItem()
==========

FUNKTION
--------

  public varargs void RemoveVItem(string key)

DEFINIERT IN
------------

  /std/container/vitems.c

ARGUMENTE
---------

  key
    Eindeutige Bezeichnung des vItems - im Raum unter dieser ID ansprechbar

BESCHREIBUNG
------------

  Das unter <key> eingetragene vItem wird aus dem Container/Raum wieder
  entfernt.

  Es wird hierbei ggf. auch das zuletzt mitgenommene Objekte
  zerstoert, falls dieses noch im Raum herumliegt (z.B. Spieler nimmt das
  vItem und laesst es fallen). Es werden jedoch keine in der Vergangenheit
  erzeugten Objekten, die wirklich mitgenommen wurden, entfernt.

SIEHE AUCH
----------

  :doc:`AddVItem`, :doc:`AddItem`, :doc:`RemoveItem`
  :doc:`../std/vitems`

Last modified: 19.03.2019, Zesstra


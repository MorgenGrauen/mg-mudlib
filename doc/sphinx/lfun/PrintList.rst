PrintList()
==============

FUNKTION
--------

    varargs protected int PrintList(string filter_fun, int liststyle)

DEFINIERT IN
------------
    /std/room/shop.c

ARGUMENTE
---------
    string filter_fun:
      Bestimmt welche Art von Items angezeigt werden sollen (z.B. nur Waffen).

    int liststyle:
      Angabe zur Formatierung.
      - 1: Einspaltige Ausgabe mit mehr Zeichen fuer den Namen.
      - 0: Die Standardausgabe mit zwei Spalten.

BESCHREIBUNG
------------

    Erstellt anhand der Parameter und dem Rueckgabewert von GetShopItems()
    die Ausgabe der zu verkaufenden Gegenstaende fuer den Spieler.

RUECKGABEWERTE
--------------

    Immer 1.

BEMERKUNGEN
-----------

  filter_fun bekommt als Argument ein Objekt uebergeben und muss einen Wert
  !=0 zurueckgeben, wenn das Objekt in die Ausgabe aufgenommen werden soll.
  Es funktionieren alle Datentypen, es empfiehlt sich aber ein Integer von 1
  fuer true.

SIEHE AUCH
----------

    :doc:`GetShopItems`

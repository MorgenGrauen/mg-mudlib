remove()
========

FUNKTION
--------

  varargs int remove(int silent);

DEFINIERT IN
------------

  /std/thing/moving.c
  /std/living/moving.c
  /std/room/moving.c
  /std/unit.c

ARGUMENTE
---------

  - silent: Falls ungleich 0, so werden beim Zerstoeren keine Meldungen
    ausgegeben.

BESCHREIBUNG
------------

  Die meisten Objekte zerstoeren sich durch den Aufruf von remove() selbst.
  
  Unitobjekte reduzieren P_AMOUNT um U_REQ Einheiten. Will man ein Unit-
  Objekt vollstaendig zerstoeren, ist vor dem Aufruf von remove() U_REQ auf
  P_AMOUNT zu setzen. Wird durch remove() P_AMOUNT 0, wird das Objekt
  natuerlich zerstoert.

  Durch Ueberladen dieser Funktion kann man diesen Vorgang noch durch die
  Ausgabe von Meldungen kommentieren, oder irgendwelche Daten
  abspeichern, oder das Zerstoeren ganz verhindern (auf diesem Weg... Mit
  destruct() kann das Objekt immer noch direkt zerstoert werden!)
  
  ACHTUNG: Wenn ein Objekt durch remove() nicht zerstoert wird, koennte das
  einen Grund haben. Bitte nicht einfach destruct() verwenden, das ist nur
  fuer Notfaelle wie ein buggendes remove().

RUeCKGABEWERT
-------------

  1, wenn sich das Objekt erfolgreich selbst zerstoert hat, sonst 0.

BEMERKUNGEN
-----------

  Nach einem erfolgreichen ::remove() gelten die selben Einschraenkungen
  wie nach einem destruct()!

SIEHE AUCH
----------

  :doc:`../efun/destruct`,
  :doc:`../props/U_REQ`, :doc:`../props/P_AMOUNT`


Letzte Aenderung: 24.07.2018, Bugfix


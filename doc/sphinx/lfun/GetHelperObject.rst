GetHelperObject()
=================

FUNKTION
--------

  public varargs object GetHelperObject(int type, int|closure strength)

DEFINIERT IN
------------

  /std/living/helpers.c

ARGUMENTE
---------

  * type: Eine der in helpers.h definierten Konstanten
  * strength: Staerke des Helfers
    (Rueckgabewert der Callback-Methode, siehe :doc:`RegisterHelperObject`.)
    oder Closure, die als Argumente das Living und die Callback-Closure des
    Helpers uebergeben bekommt. Gibt die an GetHelperObject() uebergebe
    Closure 1 zurueck, wird das aktuelle Objekt zurueckgegeben.

BESCHREIBUNG
------------

  Gibt das zuerst gefundene als Helfer registrierte Objekt zurueck, welches
  die Anforderungen erfuellt.

RUECKGABEWERT
-------------

  Objekt welches den uebergebenen Anforderungen entspricht.

SIEHE AUCH
----------

  :doc:`RegisterHelperObject`, :doc:ÙnregisterHelperObject`
  :doc:`../props/P_AERIAL_HELPERS`, :doc:`../props/P_AQUATIC_HELPERS`, :doc:`../props/P_HELPER_OBJECTS`

Letzte Aenderung: 27.02.2018, Bugfix

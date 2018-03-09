GetHelperObject()
=================

FUNKTION
--------

  public varargs object GetHelperObject(int type, int|closure strength
                                        varargs mixed* extra)

DEFINIERT IN
------------

  /std/living/helpers.c

ARGUMENTE
---------

  type:
    Eine der in helpers.h definierten Konstanten
  strength:
    minimal geforderte Staerke des Helfers oder Closure zum Filtern der
    Helferobjekte.
  extra:
    Beliebige und beliebig viele Argumente, welche an die Closure <strength>
    weitergegeben werden.

BESCHREIBUNG
------------

  Gibt das zuerst gefundene als Helfer registrierte Objekt zurueck, welches
  die Anforderungen erfuellt.

  Ist <strength> ein int, muss die Staerke/Guete des Helfers (Rueckgabewert
  der Callback-Methode, siehe :doc:`RegisterHelperObject`) mindestens so gross
  sein wie der als <strength> uebergebene Wert.

  Ist <strength> eine Closure, wird diese fuer jeden Helfer ausgewertet und
  entscheidet, ob der Helfer akzeptiert wird. Die Closure bekommt hierbei das
  Helferobjekt, die vom Helfer angegebene Staerke/Guete und etwaige hier als
  <extra> uebergebene Argument uebergeben. Ist der Rueckgabewert von
  <strength> != 0, wird der Helfer akzeptiert und von GetHelperObject
  zurueckgegeben (ohne weitere Helfer zu pruefen).


RUECKGABEWERT
-------------

  Objekt welches den uebergebenen Anforderungen entspricht.


BEISPIELE
---------

.. code-block:: pike

  // Es wird ein Helfer zum Tauchen mit der Staerke 4 gesucht
  PL->GetHelperObject(HELPER_TYPE_AQUATIC, 4);

.. code-block:: pike

  // Es wird ein Helfer zum Fliegen mit der Staerke 4 gesucht, welcher aber
  // mindestens 60 cm gross und maximal 900 g schwer ist.
  PL->GetHelperObject(HELPER_TYPE_AERIAL, function int (object h, int s)
        { return s >= 4
          && h->QueryProp(P_SIZE) >= 60 && h->QueryProp(P_WEIGHT) <= 900;
        }
        );

.. code-block:: pike

  // Es wird ein Helfer zum Fliegen mit der Staerke 4 gesucht, welcher aber
  // mindestens 60 cm gross und maximal 900 g schwer ist, aber diese Grenzen
  // werden nicht in der Funktion festgelegt, sondern als <extra> uebergeben.
  PL->GetHelperObject(HELPER_TYPE_AERIAL,
        function int (object h, int s, int min_s, int minsize, int maxweight)
          { return s >= min_s
                 && h->QueryProp(P_SIZE) >= minsize
                 && h->QueryProp(P_WEIGHT) <= maxweight;
          },
        4, 60, 900);


SIEHE AUCH
----------

  :doc:`RegisterHelperObject`, :doc:`UnregisterHelperObject`,
  :doc:`../props/P_AERIAL_HELPERS`, :doc:`../props/P_AQUATIC_HELPERS`, :doc:`../props/P_HELPER_OBJECTS`

Letzte Aenderung: 9.3.2018, Zesstra

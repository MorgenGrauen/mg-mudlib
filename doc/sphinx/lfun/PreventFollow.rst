PreventFollow()
===============

FUNKTION
--------

  int PreventFollow(object dest)

ARGUMENTE
---------

  dest:
    Zielobjekt, in das der Verfolgte bewegt werden soll.

FUNKTION
--------

  In jedem Verfolger, der mit AddPursuer in die Liste der Verfolger 
  eingetragen wurde, wird vor dem Bewegen in das Zielobjekt die Funktion
  PreventFollow mit dem Zielobjekt als Argument aufgerufen.

RUECKGABEWERT
-------------

  0:
    Verfolger darf in das Zielobjekt folgen
  1:
    Verfolger darf in dieses Zielobjekt nicht folgen
    (Verfolgung bleibt weiterhin aktiv)
  2:
    Verfolger darf in dieses Zielobjekt nicht folgen
    (Verfolgung wird abgebrochen und Verfolger aus der Verfolgerliste
    ausgetragen)

BEISPIELE
---------

  Man moechte, dass nur dann verfolgt wird, wenn das Ziel im gleichen Gebiet
  liegt, wie man selber (wenn __PATH__(1) das Gebietsverzeichnis ist):

  .. code-block:: pike

  int PreventFollow(object ziel)
  {
    if (strstr(load_name(ziel), __PATH__(1)) != 0)
      return 1;
  }

SIEHE AUCH
----------

  - :doc:`AddPursuer`, :doc:`RemovePursuer`


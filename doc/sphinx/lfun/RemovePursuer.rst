RemovePursuer()
===============

FUNKTION
--------
::

  void RemovePursuer(object pursuer)

ARGUMENTE
---------
::

  pursuer: Objekt, das aus der Verfolgerliste des Objektes, in dem
           RemovePursuer aufgerufen wurde, ausgetragen werden soll.

FUNKTION
--------
::

  Durch den Aufruf von RemovePursuer in einem Objekt, welches living() ist,
  wird das Object, welches als Argument uebergeben wurde, aus der Liste
  der Verfolger ausgetragen.

RUECKGABEWERT
-------------
::

  keiner

BEMERKUNG
---------
::

  keine

BEISPIELE
---------
::

  find_player("jof")->RemovePursuer(find_player("kirk"))
  Danach wird Jof nicht mehr von Kirk verfolgt.

SIEHE AUCH
----------

  - Lfuns:  :doc:`AddPursuer`, :doc:`PreventFollow`
  - Properties: P_PURSUERS


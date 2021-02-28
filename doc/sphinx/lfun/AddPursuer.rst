AddPursuer()
============

FUNKTION
--------
::

  void AddPursuer(object pursuer)

ARGUMENTE
---------
::

  pursuer: Objekt, das demjenigen folgen soll, in dem AddPursuer
           aufgerufen wurde.

FUNKTION
--------
::

  Durch den Aufruf von AddPursuer in einem Objekt, welches living() ist,
  wird das Object, welches als Argument uebergeben wurde, in die Liste
  der Verfolger eingetragen. Alle Objekte, die in der Verfolgerliste stehen,
  werden bei Bewegungen des Verfolgten in dasselbe Environment bewegt.

RUECKGABEWERT
-------------
::

  keiner

BEMERKUNG
---------
::

  Im Verfolger wird PreventFollow mit dem Zielobjekt, in das der Verfolgte 
  bewegt wird, aufgerufen. Dadurch kann der raeumliche Bereich, in dem 
  verfolgt wird, eingeschraenkt werden.

BEISPIELE
---------
::

  find_player("jof")->AddPursuer(find_player("kirk"))
  Danach wird Jof von Kirk verfolgt.

SIEHE AUCH
----------

  - Lfuns:       :doc:`RemovePursuer`, :doc:`PreventFollow`
  - Properties:  P_PURSUERS


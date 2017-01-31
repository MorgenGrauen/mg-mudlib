PreventFollow()
===============

FUNKTION
--------
::

  int PreventFollow(object dest)

ARGUMENTE
---------
::

  dest: Zielobjekt, in das der Verfolgte bewegt werden soll.

FUNKTION
--------
::

  In jedem Verfolger, der mit AddPursuer in die Liste der Verfolger 
  eingetragen wurde, wird vor dem Bewegen in das Zielobjekt die Funktion
  PreventFollow mit dem Zielobjekt als Argument aufgerufen.

RUECKGABEWERT
-------------
::

  0: Verfolger darf in das Zielobjekt folgen
  1: Verfolger darf in dieses Zielobjekt nicht folgen
     (Verfolgung bleibt weiterhin aktiv)
  2: Verfolger darf in dieses Zielobjekt nicht folgen
     (Verfolgung wird abgebrochen und Verfolger aus der Verfolgerliste
      ausgetragen)

BEMERKUNG
---------
::

  Durch PreventFollow kann der raeumliche Bereich, in dem verfolgt werden
  darf, eingeschraenkt werden.

BEISPIELE
---------
::

  Man moechte, dass ein NPC auf einer Insel nicht mit dem Spieler in das
  Boot steigt, um mit dem Spieler zusammen von der Insel herunterzukommen.

  #define PATH(x) ("/d/.../.../mein/pfad/+(x)")                           

  ...                                          

  int PreventFollow(object boot)                                           
   {
    if ( object_name(boot)[0..strlen(PATH("boot"))-1] == PATH("boot") )
     return 1;
   }                                                                         

  Diese Funktions-Definition ist sehr flexibel, denn sie erlaubt sowohl
  spaetere Pfadanpassung als auch mehrere Boote.
  Da ueber die Funktion strlen() nur bis zum letzten Buchstaben des    
  angegebenen Strings getestet wird, wird also gleichzeitig auch auf          
  boot[1], boot[2] usw. getestet.

SIEHE AUCH
----------
::

  "AddPursuer", "RemovePursuer"

Last modified: Tue Jun 10 13:59:30 2003 by Gabylon


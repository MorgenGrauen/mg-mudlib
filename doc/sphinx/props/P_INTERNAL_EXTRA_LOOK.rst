P_INTERNAL_EXTRA_LOOK
=====================

NAME
----

    P_INTERNAL_EXTRA_LOOK
      "internal_extralook"

DEFINIERT IN
------------

    /sys/living/description.h

BESCHREIBUNG
------------

  Diese Property enthaelt ein Mapping, in dem alle einzelnen
  Extra-Look-Eintraege des Livings enthalten sind. Dabei weden die Daten von
  AddExtraLook() und RemoveExtraLook() verwaltet. Fragt man diese Prop mit
  QueryProp() ab, erhaelt man die Ausgabe der gueltigen Extralooks des
  Livings. Bei Abfrage per Query() erhaelt man ein Mapping mit allen
  Eintraegen und deren Daten.
  Der Key ist jeweils die ID des Extralooks, der Value ist erneut ein 
  Mapping, welches folgende Keys enthalten kann:
  
  "xllook":
    String, der im Extralook des Living angezeigt wird.
  "xlduration":
    Zeitstempel (int), der angibt, wie lang der Eintrag gueltig
    ist. 0 == "Unendlich", negative Zahlen besagen, dass der Eintrag nur 
    bis Reboot/Ende gueltig sein soll und abs(xlduration) ist der Zeitpunkt
    des Eintrages dieses Extralooks.
  "xlende":
    String, der nach Ablaufen an das Living ausgegeben wird.
  "xlfun":
    Funktion, die gerufen wird und den String zurueckliefern muss, der
    ausgegeben werden soll.
  "xlendefun":
    Funktion, die gerufen wird, wenn der Eintrag abgelaufen ist und den
    String zurueckliefern muss, der dann ans Living ausgegeben wird.
  "xlobjectname":
    Objekt, in dem die o.a. Funktionen gerufen werden.

  .. warning::
  
  DIESE PROPERTY BITTE **NIEMALS** PER HAND MIT Set()/SetProp() AENDERN!
  Die Daten in dieser Prop werden vom Living selber verwaltet. Extralooks
  koennen mittel AddExtraLook() eingetragen und mit RemoveExtraLook() entfernt
  werden.

SIEHE AUCH
----------

  - :doc:`../lfun/long`
  - /std/living/description.c, /std/player/base.c
  - :doc:`../lfun/AddExtraLook`, :doc:`../lfun/RemoveExtraLook`


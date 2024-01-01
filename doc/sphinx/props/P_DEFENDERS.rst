P_DEFENDERS
===========

NAME
----

  P_DEFENDERS      "defenders"

DEFINIERT IN
------------

  /sys/new_skills.h

BESCHREIBUNG
------------

  Diese Property wird in Lebewesen gesetzt, welche zum Beispiel durch
  andere Lebewesen verteidigt werden. Die Verteidiger muessen
  natuerlich bekannt sein, damit sie per InformDefend() ueber Angriffe
  informiert werden und per DefendOther() in den laufenden Angriff
  eingreifen koennen (zum Beispiel Schaeden abwehren oder umwandeln).
  Es muessen jedoch nicht unbedingt Lebewesen oder echte Verteidiger
  sein, auch beliebige Objekte koennen ueber Angriffe informiert
  werden und in diese eingreifen. Allerdings besteht die
  Einschraenkung, dass diese Objekte in der gleichen Umgebung sein
  muessen, wie das zu verteidigende Lebewesen oder im zu verteidigenden
  Lebewesen selbst.
  Die Objekte, welche dies betrifft, sind in Form eines Arrays in
  der Property P_DEFENDERS abgelegt.
  
  ACHTUNG:
  Auf diese Property nicht von Hand zugreifen, sondern bitte die
  entsprechende Zugriffsfunktion verwenden.

SIEHE AUCH
----------

  :doc:`AddDefender`, :doc:`RemoveDefender`, :doc:`QueryDefenders`, 
  :doc:`QueryPresentDefenders`, :doc:`QueryNearDefenders`, 
  :doc:`InformDefend`, :doc:`DefendOther`,
  /std/living/combat.c

Letzte Aenderung: 28.10.2022, Bugfix

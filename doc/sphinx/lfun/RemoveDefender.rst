RemoveDefender()
================

FUNKTION
--------

  void RemoveDefender(object friend);

DEFINIERT IN
------------

  /std/living/combat.c

ARGUMENTE
---------

  friend
    Objekt (normal Lebewesen), welches zukuenftig nicht mehr ueber
    Angriffe informiert werden soll und diese auch nicht mehr abwehrt.

BESCHREIBUNG
------------

  Ein Lebewesen, welches angegriffen wird, kann andere Objekte ueber
  einen solchen Angriff per InformDefend() informieren oder ihnen
  sogar die Moeglichkeit geben, per DefendOther() direkt in den
  laufenden Angriff einzugreifen (Schaeden abwehren oder umwandeln).
  Im Normalfall handelt es sich hierbei um andere Lebewesen, welche
  als Verteidiger des angegriffenen Lebewesens auftreten: Daher der
  Name der Funktion. Ausserdem besteht die Einschraenkung, dass diese
  Objekte in der gleichen Umgebung sein muessen, wie das zu
  verteidigende Lebewesen.

SIEHE AUCH
----------

  :doc:`AddDefender`, :doc:`QueryDefenders`, 
  :doc:`QueryPresentDefenders`, :doc:`QueryNearDefenders`, 
  :doc:`InformDefend`, :doc:`DefendOther`,
  :doc:`../props/P_DEFENDER`, /std/living/combat.c

Letzte Aenderung: 28.10.2022, Bugfix

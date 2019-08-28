P_AGGRESSIVE
============

NAME
----

  P_AGGRESSIVE                  "aggressive"                  

DEFINIERT IN
------------

  /sys/npc.h

BESCHREIBUNG
------------

	Gesetzt, wenn das Wesen von sich aus Angriffe startet.

	Ueblicherweise nimmt man als Wert 1, man kann jedoch auch
	einen kleineren Wert nehmen wenn es nur mit einer bestimmten
	Wahrscheinlichkeit automatisch angreifen soll.

	Der Wert von Spieler und Monster wird addiert, um zu entscheiden,
	ob ein Spieler angegriffen werden soll,	man kann P_AGGRESSIVE
	also auch bei Spielern setzen, so dass sie von allen Monstern
	angegriffen werden.

	Bei Monstern (und NUR bei diesen) kann man hier auch ein Mapping
	angeben, das als Keys Namen von Properties des Spielers enthaelt
	und als Values Mappings, in denen steht welcher Wert bei welchen
	Wert fuer die Property genommen werden soll (Beispiele folgen).
	Mit Key 0 kann man einen Default-Wert (sowohl in inneren Mappings
	wie auch im aeusseren Mapping) festlegen. Default-Werte werden
	genommen, falls keine anderen gesetzt sind, also Vorsicht mit
	0-Eintraegen (Tip: 0.0 ist in LPC ungleich 0).
	Bei mehreren Properties werden alle gesetzten Werte gemittelt.

BEISPIELE
---------

	SetProp(P_AGGRESSIVE,([P_RACE:(["Zwerg":1, "Elf":0.0, 0:0.5])]))
	Zwerge werden immer automatisch angegriffen, Elfen nie und
	alle anderen mit 50% Wahrscheinlichkeit.
	Man beachte, dass hier 0.0 genommen werden musste anstelle von 0,
	weil sonst Elfen auch 50% Wahrscheinlichkeit bekommen haetten.

	SetProp(P_AGGRESSIVE,([P_RACE:(["Zwerg":0.3]),
	                       P_GUILD:(["Chaos":0.7])]))
	Zwerge werden mit 30% Wahrscheinlichkeit angegriffen,
	Chaoten mit 70% und Zwerg-Chaoten mit 50%.

SIEHE AUCH
----------

  :doc:`../lfun/Kill`, :doc:`../lfun/AutoAttack`

Letzte Aenderung: 28.08.2019, Bugfix

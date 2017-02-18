P_WC
====

NAME
----
::

	P_WC				"wc"

DEFINIERT IN
------------
::

	/sys/weapon.h

BESCHREIBUNG
------------
::

	Die Waffenklasse (engl: weapon class), also die Staerke der Waffe,
	stellt einen numerischen Wert dar, der umso groesser ist, desto mehr
	Schaden eine Waffe im Kampf anrichtet. Beim Zuecken oder Wegstecken
	einer Waffe durch ein Lebewesen wird innerhalb des Lebewesens auch
	die Property P_TOTAL_WC aktualisiert, welche somit immer die
	aktuelle Angriffsstaerke enthaelt. Beim Zuecken erhaelt sie hierbei
	die Waffenklasse der Waffe und beim Wegstecken die Angriffsstaerke
	aus der Property P_HANDS (Kaempfen mit blossen Haenden).
	Die Waffenklasse von einhaendigen Waffen sollte 150 nicht
	ueberschreiten, die Obergrenze fuer zweihaendige Waffen liegt bei
	200. Ausnahmen von dieser Regel beduerfen der Absprache mit der
	Balance.
	Negative Werte bewirken keinen Schaden, allerdings auch keine
	Heilung.

BEMERKUNGEN
-----------
::

	Query- und Setmethoden auf P_WC sollten unbedingt vermieden werden. Sie
	fuehren in der Regel zu massiven Inkonsistenzen im Mechanismus der 
	Ruestungsbeschaedigung und -reparatur.
	Auch mit einer HitFunc() duerfen die Obergrenzen nicht ohne
	Absprache ueberschritten werden! Ausserdem ist es ratsam, die
	zusaetzlichen Kampfeigenschaften in P_EFFECTIVE_WC gesondert
	anzugeben.

SIEHE AUCH
----------
::

	/std/weapon.c, /std/weapon/combat.c
	P_DAMAGED, P_EFFECTIVE_WC, P_WEAPON_TYPE
	Damage()

14.02.2017, Bugfix


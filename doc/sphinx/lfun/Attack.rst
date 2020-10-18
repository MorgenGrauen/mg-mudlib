Attack()
========

FUNKTION
--------
::

	void Attack(object enemy)

ARGUMENTE
---------
::

	enemy: Der Feind.

BESCHREIBUNG
------------
::

  Der Feind wird entsprechend der Angriffsstaerke der Waffe (P_WC) bzw. der 
  Haende (P_HANDS[1]) angegriffen.

RUECKGABEWERT
-------------
::

	Keiner.

BEMERKUNG
---------
::

	Diese Funktion gibt die Angriffsmeldung aus.
	Falls mit blossen Haenden angegriffen wird, ist die Staerke
	(2 * P_HANDS[1] + 10 * A_STR)/3

SIEHE AUCH
----------
::

	Defend(), QueryDamage()

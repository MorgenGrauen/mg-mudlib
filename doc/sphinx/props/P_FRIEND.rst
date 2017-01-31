P_FRIEND
========

NAME
----
::

	P_FRIEND			"friend"

DEFINIERT IN
------------
::

	<combat.h>

BESCHREIBUNG
------------
::

	Setzt man diese Property in einem NPC auf einen Wert ungleich Null,
	so wird der NPC bei Zauberspruechen, die auf bestimmte Gruppen
	wirken, der Gruppe der Spieler und nicht der Gruppe der NPCs
	zugeordnet. Ausserdem wird der NPC bei einem "toete alle" nicht mit
	angegriffen.
	Wurde der NPC einem Spieler per AssocMember() zugeordnet, so
	befindet sich der NPC automatisch im jeweiligen Team des Spielers
	 (moeglichst auch in der selben Kampfreihe), und die Property hat
	dann automatisch das Spielerobjekt als Wert.
	Da dieser Wert in diesem Fall natuerlich ungleich Null ist, wird der
	NPC dann auch der Gruppe der Spieler zugeordnet.

SIEHE AUCH
----------
::

	FindGroup(), AssocMember(), P_TEAM_ASSOC_MEMBERS, /std/living/team.c


Last modified: Tue Dec 29 17:02:55 1998 by Patryn


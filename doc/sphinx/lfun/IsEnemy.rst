IsEnemy()
=========

FUNKTION
--------
::

	int IsEnemy(object wer);

DEFINIERT IN
------------
::

	/std/living/combat.c

ARGUMENTE
---------
::

	wer
	  Das Objekt, welches darauf ueberprueft werden soll, ob es
	  derzeit als Gegner bekannt ist.

RUeCKGABEWERT
-------------
::

	Flag: 1 bei Feindschaft, 0 sonst

BESCHREIBUNG
------------
::

	Mit dieser Funktion kann man ueberpruefen, ob das Objekt <wer>, also
	im Normalfall ein Lebewesen, derzeit als Gegner erkannt wird. Nach
	einer gewissen Zeit laeuft die Feindschaft automatisch aus, sodass
	man sich nicht darauf verlassen, dass ein Gegner auch zukuenftig als
	solchiger erkannt wird. (Diese Zeit betraegt normal 300 Sekunden.)
	Solch eine Feindschaft kann im uebrigen auch einseitig sein! Mehr
	dazu findet man in der Dokumentation zu StopHuntFor(), einer
	Funktion, die Frieden zwischen Gegnern stiften soll.

SIEHE AUCH
----------
::

	SelectEnemy(), QueryEnemies(), StopHuntFor()


Last modified: Wed May 26 16:47:51 1999 by Patryn


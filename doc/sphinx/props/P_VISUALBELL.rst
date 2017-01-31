P_VISUALBELL
============

NAME
----
::

	P_VISUALBELL			"visualbell"

DEFINIERT IN
------------
::

	/sys/properties.h

BESCHREIBUNG
------------
::

	Die Property stellt ein Flag innerhalb von Spielern dar, welches
	standardmaessig nicht gesetzt ist. In diesem Fall werden Toene,
	welche innerhalb einiger Funktionen erzeugt werden, auch wirklich an
	den Spieler geschickt.
	Setzt man die Property, so erhaelt der Spieler keine Toene mehr.

BEISPIEL
--------
::

	Pieptoene werden durch den ASCII-Code 0x7 praesentiert. Ausgeben
	kann man diesen folgendermassen:
	  if(!IS_WIZARD(caster)&&!victim->QueryProp(P_VISUALBELL))
	    tell_object(victim,sprintf("%c",7));
	Das waere beispielsweise ein Codestueck aus einem Piepspell. :)
	Das Opfer bekommt den Piepton hierbei nur ab, wenn der Caster ein
	Magier ist oder das Spieleropfer die Property P_VISUALBELL gesetzt
	hat (kann mit Kommando 'ton' vom Spieler beeinflusst werden).

BEMERKUNGEN
-----------
::

  Achtung: P_VISUALBELL steht auf 1, wenn der Spieler _keine_ Piepstoene
	hoeren will!
	Die Funktionalitaet dieser Property wirkt nur soweit, wie sie auch
	von tonerzeugenden Befehlen selbst unterstuetzt wird. Es ist darauf
	zu achten, dass P_VISUALBELL zu diesem Zweck grundsaetzlich
	ausgewertet wird! Eine Ausnahme sei hierbei zugelassen: Magier
	koennen Spielern grundsaetzlich Toene zusenden.

SIEHE AUCH
----------
::

	ton, wecke, erwarte, P_WAITFOR, /std/player/base.c


Last modified: 07.02.2007 by Zesstra


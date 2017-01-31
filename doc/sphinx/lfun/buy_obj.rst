buy_obj()
=========

FUNKTION
--------
::

	static string buy_obj(mixed ob, int short);

DEFINIERT IN
------------
::

	/std/shop.c

ARGUMENTE
---------
::

	ob - der Gegenstand bei dem geprueft werden soll, ob der Laden ihn
	     an this_player() verkauft. Sollte es sich hierbei um ein
	     FixedObject handeln, wird ein String uebergeben, ansonsten ein
	     object.
        short - Bisher noch nicht in Benutzung. Aber fuer die Zukunft
             vorgesehn, falls man mehrere Objekte auf einmal kauft.
             Ein auswerten ist keine Pflicht, waere aber praktisch, damit
             der Scroll dabei nicht zu gross wird.

RUeCKGABEWERT
-------------
::

        Ein String was der Haendler sagen soll wieso der Gegenstand nicht
	verkauft wird. Der String wird dabei wie folgt umgebrochen:
        break_string(str, 78, Name(WER, 1)+" sagt: ")

BESCHREIBUNG
------------
::

	Durch ueberschreiben dieser Funktion ist es moeglich bestimmte
	Objekte (wie z.b. Questobjekte) nur an ausgewaehlte Spieler zu
	verkaufen). Aber auch abfragen ob der Laden ueberhaupt mit
	this_player() handelt, sind moeglich.

BEISPIELE
---------
::

	static string buy_obj(mixed ob, int short)
	{
	   if (PL->QueryProp(P_RACE)=="Zwerg")
	      return "Ich verkaufe nichts an Zwerge!";
	   return ::buy_obj(ob, short);
	}

SIEHE AUCH
----------
::

	sell_obj(), AddFixedObject(), RemoveFixedObject(), SetStorageRoom(),
        /std/shop.c


Last modified: Thu Mar 4 15:26:13 1999 by Padreic


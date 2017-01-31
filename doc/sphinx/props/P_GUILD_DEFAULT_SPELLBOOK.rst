P_GUILD_DEFAULT_SPELLBOOK
=========================

NAME
----
::

	P_GUILD_DEFAULT_SPELLBOOK	"guild_sb"                    

DEFINIERT IN
------------
::

	/sys/new_skills.h

BESCHREIBUNG
------------
::

	Diese Gildenproperty enthaelt den Namen des Spellbooks, welches
	standardmaessig von der Gilde verwendet wird.

BEMERKUNGEN
-----------
::

	Bei Spells kann sie mit SI_SPELLBOOK ueberschrieben werden.

BEISPIELE
---------
::

	Bei Zauberern waere folgendes denkbar:
	  SetProp(P_GUILD_DEFAULT_SPELLBOOK,"magie_sp");
	Das Spellbook ist hierbei unter "/spellbooks/magie_sp.c" zu finden.

SIEHE AUCH
----------
::

	P_GUILD


Last modified: Wed Jan 14 19:17:06 1998 by Patryn


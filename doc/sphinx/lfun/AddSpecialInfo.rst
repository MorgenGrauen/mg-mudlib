AddSpecialInfo()
================

FUNKTION
--------
::

     varargs void AddSpecialInfo( frage, meldung
			   [, indent [, [silent [, casebased] ] ] );

ARGUMENTE
---------
::

     string/string* frage
	Schluesseltext(e) auf die Informationen gegeben werden sollen.
     string/closure function
	Methodenname im NPC/Closure
     string indent
	Text, der sich bei mehrzeiligen Meldungen wiederholen soll.
     int/string silent
	Ist silent gesetzt, so erfolgt Antwort nur an Fragenden.
     string/closure casebased
	Funktionsname oder Closure mit Returnwert string oder int.

DEFINIERT IN
------------
::

     /std/npc/info.c

BESCHREIBUNG
------------
::

     Wenn ein Spieler ein NPC mittels "frage <monstername> nach <frage>" nach
     einer Information mit dem Schluessel frage fragt, so wird die Methode
     "function" gerufen. Die Rueckgabe wird als Meldung ausgegeben.

     Fuer die Beschreibung der weiteren Parameter siehe man AddInfo(L).

     AddSpecialInfo(keys, "function", ...) entspricht:
     - AddInfo(keys, #'function, ...) 

BEMERKUNGEN
-----------
::

     Da AddSpecialInfo() und AddInfo() auf die gleichen Daten zugreifen,
     kann man Informationen, die mit AddSpecialInfo() gesetzt wurden, auch
     mit RemoveInfo() entfernen. Es gibt kein RemoveSpecialInfo().

BEISPIELE
---------
::

     // Das folgende Beispiel ist auch unter man AddInfo(L) zu finden.
     ### dynamisch ###
     AddSpecialInfo(({"keks","kekse"}),
		    "query_kekse",	// der Methodenname
		    "sagt: ");
     // ist uebrigens das gleiche wie:
     // static string query_kekse();
     // ...
     // AddInfo(({"keks","kekse"}),
     //		#'query_kekse,		// ein Verweis auf die Methode
     //		"sagt: ");
     ...
     static string query_kekse() {
      if(present("keks"))
       return("Ich hab noch welche. Aetsch!");
      return("Menno. Keine mehr da!");
     }

     // "frage monster nach keks":
     // - wenn es noch Kekse hat, hoeren alle:
     //   "Das Monster sagt: Ich hab noch welche. Aetsch!
     // - sonst:
     //   "Das Monster sagt: "Menno. Keine mehr da!

SIEHE AUCH
----------
::

     AddInfo(L), RemoveInfo(L)

7.Apr 2004 Gloinson


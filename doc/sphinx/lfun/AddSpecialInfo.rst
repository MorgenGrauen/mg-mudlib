AddSpecialInfo()
================

FUNKTION
--------
::

  public varargs void AddSpecialInfo(string|string* keys, string functionname,
                 string indent, int|string silent, string|closure  casebased);


ARGUMENTE
---------
::

     string|string* frage
       Schluesselwoerter, fuer die der NPC eine Antwort geben soll, wenn
       man ihn danach fragt

     string functionname
       Name der Funktion, die gerufen werden soll, um den Informationstext
       des NPCs zu ermitteln. Der gerufenen Funktion werden keine Argumente
       uebergeben.

     string indent    (optional)
       Text, der sich bei mehrzeiligen Meldungen wiederholen soll.

     int|string silent    (optional)
       Ist silent gesetzt, so erfolgt Antwort nur an Fragenden. 

     string|closure casebased   (optional)
       Closure mit Returnwert string oder int.
       Der Funktion werden keine Argumente uebergeben.

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

     Verwandt:
       :doc:`AddInfo`, :doc:`RemoveInfo`
     Props:
       :doc:`../props/P_PRE_INFO`
     Files:
       /std/npc/info.c
     Loggen:
       :doc:`../props/P_LOG_INFO`
     Interna:
       :doc:`GetInfoArr` , :doc:`do_frage`

24.09.2020, Arathorn


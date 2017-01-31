match_ids()
===========

FUNKTION
--------
::

     int match_ids(string *list);

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     *list	String-Array mit zu testenden IDs

BESCHREIBUNG
------------
::

     Die Liste der uebergebenen IDs wird mit den IDs des Objektes
     UND-Verknuepft. Die Groesse des resultierenden Arrays wird
     zurueckgegeben.
     Diese Funktion erlaubt also das gleichzeitige Pruefen auf
     mehrere IDs. Allerdings werden - im Gegensatz zu id() -
     Adjektive und Ausdruecke der Art "<ID> <nummer>" nicht
     beruecksichtigt.
     Ebenso werden Spezialitaeten wie Unitobjekte und Objekte mit
     ueberschriebenem id() nicht beruecksichtigt! Im Zweifelsfall ist daher
     doch die Nutzung von id() zu empfehlen.

RUeCKGABEWERT
-------------
::

     Anzahl der zutreffenden IDs.

BEISPIELE
---------
::

     Angenommen, ein Objekt habe folgende Bezeichner:

     AddId( ({"murmel","kugel","glasmurmel","glaskugel"}) );
     AddAdjective( "rund" );

     Dann liefern die angegebenen match_ids()-Aufrufe folgende Ergebnisse:

     match_ids( ({"murmel","stein"}) );         => 1
     match_ids( ({"murmel","kugel"}) );         => 2
     match_ids( ({"runde murmel"}) );           => 0
     match_ids( ({"murmel 2"}) );               => 0, auch wenn dies die 
                                               zweite Murmel in der
                                               gleichen Umgebung ist

SIEHE AUCH
----------
::

     AddId(), AddAdjective(), AddSingularId(), AddPluralId(), present(),
     id(), /std/thing/description.c, /std/unit.c


Last modified: Sat Mar 15 21:40:00 2000 by Paracelsus


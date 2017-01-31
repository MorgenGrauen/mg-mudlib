AddAdjective()
==============

FUNKTION
--------
::

     void AddAdjective(string|string* adj);

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     adj
          String oder Array von String mit den Adjektiven.

BESCHREIBUNG
------------
::

     Zusaetzlich zu den mit AddId() vergebenen Bezeichnern laesst sich mit
     der Vergabe von Adjektiven die Ansprechbarkeit eines Objektes erhoehen.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Die Adjektive werden nicht dekliniert, man muss also fuer jeden
     sinnvollen Fall ein Adjektiv uebergeben.

BEISPIELE
---------
::

       AddId( ({ "zettel", "blatt" }) );
       AddAdjective( ({ "kleinen", "kleines" }) );

     Das Objekt reagiert jetzt auf "zettel", "kleinen zettel", "blatt",
     "kleines blatt" sowie auf die (sprachlich nicht ganz so korrekten)
     Konstruktionen "kleines zettel", "kleinen blatt", "kleines kleinen
     zettel", ...

SIEHE AUCH
----------
::

     AddId(), RemoveAdjective() id(), present(), /std/thing/description.c


20.01.2015, Zesstra


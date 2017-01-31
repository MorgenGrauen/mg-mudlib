AddId()
=======

FUNKTION
--------
::

     void AddId(string|string* ids);

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     ids
          String oder Array von Strings mit den Bezeichnungen, mit denen
          sich sich das Objekt ansprechen lassen soll.

BESCHREIBUNG
------------
::

     Jedes Objekt sollte sich auf die eine oder andere Weise ansprechen
     lassen. Zu diesem Zweck kann man dem Objekt mit dieser Funktion
     Bezeichner uebergeben.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Jedes Objekt sollte man zumindest mit seiner Kurzbeschreibung
     ansprechen koennen! Fuer Abfragen von Questobjeken o.ae. sollte man
     zusaetzlich IDs verwenden, die Sonderzeichen wie "\n" oder "\t"
     enthalten, damit sichergestellt ist, dass der Spieler auch wirklich die
     richtigen Objekte dabeihat.

BEISPIELE
---------
::

     AddId( "buch" );
     AddId( "buechlein" );

     Das Objekt laesst sich jetzt als "buch" und als "buechlein" ansprechen.

     AddId( ({ "buch", "buechlein" }) );

     Diese Zeile bewirkt das gleiche wie die obigen zwei Zeilen.

     AddId( ({ "puzzle", "\nquest_puzzle" }) );

     Der Spieler kann das Objekt als "puzzle" ansprechen, questrelevante
     Objekte koennen mit der ID "\nquest_puzzle" nach ihm suchen.

SIEHE AUCH
----------
::

     AddAdjective(), RemoveId(), id(), present(), /std/thing/description.c

     -----------------------------------------------------------------------
20.01.2015, Zesstra


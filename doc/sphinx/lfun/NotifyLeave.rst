NotifyLeave()
=============

FUNKTION
--------
::

     void NotifyLeave(object ob, object dest);

ARGUMENTE
---------
::

     ob
          Das Objekt, das aus dem Behaelter entfernt wurde.
     dest
          Das Objekt, in das <ob> bewegt wurde.

BESCHREIBUNG
------------
::

     Diese Funktion wird im Behaelter aufgerufen, nachdem ein Objekt aus
     besagten Behaelter entfernt wurde.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Diese Funktion wird nur im Falle unbelebter Objekte gerufen. Fuer
     Lebewesen s. bitte. exit().

SIEHE AUCH
----------
::

    NotifyRemove(), NotifyInsert(), PreventInsert(), PreventLeave(), move(),
    exit(), init(), NotifyMove(), PreventMove()


Last modified: 21.05.2012, Zesstra


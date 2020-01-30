PreventInsert()
===============

FUNKTION
--------

     int PreventInsert(object ob);

DEFINIERT IN
------------

     /std/container/restrictions.c

ARGUMENTE
---------

     ob
          Das Objekt, das in den Behaelter eingefuegt werden soll.

BESCHREIBUNG
------------

     Mit dieser Funktion kann ein Behaelter pruefen, ob er das Objekt ob
     aufnehmen moechte oder nicht.

     Man beachte bitte, dass nach dieser Funktion *nicht* garantiert ist,
     dass das Objekt wirklich bewegt wird, es handelt sich lediglich um die
     Abfrage der Erlaubnis! Die Bewegung kann aus einer Vielzahl an Gruenden
     noch abgebrochen werden.

     Wenn ob mit dem Flag M_NOCHECK bewegt wird, wird PreventInsert() zwar
     aufgerufen, aber das Ergebnis ignoriert, d.h. die Aufnahme kann in dem
     Fall nicht verhindert werden.

RUeCKGABEWERT
-------------

     0
       wenn das Objekt aufgenommen werden kann;
     > 0
       ein Wert groesser als 0 zeigt an, dass das Objekt nicht aufgenommen
       werden soll.

BEISPIELE
---------

     Um zu verhindern, dass man Geld in einen Behaelter stecken kann, sollte
     man wie folgt vorgehen:

     .. code-block:: pike

       public int PreventInsert(object ob)
       {
         // Wenn es Geld ist, erheben wir sofort Einspruch
         if (ob->id("geld"))
           return 1;
         // Ansonsten koennte ein ererbtes Objekt noch Einspruch erheben!
         else
           return ::PreventInsert(ob);
       }

SIEHE AUCH
----------

     :doc:`PreventLeave`, :doc:`NotifyInsert`, :doc:`NotifyLeave`,
     :doc:`MayAddObject`, :doc:`MayAddWeight`, :doc:`PreventInsertLiving`,
     :doc:`PreventLeaveLiving`, :doc:`NotifyMove`, :doc:`PreventMove`,
     :doc:`move`, /std/container/restrictions.c

Last modified: 30.01.2020, Zesstra


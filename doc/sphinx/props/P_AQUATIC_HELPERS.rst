P_AQUATIC_HELPERS
=================

NAME
----
::

     P_AQUATIC_HELPERS "lib_p_aquatic_helpers"

DEFINIERT IN
------------
::

     <living/helpers.h>

BESCHREIBUNG
------------
::

     Diese Property kann in allen Lebewesen abgefragt werden, um die Objekte
     zu ermitteln, die fuer die Unterstuetzung beim Tauchen bei diesem 
     Lebewesen registriert haben. Die Daten werden als Mapping der folgenden
     Form zurueckgeliefert:
     ([ Objekt : Rueckgabewert von dessen Callback-Methode ])
     Eine Erlaeuterung dazu findet sich in der Dokumentation zu 
     RegisterHelperObject().

BEMERKUNGEN
-----------
::

     Diese Property kann nur abgefragt werden.
     Es ist erwuenscht, dass entsprechende, neu geschaffene Stellen jegliche 
     Helfer akzeptieren, deren Callback-Methode >0 zurueckgibt.

BEISPIEL
--------
::

     Um zu ermitteln, ob der Spieler mindestens ein Objekt bei sich hat, das 
     beim Tauchen hilft, sucht man alle Objekte aus dem Mapping heraus, die
     einen Wert >0 eingetragen haben und prueft deren Anzahl:

     mapping aquatic = this_player()->QueryProp(P_AQUATIC_HELPERS);
     object* helpers = filter( aquatic, function int (object h) {
                         return (aquatic[h]>0); });
     if ( sizeof(helpers) ) {
       tell_object(this_player(), "Du stuerzt Dich in die Fluten und "
         "stellst ueberrascht fest, dass Du mit Hilfe "+
         helpers[0]->name(WESSEN,1)+" sogar unter Wasser atmen kannst!\n");
     }
     else {
       tell_object(this_player(), "Du hast nichts zum Tauchen bei Dir.\n");
     }

SIEHE AUCH
----------
::

     Methoden:    RegisterHelperObject(L), UnregisterHelperObject(L)
     Properties:  P_HELPER_OBJECTS, P_AERIAL_HELPERS


06.04.2016, Arathorn


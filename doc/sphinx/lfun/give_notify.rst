give_notify()
=============

FUNKTION
--------
::

     void give_notify(object obj);

DEFINIERT IN
------------
::

     /std/npc/put_and_get.c

ARGUMENTE
---------
::

     obj
       an den NPC uebergebenes Objekt

RUeCKGABEWERT
-------------
::

     keiner

BESCHREIBUNG
------------
::

     Diese Funktion wird automatisch immer dann aufgerufen, wenn ein
     Lebewesen (welches kein Spielercharakter ist) ein Objekt uebergeben
     bekommt. Dies muss jedoch ueber die Funktionalitaet von
     put_and_get.c geschehen sein, innerhalb von move() wird die Funktion
     nicht aufgerufen!

BEISPIEL
--------
::

     Oftmals will man in Quests erreichen, dass einem NPC ein bestimmtes
     Item als Beweis der Erfuellung einer bestimmten Aufgabe ueberbracht
     wird. Folgendermasse kann dies realisiert werden:
     void create() {
       ::create();
       ...
       SetProp(P_REJECT,({REJECT_GIVE,
         Name(WER)+" sagt: Das brauche ich nicht!\n"}));
       ...
     }

     void quest_ok(object obj) { ...
       // Vernichtung des Questobjektes und Questtexte
       // Questbelohnung und Questanerkennung
     }

     void give_notify(object obj) {
       if(obj->id("\nquestitem")) // Questitem bekommen?
         quest_ok(obj);
       else
         ::give_notify(obj);  // P_REJECT soll sonst greifen
     }
     Der Aufruf von ::give_notify() stellt sicher, dass ein Objekt
     zurueckgegeben wird, sofern es nicht das gesuchte ist. Erreicht wird
     dies ueber P_REJECT (siehe Bemerkungen).

BEMERKUNGEN
-----------
::

     Speziell in NPCs ist diese Funktion normalerweise dafuer
     verantwortlich, dass mittels der Property P_REJECT die Annahme von
     Objekten verweigert werden kann. Ueberschreibt man sie, so sollte
     man gegebenenfalls darauf achten, dass diese Standardfunktion
     ebenfalls aufgerufen wird.

SIEHE AUCH
----------
::

     P_REJECT, show_notify(), 
     /std/npc/put_and_get.c, /std/living/put_and_get.c

22. Oktober 2013, Arathorn.


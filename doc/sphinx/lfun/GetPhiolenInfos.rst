GetPhiolenInfos()
=================

GetPhiolenInfos(L)
------------------
::

FUNKTION
--------
::

     mixed *GetPhiolenInfos();

BESCHREIBUNG
------------
::

     Wird von der Phiole aufgerufen:
     /d/inseln/miril/sternquest/obj/phiole.c
     Der Raum muss /p/service/miril/phiole.h includen.     
     Mit der Phiole kann ein Spieler durch Tueren schauen. Bei Tueren, die
     vom Doormaster (/obj/doormaster.c) verwaltet werden, klappt dies auto-
     matisch. Moechte man das verbieten, so muss die Funktion 
     GetPhiolenInfos() im entsprechenden Raum definiert sein.
     Befinden sich Tueren im Raum, die nicht vom Doormaster verwaltet werden,
     aber bei denen die Phiole funktionieren soll, so kann dies ebenfalls
     mittels GetPhiolenInfos() geschehen.

     

     Funktion der Phiole:
     Sie projiziert ein Bild dessen, was hinter der Tuer liegt auf die Tuer.
     Steht die Tuer offen, so sieht der Spieler nur eine Wand, denn es wird
     gezeigt, was hinter dieser Tuer ist.

     

     Ist die Tuer jedoch ge- oder verschlossen, so sieht er auf der Tuer
     die Langbeschreibung des Raumes, der hinter der Tuer liegt, sowie das
     Inventar dieses Raumes. 

     Aufbau der Funktion:
     mixed *GetPhiolenInfos() {
       return ({
                ([
                  PHIOLE_DEST:     string,       
                  PHIOLE_IDS:      *string,             
                  PHIOLE_GENDER:   int,           
                  PHIOLE_STATUS:   int,     (optional)
                  PHIOLE_LONG:     string,  (optional)
                  PHIOLE_VERBOTEN: int      (optional)
                ]),
                ([
                  ... Naechste Tuer
                ])
              });
     }

     

     PHIOLE_DEST      Dateiname des Zielraums
     PHIOLE_IDS	      Array der Strings, mit denen sich die Tuer 
                      ansprechen laesst
     PHIOLE_GENDER    Geschlecht der Tuer
     PHIOLE_STATUS    aktueller Status der Tuer:
                      0 geschlossen/verschlossen, 1 offen
                      Bei Tueren, die mit NewDoor erzeugt wurden
                      nicht setzen
     PHIOLE_LONG      Beschreibung des Zielraums (z.B. bei einer 
                      Faketuer)
     PHIOLE_VERBOTEN  0 erlaubt, 1 verboten

BEISPIEL
--------
::

     Ein Raum enthaelt eine Holztuer und ein Portal. Erstere ist mittels 
     NewDoor(...) gebaut, soll aber nicht zu durchschauen sein, letztere ist 
     selbstgestrickt. Die erste fuehrt in den Workroom von Jof, die zweite
     ist nur ein Fake und immer geschlossen. Bei vom Doormaster erzeugten 
     Tueren sollte PHIOLE_STATUS nicht gesetzt werden, da dies automatisch
     abgefragt wird.

     

     #include "/p/service/miril/phiole.h"
     ...
     mixed *GetPhiolenInfos(){
       return ({
                ([
                  PHIOLE_DEST:"/players/jof/workroom",
                  PHIOLE_IDS:({"holztuer"}),           
                  PHIOLE_GENDER:FEMALE,                    
                  PHIOLE_VERBOTEN:1          
                ]),
                ([
                  PHIOLE_DEST:0,
                  PHIOLE_IDS:({"portal"}),           
                  PHIOLE_GENDER:NEUTER, 
                  PHIOLE_STATUS:0,
                  PHIOLE_LONG:"Du stehst in einer riesigen Schatzkammer und "
                              "kannst Dein Glueck kaum fassen. Nun brauchst "
                              "Du nur noch zuzugreifen und bist der reichste "
                              "Bewohner des MorgenGrauen.",
                  PHIOLE_VERBOTEN:0
                ])
              });
      }
      Mittels PHIOLE_LONG laesst sich auch bei erlaubten und verbotenen Tueren 
      einstellen, was der Spieler statt der normalen Raumbeschreibung und den 
      im Raum befindlichen Objekten sehen soll. Das koennte z.B. sinnvoll 
      sein, falls der Spieler zwar die Raumbeschreibung, aber nicht den fiesen 
      Drachen sehen soll, der sich ebenfalls im Raum befindet.

      

SIEHE AUCH
----------
::

      NewDoor(), QueryDoorStatus(), SetDoorStatus(), QueryAllDoors()

Letzte Aenderung: Sam, 14. Jan 2006, 12:21, Miril


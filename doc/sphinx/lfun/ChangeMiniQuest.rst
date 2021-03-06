ChangeMiniQuest()
=================

FUNKTION
--------
::

    int ChangeMiniQuest(mixed questgeber, int parameter, mixed newvalue)

DEFINIERT IN
------------
::

    /secure/questmaster

BESCHREIBUNG
------------
::

    Diese Funktion aendert einen Parameter einer Miniquest im Questmaster,
    schreibt fuer diese Aktion einen Log-Eintrag und erstellt das Miniquest-
    Dumpfile neu.

ARGUMENTE
---------
::

    questgeber - Ladename des Objekts (string), das die Miniquest vergibt, 
                 oderdie Indexnummer (int) der Miniquest in der MQ-Liste
    parameter  - Angabe des zu aendernen Parameters (Position des Values
                 im Miniquests-Mapping):
                 0 : Miniquest-Stufenpunkte, mind. 1
                 2 : Aufgabenbeschreibung der Miniquest (string)
                 3 : Sichtbarkeit der Miniquest (0/1), default ist 1
                 4 : aktiv/inaktiv (1/0)
                 5 : Titel der Miniquest
                 6 : "geschafft"-Beschreibung nach Abschluss der MQ
                 7 : Voraussetzungen, Mapping im Format von P_RESTRICTIONS
                 8 : zugeordnete Region, String wie z.B."polar", "gebirge"
                 9 : erlaubte Abfrageobjekte, Array von Ladenamen, z.B.
                     ({"/d/region/magier/npc/infonpc"}), es koennen mehrere 
                     Objekte eingetragen sein
    newvalue   - neuer Wert fuer den angegebenen Parameter

RUECKGABEWERTE
--------------
::

     1: hat geklappt
     0: Zugriff verweigert
    -2: ungueltiger Datentyp eines der Argumente, bei Parameter 9 wird
        ein uebergebenes Array zusaetzlich auf Leerstrings und Elemente
        geprueft, die keine Strings sind. Wenn das Array ausschliesslich
        aus solchen Elementen besteht, wird ebenfalls -2 zurueckgegeben.

BEMERKUNGEN
-----------
::

    Das Flag "active" laesst sich bequemer ueber die Questmaster-Funktion
    SwitchMiniQuestActive() umschalten.
    Der Miniquest-Titel darf kein "in" oder "im" enthalten, weil dann die
    Eintraege in der Fraternitas-Bibliothek nicht gelesen werden
    koennen.

SIEHE AUCH
----------
::

   AddMiniQuest(L)
   P_RESTRICTIONS


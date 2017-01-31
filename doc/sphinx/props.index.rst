.. Morgengrauen-Mudlib - Ueber Properties

Properties
==========

Properties sind im MG Eigenschaften eines Objektes, welche von (in der Regel)
von aussen gesetzt werden koennen (im Gegensatz zu Variablen innerhalb von
Objekten).

BESCHREIBUNG:
-------------

Im Gegensatz zu Variablen innerhalb eines Objektes, kann man Properties
von aussen veraendern, ohne eine besondere Funktion geschrieben zu haben.

1. Das zugrundeliegende Prinzip
   Das grundlegende Konzept der MUDlib ist, dass wichtige, objektbezogene
   Informationen in den sogenannnten Properties gespeichert werden (engl.
   property -- Eigenschaft, Eigentum).
   Diese Informationen koennen einfache Werte, wie z.B. Zahlen, Zeichen oder
   Objekte, aber auch kompliziertere Strukturen sein.
   Jedes Objekt kann beliebig viele solcher Properties besitzen und deren
   Namensgebung ist nicht nur auf die von der MUDlib bereitgestellten
   Standardproperties begrenzt. Das heisst, das fuer eigene Anwendungen die
   Menge der Properties fuer ein Objekt beliebig erweitert werden kann.
   Damit sind auch schon die beiden Hauptmerkmale einer Property ange-
   sprochen:

   #. ein Name oder Kennung und
   #.ein Wert, der durch den Namen repraesentiert wird.

  Das reine Verwalten einer Property mit Namen und Wert ist aber nicht sehr
  sinnvoll und so gehoeren zu jeder Property noch zwei weitere wichtige
  Dinge. Zu jeder Property wurden jeweils zwei Operationen eingefuehrt,
  welche den uebergebenen Wert vor der Speicherung oder Abfrage bearbeiten.

  Zusammenfassend laesst sich das Konzept der Property in folgendem Schema
  darstellen::

               +-------------------------------------------+
               | Property                                  |
               +-------------------------------------------+
               | privater Datenbereich (Property Werte)    |
               +-------------------------------------------+
               | Direktzugriff auf den Datenbereich        |
               +-------------------------------------+     |
               |     ^       Methoden       v        | ^ v |
               |   Setzen       |        Abfragen    |     |
               +-------------------------------------+-----+
                     ^                      |
                     |                      V
                  SetProp()             QueryProp()


  Aus dem Schema laesst sich Folgendes erkennen

  * beim Setzen und Abfragen wird der Wert einer Methode uebergeben, die
    den Wert zurueckgibt oder ggf. die Aenderungen vornimmt
  * ein direkter Zugriff auf den Wert der ist ebenfalls moeglich, sollte
    aber nicht der Normalfall sein, da die Methoden Nebeneffekte erzeugen
  * in bestimmten Faellen kann man den aeusserlich aendernden Zugriff
    vollkommen unterbinden (NOSETMETHOD, PROTECT)
    (VORSICHT bei mappings/arrays, diese werden bei QueryProp()
    als Referenz zurueckgegeben, sind also so aenderbar)

2. Implementation
   Die Klasse /std/thing/properties.c stellt folgende Funktionen fuer die
   Behandlung von Properties bereit:

   * Normaler Zugriff

     * mixed SetProp(<name>, <wert>)
       setzt den Wert von <name> auf <wert>
     * mixed QueryProp(<name>)
       gibt den Wert von <name> zurueck

   * Direkter Zugriff

     * mixed Set(<name>, <wert>, <interpretation>)
       setzt fuer <name> einen <wert>:

        * den normalen Wert: <interpretation> := F_VALUE
        * eine Methode: <wert>:= closure, <interpretation> := F_SET_METHOD, F_QUERY_METHOD
        * ein Flag/Modus: <wert>:= SAVE, SECURED, PROTECTED, NOSETMETHOD, <interpretation>:= F_MODE, F_MODE_AS, F_MODE_AD
     * mixed Query(<name>, <interpretation>)
       fragt fuer <name> einen <wert> ab

       * F_SET_METHOD, F_QUERY_METHOD: die Closure/0
       * F_MODE: das (veroderte!) Flag

3. Besonderheiten/Eingebaute Properties
   Existiert zu einer Property eine Funktion mit dem selben Namen und einem
   ``_set_`` bzw ``_query_`` davor, so wird nicht auf die das Property-Mapping
   zugegriffen, sondern es werden die Argumente an diese Funktion uebergeben
   und der Rueckgabewert dieser Funktion zurueckgegeben.

   * Vorteile

      * so kann man Daten, die schnell verfuegbar sein muessen,	(bei denen
        also Effizienz gegen SetProp/QueryProp spricht) trotzdem nach aussen
        einheitlich zugreifbar machen

   * Nachteile

      * nicht wirklich sauber
      * Speichern muss man selbst vornehmen
      * Set/Query gehen wie auch bei Methoden an _set_*/_query_* vorbei
      * dieses Verhalten sollte der Mudlib vorbehalten bleiben, fuer eigene
        Prueffunktionen (wird etwas gesetzt/abgefragt) bzw. Aenderungen
        sollte man Methoden (F_SET_METHOD/F_QUERY_METHOD) benutzen

SIEHE AUCH:
-----------

* SetProp(L), QueryProp(L), Set(L), Query(L), SetProperties(L),
* QueryProperties(L)
* objekte, effizienz, closures


Verzeichnis der dokumentierten Properties:
------------------------------------------
.. toctree::
   :maxdepth: 1
   :glob:

   props/*


FAMILIEN
========

BESCHREIBUNG
------------

  Eine Familie umfasst den Erstie und alle Zweities, sprich alle diese haben
  den gleichen "Familiennamen". Dieser Name ist in der Regel die UUID des
  Ersties dieser Familie. Falls aber der Erstie sich aendern sollte (z.B.
  Magierwerdung eines Zweities) und sich der Familienname nicht aendern soll,
  dann koennen wir den Namen dieser Familie beibehalten (d.h. alter Erstie).

  Will man wissen, welcher Familie ein Char angehoert (egal, ob Erstie oder
  Zweitie), dann geht das mit::

    # xcall /secure/zweities->QueryFamilie(player_object)

  Des Weiteren liefert dieses Objekt auch noch zu jedem Zweitie den Erstie
  und zu jedem Erstie die Liste aller bekannten Zweities::
    
    # xcall /secure/zweities->QueryErstieName(player_object)
    # xcall /secure/zweities->QueryErstieUUID(player_object)
    # xcall /secure/zweities->QueryZweities(player_object)

  Der Datenbestand ist (noch) nicht vollstaendig, daher fehlen da noch viele
  Chars. Die werden aber in absehbarer Zeit dort nachgetragen.

  Die Familie wird in Zukunft genutzt, um Dinge zu personalisieren, welche
  fuer den Spieler, aber nicht fuer den Char individuell sein sollen. Sprich:
  personalisiert man irgendwas ueber die Familie, koennen alle Chars dieser
  Familie das irgendwas nutzen.

VERWALTUNG
----------

  Wenn sich der Erstie aendert, aber die Familie aller Zweities erhalten
  bleiben soll (z.B. weil sich der Erstie innerhalb der Familie wegen
  Magierwerdung aendert), muss dies in der Familienverwaltung hinterlegt
  werden, indem fuer den *neuen* Erstie die alte Familien-ID eingetragen wird.
  Dies koennen zur Zeit EM+.

  Beispiel: Der alte Erstie einer Familie ist bert_123456, der neue Erstie ist
  alice_654321. Die Familie soll aber weiterhin bert_123456 sein::
    
    # xcall /secure/zweities->SetFamilie("alice_654321, "bert_123456")
  
  Ein solcher expliziter Familieneintrag kann wieder geloescht werden::
    
    # xcall /secure/zweities->DeleteFamilie("alice_654321")

  EM+ koennen Zweities wieder aus der Datenbank loeschen (UUID angeben!)::
    
    # xcall /secure/zweities->DeleteZweitie("bert_12345")

SIEHE AUCH:
  :doc:`../props/P_SECOND`, :doc:`../props/P_SECOND_MARK`

LETZTE AeNDERUNG:
  8.1.2019, Zesstra


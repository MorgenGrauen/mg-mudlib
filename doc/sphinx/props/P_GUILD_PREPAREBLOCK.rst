P_GUILD_PREPAREBLOCK
====================

P_GUILD_PREPAREBLOCK (int)
--------------------------
::

NAME
----
::

  P_GUILD_PREPAREBLOCK                           "guild_prepareblock" 

DEFINIERT IN
------------
::

  /sys/new_skills.h

BESCHREIBUNG
------------
::

  Diese Property enthaelt die Information, ob das Lebewesen in
  der Konzentrationsphase eines Spells weiterkaempft oder ob
  die Angriffe derweil ausgesetzt werden.

BEMERKUNGEN
-----------
::

  In der Property selbst wird eigentlich ein Mapping eingetragen,
  welches die Gildennamen als Schluessel und das dazugehoerige
  Block-Flag als Eintrag enthaelt. Bei der Abfrage der Property wird
  jedoch die Gilde beruecksichtigt und das aktuelle Flag
  zurueckgeliefert.
  Dementsprechend das diese Prop nicht mit Set() manipuliert werden, 
  bitte ausschliessliche SetProp() verwenden.

BEISPIELE
---------
::

  Die Property sollte natuerlich nur von der Gilde gesetzt werden
  und auch nur bei Gildenmitgliedern

  if ( IsGuildMember(pl) )
    pl->SetProp(P_GUILD_PREPAREBLOCK,1);

  Resultat: Kein Ausfuehren von Attack(), wenn pl sich auf einen
  Zauberspruch konzentriert.

SIEHE AUCH
----------
::

  P_PREPARED_SPELL


18.03.2008, Zesstra


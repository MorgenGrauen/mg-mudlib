QueryWizardsForUID()
====================

FUNKTION
--------
::

    varargs string* QueryWizardsForUID(string uid, int recursive);

DEFINIERT IN
------------
::

    /secure/master/userinfo.c

ARGUMENTE
---------
::

    uid
      Die UID, fuer die man die Magier ermitteln will, die fuer sie
      zustaendig sind.
    recursive (optional)
      gibt an, ob das QueryWizardsForUID() (indirekt) aus einem 
      QueryWizardsForUID() heraus gerufen wurde. Sollte nicht manuell gesetzt
      werden.

BESCHREIBUNG
------------
::

    Die Funktion ermittelt die Magier, die fuer diese UID zustaendig sind.

RUeCKGABEWERT
-------------
::

    Zurueckgeliefert wird ein Array von Strings, jedes Element ist ein Magier.
    Sollte fuer eine UID kein Magier ermittelbar sein, ist das Array leer.
    Wenn z.B. fuer die UID der Magier "Atamur" gefunden wird, aber fuer alle 
    UIDs von "Atamur" auch der Magier "Rumata" zustaendig sein sollte, wird 
    "Rumata" ueber eine rekursive Suche gefunden.

BEMERKUNGEN
-----------
::

    Wenn die UID den Magier nicht implizit enthaelt (z.B. GUILD.gilde, im 
    Gegensatz zu d.region.magier), findet diese Funktion nur Magier, fuer die 
    seit Laden des Master bereits einmal get_userinfo() oder 
    QueryUIDsForWizard() im Master gerufen wurde, was z.B. Einloggen passiert.
    Magier, die lang nicht einloggten, werden also manchmal nicht gefunden,
    was in der Regel nicht schlimm sein sollte, da sie ja ohnehin den
    entsprechenden Code gerade nicht warten...

BEISPIELE
---------
::

    string *wiz = master()->QueryWizards("GUILD.klerus");
    // wiz enthaelt nun: ({"morgoth","magdalena"})

SIEHE AUCH
----------
::

    QueryUIDsForWizard(), 
		AddWizardForUID(), RemoveWizardFromUID()

16.12.2007, Zesstra


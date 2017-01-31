QueryUIDsForWizard()
====================

FUNKTION
--------
::

    varargs string* QueryUIDsForWizard(string wizname, int recursive);

DEFINIERT IN
------------
::

    /secure/master/userinfo.c

ARGUMENTE
---------
::

    wizname
      Der Magier, fuer den man die UIDs ermitteln will, fuer die er
      zustaendig ist.
    recursive (optional)
      Gibt an, ob QueryUIDsForWizard() (indirekt) rekursiv aufgerufen wurde.
      Sollte normalerweise nicht per Hand gesetzt werden.

BESCHREIBUNG
------------
::

    Die Funktion ermittelt die UIDs, fuer die dieser Magier zustaendig ist.
    Dabei wird impliziert beruecksichtigt, wenn der Magier RM einer Region
    oder Gildenmagier einer Gilde ist, ebenso wie Verzeichnisse in den
    Regionen oder in /p/service.
    Ausserdem wird nachgeschaut, fuer welche UIDs dieser Magier explizit
    eingetragen worden ist.
    Wenn z.B. Magier A auch fuer alle UIDs von Magier B zustaendig sein
    sollte, liefert die Funktion auch die UIDs von B zurueck.

RUeCKGABEWERT
-------------
::

    Zurueckgeliefert wird ein Array von Strings, jedes Element ist eine UID.
    Sollte fuer einen Namen keine UID ermittelbar sein, ist das Arrays leer.

BEISPIELE
---------
::

    string *uids = master()->QueryUIDsForWizard("ennox");
    // uids enthaelt nun:
    // ({"ennox","d.anfaenger.ennox","d.schattenwelt.ennox",
    //   "p.service.ennox","GUILD.chaos","p.chaos"})

SIEHE AUCH
----------
::

    QueryWizardsForUID(), 
		AddWizardForUID(), RemoveWizardFromUID()

16.12.2007, Zesstra


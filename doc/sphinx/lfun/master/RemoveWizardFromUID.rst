RemoveWizardFromUID()
=====================

FUNKTION
--------
::

    string* RemoveWizardFromUID(string uid, string wizard);

DEFINIERT IN
------------
::

    /secure/master/userinfo.c

ARGUMENTE
---------
::

    uid
      Die UID, fuer die man einen zustaendigen Magier austragen will.
    wizard
      Der Magier, den man austragen moechte.

BESCHREIBUNG
------------
::

    Die Funktion loescht die UID 'uid' aus der Liste der UIDs, fuer die
    'wizard' explizit zustaendig ist.

    Berechtigt zum Austragen von Magiern fuer bestimmte UIDs sind alle Magier,
    die (implizit oder explizit) verantwortlich fuer die jeweilige UID sind.
    Man kann sich auch selber austragen. ;-)

RUeCKGABEWERT
-------------
::

    Zurueckgeliefert wird ein Array von Strings, jedes Element ist eine UID,
    fuer die dier Magier jetzt explizit eingetragen ist.

BEISPIELE
---------
::

    master()->RemoveWizardFromUID("p.waterluh","zook");

		

    string *uids = master()->RemoveWizardFromUID("jof","zook");
    printf("Zook ist nun explizit zustaendig fuer: %s\n",CountUp(uids));

SIEHE AUCH
----------
::

    QueryWizardsForUID(), QueryUIDsForWizard()
		AddWizardForUID()

20.02.2007, Zesstra


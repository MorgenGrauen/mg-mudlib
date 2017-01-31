RemoveReadDetail()
==================

FUNKTION
--------
::

    void RemoveReadDetail(string|string* keys);

DEFINIERT IN
------------
::

    /std/thing/description.c

ARGUMENTE
---------
::

    keys
         String oder Array von Strings mit den zu entfernenden Details.

BESCHREIBUNG
------------
::

    Entfernt die in keys angegebenen Details aus der Liste der vorhandenen
    lesbaren Details.

BEMERKUNGEN
-----------
::

    Uebergibt man fuer keys eine 0, so werden saemtliche lesbaren Details
    entfernt!

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveDetail(), RemoveSmells(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

20.01.2015, Zesstra


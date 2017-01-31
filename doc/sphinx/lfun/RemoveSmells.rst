RemoveSmells()
==============

FUNKTION
--------
::

    void RemoveSmells(string|string* keys);

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

    Diese Funktion entspricht dem RemoveDetail() fuer Standarddetails,
    nur koennen hiermit Gerueche entfernt werden:
    Entfernt die in <keys> angegebenen Details aus der Liste der
    vorhandenen Details. Uebergibt man fuer <keys> eine 0, so werden
    saemtliche Details entfernt!

BEISPIEL
--------
::

    Zuerst erzeugen wir ein kleines Detail, mit dem wir am Rauch riechen
    koennen. Das Feuer brennt langsam ab und das Detail wird wieder
    entfernt:

      AddSmells("rauch",* H U S T *\n");
      call_out(#'RemoveSmells, 100, "rauch");

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveDetail(), RemoveReadDetail(), RemoveSmells(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

20.01.2015, Zesstra


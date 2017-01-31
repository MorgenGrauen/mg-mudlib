RemoveDetail()
==============

FUNKTION
--------
::

    void RemoveDetail(mixed *keys);

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

    Entfernt die in <keys> angegebenen Details aus der Liste der
    vorhandenen Details. Uebergibt man fuer <keys> eine 0, so werden
    saemtliche Details entfernt!

BEISPIEL
--------
::

    Ein kleines Beispiel, bei dem eine Oeffnung erscheint und wieder
    verschwindet, je nachdem, ob man eine Luke oeffnet oder schliesst.
      int oeffneLuke(string str);
      int schliesseLuke(string str);
      ...
      AddCmd("oeffne", #'oeffneLuke);
      AddCmd("schliesse", #'schliesseLuke);
      ...
      int oeffneLuke(string str) {
        if(str!="luke" || GetDetail("oeffnung"))
          return 0;
        AddDetail("oeffnung","Du siehst eine kleine Oeffnung.\n");
        return 1;
      }

      int schliesseLuke(string str) {
        if(str!="luke" || !GetDetail("oeffnung"))
          return 0;
        RemoveDetail("oeffnung"); // Detail wieder entfernen
        return 1;
      }

BEMERKUNGEN
-----------
::

    Da intern Details und SpecialDetails im gleichen Mapping verwaltet
    werden, lassen sich mit dieser Funktion auch SpecialDetails
    entfernen.
    Die Funktion RemoveSpecialDetail() sollte also nicht genutzt werden!

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveReadDetail(), RemoveSmells(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS,
               P_TOUCH_DETAILS, P_SPECIAL_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

8. Juli 2011 Gloinson


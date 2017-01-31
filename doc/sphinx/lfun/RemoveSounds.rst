RemoveSounds()
==============

FUNKTION
--------
::

    void RemoveSounds(string|string* keys);

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
    nur koennen hiermit Gerauesche entfernt werden:
    Entfernt die in <keys> angegebenen Details aus der Liste der
    vorhandenen Details. Uebergibt man fuer <keys> eine 0, so werden
    saemtliche Details entfernt!

BEISPIEL
--------
::

    Wir lassen etwas Musik spielen und wenn wir den Plattenspieler
    ausmachen, dann endet sie und man hoert auch nichts mehr:

      int ausmachen(string str);
      ...
      AddSounds(({"musik","hip-hop"}) ,"Klingt nach Hip-Hop...\n");
      AddCmd("mach|mache&plattenspieler&aus", #'ausmachen,
             "Was willst du (aus)machen?|Willst du den ausmachen?^");
      ...
      int ausmachen(string str) {
        if(!GetDetail("musik", 0, SENSE_SOUND)) // existiert Musikdetail ?
          return("Der Plattenspieler laeuft doch gar nicht!\n", 1);
        RemoveSounds(0);
         return 1;
      }

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveDetail(), RemoveReadDetail(), RemoveSmells(),
               RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

8. Juli 2011 Gloinson


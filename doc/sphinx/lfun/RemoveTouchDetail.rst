RemoveTouchDetail()
===================

FUNKTION
--------
::

    void RemoveTouchDetail(string|string* keys);

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
    nur koennen hiermit (ab)tastbare Details entfernt werden:
    Entfernt die in <keys> angegebenen Details aus der Liste der
    vorhandenen Details. Uebergibt man fuer <keys> eine 0, so werden
    saemtliche tastbaren/fuehlbaren Details entfernt!

BEISPIEL
--------
::

    Zuerst wird ein Detail "boden" erzeugt, das abgetastet werden kann.
    Dieses kann durch Betaetigen eines Knopfes, aeh, entfernt werden.

      int knopf();
      void knopf2();

      AddTouchDetail("boden", "Er ist aus Stein.\n");
      AddCmd("drueck|druecke&knopf", #'knopf, "Was willst du druecken?");

      void knopf() {
        tell_room(this_object(), break_string(
          this_player()->Name(WER)+" drueckt einen Knopf, der dabei satt "+
          "klackt.", 78));

        if(find_call_out(#'knopf2)<=0)
          call_out(#'knopf2, 1);
      }

      

      void knopf2() {
        tell_room(this_object(), "Uhoh. Der Boden klappt weg. Du faellst.");
        RemoveTouchDetails("boden");
      }

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveDetail(), RemoveReadDetail(), RemoveSmells(),
               RemoveSounds()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

20.01.2015, Zesstra


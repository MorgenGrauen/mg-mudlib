GetDetail()
===========

FUNKTION
--------
::

    varargs string GetDetail(string key, string race, int sense)

DEFINIERT IN
------------
::

    /std/thing/description.c

ARGUMENTE
---------
::

    key
      Das zu ermittelnde Detail.
    race
      Rasse des ermittelnden Objektes (falls es ein Lebewesen ist).
    sense
      Die Art des zu untersuchenden Details:
        Untersuchen, Riechen, Hoeren, Tasten.

BESCHREIBUNG
------------
::

    Die Beschreibung des gewuenschten Details wird ermittelt. Dabei
    werden rassenspezifische Details beruecksichtigt. Es gibt hierbei
    verschiedene Detailarten, deren Typ man in <sense> angibt:
      SENSE_VIEW    - Fuer Defaultdetails zum Untersuchen.
      SENSE_SMELL   - Fuer Details, die man riechen kann.
      SENSE_SOUND   - Fuer Details, die man hoeren kann.
      SENSE_TOUCH   - Fuer Details, die man abtasten kann.
      SENSE_READ    - Fuer Details, die man lesen kann.

    Dabei ist 0 == SENSE_VIEW.

RUeCKGABEWERT
-------------
::

    Die Beschreibung des Details oder 0, wenn es dieses Detail nicht
    gibt.

BEISPIEL
--------
::

    Im folgenden wird ein kleines Testdetail generiert:
      AddDetail("test","Das ist ein Test!\n");
    Im folgenden wird das Detail entfernt, wenn es existiert. Dies ist
    eigentlich nicht noetig, da RemoveDetail() damit zurechtkommt, aber
    eventuell sind ja noch weitere Aktionen noetig.
      if(GetDetail("test"))
      { RemoveDetail("test");
        ...
      }
    Ein Geruch kann man folgendermassen erzeugen:
      AddSmells("gold",
        ([0      :"Gold kann man nicht riechen!\n",
          "zwerg":"Deine trainierte Nase riecht es muehelos!\n"]));
    Die Abfrage des Details gestaltet sich recht einfach:
      GetDetail("gold","zwerg",SENSE_SMELL);
    Die Funktion liefert das Detail fuer den Zwerg.
      GetDetail("gold",0,SENSE_SMELL);
    Die Funktion liefert das Detail fuer die restlichen Rassen.
      GetDetail("gold",0,SENSE_SOUND);
    Ein Sounddetail mit dem Namen "gold" existiert nicht, die Funktion
    liefert 0 zurueck.

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveReadDetail(), RemoveSmells(), RemoveDetail(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS,
               P_TOUCH_DETAILS, P_SPECIAL_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: break_string()

27. Jan 2013 Gloinson


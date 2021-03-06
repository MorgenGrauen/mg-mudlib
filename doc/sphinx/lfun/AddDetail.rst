AddDetail()
===========

FUNKTION
--------
::

    void AddDetail(string|string* keys,
                   string|string*|mapping|closure desc);

DEFINIERT IN
------------
::

    /std/thing/description.c

ARGUMENTE
---------
::

    keys
      String oder Array von Strings mit den Namen der Details.
    desc
      String, Mapping, String-Array oder Closure mit/zur Beschreibung.

BESCHREIBUNG
------------
::

    Die Beschreibung der Details <keys> wird gesetzt. Wie die Details
    bei der Untersuchung aussehen, haengt im wesentlichen vom Typ der
    Beschreibung <desc> ab:
      <desc> ist ein String.
        Beim Untersuchen wird dieser String zurueckgegeben.
      <desc> ist ein String-Array.
        Beim Untersuchen wird zufaellig einer der Strings zurueckgegeben.
      <desc> ist ein Mapping.
        Das Mapping muss folgenden Aufbau haben:
          ([0:        "Defaulttext",
            "rasse1": "r1text", ...]).
        
        Die Rassenstrings muessen hierbei klein geschrieben werden, anders
        als in P_RACE und P_REAL_RACE.

        Falls fuer die Rasse des Spielers, der das Detail untersucht, ein
        Eintrag im Mapping existiert, wird der entsprechende Text
        zurueckgegeben, ansonsten der Defaulttext. Auf diese Weise sind
        rassenabhaengige Details moeglich.
      <desc> ist eine Closure.
        In diesem Fall wird die Closure ausgefuehrt und das Ergebnis
        zurueckgegeben. Die Closure bekommt dabei den Namen des Details
        als Parameter uebergeben.

    Fuer Details koennen Forscherpunkte eingetragen werden.

BEISPIELE
---------
::

    Ein schlichtes Detail:

      AddDetail(({"sofa","couch"}), "Eine kleine Couch.\n");

    Laengere Details sollten hierbei nicht per Hand umgebrochen werden,
    sondern man kann hierzu die Funktion break_string() nutzen:

      AddDetail("detail", break_string(
        "Du wolltest es ja nicht anders, jetzt musst Du Dir dieses "
        "fuerchterlich lange Detail durchlesen!!!", 78));

    Noetige Zeilenumbrueche bei Zeilenlaengen groesser 77 werden so
    automatisch generiert.
    Ein rassenabhaengiges Detail:

      AddDetail(({"bett","bettchen"}),
        ([0      :"Ein kleines Bett.\n", // Der Defaulttext
          "zwerg":                       // Die Rasse klein schreiben
                "Das Bett laedt geradezu zu einem Nickerchen ein.\n"]));

    Und nun ein Detail mit Closure (diese Version ersetzt das Verhalten
     von AddSpecialDetail).

      int hebel_betaetigt;
      ...
      string hebel(string str); // Funktion bekannt machen (Prototyping)
      ...
      AddDetail(({"hebel","schalter"}), #'hebel);
      ...
      string hebel(string key) {
        if(hebel_betaetigt)
          return "Der "+capitalize(key)+" steht auf EIN.\n";
        else
          return "Der "+capitalize(key)+" steht auf AUS.\n";
      }

    Man erhaelt verschiedene Ergebnisse beim Untersuchen, je nachdem
    ob das Flag hebel_betaetigt gesetzt ist oder nicht.

SIEHE AUCH
----------
::

    Setzen:    AddReadDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveDetail(), RemoveReadDetail(), RemoveSmells(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS,
               P_TOUCH_DETAILS, P_SPECIAL_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

11.04.2020, Arathorn


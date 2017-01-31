AddTouchDetail()
================

FUNKTION
--------
::

    void AddTouchDetail(string|string* keys,
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

    Diese Funktion entspricht dem AddDetail() fuer Standarddetails, nur
    koennen hiermit (ab)tastbare bzw. fuehlbare Details realisiert werden:
    Die Beschreibung der Details <keys> wird gesetzt. Wie die Details
    beim (Ab)Tasten aussehen, haengt im wesentlichen vom Typ der
    Beschreibung <desc> ab:
     <desc> ist ein String.
       Beim Untersuchen wird dieser String zurueckgegeben.
      <desc> ist ein String-Array.
        Beim Untersuchen wird zufaellig einer der Strings zurueckgegeben.
     <desc> ist ein Mapping.
       Das Mapping muss folgenden Aufbau haben:
         ([0:        "Defaulttext",
           "rasse1": "r1text", ...]).

       Falls fuer die Rasse des das Detail untersuchenden Spielers ein
       Eintrag im Mapping existiert, wird der entsprechende Text
       zurueckgegeben, ansonsten der Defaulttext. Auf diese Weise sind
       rassenabhaengige Details moeglich. Siehe auch die Beispiele.
     <desc> ist eine Closure.
       In diesem Fall wird die Closure ausgefuehrt und das Ergebnis
       zurueckgegeben. Die Closure bekommt dabei den Namen des Details
       als Parameter uebergeben.

    Fuer fuehlbare Details koennen Forscherpunkte eingetragen werden.

    Fuehlbare Details koennen allgemein einen Raum oder Objekt zugeordnet
    werden: dafuer muss man als <key> SENSE_DEFAULT uebergeben.

    Spielerkommandos: "taste"

BEISPIELE
---------
::

    Ein kleines Beispiel fuer rassenabhaengige, fuehlbare Details mit Closures:
      string strafe(string key);
      ...
      AddTouchDetail(SENSE_DEFAULT, "Du fuehlst einige Knollen\n");
      AddTouchDetail(({"knollen"}),
                     ([0:       "Sie fuehlen sich an wie Knoblauchknollen. "
                                "Riech doch mal daran.\n",
                       "vampir": #'strafe]));

      string strafe(string key) {
        this_player()->reduce_hit_points(100);
        return "Au! Das waren ja Knoblauchknollen!\n";
      }

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddReadDetail(), AddSmells(), AddSounds()
    Loeschen:  RemoveDetail(), RemoveReadDetail(), RemoveSmells(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

20.01.2015, Zesstra


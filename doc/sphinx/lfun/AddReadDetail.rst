AddReadDetail()
===============

FUNKTION
--------
::

    void AddReadDetail(string|string*keys,
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
    beim Lesen ausgegeben werden, haengt im wesentlichen vom Typ der
    Beschreibung <desc> ab:
      <desc> ist ein String.
       Beim Lesen wird dieser String zurueckgegeben.
      <desc> ist ein String-Array.
       Beim Lesen wird zufaellig einer der Strings zurueckgegeben.
      <desc> ist ein Mapping.
        Das Mapping muss folgenden Aufbau haben:
          ([0:        "Defaulttext",
            "rasse1": "r1text", ...]).

        Falls fuer die Rasse des das Detail untersuchenden Spielers ein
        Eintrag im Mapping existiert, wird der entsprechende Text
        zurueckgegeben, ansonsten der Defaulttext. Auf diese Weise sind
        rassenabhaengige Texte moeglich.
      <desc> ist eine Closure.
        In diesem Fall wird die Closure ausgefuehrt und das Ergebnis
        zurueckgegeben. Die Closure bekommt dabei den Namen des Details
        als Parameter uebergeben.

    Fuer lesbare Details koennen Forscherpunkte eingetragen werden.

    Will man ein lesbares Detail an einem Objekt haben, welches der Spieler
    mit "lies <id>" (<id> ist eine ID des Objekts) bekommt, muss man ein
    Detail SENSE_DEFAULT hinzufuegen.
    (Ein Detail "<id>" hinzuzufuegen, hat einen ganz anderes Effekt! Dieses
     wuerde vom Spieler mit "lies <id> an <id>" gelesen werden und ist
     meistens nicht das, was gewuenscht wird.)

BEMERKUNGEN
-----------
::

    (1) Auf die 'desc' wird kein process_string() mehr angewendet.
        Bitte stattdessen lfun closures bzw. 'inline closures'
        verwenden.

    (2) Im Gegensatz zum Verhalten von AddTouchDetail(), AddSmells() und
        AddSounds() wirkt ein SENSE_DEFAULT-Detail in einem Raum nicht.
        Ein einfaches "lies" bleibt dann ohne Rueckgabewert.

BEISPIELE
---------
::

    AddReadDetail( ({ "schild" }),
      "BETRETEN STRENGSTENS VERBOTEN!\n" );

    AddReadDetail("inschrift",
                  ([0: "Dort steht: Ein Ring sie zu binden. ....\n",
                    "elf": "Alles in dir straeubt sich, DAS DA zu lesen.\n"]));

    AddReadDetail("regeln",
      function string() {
        this_player()->More("/etc/WIZRULES", 1);
        return "";
      });

SIEHE AUCH
----------
::

    Setzen:    AddDetail(), AddSmells(), AddSounds(),
               AddTouchDetail()
    Loeschen:  RemoveDetail(), RemoveReadDetail(), RemoveSmells(),
               RemoveSounds(), RemoveTouchDetail()
    Daten:     P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS
    Veraltet:  AddSpecialDetail(), RemoveSpecialDetail(), P_READ_MSG
    Sonstiges: GetDetail(), break_string()

20.01.2015, Zesstra


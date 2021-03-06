SetSpellFatigue()
=================

FUNKTION
--------
::

    public varargs int SetSpellFatigue(int duration, string key)

DEFINIERT IN
------------
::

    /std/living/skills.c
    /std/player/skills.c
    /sys/living/skills.h

ARGUMENTE
---------
::

    int duration: Wie lang soll die Spruchermuedung andauern?
    string key  : Eindeutiger Name des Spruches, einer Gruppe von Spruechen
                  oder 0 fuer die globale Spruchermuedung.

BESCHREIBUNG
------------
::

    Diese Funktion dient zum Verwalten von individuellen Spruchermuedungen
    (Spellfatigue, Spruchsperren).
    Hiermit lassen sich unabhaengige Ermuedungen/Sperren fuer einzelne
    Sprueche oder Gruppen von Spruechen gestalten.

    <duration> ist die Zeit (in Sekunden), welche die Spruchermuedung
    anhalten soll (nicht die Endzeit).

    Wird <key> nicht angegeben oder ist 0, wird die globale Spruchsperre
    gesetzt (identisch zu der Property P_NEXT_SPELL_TIME), anderenfalls die
    unter <key> gespeicherte Spruchermuedung.
    Setzt man einen Eintrag ohne Angabe von <key> bedeutet dies damit auch,
    dass der Wert von P_NEXT_SPELL_TIME geaendert wird.

RUeCKGABEWERT
-------------
::

    -1    Der Eintrag <key> ist noch nicht abgelaufen, es wurde _keine_
          neue Spruchermuedung/-Sperre gespeichert.

    >0    Eintrag wurde gespeichert, Rueckgabewert ist die Zeit, zu der die
          Sperre ablaeuft.

BEISPIELE
---------
::

    Ein Spell gehoert zu einer Gruppe von Spells mit dem Namen 'extrasuess'.
    Er darf nur ausgefuehrt werden, wenn seit 5s kein anderer Spruch aus der
    Gruppe ausgefuehrt wurde.
    if (CalculateSpellSuccess(...) > x) {
      // Spellfatigue setzen (und Erfolg pruefen!)
      if (ob->SetSpellFatigue(5, "extrasuess") > -1) {
        tell_object(ob, "Du fuetterst " + enemy->Name(WEN) + " mit einem "
          "Stueck suesser Schokotorte.\n");
        ...
      }
      else {
        // Sauerei! Zu ermuedet fuer diesen Spruch. Fehlermdeldung ...
      }
    }
    Dieses setzt natuerlich voraus, dass alle anderen Sprueche der Gruppe
    "extrasuess" den gleichen <key> pruefen und setzen.
    (Will man vor CalculateSpellSuccess() wissen, ob der Spruch ueberhaupt
     gewirkt werden duerfte, sollte man hierzu dann CheckSpellFatigue()
     verwenden.)

BEMERKUNGEN
-----------
::

    Die genauen Zeitdauern koennen von Spielern beeinflusst werden, sie
    unterliegen der jeweiligen Einstellung von 'spruchermuedung', d.h. koennen
    auf volle 2s aufgerundet werden. (Dies ist nicht der Fall bei NPC.)
    Auch wenn diese Funktion zum Verwalten von beliebigen Zeitsperren genutzt
    werden koennen, beschraenkt euch bitte auf Spruchermuedungen und benutzt
    ansonsten check_and_update_timed_key(). Falls ihr diesbzgl. weitere/andere
    Wuensche habt, sprecht den/die Mudlib-EM an.

SIEHE AUCH
----------
::

    CheckSpellFatigue(L), DeleteSpellFatigue(L)
    P_NEXT_SPELL_TIME
    spruchermuedung


27.03.2010, Zesstra


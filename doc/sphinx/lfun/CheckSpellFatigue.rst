CheckSpellFatigue()
===================

FUNKTION
--------
::

    public varargs int CheckSpellFatigue(string key)

DEFINIERT IN
------------
::

    /std/living/skills.c
    /std/player/skills.c
    /sys/living/skills.h

ARGUMENTE
---------
::

    string key  : Eindeutiger Name des Spruches, einer Gruppe von Spruechen
                  oder 0 fuer die globale Spruchermuedung.

BESCHREIBUNG
------------
::

    Diese Funktion dient zum Pruefen von individuellen Spruchermuedungen
    (Spellfatigue, Spruchsperren).
    Hiermit lassen sich unabhaengige Ermuedungen/Sperren fuer einzelne
    Sprueche oder Gruppen von Spruechen gestalten.

    Wird <key> nicht angegeben oder ist 0, wird die globale Spruchsperre
    geprueft (identisch zu der Property P_NEXT_SPELL_TIME), anderenfalls 
    die unter <key> gespeicherte Spruchermuedung.
    Prueft man einen Eintrag ohne Angabe von <key> ist das Ergebnis dieser
    Funktion identisch zur Abfrage von P_NEXT_SPELL_TIME.

RUeCKGABEWERT
-------------
::

    0    Spruchermuedung existiert nicht oder ist abgelaufen.

    >0   Spruchermuedung ist noch nicht abgelaufen, Rueckgabewert ist die
         Zeit, bei der dieser Eintrag ablaeuft. Der Spruch darf _nicht_
         ausgefuehrt werden.

BEISPIELE
---------
::

    Ein Spell gehoert zu einer Gruppe von Spells mit dem Namen 'extrasuess'.
    Er darf nur ausgefuehrt werden, wenn seit 5s kein anderer Spruch aus der
    Gruppe ausgefuehrt wurde.
    if (ob->CheckSpellFatigue("extrasuess") {
      // alte Sperre noch nicht abgelaufen.
      tell_object(ob, "Deine unendliche Schokotorte ist noch nicht wieder "
        "nachgewachsen.\n");
      return ... ;
    }

BEMERKUNGEN
-----------
::

    Die genauen Zeitdauern koennen von Spielern beeinflusst werden, sie
    unterliegen der jeweiligen Einstellung von 'spruchermuedung', d.h. koennen
    auf volle 2s aufgerundet werden.
    Auch wenn diese Funktion zum Verwalten von beliebigen Zeitsperren genutzt
    werden koennen, beschraenkt euch bitte auf Spruchermuedungen und benutzt
    ansonsten check_and_update_timed_key(). Falls ihr diesbzgl. weitere/andere
    Wuensche habt, sprecht den/die Mudlib-EM an.

SIEHE AUCH
----------
::

    SetSpellFatigue(L), DeleteSpellFatigue(L)
    P_NEXT_SPELL_TIME
    spruchermuedung


27.03.2010, Zesstra


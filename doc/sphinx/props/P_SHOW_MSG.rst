P_SHOW_MSG
==========

NAME
----
::

    P_SHOW_MSG                          "show_message"

DEFINIERT IN
------------
::

    /sys/living/put_and_get.h

BESCHREIBUNG
------------
::

    Mit P_SHOW_MSG kann man die Meldungen, die beim Vorzeigen eines Objektes
    ausgegeben werden, modifizieren.

    Folgende Werte sind moeglich:

    o 0
      Es wird eine Standardmeldung ausgegeben. Dies ist Voreinstellung.

    o NO_PNG_MSG        == -1
      Es wird keinerlei Meldung ausgegeben

    o Ein Array aus Strings
      Der erste String wird an den Spieler ausgegeben, der zweite
      (optionale) an den Raum, der dritte (ebenfalls optionale) an den
      Empfaenger.

      Die Strings werden durch die Funktion replace_personal() geparst.
        Objekt1 - Spieler
        Objekt2 - das Objekt, das uebergeben wird
        Objekt3 - Empfaenger

      Wird der zweite String nicht angegeben, erfolgt keine Meldung an den
      Raum. Beim Fehlen des dritten gibt es keine Meldung an den Empfaenger.

BEISPIEL
--------
::

    SetProp(P_SHOW_MSG, ({
      "Du haeltst @WEM3 @WEN2 unter die Nase.",
      "@WER1 haelt @WEM3 @WENU2 unter die Nase.",
      "@WER1 haelt Dir @WENU2 unter die Nase."
    }));

    Das fuehrt bei Ugars "zeig peter medaille" zu folgenden Meldungen:

    Ugar: "Du haeltst Peter die Medaille unter die Nase."
    Raum: "Ugar haelt Peter eine Medaille unter die Nase."
    Peter: "Ugar haelt Dir eine Medaille unter die Nase."

SIEHE AUCH
----------
::

     Aehnliches: P_DROP_MSG, P_PUT_MSG, P_PICK_MSG, P_GIVE_MSG
     Sonstiges:  replace_personal(E), show(L), show_objects(L),
                 show_notify(L), /std/living/put_and_get.c

3. Juni 2008 Amynthor


P_GIVE_MSG
==========

NAME
----
::

     P_GIVE_MSG				"give_message"

DEFINIERT IN
------------
::

     /sys/living/put_and_get.h

BESCHREIBUNG
------------
::

     Mit P_GIVE_MSG kann man die Meldung, die man beim Uebergeben eines
     Objektes bekommt, modifizieren.

     Folgende Werte sind moeglich:

     o 0
       Es wird eine Standardmeldung ausgegeben. Dies ist Voreinstellung.

     o NO_PNG_MSG	== -1
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

     void create() {
      ...
      SetProp( P_SHORT, "Etwas Sand" );
      SetProp( P_LONG, break_string(
	"Ein wenig magischer Sand. Sehr feinkruemelig.",78 ));

      SetProp( P_NAME, "Sand" );
      AddId( ({"sand"}) );
      SetProp( P_GENDER, MALE );

      SetProp( P_GIVE_MSG, ({
       "Du laesst @WEN2 in @WESSEN3 Haende rieseln.",
       "@WER1 laesst @WENU2 in @WESSEN3 Haende rieseln.",
       "@WER1 laesst @WENU2 in deine Haende rieseln."}));
       ...
      }

     Das ganze fuehrt bei Ugars "gib sand an peter" zu folgenden
     Meldungen:

     Ugar: "Du laesst den Sand in Peters Haende rieseln."
     Raum: "Ugar laesst Sand in Peters Haende rieseln."
     Peter: "Ugar laesst Sand in deine Haende rieseln."

SIEHE AUCH
----------
::

     Aehnliches: P_DROP_MSG, P_PUT_MSG, P_PICK_MSG, P_SHOW_MSG
     Fehler:     P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_TOO_MANY_MSG,
                 P_NOINSERT_MSG, P_NOLEAVE_MSG, P_NODROP, P_NOGET 
     Sonstiges:  replace_personal(E), give(L), give_objects(L),
		 give_notify(L), /std/living/put_and_get.c

14. Maerz 2004 Gloinson


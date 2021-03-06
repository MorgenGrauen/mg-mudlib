P_PICK_MSG
==========

NAME
----
::

     P_PICK_MSG				"pick_message"

DEFINIERT IN
------------
::

     /sys/living/put_and_get.h

BESCHREIBUNG
------------
::

     Mit P_PICK_MSG kann man die Meldung, die man beim Aufnehmen eines
     Objektes bekommt, modifizieren.

     Folgende Werte sind moeglich:

     o 0
       Es wird eine Standardmeldung ausgegeben. Dies ist Voreinstellung.

     o NO_PNG_MSG       == -1
       Es wird keinerlei Meldung ausgegeben

     o Ein Array aus Strings
       Der erste String wird an den Spieler ausgegeben, der zweite
       (optionale) an den Raum.

       Die Strings werden durch die Funktion replace_personal() geparst.
	Objekt1 - Spieler
        Objekt2 - das Objekt, das genommen wird

       Wird der zweite String nicht angegeben, erfolgt keine Meldung an den
       Raum.

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

      SetProp( P_PICK_MSG, ({
       "Du schaufelst @WEN2 in deine Hand.",
       "@WER1 schaufelt @WEN2 in eine Hand."}));
      ...
     }

     Das ganze fuehrt bei Ugars "nimm sand" zu folgenden
     Meldungen:

     Ugar: "Du schaufelst den Sand in deine Hand."
     Raum: "Ugar schaufelt den Sand in eine Hand."

SIEHE AUCH
----------
::

     Aehnliches: P_DROP_MSG, P_PUT_MSG, P_GIVE_MSG, P_WEAR_MSG, P_WIELD_MSG
     Fehler:     P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG, P_TOO_MANY_MSG,
                 P_NOINSERT_MSG, P_NOLEAVE_MSG, P_NODROP, P_NOGET 
     Sonstiges:  replace_personal(E), pick_obj(L), /std/living/put_and_get.c

14. Maerz 2004 Gloinson


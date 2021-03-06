environment
===========

BEMERKUNGEN
-----------

  Blueprints, wie zum Beispiel Raeume haben oft kein environment(). Man sollte
  daher ueberpruefen, ob ein environment() existiert, wenn man darin oder
  darauf eine Funktion aufrufen will.

BEISPIELE
---------

   .. code-block:: pike

     // In der Variable "raum" steht der Raum, in dem sich der Spieler
     // derzeit aufhaelt - das kann auch 0 sein!

     raum = environment(this_player());

     // Dieses Objekt hat noch kein environment, da es eben erst geclont
     // wurde. Ergo steht in env eine 0.
     obj = clone_object("/std/thing");
     env = environment(obj);

     // alle Methoden die auf Environment arbeiten, sollten das vorher
     // pruefen - insbesondere tell_room()
     if(this_player() && environment(this_player()) &&
        objectp(o=present("schild",environment(this_player()))) {

      write("Du klebst Deine Plakette auf "+o->name(WEN)+".\n");
      tell_room(environment(this_player()), break_string(
            this_player()->Name(WER)+" pappt einen Aufkleber auf "
            +o->name(WEN)+".",78), ({this_player()}));
     }

     // wenn Dinge sehr offensichtlich in Leuten kaputtgehen wird es
     // komplexer (man kann das natuerlich noch weiter schachteln oder
     // ueber all_environment() versuchen zu loesen
     if(environment()) {
      object ee;
      ee=environment(environment());
      if(living(environment()))
      {
        tell_object(environment(),Name(WER)+" zerfaellt.\n");
        if(ee)
          tell_room(ee, environment()->Name(WESSEN)
               +" "+name(RAW)+" zerfaellt.\n", ({environment()}));
      }
      else if(ee && living(ee))
      {
        if(environment()->QueryProp(P_TRANSPARENT))
          tell_object(ee, Name(WER)+" zerfaellt in Deine"
              + (environment()->QueryProp(P_PLURAL) ? "n" : 
                  (environment()->QueryProp(P_GENDER)==FEMALE?"r":"m"))
              + environment()->name(RAW)+".\n");
      }
      else
        tell_room(environment(),Name(WER)+" zerfaellt.\n");
     }


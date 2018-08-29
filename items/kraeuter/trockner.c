#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_shadow

inherit "/std/container";

#include <properties.h>
#include <defines.h>
#include <items/kraeuter/kraeuter.h>

#define BS(x) break_string(x, 78, 0, BS_LEAVE_MY_LFS)

// Enthaelt die raumabhaengig variable Kurzbeschreibung, die der Trockner 
// annimmt, wenn gerade ein Kraut getrocknet wird.
private string short_desc;

// Globale Variable fuer die Qualitaet, damit man den Wert nicht im 
// call_out() mitgeben muss, und sie somit auch nicht mit call_out_info()
// abfragbar ist.
private int drying_quality;

private void dry_plant(object kraut, string *msgs);
private void destroy_herb(object kraut);
private string my_short();
private int|string my_noget();
private string my_long();
private string* my_ids();
private string|string* my_name();
private mixed my_mat();

protected void create() {
  if ( !clonep(ME) ) {
    set_next_reset(-1);
    return;
  }
  ::create();

  Set(P_SHORT, #'my_short, F_QUERY_METHOD);
  Set(P_LONG, #'my_long, F_QUERY_METHOD);
  Set(P_NOGET, #'my_noget, F_QUERY_METHOD);
  Set(P_IDS, #'my_ids, F_QUERY_METHOD);
  Set(P_NAME, #'my_name, F_QUERY_METHOD);
  Set(P_MATERIAL, #'my_mat, F_QUERY_METHOD);
  // Properties zu securen ist vielleicht etwas sehr paranoid.
  Set(P_SHORT, SECURED|NOSETMETHOD, F_MODE_AS);
  Set(P_LONG, SECURED|NOSETMETHOD, F_MODE_AS);
  Set(P_NOGET, SECURED|NOSETMETHOD, F_MODE_AS);
  Set(P_IDS, SECURED|NOSETMETHOD, F_MODE_AS);
  Set(P_NAME, SECURED|NOSETMETHOD, F_MODE_AS);
  SetProp(P_MAX_OBJECTS,1);
  SetProp(P_MAX_WEIGHT,100000);
  SetProp(P_TRANSPARENT,0);

  AddCmd("trockne&@PRESENT", "cmd_trocknen", "Was willst Du trocknen?");
}

private string my_short() {
  if ( first_inventory(ME) )
    return short_desc;
  return 0;
}

// Querymethoden, die Langbeschreibung und weitere Properties des im 
// Trockner enthaltenen Krautobjektes nach draussen weiterreichen, um die
// Illusion zu erzeugen, dass wirklich das echte Kraut im Raum zu sehen sei.
// Alle Funktionen gehen davon aus, dass das zu trocknende Kraut das erste
// (und einzige) Objekt im Inventar des Trockners ist und geben dessen
// Properties zurueck.
private string my_long() {
  object inv = first_inventory(ME);
  if ( objectp(inv) )
    return inv->QueryProp(P_LONG)+inv->Name(WER,1)+
      " wird gerade getrocknet.\n";
  return 0;
}

private string|string* my_name() {
  object inv = first_inventory(ME);
  if ( objectp(inv) )
    return inv->QueryProp(P_NAME);
  return Query(P_NAME,F_VALUE);
}

private mixed my_mat() {
  object inv = first_inventory(ME);
  if ( objectp(inv) )
    return inv->QueryProp(P_MATERIAL);
  return ([]);
}

private int|string my_noget() {
  object inv = first_inventory(ME);
  if ( objectp(inv) ) {
    if ( find_call_out(#'destroy_herb) > -1 ) {
      return inv->Name(WER,1)+" wurde zu lange getrocknet und ist inzwischen "
        "so heiss, dass Du "+inv->QueryPronoun(WEN)+" nicht mehr an Dich "
        "nehmen kannst. Verzweifelt tippelst Du vor dem Kraut hin und her, "
        "siehst aber keinen Weg, es noch zu retten.";
    }
    else {
      return inv->Name(WER,1)+" wird gerade getrocknet, Du solltest "+
        inv->QueryPronoun(WEN)+" liegenlassen, bis "+
        inv->QueryPronoun(WER)+" fertig ist.";
    }
  }
  return 1;
}

private string* my_ids() {
  object inv = first_inventory(ME);
  if ( objectp(inv) )
    return inv->QueryProp(P_IDS);
  return Query(P_IDS,F_VALUE);
}

// Kommandofunktion zum Starten des Tocknungsvorganges. Holt sich die
// relevanten Daten aus dem Krautmaster ab, setzt Meldungen und Texte und
// wirft den Trocknungs-Callout an.
static int cmd_trocknen(string str, mixed *param) {
  // Master liefert leeres Array oder eins mit 2 Elementen ({delay, quality})
  // environment(ME) liest er selbstaendig aus
  int *drying_data = PLANTMASTER->QueryDryingData();
  object kraut = param[0];

  // Der Trockner taeuscht vor, selbst das Kraut zu sein, das zum Trocknen
  // im Raum liegt. Daher wird hier noch geprueft, ob der Spieler vielleicht
  // den Trockner selbst zu trocknen versucht. 
  if ( kraut == ME ) {
    tell_object(PL, BS(kraut->Name(WER,1)+" wird bereits getrocknet, Du "
      "solltest "+kraut->QueryPronoun(WEN)+" besser liegenlassen."));
  }
  // Es muss sich auch um ein Kraut handeln und nicht irgendwas anderes.
  else if ( !IS_PLANT(kraut) ) {
    return 0;
  }
  // Spieler muss das Kraut im Inventar haben.
  else if ( environment(kraut) != PL ) {
    tell_object(PL, BS(
      "Du musst "+kraut->name(WEN,1)+" schon in die Hand nehmen, um "+
      kraut->QueryPronoun(WEN)+" sorgfaeltig trocknen zu koennen."));
  }
  // Das Kraut darf nicht unwirksam sein, was durch eine Plant-ID von -1 
  // gekennzeichet ist.
  else if ( param[0]->QueryPlantId() == -1 ) {
    tell_object(PL, BS(
      kraut->Name(WER,1)+" haette ohnehin keine Wirkung, da kannst Du Dir "
      "die Muehe sparen, "+kraut->QueryPronoun(WEN)+" noch aufwendig zu "
      "trocknen."));
  }
  // Master hat keine Daten geliefert, also befindet sich der Trockner
  // offenbar in einem unzulaessigen Raum.
  else if ( sizeof(drying_data) != 2 ) {
    tell_object(PL, BS(
      "Dieser Ort ist nicht geeignet, um "+kraut->name(WEN,1)+" hier zu "
      "trocknen."));
  }
  // Kraut ist schon getrocknet? Dann waere eine weitere Trocknung unsinnig.
  else if ( kraut->QueryDried() ) {
    tell_object(PL, BS(kraut->Name(WER,1)+" ist schon getrocknet, eine "
      "weitere Behandlung wuerde "+kraut->QueryPronoun(WEM)+" zu stark "
      "zusetzen, "+kraut->QueryPronoun(WEN)+" gar zerstoeren."));
  }
  // Es ist schon eine Pflanze im Trockner? Dann nicht noch eine reintun.
  else if ( first_inventory(ME) ) {
    tell_object(PL, BS("Hier wird gerade schon etwas getrocknet."));
  }
  // Aus irgendeinem Grund schlaegt die Bewegung des Krautes in den Trockner
  // fehl? Dann muss sich das ein Magier anschauen, denn das geht nicht mit
  // rechten Dingen zu.
  else if ( kraut->move(ME, M_PUT) != MOVE_OK ) {
    tell_object(PL, BS("Aus einem Dir unerfindlichen Grund schaffst Du es "
      "nicht, die Trocknung "+kraut->name(WESSEN,1)+" zufriedenstellend "
      "durchzufuehren und brichst den Versuch wieder ab. Du solltest einem "
      "Magier Bescheid sagen, dass hier etwas nicht stimmt."));
  }
  // Alles geklappt, alle Bedingungen erfuellt? Dann koennen wir jetzt
  // tatsaechlich endlich das Kraut trocknen.
  else {
    int drying_delay = drying_data[0]; // nur lokal benoetigt
    drying_quality = drying_data[1]; // globale Variable
    string where = load_name(environment(ME));
    string msg_self, msg_other;
    string kr = kraut->name(WEN,1);
    string* callout_msgs = ({
        kraut->Name(WER,1)+" ist jetzt in einem zufriedenstellenden "
        "Zustand. Besser wirst Du es an diesem Ort vermutlich nicht "
        "hinbekommen, daher beendest Du die Trocknung und nimmst "+
        kraut->QueryPronoun(WEN)+" wieder an Dich.",
        PL->Name(WER)+" schaut "+kr+" pruefend an und "
        "beendet dann die Trocknung, offenbar zufrieden mit dem Resultat."});
    int blocker; // auf 1 setzen, falls das Trocknen verhindert werden soll
    // Hier koennen jetzt abhaengig vom Raum passende Meldungen gesetzt
    // werden. Die zulaessigen Standorte hier noch weiter zu obfuscaten
    // waere zwar moeglich, aber zu unuebersichtlich geworden.
    switch(where) {
      /*
       * GEBIRGE 
       */
      case "/d/gebirge/silvana/cronoertal/room/th7u":
        msg_self = "Du legst "+kr+" vorsichtig und in gebuehrendem Abstand "
          "zu den Flammen neben die Feuerstelle und wartest gespannt, ob "
          "die Trocknung wohl gelingen wird.";
        msg_other = PL->Name(WER)+" legt etwas neben die Feuerstelle, "
          "vermutlich, um es zu trocknen.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen am Feuer";
        break;
      /*
       * EBENE 
       */
      case "/d/ebene/zardoz/burg/kueche":
        msg_self = "Du legst "+kr+" vorsichtig an eine der kuehleren "
          "Stellen des Bleches im Ofen, es soll ja trocknen, und nicht "
          "backen.";
        msg_other = PL->Name(WER)+" legt etwas Gruenzeug auf das Blech im "
          "Ofen.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen im Ofen";
        break;
      case "/d/ebene/esme/masinya/rooms/kueche":
        msg_self = "Du haengst "+kr+" ueber den Herd, um "+
          kraut->QueryPronoun(WEN)+" in der Abwaerme trocknen zu lassen.";
        msg_other = PL->Name(WER)+" haengt ein Kraut zum Trocknen ueber "
          "den Herd.";
        short_desc = kraut->Name(WER)+" haengt zum Trocknen ueber dem Herd";
        break;
      case "/d/ebene/throin/brauerei/room/darre06":
        msg_self = "Du legst "+kr+" zu dem Malz in den "
          "Keimkasten in der Hoffnung, dass "+kraut->QueryPronoun(WER)+
          " auf diese Weise getrocknet werden kann.";
        msg_other = PL->Name(WER)+" legt etwas eigenes Gruenzeug zu dem "
          "Malz in den Keimkasten.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen im Keimkasten";
        break;
      case "/d/ebene/arathorn/orakel/room/zelt":
        msg_self = "Du legst "+kr+" vorsichtig ans "
          "Lagerfeuer. Du schaust unsicher zu Chinkuwaila, doch der "
          "alte Schamane nickt zustimmend, Du hast wohl alles richtig "
          "gemacht.";
        msg_other = PL->Name(WER)+" legt eine Pflanze ans Lagerfeuer, wohl "
          "um sie zu trocknen.";
        short_desc = kraut->Name(WER)+" wird gerade am Lagerfeuer getrocknet";
        break;
      /*
       * WUESTE
       */
      case "/d/wueste/tsunami/schule/rooms/kraeuter":
        blocker = objectp(present_clone("/d/wueste/tsunami/schule/mon/hexe", 
                    environment(ME)));
        if ( !blocker ) {
          msg_self = "Du steckst "+kr+" in Muetterchen "
            "Isewinds Trockenofen, der wohlig zu knistern beginnt.";
          msg_other = PL->Name(WER)+" legt vorsichtig ein Kraut ";
          short_desc = kraut->Name(WER)+" wird gerade im Ofen getrocknet";
        }
        else {
          msg_self = "Muetterchen Isewind haelt Dich auf, sie scheint "
            "niemanden an ihren Trockenofen heranlassen zu wollen.";
          msg_other = PL->Name(WER)+" wird von Muetterchen Isewind an der "
            "Benutzung des Ofens gehindert.";
        }
        break;
      /*
       * WALD
       */
      case "/d/wald/feigling/quest/room/huette3":
        msg_self = "Du legst "+kr+" so nah ans Feuer, "
          "wie Du glaubst, dass es der Trocknung nicht schadet.";
        msg_other = PL->Name()+" legt eine Pflanze an die Kochstelle.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen an der "
          "Feuerstelle";
        break;
      case "/d/wald/leusel/quest/rooms/laborsuedosten":
        msg_self = "Du legst "+kr+" in eins der "
          "Tonschiffchen und schiebst es in den mittelheissen Ofen hinein. "
          "Hoffentlich geht das gut, es kommt Dir da drinnen schon fast zu "
          "warm fuer eine ordnungsgemaesse Trocknung vor.";
        msg_other = PL->Name(WER)+" schiebt ein Kraut in einem "
          "Tonschiffchen in einen der Oefen hinein, um es zu trocknen.";
        short_desc = kraut->Name(WER)+" dampft in einem Tonschiffchen im "
          "ersten Ofen vor sich hin";
        break;
      /*
       * INSELN
       */
      case "/d/inseln/zesstra/vulkanweg/room/r8":
        msg_self = "Du legst "+kr+" vorsichtig auf die "
          "heissen Felsen in der Naehe des Lavasees, auf dass die "
          "heissen Winde "+kraut->QueryPronoun(WEN)+" trocknen moegen.";
        msg_other = PL->Name(WER)+" legt ein Kraut auf den Felsen ab, um es "
          "von der heissen Luft trocknen zu lassen.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen auf dem Felsen";
        break;
      case "/d/inseln/miril/zyklopen/room/palast/insel1p6":
        // QueryOven() liefert 2 fuer "Feuer", 1 fuer "Glut", 0 fuer "aus".
        switch (environment(ME)->QueryOven()) {
          case 2:
            msg_self = "Du legst "+kr+" vorsichtig an das Feuer, das in "
              "der Feuerstelle brennt, sorgsam bemueht, dass "+
              kraut->QueryPronoun(WER)+" nicht zuviel Hitze abbekommt.";
            msg_other = PL->Name(WER)+" legt sorgsam ein Kraut in die Naehe "
              "des Feuers, das in der Feuerstelle brennt.";
            break;
          case 1:
            msg_self = "Du legst "+kr+" an die Feuerstelle, pruefst die "
              "Hitze und rueckst "+kraut->QueryPronoun(WEN)+" noch etwas "
              "naeher an die Glut, dann trittst Du zufrieden einen Schritt "
              "zurueck.";
            msg_other = PL->Name(WER)+" legt "+kraut->name(WEN)+" an die "
              "Feuerstelle, schubst noch ein wenig daran herum und tritt "
              "dann von der Glut zurueck, "+PL->QueryPronoun(WER)+" scheint "
              "recht zufrieden zu sein.";
            break;
          default:
            blocker=1;
            msg_self = "In dem Kamin findest Du nicht einmal etwas Glut, "
              "geschweige denn offenes Feuer. So wird das mit dem Trocknen "
              "nichts, und auf dem Herd ist Dir das Risiko zu gross, dass "
              "Fett aus der Pfanne auf "+kr+" spritzt.";
            break;
        }
        short_desc = kraut->Name(WER)+" liegt zum Trocknen an der "
          "Feuerstelle";
        break;
      /*
       * POLAR
       */
      case "/d/polar/tilly/llp/rentner/kueche":
        msg_self = "Der Herd gibt eine infernalische Hitze ab. Zum Kochen "
          "ist das toll, aber Du brauchst doch einen Moment, um ein "
          "geeignetes Plaetzchen fuer "+kr+" zu finden.";
        msg_other = PL->Name(WER)+" legt ein Kraut auf den heissen Herd "
          "und schiebt es unruhig noch ein wenig hin und her, als waere "+
          PL->QueryPronoun(WEM)+" die Hitze beinahe ein wenig zu gross.";
        short_desc = kraut->Name(WER)+" liegt auf dem schmiedeeisernen "
          "Herd";
        break;
      /*
       * VLAND
       */
      case "/d/vland/morgoth/room/kata/ukat13":
        object c = present_clone("/d/vland/morgoth/obj/kata/rfdcorpse", 
              environment(ME));
        // Bequemer, das auf 0 zu setzen, wenn's klappt, bei sovielen
        // Hinderungsgruenden. ;-)
        blocker = 1;
        // Leiche liegt da.
        if ( objectp(c) ) 
        {
          // Feuerdaemon anwesend: der blockt den Versuch
          if ( present_clone("/d/vland/morgoth/npc/kata/firedemon3",
                environment(ME)) ) 
          {
            msg_self = "Der Feuerdaemon droht Dir mit sengender Hoellenpein. "
              "Beschwichtigend trittst Du einen Schritt zurueck.";
            msg_other = PL->Name(WER)+" tritt auf den Aschehaufen zu, wird "
              "aber von dem Feuerdaemon bedroht und weicht wieder zurueck.";
          }
          // Aschehaufen nicht mehr heiss genug?
          else if ( c->QueryDecay()<2 ) {
            msg_self = c->Name(WER,1)+" ist schon zu sehr abgekuehlt und "
              "wuerde nicht mehr genug Hitze spenden, um "+kr+" zu trocknen.";
          }
          else 
          {
            blocker = 0;
            msg_self = "Dich vorsichtig umschauend, legst Du "+kr+" in die "
              "Naehe "+c->name(WESSEN,1)+". Hoffentlich gelingt das in "
              "dieser unwirtlichen Umgebung!";
            msg_other = PL->Name(WER)+" beugt sich zum Boden hinuntern und "
              "legt vorsichtig ein Kraut in die Naehe "+c->name(WESSEN,1)+".";
          }
          short_desc = kraut->Name(WER)+" liegt neben "+c->name(WEM,1)+
            ", offenbar soll "+kraut->QueryPronoun(WER)+" getrocknet werden";
        }
        // Keine Leiche da? Dann geht's nicht.
        else {
          msg_self = "Genausowenig, wie Dir die Flammen in diesem Inferno "
            "etwas anhaben koennen, so wenig kannst Du sie nutzen, um "+
            kr+" zu trocknen.";
        }
        break;
      case "/d/vland/morgoth/room/kata/kata5":
        object ob = present_clone("/d/vland/morgoth/npc/kata/ghost",
            environment(ME));
        blocker = 1;
        // Geist anwesend? Keine Chance.
        if ( objectp(ob) ) {
          msg_self = ob->Name(WER,1)+" stoert Dich in Deiner Konzentration, "
            "Du kannst so nicht arbeiten!";
        }
        // Umgebung noch neblig? Dann nicht trocknen.
        else if ( environment(ME)->QueryFog() ) {
          msg_self = "In diesem verdammten Nebel ist absolut nichts zu "
            "sehen. Ausserdem ist es hier viel zu feucht, "+kr+" wuerde "
            "ohnehin nur vor Deiner Nase wegschimmeln.";
        }
        // Feuer brennt nur noch 90 Sekunden? Dann lohnt sich das nicht.
        else if ( query_next_reset(environment()) < time()+90 ) {
          msg_self = "Ein pruefender Blick auf das Feuer zeigt Dir, dass "
            "es wohl nicht mehr lange genug brennen wird, um "+kr+" noch "
            "erfolgreich trocknen zu koennen.";
        }
        else {
          blocker = 0;
          msg_self = "Du legst "+kr+" in angemessenem Abstand zum Feuer "
            "auf den Boden und wartest gespannt, ob Dir hier wohl eine "
            "brauchbare Trocknung gelingen wird.";
          msg_other = PL->Name(WER)+" bueckt sich zum Boden und legt etwas "
            "ans Feuer, anscheinend ein Kraut, das "+PL->QueryPronoun(WER)+
            " trocknen will.";
          short_desc = kraut->Name(WER)+" liegt zum Trocknen am Feuer";
        }
        break;
      case "/d/vland/alle/koomi_v/wschenke/room/waldschenke":
        msg_self = "Dieser Kachelofen ist ungemein praktisch. Du legst "+
          kr+" einfach oben drauf, und die kuschelige Waerme trocknet "+
          kraut->QueryPronoun(WEN)+" beinahe von selbst.";
        msg_other = PL->Name(WER)+" legt ein Kraut zum Trocknen oben auf "
          "den Kachelofen, offenbar recht angetan von dessen kuscheliger "
          "Waerme.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen auf dem "
          "Kachelofen";
        break;
      /*
       * DEBUGZWECKE
       */
      case "/players/arathorn/workroom":
        msg_self = "Du haeltst das Kraut vors Feuer und beginnst die "
          "Trocknung.";
        msg_other = PL->Name(WER)+" schickt sich an, ein Kraut am Feuer zu "
          "trocknen.";
        short_desc = kraut->Name(WER)+" liegt zum Trocknen am Feuer";
        break;
    }
    // Raummeldungen entsprechend der eingestellten Texte ausgeben.
    tell_object(PL, BS(msg_self));
    if ( msg_other )
      tell_room(environment(ME), BS(msg_other), ({PL}));
    // Callout starten, wenn niemand das Trocknen verhindert.
    if ( !blocker )
      call_out(#'dry_plant, drying_delay, kraut, callout_msgs);
    // Ansonsten das Kraut in den Spieler zurueck, das ja oben schon
    // in den Trockner bewegt wurde.
    else {
      kraut->move(PL, M_GET);
    }
  }
  return 1;
}

// Kraut wird getrocknet, sofern der Spieler noch im Raum ist, ...
private void dry_plant(object kraut, string *msgs) {
  if ( objectp(PL) && environment(PL) == environment(ME) )
  {
    if (kraut->move(PL, M_GET) == MOVE_OK )
    {
      tell_object(PL, BS(msgs[0]));
      tell_room(environment(ME), BS(msgs[1]), ({PL}));
      kraut->DryPlant(drying_quality);
      return;
    }
    else
    {
      tell_object(PL, BS("Du kannst " + kraut->name(WEN,1) + " nicht mehr "
                         "tragen."));
    }
  }
  // ... ansonsten laeuft die Trocknung weiter, und das Kraut verbrennt.
  tell_room(environment(ME), BS(kraut->Name(WER,1)+" wird extrem dunkel, "
    "bald wird "+kraut->QueryPronoun(WER)+" zu nichts mehr zu gebrauchen "
    "sein!"));
  // Das Delay fuer diesen zweiten Callout ist immer fix. Kommt hoffentlich
  // selten genug vor und braucht daher eher nicht extra aus dem Master
  // geholt zu werden.
  call_out(#'destroy_herb, 20, kraut);
}

// Zerstoerung des Krautes. Da die Krautobjekte selbst eine Meldung 
// ausgeben, wenn DryPlant(0) gerufen wird, wird erst das Kraut ins
// Environment bewegt und erst danach die Funktion gerufen.
private void destroy_herb(object kraut) {
  kraut->move(environment(ME),M_PUT);
  kraut->DryPlant(0);
}

// Nur die korrekten Krautobjekte koennen in den Trockner bewegt werden.
// Schuetzt natuerlich nicht vor M_NOCHECK, aber wenn das vorkommen sollte,
// muss vielleicht noch weiter abgesichert werden, oder der verursachende
// Magier ausgeschimpft. ;-) 
varargs int PreventInsert(object ob) {
  if (load_name(ob) == PLANTITEM && clonep(ob)) 
    return ::PreventInsert(ob);
  return 1;
}

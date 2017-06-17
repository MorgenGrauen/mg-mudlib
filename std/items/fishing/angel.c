/*
  Zusammenfassung Bedingungen:
  - Angel auswerfen
      - Haken muss dran sein
      - Haken muss Koeder haben
      - darf nur ein Haken drin sein, sonst nix, und genau ein Haken
      - Angel im Inventar
      - Angel darf nicht schon ausgeworfen sein
      - passendes Gewaesser im Raum
      - Move-Hook muss korrekt registrierbar sein
  - Angel einholen
      - Angel-Callout muss ueberhaupt erstmal laufen
  - Waehrend des Angelns
      - bewegen beendet das Angeln
      - Ausloggen beendet das Angeln
      - Ende beendet das Angeln
      - nichts rausnehmen
      - nichts reintun, ausser Fisch am Ende der Wartezeit
      - Langbeschreibung zeigt "ausgeworfen", Koeder und gefangenen Fisch an
  - Fisch fangen (call_out zuende)
      - Gewaesser passt (immer noch)
      - Fisch passt noch in die Angel rein
  - Haken dranhaengen
      - Haken im Inv
      - Haken muss "echter" Angelhaken sein
      - Angel im Inv
      - Koeder am Haken
      - noch kein anderer Koeder dran
      - Angel ist nicht ausgeworfen
  - generell
      - nur Haken dranstecken, nix anderes reintun
      - reintun nur mit Syntax "haenge" etc. (damit sind Fische aussen vor,
        weil die Syntax auf Haken prueft)
        (ggf. ist dann das PreventInsert() zu vereinfachen)
 */

#pragma strong_types, save_types, rtt_checks
#pragma no_clone, no_shadow

inherit "/std/container";
inherit "/std/hook_consumer";

#include <new_skills.h>
#include <language.h>
#include <properties.h>
#include <moving.h>
#include <hook.h>
#include <wizlevels.h>
#include <items/fishing/fishing.h>
#include <items/fishing/aquarium.h>

#define TP this_player()
#define ME this_object()
#define BS(x) break_string(x, 78)

#define MIN_DELAY       80   // Mindestdelay bis Fisch beisst!
#define STD_DELAY_PLUS  60   //  +/- 60 sekunden Tolerant
#define MAX_FISH_BONUS  60   // max. Wartezeitverkuerzung durch P_FISH
#define WRONGWATER      60   // 1-2 min, falls Angel nicht geeignet
#define WRONGWORM       60   // 1-2 min, falls Koeder nicht geeignet

#define KOEDER_LOST     10
// Verlustwahrsch. fuer Koeder falls Fisch zu schwer

private object room, current_user;
private int active, actime;
private mapping aquarium = STDFISHLIST;

nomask varargs void StopFishing(string msg_me, string msg_room);
int IsFishing();

protected void create() {
  ::create();
  
  SetProp(P_SHORT, "Eine Angel");
  SetProp(P_LONG, "Eine nagelneue Angel.\n");
  SetProp(P_NAME, "Angel");
  AddId(({"angel", "rute", ANGEL_ID}));

  SetProp(P_GENDER, FEMALE);
  SetProp(P_MATERIAL,([MAT_MISC_WOOD:90,MAT_WOOL:10]));
  SetProp(P_TRANSPARENT,1);
  SetProp(P_WEIGHT,200);
  SetProp(P_MAX_WEIGHT, 10000);
  SetProp(P_WATER, W_USER);     //muss (mehrere) Flags gesetzt haben!
  SetProp(P_SOURCE_PREPOSITION, "von");
  SetProp(P_NR_HANDS, 2); 

  Set(P_NODROP, function string () {
    return (IsFishing()?"Du solltest die Angel nicht fallenlassen, solange "
      "Du angelst.\n":0);
  }, F_QUERY_METHOD);

  AddCmd(({"angel","angle"}),"angel");
  AddCmd("hole|hol&@ID&ein", "stopit", "Was willst Du (ein)holen?|"
    "Willst Du @WEN2 einholen?");
  AddCmd("haenge|haeng|befestige&@PRESENT&@ID", "move_in",
    "Was willst Du woran @verben?|"
    "Woran willst Du @WEN2 denn @verben?");

  AddCmd("angeltest","qangel");
}
 
protected void create_super() {
  set_next_reset(-1);
}
     
static int stopit(string str) {
  if( find_call_out("do_angel")==-1 )
    tell_object(TP, "Du angelst nicht!\nTP, ");
  else 
    StopFishing();
  return 1;
}

varargs string long(int mode) {
  string descr = QueryProp(P_LONG);
  if(!QueryProp(P_TRANSPARENT)) 
    return descr;

  if ( find_call_out("do_angel")!=-1 )
    descr += capitalize(QueryPronoun(WER))  + " ist ausgeworfen.\n";

  object *inv = all_inventory(ME);
  string inv_descr = make_invlist(TP, inv);
  if ( inv_descr != "" )
    descr += "An "+QueryPronoun(WEM) + " haeng"+(sizeof(inv)>1?"en":"t")+
    ":\n" + inv_descr;
  
  return descr;
}

/*int tell_stat()
{
  int n;
  
  if(!IS_WIZARD(TP))
    return 0;
  if(!active){ 
    write("Du angelst nicht\n"); return 1; 
  }
  n=find_call_out("do_angel");
  if(active)
    write(
     "----------------------------------------------------------------------\n"
     +"Der Fisch beisst in "+n+" sec!\n"
     +"Bestandswert des Raumes: "+room->QueryProp(P_FISH)
     +" = "+(room->QueryProp(P_FISH)+100)+"%.\n"
     +"Bestandswert des Koeders: "+koeder->QueryProp(P_FISH)
     +" = "+(100+koeder->QueryProp(P_FISH))+"%.\n"
     +"Gesamt:"+bestand+" = "+(bestand+100)+"%.\n");
  if(!(QueryProp(P_WATER)&room->QueryProp(P_WATER)))
    write("Die Angel passt nicht zum Gewaessertypen.\n");
  else
    write("Die Angel passt zum Gewaessertypen.\n");
  if(!(room->QueryProp(P_WATER)&koeder->QueryProp(P_WATER)))
    write("Der Koeder passt nicht zum Gewaessertypen.\n");
  else
    write("Der Koeder passt zum Gewaessertypen.\n");
  write(" => Delay="+delay+".\n"
    +"----------------------------------------------------------------------\n"
);
  return 1;
}*/

static int move_in(string str, mixed *param) {
  object haken = param[0];
  // param[1] sind wir selbst, daher ab hier ME verwendet.

  if ( find_call_out("do_angel")!=-1 ) { // angelt noch nicht?
    tell_object(TP, BS(Name(WER,1)+" ist bereits ausgeworfen, da haengst "
      "Du jetzt besser nichts mehr dran, das vertreibt nur die Fische."));
  }
  else if ( !haken->id(HAKEN_ID) ) { // echter Angelhaken?
    tell_object(TP, BS(haken->Name(WER,1)+" ist kein Angelhaken, "+
      haken->QueryArticle(WEN, 1, 1)+"kannst Du nicht an "+name(WEN,1)+
      " haengen."));
  }
  else if ( environment(haken) != TP ) { // Haken im Inv?
    tell_object(TP, BS("Wie er da so auf dem Boden liegt, schaffst Du es "
      "einfach nicht, "+haken->name(WEN,1)+" vernuenftig an "+name(WEM,1)+
      " zu befestigen."));
  }
  else if ( environment(ME) != TP ) { // Angel im Inv?
    tell_object(TP, BS("Es stellt sich viel fummeliger heraus als gedacht, "+
      haken->name(WEN,1)+" an "+name(WEM,1)+" zu befestigen, solange "+
      QueryPronoun(WER)+" auf dem Boden herumliegt."));
  }
  else if ( present(HAKEN_ID, ME) ) { // anderer Haken schon dran?
    tell_object(TP, BS("An "+name(WEM,1)+" haengt bereits ein Angelhaken."));
  }
  else if ( !haken->QueryKoeder() ) { // Koeder am Haken?
    tell_object(TP, BS("Du willst "+haken->name(WEN,1)+" gerade an "+
      name(WEN,1)+" haengen, als Dir auffaellt, dass Du beinahe den Koeder "
      "vergessen haettest."));
  }
  else { // dann darf der Haken rangehaengt werden.
    if ( haken->move(ME, M_PUT) == MOVE_OK ) {
      tell_object(TP, BS("Du haengst "+haken->name(WEN,1)+" an "+
        name(WEN,1)+"."));
      tell_room(environment(TP), BS(TP->Name(WER)+" haengt etwas metallisch "
        "Glaenzendes an "+name(WEN)+", vermutlich einen Angelhaken."), 
        ({TP}));
    }
    else {
      tell_object(TP, BS(haken->Name(WER,1)+" laesst sich nicht an "+
        name(WEM,1)+" befestigen."));
    }
  }
  return 1;
}

static int qangel() {
  if(IS_WIZARD(TP)) {
    call_out("do_angel",1);
    return 1;
  }
  return 0;
}

static int angel(string str) {
  object haken = present(HAKEN_ID,ME);
  if ( environment(ME) != TP ) {
    tell_object(TP, BS("Dafuer musst Du die Angel in die Hand nehmen."));
  }
  else if ( find_call_out("do_angel")!=-1 ) {
    tell_object(TP, "Das tust Du doch schon.\n");
  }
  else if(!objectp(haken)) {
    tell_object(TP, "Wie soll das gehen? Du hast ja nichtmal einen Haken "
      "an der Angel!\n");
  }
  else if(!haken->QueryKoeder()) {
    tell_object(TP, break_string("Ohne Koeder am Haken wird sich kein "
      "Fisch der Welt fuer Deine Angel interessieren.",78));
  }
  else if(present(FISCH_ID,ME)) {
    tell_object(TP, "Nimm erst mal die Beute von der Angel, die noch daran "
      "zappelt!\n");
  }
  else if ( !TP->UseHands(ME, QueryProp(P_NR_HANDS)) ) { // freie Haende?
    tell_object(TP, BS("Du musst schon beide Haende frei haben, um die "
      "Angel sicher festhalten zu koennen."));
  }
  else {
    // Raum festhalten, in dem geangelt wird.
    room = environment(TP);
    // Gewaessertyp abfragen
    active = room->QueryProp(P_WATER);
    // Kein Wasser vorhanden, oder nicht-befischbarer Sondertyp?
    if( (!active) || (active & W_OTHER) ) {
      tell_object(TP, "Du kannst hier nicht angeln.\n");
      TP->FreeHands(ME);
    }
    // totes Gewaesser erwischt
    else if ( active & W_DEAD ) {
      write("In dem Gewaesser hier gibt es keine Fische.\n");
      TP->FreeHands(ME);
    }
    // Jetzt aber: es gibt Fisch, Baby. ;-)
    else {
      int delay = MIN_DELAY; // 80 Sekunden.
      // Fischbestand ermitteln aus Raum- und Koederparametern
      int bonus = room->QueryProp(P_FISH) + haken->QueryProp(P_FISH);
      // Ist kein Bonus, sondern ein Malus rausgekommen?
      if ( bonus < 0 ) {
        // Dann begrenzen auf 5 Min.
        if ( bonus < -300 )
          bonus = -300;
        // Wartezeit erhoehen.
        delay += random(-bonus);
      }
      else {
        // Bonus deckeln auf Max-Wert (60 Sekunden)
        if ( bonus > MAX_FISH_BONUS ) {
          bonus = MAX_FISH_BONUS;
        }
        // Delay reduzieren (minimal 20 Sekunden)
        delay -= random(bonus);
      }
  
      // passt das Gewaesser zur Angel/zum Koeder ?
      if( !(QueryProp(P_WATER) & room->QueryProp(P_WATER)) )
        delay += WRONGWATER + random(WRONGWATER);
      if( !(room->QueryProp(P_WATER) & haken->QueryProp(P_WATER)) &&
          room->QueryProp(P_WATER) != W_USER )
        delay += WRONGWORM + random(WRONGWORM);

      int hook = TP->HRegisterToHook(H_HOOK_MOVE, ME, H_HOOK_OTHERPRIO(1),
                                     H_LISTENER, 0);
      if ( hook != 1 ) {
        active = 0;
        room = 0;
        TP->FreeHands(ME);
        tell_object(TP, BS(
          "Du schleuderst die Angel mit Schwung in Richtung Wasser, aber der "
          "Haken verfaengt sich in Deinen Sachen und piekst Dir in den "
          "Allerwertesten. Das versuchst Du besser noch einmal anders."));
        tell_room(environment(TP), BS(
          TP->Name()+" schleudert eine Angel in Richtung Wasser, ist dabei "
          "aber so ungeschickt, dass sich der Haken verfaengt."), ({TP}));
      }
      else {
        // Alle Bedingungen erfuellt: 
        // - Angel in der Hand.
        // - Gewaesser stimmt.
        // - Haken mit Koeder ist dran.
        // - alte Beute wurde abgenommen.
        // - Move-Hook gesetzt, um Abbruch ausloesen zu koennen.
        actime = time();
        current_user = TP;
        tell_object(TP, "Du wirfst die Angel aus.\n");
        tell_room(environment(TP), TP->Name()+" wirft eine Angel aus.\n", 
          ({TP}));
        call_out("do_angel", delay);
      }
    }
  }
  return 1;
}

private object GetFish(object room) {
  int wtype = room->QueryProp(P_WATER);
  string *fische;
  
  // Wenn GetAquarium() bei W_USER nichts zurueckliefert, geht der Angler
  // leer aus. Es kann kein Fisch aus der Std-Liste genommen werden, weil
  // der Gewaessertyp ja per Definition benutzerdefiniert ist.
  if ( wtype == W_USER )
    fische = room->GetAquarium(ME)||({});
  else 
    fische = aquarium[wtype];

  string beute = fische[random(sizeof(fische))];

  if ( !beute )
    return 0;
  // GetAquarium() liefert volle Pfade, STDFISHLIST nur Dateinamen relativ
  // zu FISH()
  else if ( wtype == W_USER )
    return clone_object(beute);
  else
    return clone_object(FISH(beute));
}

static void do_angel() {
  object haken = present(HAKEN_ID, ME);
  object room  = environment(TP);
  object fisch;

  string msg_self, msg_other;
  if ( member(TP->QueryProp(P_HANDS_USED_BY), ME)==-1 ) {
    tell_object(TP, BS("Waehrend Du vertraeumt vor Dich hingeangelt hast, "
      "hat sich Dein Griff an der Angel wohl gelockert, so dass der "
      "Fisch, der gerade anbeissen wollte, sie Dir beinahe entrissen "
      "haette."));
    tell_room(environment(TP), BS(TP->Name(WER)+" hat sich gerade "
      "ziemlich erschreckt, als "+TP->QueryPronoun(WEM)+" beinahe "+
      TP->QPP(ME, WEN, SINGULAR)+" "+Name(RAW)+" entrissen worden waere."),
      ({TP}));
    if ( !random(3) && haken->KoederGefressen() )
      tell_object(TP, BS("Der Koeder ist jedenfalls futsch."));
    return;
  }

  if ( !objectp(haken) ) {
    msg_self = "Huch, anscheinend hat sich in einem unbeobachteten "
      "Moment der Angelhaken von der Angelschnur geloest. War der Knoten "
      "doch nicht gut genug gesichert?";
    msg_other = TP->Name()+" zieht unglaeubig eine leere Angelschnur aus "
      "dem Wasser.";
  }
  else if ( !objectp(fisch=GetFish(room)) || 
            active != room->QueryProp(P_WATER) )
  {
    msg_self = "Anscheinend gibt es hier doch keine Fische. Du gibst "
      "auf und holst "+name(WEN,1)+" wieder ein.";
    // Leaken von Fischobjekten verhindern.
    if (objectp(fisch))
      fisch->remove();
  }
  else if ( fisch->move(ME) != MOVE_OK ) {
    msg_self = fisch->Name(WER)+" biss an, war aber zu schwer fuer "
      "Dich (oder Deine Angel). "+capitalize(fisch->QueryPronoun(WER))+
      " entgleitet Dir und plumpst zurueck ins Wasser!";
    
    if( !(fisch->QueryProp(P_FISH) & F_NOTHUNGRY) ) {
      haken->KoederGefressen();
      if(!random(KOEDER_LOST)) {
        msg_self += " Dein Haken ist dabei leider abgerissen!";
        tell_room(environment(TP), BS(Name(WER,1)+" von "+TP->Name(WEM)+
          " wackelte sehr stark."), ({TP}));
        haken->remove();
      }
    }
    fisch->remove(1);
  }
  else {
    haken->KoederGefressen();
  }
  StopFishing(msg_self, msg_other);
}

varargs int remove(int silent) {
  if ( find_call_out("do_angel")!=-1 )
    StopFishing(Name(WER,1)+" loest sich ploetzlich im Nichts auf.");
  return ::remove(silent);
}

// Jemand macht "ende" waehrend er angelt?
protected void NotifyMove(object dest, object oldenv, int method) {
  if ( find_call_out("do_angel")!=-1 && oldenv == current_user && 
       dest->IsRoom() ) {
    StopFishing("Du holst die Angel ein und legst sie beiseite.");
  }
  return ::NotifyMove(dest, oldenv, method);
}

// Spieler loggt waehrend des Angelns aus? Dann angeln einstellen.
void BecomesNetDead(object pl) {
  if ( find_call_out("do_angel")!=-1 && pl == current_user ) {
    StopFishing("Angeln ist so eine beruhigende Freizeitbeschaeftigung! "
      "Bevor Du mit der Angel in der Hand einschlaeft, holst Du sie "
      "lieber schnell ein.", pl->Name()+" holt schnell noch die Angel ein, "
      "bevor "+pl->QueryPronoun(WER)+" einschlaeft.");
  }
}

int PreventInsert(object ob) {
  // Funktion wird aus einer Spielereingabe heraus gerufen
  // Ich nehme hier nicht TP, weil der call_out("do_angel") die
  // gefangenen Fische per move() in die Angel bewegt: Bei call_out()s wird
  // TP durch die Kette weitergereicht, so dass dann hier keine 
  // Unterscheidung zwischen Spielereingabe und call_out() moeglich waere.
  SetProp(P_NOINSERT_MSG, 0);
  if ( stringp(query_verb()) &&
       member(({"haenge","haeng","befestige"}), query_verb()) == -1 ) {
    SetProp(P_NOINSERT_MSG, BS(Name(WER,1)+" ist nur dafuer geeignet, "
      "Angelhaken daranzuhaengen."));
    return 1;
  }
  if( ob->id(FISCH_ID) ) {
    write("Etwas zappelt an Deiner Angel.\n");
  }
  return ::PreventInsert(ob);
}

// Wenn geangelt wird, nimmt hier niemand was raus.
public int PreventLeave(object ob, mixed dest) {
  SetProp(P_NOLEAVE_MSG, 0);
  if ( find_call_out("do_angel")!=-1 ) {
    if ( objectp(TP) && ob->id(HAKEN_ID) )
      SetProp(P_NOLEAVE_MSG, BS("Der Haken ist gerade unter Wasser. Es "
        "waere kontraproduktiv, ihn ausgerechnet jetzt abzunehmen."));
    return 1;
  }
  return ::PreventLeave(ob, dest);
}

// Beendet das Angeln, indem Aktiv-Flag geloescht, Hook deregistriert
// und call_out() geloescht wird. Zusaetzlich werden die als Argument ueber-
// gebenen Meldungen ausgegeben, oder passende generische verwendet.
nomask varargs void StopFishing(string msg_me, string msg_room) {
  active = 0;
  while(remove_call_out("do_angel")!=-1);
  object env = environment(ME);
  if ( objectp(env) && env == current_user ) {
    env->FreeHands(ME);
    current_user = 0;
    env->HUnregisterFromHook(H_HOOK_MOVE, ME);
    tell_object(env, BS(msg_me||"Du holst Deine Angel wieder ein."));
    tell_room(environment(env), BS(msg_room||
      env->Name(WER)+" holt "+env->QueryPossPronoun(ME,WEN,SINGULAR)+" "+
      name(RAW)+" ein."), ({env}));
  }
}

// Diese Methode wird in jedem Hook-Konsumenten eines Hook-Providers
// aufgerufen, solange die Verarbeitung nicht vorher abgebrochen wurde.
// Dann jedoch wurde auch die Aktion (Bewegung) nicht ausgefuehrt, d.h.
// solange hier kein Event ankommt, wurde keine Bewegung ausgefuehrt und 
// das Angeln kann weiterlaufen.
mixed HookCallback(object hookSource, int hookid, mixed hookData) {
  if ( hookid == H_HOOK_MOVE && hookSource == current_user ) {
    StopFishing("Deine Angelschnur ist zu kurz, um damit rumlaufen zu "
      "koennen. Du holst die Angel wieder ein.");
  }
  return ({H_NO_MOD, hookData});
}

// Wird gerufen, wenn der Konsument von einem anderen mit hoeherer Prioritaet
// verdraengt wurde.
void superseededHook(int hookid, object hookSource) {
  if ( hookid == H_HOOK_MOVE && hookSource == current_user ) {
    StopFishing("Irgendetwas ist gerade passiert, das alle Fische "
      "vertrieben hat. Du siehst sie unter der Wasseroberflaeche davon"
      "flitzen. Ernuechtert holst Du Deine Angel wieder ein.");
  }
}

// Angelzustand abfragen.
int IsFishing() {
  return (find_call_out("do_angel")!=-1);
}

// Gewaessertyp abfragen, in dem gerade geangelt wird.
int query_active() {
  return active;
}

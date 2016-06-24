//Revision 1.1: QueryQuality() added, sollte aber noch verbessert werden!
/*
Letzte Aenderung: Vanion, 26.05.02
                  - Die Fische haben jetzt wieder einen Wert. Der Preis wird 
                    von einem Master kontrolliert. Je mehr Fisch ein Spieler
                    angelt, desto billiger wird der Fisch.
                  - Gross-Kleinschreibung beim essen korrigiert.
*/

#pragma strong_types, save_types, rtt_checks
#pragma no_clone, no_shadow

inherit "/std/thing";

#include <language.h>
#include <properties.h>
#include <defines.h>
#include <items/fishing/fishing.h>

#define TP this_player()
#define BS(x) break_string(x, 78)

private string eatmessage;
string corpseobject = "/items/fishing/graeten";
static int price_modifier;

#define DECAYSTART   600  // beginnt nach 10 min zu faulen
#define DECAYSTEP    120  // alle 2 Minuten wird er schlechter
#define MAXQUALITY     4  // nach 4 Decays ist er giftig
#define KILLFACTOR     5  // reduce_hit_points(STEPS*KILLFACTOR)
#define HEALFACTOR    32  // 800 g = 25 LP
#define MAXHEAL       40  // Kein Fisch bringt mehr als 40 LP
#define MINHEAL        0  // oder weniger als 0
#define FOODHEALFACTOR 3  // LP=LP + 1/3 des Gewichts

void SetEatMessage(string str);
void SetCorpseObject(string str);
int QueryQuality();

protected void create() {
  ::create();
  // Reset kurz nach Erreichen des max. Gammelzustands, dann zerstoeren.
  set_next_reset(DECAYSTART+(MAXQUALITY+1)*DECAYSTEP);
  AddId(({FISCH_ID ,"fisch"}));
  SetProp(P_NAME, "Fisch" );
  SetProp(P_GENDER , MALE);
  SetProp(P_SHORT, "Ein Fisch" );
  SetProp(P_LONG, "Ein stinknormaler Fisch.\n");
  SetProp(P_MATERIAL, MAT_MISC_FOOD);
  SetProp(P_NOBUY,1);
  SetProp(P_WEIGHT, 500);
  AddCmd("iss&@ID", "eatit", "Was willst Du essen?");

  // Vanion, 26.05.02: Der Preis wird erst nach dem create() geholt, 
  // da das Gewicht dann schon gesetzt werden muss.
  call_out("GetPriceModifier",0);
}

protected void create_super() {
  set_next_reset(-1);
}

// Wert des Fisches ist abhaengig vom price_modifier und von P_VALUE
static int _query_value() {
  int value;

  // Minimalpreis ist immer 1 Muenze. Damit man ihn loswird.
  if(!price_modifier) 
    return 1;

  value = Query(P_VALUE, F_VALUE);

  if(!value)
    return 0; 

  return (value*price_modifier)/100 ;
}

/*Eingebaut von Vanion, 26.05.02
  Die Funktion holt den price_modifier aus dem Fish-Master.
  Hierbei wird ein Wert zwischen 0 und 100 zurueckgegeben, der 
  den Prozentsatz des Wertes von P_VALUE angibt. 50 bedeutet, dass
  der Fisch 50% seines Maximalwertes hat. */

#define FISHMASTER "p/daemon/fishmaster"

void GetPriceModifier() {
  if (!this_player()) return;

  // Jetzt wird der Preis fuer den Fisch bestimmt.
  price_modifier = FISHMASTER->PriceOfFish(TP, ME);
  return;
}

void SetEatMessage(string str) {
  eatmessage=str;
}

void SetCorpseObject(string str) {
  corpseobject=str;
}

int _set_weight(int gramm) {
  int tmp = gramm/2 + random(gramm/2); // "nat. Schwankungen" nachbilden
  Set(P_WEIGHT, tmp, F_VALUE);
  SetProp(P_VALUE, tmp/4); // Im normalen Laden nur 1/4 Gewicht in Muenzen
  return tmp;
}

void init() {
  ::init();
  if( QueryProp(P_FISH)&F_REPLACE ) {
    if(query_once_interactive(environment(ME))) {
      call_out("ReplaceFish",0);
    }
  }
  return;
}
 
varargs string long(int mode) {
  string pron = capitalize(QueryPronoun(WER));
  string txt = QueryProp(P_LONG);
 
  switch(QueryQuality()) {
    case 4:  txt += pron+" ist fangfrisch.\n";            break;
    case 3:  txt += pron+" ist noch recht frisch.\n";     break;
    case 2:  txt += pron+" ist schon etwas runzlig.\n";   break;
    case 1:  txt += pron+" sieht schon recht alt aus.\n"; break;
    case 0:  txt += pron+" ist nicht mehr geniessbar!\n"; break;
    case -1: txt += pron+" ist vergammelt.\n";            break;
    default:
      txt += (pron+" stinkt widerlich und ist bereits voller Maden!\n"+pron+
        " zerfaellt noch waehrend Du "+QueryPronoun(WEN)+" betrachtest.\n");
      remove();
      break;
  }
  return(txt);
}

int _query_fish_age() {
  return (time()-QueryProp(P_CLONE_TIME));
}

static int eatit(string str) {
  string msg_other = TP->Name()+" isst "+name(WEN)+".";

  if ( !eatmessage )
    eatmessage = "Du isst "+name(WEN,1)+" auf.";

  if ( stringp(corpseobject) ) {
    object muell=clone_object(corpseobject);
    if ( muell->move(TP, M_GET) != MOVE_OK ) {
      muell->move(environment(TP), M_GET);
    }
  }
  
  // Heilung berechnen.
  // Heilwert runterskalieren mit Qualitaet, wenn das Mindestalter erreicht
  // ist, aber nur, wenn der Fisch auch vergammeln kann.
  int healing = QueryProp(P_WEIGHT)/HEALFACTOR;
  int age = QueryProp(P_FISH_AGE);
  if ( age > DECAYSTART && !(QueryProp(P_FISH)&F_NOROTTEN)) {
    healing = healing * (QueryQuality()*25)/100;
  }
  if ( healing > MAXHEAL )
    healing = MAXHEAL;
  else if ( healing < MINHEAL )
    healing = MINHEAL;

  if( age > DECAYSTART + MAXQUALITY*DECAYSTEP) {
    tell_object(TP, BS(eatmessage + " Dir wird speiuebel, der Fisch war "
      "zu alt!"));
    tell_room(environment(TP), BS(msg_other + TP->QueryPronoun(WER)+
      " verdreht angeekelt die Augen."), ({TP}));
    TP->reduce_hit_points( ((age-DECAYSTART)/DECAYSTEP)*KILLFACTOR );
    remove();
  }
  else {
    if ( TP->eat_food(healing*FOODHEALFACTOR) ) {
      tell_object(TP, BS(eatmessage));
      tell_room(environment(TP), BS(msg_other), ({TP}));
      if ( !(QueryProp(P_FISH)&F_NOHEAL) )
        TP->buffer_hp(healing,5);
      remove();
    }
  }
  return 1;
}

// Qualitaet des Fisches ermitteln. Rueckgabewert: 4, 3, 2, 1, 0, -1 ...
int QueryQuality() {
  // Alter in Sekunden ueber der Grenze, ab der der Fisch anfaengt zu gammeln
  int age = QueryProp(P_FISH_AGE)-DECAYSTART;
  // Wenn die Grenze noch nicht erreicht ist, oder der Fisch generell nicht
  // gammelt (F_NOROTTEN), dann ist das Alter 0 und die Qualitaet max.
  if ( age<0 || QueryProp(P_FISH) & F_NOROTTEN ) 
    age = 0;

  // Qualitaet in DECAYSTEP-Stufen reduzieren (60 s pro Gammelstufe)
  return MAXQUALITY-(age/DECAYSTEP);
}

void reset() {
  if (clonep(ME)) {
    string msg = BS(Name()+" ist vergammelt und zerfaellt.") ;
    if ( interactive(environment()) ) {
      tell_object(environment(), msg);
    }
    else if ( environment()->IsRoom() ) {
      tell_room(environment(), msg);
    }
    if ( stringp(corpseobject) ) {
      object muell=clone_object(corpseobject);
      if ( muell->move(environment(), M_GET) != MOVE_OK ) {
        muell->move(environment(environment()), M_GET);
      }
    }
    call_out("remove",0);
    return; 
  }
  return ::reset();
}

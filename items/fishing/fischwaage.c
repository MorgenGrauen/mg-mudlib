// Aenderungshistorie:
// 25.07.2015, Amaryllis: QueryFish implementiert, AddFishFood an/von Fraggle adaptiert

inherit "/std/container";

#include <moving.h>
#include <fishing.h>
#include <language.h>
#include <properties.h>
#include <defines.h>
#include <money.h>

#define TP this_player()
#define BS(x) break_string(x, 78)

private int good_price, bad_price, FAC, FOC, DIV;
private int waagen_preis_faktor=4; // Um wieviel teurer als in Laeden wird 
                           // der Fisch verkauft / Im Fisch wird durch 
                           // 4 geteilt, das wird hier wieder ausgeglichen.

private int get_price();
private void RemoveFish();

// Amaryllis, 25.07.2015: QueryFish "quick and dirty" reingehackt, 
// damits funktioniert. Kann man sicher besser machen...
private int QueryFish(object ob); // wird hier im Objekt in PreventInsert verwendet

private int QueryFish(object ob) {
  return (objectp(ob))&&(ob->id(FISCH_ID));
}

private int SetFactors(int good_value, int bad_value) {
  FAC=good_value;
  FOC=bad_value;
  DIV=100;
  if(!(FAC && FOC && DIV))
    SetFactors(100,50);
  return 1;
}

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  SetFactors(100,50); //100% fuer frische, 50% fuer alte Fische
  SetProp(P_NAME, "Fischwaage");
  AddId(({"waage","fischwaage"}));
  SetProp(P_GENDER, FEMALE);
  SetProp(P_SHORT, "Eine Fischwaage");
  SetProp(P_LONG,
    "Eine sehr genaue Fischwaage mit einem grossen Zeiger. Du kannst\n"
    "Deine Beute drauflegen, den Zeiger ablesen und, sofern Du mit\n"
    "dem Preis einverstanden bist, die Verkaufsglocke laeuten.\n");
  SetProp(P_DEST_PREPOSITION, "auf");
  SetProp(P_SOURCE_PREPOSITION, "von");
  SetProp(P_MATERIAL, MAT_MISC_METAL);
  SetProp(P_TRANSPARENT, 1);
  SetProp(P_NOGET, 1);
  SetProp(P_MAX_WEIGHT, 10000);
  SetProp(P_WEIGHT, 5000);
  
  AddDetail(({"zeiger"}), BS(
    "Der Zeiger besteht aus Metall. Du kannst ihn ablesen, wenn Du "
    "genau wissen willst, was Dein Fang wert ist."));
  AddDetail(({"fang","beute"}), function string (string key) {
    return BS(sizeof(all_inventory())?"Du schaust zufrieden auf die Beute, "
      "die auf der Waage liegt. Welcher Preis sich dafuer wohl erzielen "
      "laesst?":"Du muesstest erst einmal etwas drauflegen.");
  });
  AddReadDetail(({"zeige","zeiger ab"}), function string (string key) {
    int current_weight = query_weight_contents();
    get_price();
    return
      "Du schaust auf den grossen Zeiger der Waage, der erstaunlicherweise\n"
      "nicht nur das Gewicht, sondern auch den Preis anzeigt.\n"
      "Der Zeiger steht auf "+
        (current_weight?current_weight+" Gramm.\n":"Null.\n")+
      (good_price?good_price+" Goldmuenzen fuer fangfrischen Fisch.\n":"")+
      (bad_price?bad_price+" Goldmuenzen fuer etwas aelteren Fisch.\n":"");
  });
  AddDetail(({"glocke"}), BS(
    "An der Waage befindet sich eine kleine Glocke die Du laeuten kannst, "
    "um den Inhalt der Waage zu verkaufen. Und warte damit nicht zu lange, "
    "je aelter der Fisch, desto niedriger der Preis!"));

  AddCmd("laeute&glocke|verkaufsglocke","cmd_laeute",
    "Was willst Du denn laeuten?");
}

static int cmd_laeute(string str, mixed *param) {
  tell_room(ME, TP->Name()+" laeutet eine kleine Glocke an der Fischwaage.",
    ({TP}));
  tell_object(TP, "Du laeutest die Glocke, um Deine Ware zu verkaufen.\n");
  foreach(object ob: all_inventory(ME)) {
    if ( !ob->id(FISCH_ID) ) {
      tell_object(TP, 
        "Auf der Waage liegt nicht nur Fisch!\n"
        "Du solltest nicht versuchen, hier zu betruegen!\n");
      tell_room(ME, TP->Name()+" macht sich hier gerade unbeliebt.\n",({TP}));
      return 1;
    }
    if ( ob->QueryQuality()<=0 ) {
      tell_object(TP, "Auf der Waage liegt alter, gammliger Fisch.\n"
           "Den kannst Du nicht verkaufen!\n");
      tell_room(ME, TP->Name()+" macht sich hier gerade unbeliebt.\n",({TP}));
      return 1;
    }
  }
  
  int price=get_price();
  if( !price ) {
    tell_object(TP, 
      "Du solltest Deine Beute erstmal auf die Waage legen!\n");
    return 1;
  }
  tell_object(TP, "Du erhaeltst "+price+" Goldmuenzen fuer Deinen Fisch.\n");
  tell_room(ME, TP->Name()+" erhaelt haufenweise Geld fuer "+
    TP->QueryPossPronoun(MALE, WEN)+" Fisch.\n",({TP}));
  //ZZ: AddMoney() gibt die werte von move() zurueck, d.h. !=0 geht nicht.
  if (this_player()->AddMoney(price) != MOVE_OK) { 
    object money=clone_object(GELD);
    tell_object(TP, BS(
      "Du kannst das Geld nicht mehr tragen. Es faellt klimpernd zu "
      "Boden."));
    money->SetProp(P_AMOUNT, price);
    money->move(environment(ME), M_MOVE_ALL|M_PUT);
  }
  RemoveFish();
  return 1;
}

private int get_price() {
  int preis, preis_von_diesem;
  int gewicht;

  good_price     = 0;
  bad_price      = 0;

  foreach(object ob : all_inventory(ME)) {
    preis_von_diesem = ob->QueryProp(P_VALUE);
    if(ob->QueryQuality()>2)
      good_price += preis_von_diesem*FAC/DIV;
    else
      bad_price +=preis_von_diesem *FOC/DIV;
  }

  good_price *= waagen_preis_faktor;
  bad_price  *= waagen_preis_faktor;
  return (good_price + bad_price);
}

int PreventInsert(object obj) {
//  if( objectp(obj) && obj->QueryFish() )
  if( objectp(obj) && QueryFish(obj) )
    return ::PreventInsert(obj);
  write("Das ist eine Fischwaage! Und keine Waage fuer "+obj->name(WEN)+
    ".\n");
  return 1;
}

#define ETO environment(this_object())

private void RemoveFish() {
  string pubname = ETO->GetPub();
  object pub;

  if ( stringp(pubname) )
    pub = load_object(pubname);
  else 
    pub = ETO;

  foreach(object ob : all_inventory(ME)) {
    // pub->AddFishFood(ob);
    // Amaryllis, 25.07.2015: Angepasst wegen AddFishFood von Fraggle
    pub->AddFishFood(ob->name(WER,0),ob->QueryProp(P_WEIGHT),ob->QueryQuality());
    ob->remove();
  }
}

#undef ETO

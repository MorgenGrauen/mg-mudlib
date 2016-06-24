// MorgenGrauen MUDlib
//
// shells/hobbit.c -- Hobbit Shell
//
//   9.April 1995  V1.0 Gundur
//
//   15.Juni prayroom und defHome auf Hobbitdorf gesetzt von Gundur
//
// $Id: hobbit.c 8920 2014-09-02 20:18:38Z Zesstra $

#pragma strong_types,save_types

inherit "std/player/base";

#include <properties.h>
#include <attributes.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>
#include <language.h>
#include <combat.h>
#include <moving.h>
#include <defuel.h>



void create(){
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
  }

  mixed res;

  base::create();
  SetDefaultHome("/d/wald/gundur/hobbitdorf/schrein");
  SetDefaultPrayRoom("/d/wald/gundur/hobbitdorf/schrein");
  SetProp(P_ATTRIBUTES_OFFSETS,([A_DEX:4,A_CON:2]));
  SetProp(P_AVERAGE_SIZE, 105);
  SetProp(P_AVERAGE_WEIGHT, 60000);
  SetProp(P_MATERIAL_KNOWLEDGE,([MATGROUP_EATABLE:30, 
                                 MATGROUP_DRUG:30, 
                                 MATGROUP_POISONOUS:10]));
  SetProp(P_RESISTANCE_STRENGTHS,
	  ([ DT_TERROR : -0.1,
	   DT_MAGIC : -0.1,
	   DT_SOUND : 0.2,
	   DT_LIGHTNING : 0.1,
	   DT_POISON : 0.1 ]));
  SetProp(P_MAX_FOOD,250);
  SetProp(P_MAX_DRINK,100);
  SetProp(P_MAX_ALCOHOL,150);

  SetProp(P_SP_DELAY,HEAL_DELAY+1);
  SetProp(P_POISON_DELAY,POISON_DELAY-1);
  SetProp(P_FOOD_DELAY,FOOD_DELAY-2);
  SetProp(P_ALCOHOL_DELAY,ALCOHOL_DELAY+1);

  SetProp(P_MAGIC_RESISTANCE_OFFSET,
          ([ MT_ANGRIFF : 500,
	   MT_HELLSICHT : -500,
	   MT_PSYCHO : -500 ]));

  if(!(res=QueryProp(P_HANDS)) || !pointerp(res) || (sizeof(res)<3))
    res=({" mit pelzigen Haenden",25,({DT_BLUDGEON})});
  res[1]=25;
  SetProp(P_HANDS,res);
  SetProp(P_BODY,15);

  /* Groesse wird nur einmal gesetzt */
  if(!QueryProp(P_SIZE)){
    SetProp(P_SIZE,95+random(21));
    Set(P_SIZE,SAVE,F_MODE_AS);
  }

  /* Dito Gewicht */
  if(!QueryProp(P_WEIGHT) || (QueryProp(P_WEIGHT) == 75000)){
    SetProp(P_WEIGHT,50000+random(20001));
    if(QueryProp(P_GENDER)==FEMALE)
      SetProp(P_WEIGHT,QueryProp(P_WEIGHT)-5000);
    Set(P_WEIGHT,SAVE,F_MODE_AS);
  }
  SetProp(P_DEFUEL_LIMIT_FOOD,140);
  SetProp(P_DEFUEL_LIMIT_DRINK,50);
  SetProp(P_DEFUEL_TIME_FOOD,850);
  SetProp(P_DEFUEL_TIME_DRINK,450);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.8);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.6);

}

string _query_race()
{
  return "Hobbit";
}

string _query_real_race()
{
  return "Hobbit";
}

string _query_racedescr()
{
  return "Hobbits sind kleine Wesen, die am ehesten den Menschen aehneln.\n"+
    "Sie zeichnen sich trotz Ihrer Groesse durch ihren Mut und Standfestigkeit "+
	"aus.\nObwohl sie viel lieber zuhause vorm warmen Kamin sitzen, sind sie "+
	"immer\nfuer ein Abenteuer zu haben.\n";
}

int QueryAllowSelect() { return 1; }

string *_query_racestring(){
  if (QueryProp(P_GENDER) == FEMALE)
    return ({"Hobbitfrau","Hobbitfrau","Hobbitfrau","Hobbitfrau"});
  return ({"Hobbit","Hobbits","Hobbit","Hobbit"});
}

string _query_default_guild(){
  return "abenteurer";
}

void FinalSetup() {
  if(!present("pfeifchen",this_object()))
    clone_object("/items/pfeifchen")->move(this_object(),M_NOCHECK);
}

mixed RaceDefault(string arg)
{
  if (!arg)
    return 0;
  switch(arg)
  {
    case P_HANDS :
      return ({" mit pelzigen Haenden",25,({DT_BLUDGEON}) });
    case P_BODY :
      return 15;
  }
  return base::RaceDefault(arg);
}

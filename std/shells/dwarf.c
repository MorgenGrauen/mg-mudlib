// MorgenGrauen MUDlib
//
// shells/dwarf.c -- Dwarven Shell
//
// $Id: dwarf.c,v 3.14 2004/12/13 12:54:31 Zook Exp $

#pragma strong_types,save_types

inherit "std/player/base";

#include <properties.h>
#include <attributes.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>
#include <language.h>
#include <combat.h>
#include <defuel.h>


void create(){
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
  }

  mixed res;

  base::create();
  SetDefaultHome("/d/gebirge/room/zkapelle");
  SetDefaultPrayRoom("/d/gebirge/room/zkapelle");
  SetProp(P_ATTRIBUTES_OFFSETS,([A_STR:2,A_DEX:1,A_CON:3]));
  SetProp(P_SKILL_ATTRIBUTE_OFFSETS,([SA_DURATION:110]));
  SetProp(P_AVERAGE_SIZE,120);
  SetProp(P_AVERAGE_WEIGHT,75000);
  SetProp(P_MATERIAL_KNOWLEDGE,([MATGROUP_STONE:30,
                                 MATGROUP_METAL:30, 
                                 MATGROUP_PRECIOUS_METAL: 40, 
                                 MAT_GOLD:100]));
  SetProp(P_RESISTANCE_STRENGTHS,
	  ([ DT_FIRE : -0.2,
	   DT_WATER : 0.4 ]));
  SetProp(P_MAX_ALCOHOL,200);
  SetProp(P_MAX_FOOD,160);

  SetProp(P_SP_DELAY,HEAL_DELAY+1);
  SetProp(P_POISON_DELAY,POISON_DELAY+1);
  SetProp(P_FOOD_DELAY,FOOD_DELAY-1);
  SetProp(P_ALCOHOL_DELAY,ALCOHOL_DELAY-1);

  SetProp(P_MAGIC_RESISTANCE_OFFSET,
          ([ MT_ANGRIFF : 200,
	   MT_ILLUSION : -500,
           MT_BEHERRSCHUNG : 1000,
	   MT_VERWANDLUNG : 500 ]));

  if(!IS_SEER(this_object())){
    SetProp(P_MSGIN,"stapft herein");
    SetProp(P_MSGOUT,"stapft");
  }

  if(!(res=QueryProp(P_HANDS)) || !pointerp(res) || (sizeof(res)<3))
    res=({" mit blossen Haenden",35,({DT_BLUDGEON}) });
  res[1]=35;
  SetProp(P_HANDS,res);
  SetProp(P_BODY,10);

  /* Groesse wird nur einmal gesetzt */
  if(!QueryProp(P_SIZE)){
    SetProp(P_SIZE,110+random(21));
    Set(P_SIZE,SAVE,F_MODE_AS);
  }

  /* Dito Gewicht */
  if(!QueryProp(P_WEIGHT) || (QueryProp(P_WEIGHT) == 75000)){
    SetProp(P_WEIGHT,65000+random(20001));
    if(QueryProp(P_GENDER)==FEMALE)
      SetProp(P_WEIGHT,QueryProp(P_WEIGHT)-5000);
    Set(P_WEIGHT,SAVE,F_MODE_AS);
  }

  SetProp(P_DEFUEL_LIMIT_FOOD,70);
  SetProp(P_DEFUEL_LIMIT_DRINK,50);
  SetProp(P_DEFUEL_TIME_FOOD,535);
  SetProp(P_DEFUEL_TIME_DRINK,500);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.8);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.6);

}

string _query_race()
{
  return "Zwerg";
}

string _query_real_race()
{
  return "Zwerg";
}

string _query_racedescr()
{
  return
  "Zwerge sind kleine aber kraeftige Gebirgsbewohner, nicht sehr gespraechig,\n"
    +"leicht erzuernt, aber eine schlagkraeftige Unterstuetzung fuer ihre Freunde."
    +"\n"
    +"Ihr Mut und ihre Standfestigkeit ist weit und breit beruehmt, auch ihr\n"
    +"Geschick im Umgang mit Zwergenwaffen verleiht ihnen zusaetzliche Kraft.\n"
    +"Leider sind Zwerge nicht allzu schlau, sie verlassen sich lieber auf\n"
    +"ihre Kraft als auf ihr Gehirn.\n";
}

int QueryAllowSelect() { return 1; }

string *_query_racestring()
{
  if (QueryProp(P_GENDER)==2)
    return ({"Zwergin","Zwergin","Zwergin","Zwergin"});
  return ({"Zwerg","Zwerges","Zwerg","Zwerg"});
}
int _query_hp_delay(){
  int re;
  re = Query(P_HP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re--;
  else
    re++;
  return re;
}
int _query_sp_delay(){
  int re;
  re = Query(P_SP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re--;
  else
    re++;
  return re;
}

string _query_default_guild(){
  return "abenteurer";
}

mixed RaceDefault(string arg)
{
  if (!arg)
    return 0;
  switch(arg)
  {
    case P_HANDS :
      return ({" mit blossen Haenden",35,({DT_BLUDGEON}) });
    case P_BODY :
      return 10;
  }
  return 0;
}

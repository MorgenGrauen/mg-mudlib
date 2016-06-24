// MorgenGrauen MUDlib
//
// shells/elf.c -- Elven Shell
//
// $Id: elf.c 8928 2014-09-08 16:18:41Z Zesstra $

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



void create()
{
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
  }

  base::create();
  SetDefaultHome("/d/wald/room/es_mitte");
  SetDefaultPrayRoom("/d/wald/room/es_mitte");
  SetProp(P_AVERAGE_SIZE,195);
  SetProp(P_AVERAGE_WEIGHT,70000);
  SetProp(P_SKILL_ATTRIBUTE_OFFSETS,([SA_ENEMY_SAVE:110]));
  SetProp(P_ATTRIBUTES_OFFSETS,([A_INT:3,A_DEX:2,A_CON:1]));
  SetProp(P_MATERIAL_KNOWLEDGE,([MATGROUP_WOOD:30, 
                                 MATGROUP_HERBAL:30, 
                                 MATGROUP_LIVING:20]));

  SetProp(P_MAX_FOOD,80);
  SetProp(P_MAX_DRINK,150);
  SetProp(P_MAX_ALCOHOL,70);

  SetProp(P_SP_DELAY,HEAL_DELAY-1);
  SetProp(P_FOOD_DELAY,FOOD_DELAY+1);
  SetProp(P_DRINK_DELAY,DRINK_DELAY-2);
  SetProp(P_ALCOHOL_DELAY,ALCOHOL_DELAY+1);

  SetProp(P_MAGIC_RESISTANCE_OFFSET,
          ([ MT_ILLUSION : 800,
           MT_ANGRIFF : 200,
	   MT_VERWANDLUNG : 400,
	   MT_PSYCHO : 500 ]));

  // Elfen kriegen die Ebene der Wipfellaeufer per default. (Zu diesem
  // Zeitpunkt stehen in P_CHANNELS nur die default channel drin, wird dann
  // ueber das Einlesen des Savefiles ggf. ueberschrieben.)
  SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({"wipfellaeufer"}));

  if(!IS_SEER(this_object())){
    SetProp(P_MSGIN,"wandelt herein");
    SetProp(P_MSGOUT,"wandelt");
  }

  /* Groesse wird nur einmal gesetzt */
  if(!QueryProp(P_SIZE)){
    SetProp(P_SIZE,185+random(21));
    Set(P_SIZE,SAVE,F_MODE_AS);
  }

  /* Dito Gewicht */
  if(!QueryProp(P_WEIGHT) || (QueryProp(P_WEIGHT) == 75000)){
    SetProp(P_WEIGHT,60000+random(20001));
    if(QueryProp(P_GENDER)==FEMALE)
      SetProp(P_WEIGHT,QueryProp(P_WEIGHT)-5000);
    Set(P_WEIGHT,SAVE,F_MODE_AS);
  }
  SetProp(P_DEFUEL_LIMIT_FOOD,40);
  SetProp(P_DEFUEL_LIMIT_DRINK,20);
  SetProp(P_DEFUEL_TIME_FOOD,400);
  SetProp(P_DEFUEL_TIME_DRINK,200);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.4);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.35);

}

string _query_race()
{
  return "Elf";
}

string _query_real_race()
{
  return "Elf";
}

string _query_racedescr()
{
  return 
"\
Als Elfen bezeichnet man in der Regel jene hageren Hinterwaeldler, deren\n\
demonstratives Naturgehabe in der Regel nur durch ihre Liebe zu kitschigen\n\
Gedichten und ausschweifendem Geschlechtsleben in den Schatten gestellt wird.\n\
Einen Elf kann man im allgemeinen nicht nur an aeusseren Missbildungen\n\
(spitze Ohren, spindelduerre Gestalt, blonde Haare) erkennen, sondern auch\n\
an seiner aufdringlichen Art, ueber jeden und alles hemmungslos ins Gruene\n\
loszuphilosophieren.\n";

}

int QueryAllowSelect() { return 1; }

string *_query_racestring()
{
  if (QueryProp(P_GENDER)==2)
    return ({"Elfe","Elfe","Elfe","Elfe"});
  return ({"Elf","Elfen","Elf","Elf"});
}
int _query_hp_delay(){
  int re;
  re = Query(P_HP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re++;
  else
    re--;
  return re;
}
int _query_sp_delay(){
  int re;
  re = Query(P_SP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re++;
  else
    re--;
  return re;
}

string _query_default_guild(){
  return "wipfellaeufer";
}

static void FinalSetup()
{
  if(QueryProp(P_GUILD) != "chaos")
    SetProp(P_RESISTANCE_STRENGTHS,
            ([ DT_MAGIC : -0.2,
               DT_HOLY : 0.1,
               DT_UNHOLY : 0.3 ]));
  else
    SetProp(P_RESISTANCE_STRENGTHS,
            ([ DT_MAGIC : -0.2,
               DT_UNHOLY : 0.1,
               DT_HOLY : 0.3 ]));
}

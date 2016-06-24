// MorgenGrauen MUDlib
//
// shells/human.c -- Human Shell
//
// $Id: human.c 9022 2015-01-10 21:50:50Z Zesstra $

#pragma strong_types,save_types

inherit "/std/player/base";

#include <properties.h>
#include <attributes.h>
#include <moving.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>
#include <language.h>
#include <defuel.h>



void create() {
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
  }

  base::create();
  SetDefaultHome("/gilden/abenteurer");
  SetDefaultPrayRoom("/d/ebene/room/PortVain/pray_room");
  SetProp(P_ATTRIBUTES_OFFSETS,([A_INT:1,A_STR:1,A_CON:1,A_DEX:3]));
  SetProp(P_AVERAGE_SIZE,170);
  SetProp(P_AVERAGE_WEIGHT,75000);
  SetProp(P_MATERIAL_KNOWLEDGE,([MATGROUP_WOOD:20, 
                                 MATGROUP_METAL:20, 
                                 MATGROUP_ELEMENTAL:20, 
                                 MATGROUP_CLOTH:20]));

  SetProp(P_MAX_FOOD,120);
  SetProp(P_MAX_DRINK,120);
  SetProp(P_MAX_ALCOHOL,120);

  SetProp(P_MAGIC_RESISTANCE_OFFSET,
          ([ MT_ANGRIFF : 500,
	   MT_ILLUSION : 700,
           MT_BEHERRSCHUNG : 500,
	   MT_HELLSICHT : 1000,
	   MT_VERWANDLUNG : -500,
	   MT_PSYCHO : -500 ]));

  // Zukuenftig 0, nicht mehr -5. Ark, 04.01.08.
  SetProp(P_BODY,0);

  /* Groesse wird nur einmal gesetzt */
  if(!QueryProp(P_SIZE)){
    SetProp(P_SIZE,160+random(21));
    Set(P_SIZE,SAVE,F_MODE_AS);
  }

  /* Dito Gewicht */
  if(!QueryProp(P_WEIGHT) || (QueryProp(P_WEIGHT) == 75000)){
    SetProp(P_WEIGHT,65000+random(20001));
    if(QueryProp(P_GENDER)==FEMALE)
      SetProp(P_WEIGHT,QueryProp(P_WEIGHT)-5000);
    Set(P_WEIGHT,SAVE,F_MODE_AS);
  }
  SetProp(P_DEFUEL_LIMIT_FOOD,60);
  SetProp(P_DEFUEL_LIMIT_DRINK,50);
  SetProp(P_DEFUEL_TIME_FOOD,500);
  SetProp(P_DEFUEL_TIME_DRINK,245);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.75);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.5);

}

string _query_race()
{
  return "Mensch";
}

string _query_real_race()
{
  return "Mensch";
}

string *_query_racestring()
{
  if (QueryProp(P_GENDER)==2)
    return ({"Menschenfrau","Menschenfrau","Menschenfrau","Menschenfrau"});
  return ({"Mensch","Menschen","Mensch","Menschen"});
}

string _query_racedescr()
{
  return "Die Staerke des Menschen ist seine Vielseitigkeit.\n"+
    "Der Mensch kann zwar nichts besonders gut - dafuer aber eigentlich alles.\n";
}

int QueryAllowSelect() { return 1; }

void FinalSetup()
{
  // Im MG gibt fuer kleine Spieler eine Karte von Port Vain. Die gibt es
  // woanders meist nicht.
#if MUDNAME == "MorgenGrauen"
  if (QueryProp(P_LEVEL)<=3 && !present("portvainkarte",this_object()))
    clone_object("/d/ebene/obj/pv")->move(this_object(),M_NOCHECK);
#endif
}

string _query_default_guild(){
  return "abenteurer";
}

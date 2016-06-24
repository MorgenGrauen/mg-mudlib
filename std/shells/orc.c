#pragma strong_types,save_types

inherit "std/player/base";

#include <properties.h>
#include <attributes.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>
#include <language.h>
#include <combat.h>
#include <defines.h>
#include <defuel.h>

/*
 * Orks:
 * Orks sind eigentlich boese und blutruenstig, was auch oft genug zum 
 * Vorschein tritt :) Wenn ein Ork zu heftig forscht, quengelt er rum 
 * und weigert sich, bis er nicht wieder ein bisschen Blut verspritzt
 * hat,
*/ 
#define F_MAX 500
#define F_DEG 3 
#define NO_EXAMINE ({ \
    "Du knurrst: Ich will Blut, keine Bluemchen.", \
    "Du grummelst: Bin ich ein Forscher, oder was?" \
    })

static int f_cnt, f_deg;

int 
QueryFCnt() {
  return f_cnt;
}

int 
SetFCnt(int fc) {
  if(fc > -1 && fc < F_MAX)
    f_cnt = fc;
  return f_cnt;
}
  
int
QueryFDeg() {
  return f_deg;
}

void 
create() {
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
  }

  mixed res;

  base::create();

  f_cnt=0;
  f_deg=F_DEG;
  
  SetDefaultHome("/d/vland/morgoth/room/city/rathalle");
  SetDefaultPrayRoom("/d/vland/morgoth/room/city/c0606");
  SetProp(P_ATTRIBUTES_OFFSETS,([A_STR:3,A_INT:-1,A_CON:2]));
  /* Kleine aeh grosse Muskelpakete */
  SetProp(P_SKILL_ATTRIBUTE_OFFSETS,([SA_DAMAGE:110]));
  SetProp(P_AVERAGE_SIZE,195);
  SetProp(P_AVERAGE_WEIGHT,125000); // ziemlich schwer, viele Muskeln
  SetProp(P_MATERIAL_KNOWLEDGE,([MATGROUP_DEAD:60,
                                 MATGROUP_BIO:40, 
                                 MATGROUP_ELEMENTAL: 20, 
                                 MAT_BLOOD:100]));
  SetProp(P_CHANNELS, QueryProp(P_CHANNELS) + ({"Uruk-Hai"}));
  SetProp(P_RESISTANCE_STRENGTHS,
	  ([ 
     DT_FIRE : -0.2,
     DT_HOLY :0.3,
     DT_UNHOLY : -0.2,
	   DT_ACID : 0.2 ]));

  SetProp(P_MAX_FOOD,110);
  SetProp(P_MAX_DRINK,110);
  SetProp(P_MAX_ALCOHOL,150);
  SetProp(P_DEFUEL_LIMIT_FOOD,50);
  SetProp(P_DEFUEL_LIMIT_DRINK,70);
  SetProp(P_DEFUEL_TIME_FOOD,300);
  SetProp(P_DEFUEL_TIME_DRINK,400);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.4);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.35);

  /* SP regenerieren sie nich ganz so schnell, dafuer sind sie
   * ein bischen schneller bei Gift und Futter */
  SetProp(P_SP_DELAY,HEAL_DELAY+1);
  SetProp(P_POISON_DELAY,POISON_DELAY-1);
  SetProp(P_FOOD_DELAY,FOOD_DELAY-1);

  SetProp(P_MAGIC_RESISTANCE_OFFSET,
          ([ MT_ANGRIFF : 500,
        	   MT_ILLUSION : -250,
             MT_BEHERRSCHUNG : -250,
        	   MT_VERWANDLUNG : 500 ]));


  if(!(res=QueryProp(P_HANDS)) || !pointerp(res) || (sizeof(res)<3))
    res=({" mit starken Haenden",35,({DT_BLUDGEON, DT_RIP}) });
  SetProp(P_HANDS,res);
  /* Orks haben dicke Haut */
  SetProp(P_BODY,20);

  /* Groesse wird nur einmal gesetzt */
  if(!QueryProp(P_SIZE)){
    SetProp(P_SIZE,180+random(31));
    Set(P_SIZE,SAVE,F_MODE_AS);
  }

  /* Dito Gewicht */
  if(!QueryProp(P_WEIGHT) || (QueryProp(P_WEIGHT) == 75000)){
    SetProp(P_WEIGHT,100000+random(25001)+random(25001));
    Set(P_WEIGHT,SAVE,F_MODE_AS);
  }
}

string 
_query_race() {
  return "Ork";
}

string 
_query_real_race() {
  return "Ork";
}

string 
_query_racedescr() {
  return break_string("Ein Ork. Die brutale Macht des Boesen. Ein "
      "erbitterter Kaempfer, ohne Furcht vor dem Tod. So stellst Du "
      "Dir einen Ork vor.\nDiese Orks sehen vielleicht ausserlich so "
      "aus: Eine dicke und dunkle, lederartige Haut, die sicher "
      "einiges an Schlaegen abhaelt. Lange Eckzaehne, die sicher boese "
      "Wunden reissen koennen.\nDu spuerst, dass diese Orks hier ein "
      "wenig anders sind. Ihren Drang nach dem Blut ihrer Feinde scheinen "
      "sie recht gut unter Kontrolle zu haben, die meiste Zeit zumindest. "
      "Sie sind in der Lage, friedlich zwischen den anderen Rassen zu "
      "wandeln. Dennoch sind vor allem die Uruk-Hai Orks als Gegner nicht "
      "zu unterschaetzen, sollte es doch einmal zu einem Kampf kommen.",
      78,0,BS_LEAVE_MY_LFS);
}

int 
QueryAllowSelect() { 
  return 1; 
}

string 
*_query_racestring() {
  if (QueryProp(P_GENDER)==FEMALE)
    return ({"Orkin","Orkin","Orkin","Orkin"});
  return ({"Ork","Orkes","Ork","Ork"});
}

string 
_query_default_guild(){
  return "urukhai";
}

mixed 
RaceDefault(string arg) {
  if (!arg)
    return 0;
  switch(arg) {
    case P_HANDS :
      return ({" mit starken Haenden",35,({DT_BLUDGEON,DT_RIP}) });
    case P_BODY :
      return 25;
  }
  return base::RaceDefault(arg);
}

protected void heart_beat() {
  ::heart_beat();

  if(f_cnt > 0 && !--f_deg) {
    f_cnt--;
    f_deg=F_DEG;
  }
}

void
Attack(object enemy) {
  if(f_cnt > 0)
    f_cnt--;
  return ::Attack(enemy);
}

varargs int 
_examine(string str, int mode) {

  if(++f_cnt > F_MAX) {
    tell_object(this_object(),break_string(NO_EXAMINE[random(sizeof(NO_EXAMINE))],78));
    return 1;
  }
  return ::_examine(str,mode);
}

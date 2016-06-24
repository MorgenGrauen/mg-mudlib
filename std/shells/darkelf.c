// MorgenGrauen MUDlib
//
// shells/darkelf.c -- Darkelf Shell
//
// $Id: darkelf.c 8675 2014-02-18 20:39:54Z Zesstra $

#pragma strong_types,save_types

inherit "/std/player/base";

#include <properties.h>
#include <attributes.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>
#include <language.h>
#include <defines.h>
#include <combat.h>
#include <defuel.h>
#include <errord.h>


protected void create()
{
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
    set_next_reset(-1);
    return;
  }
  base::create();
  SetDefaultHome("/gilden/dunkelelfen");
  SetDefaultPrayRoom("/d/unterwelt/cadra/room/town/templemain");
  SetProp(P_AVERAGE_SIZE,175);
  SetProp(P_AVERAGE_WEIGHT,70000);
  SetProp(P_ALIGN, -500);
  SetProp(P_SKILL_ATTRIBUTE_OFFSETS,([SA_ENEMY_SAVE:110]));
  SetProp(P_ATTRIBUTES_OFFSETS,([A_INT:4,A_DEX:2]));
  SetProp(P_MATERIAL_KNOWLEDGE,([MATGROUP_UNHOLY: 100,
                                 MATGROUP_MAGIC:   70,
                                 MATGROUP_DEAD:    50,
                                 MATGROUP_METAL:   30]) );
  SetProp(P_RESISTANCE_STRENGTHS,
            ([ DT_HOLY :    0.25,
               DT_UNHOLY : -0.15,
               DT_TERROR : -0.05 ]));
  SetProp(P_MAX_FOOD,80);
  SetProp(P_MAX_DRINK,150);
  SetProp(P_MAX_ALCOHOL,70);

  SetProp(P_SP_DELAY, HEAL_DELAY-2); // dafuer regeneriert man im freien
  SetProp(P_HP_DELAY, HEAL_DELAY-1); // wirklich _nichts_
  SetProp(P_FOOD_DELAY,FOOD_DELAY+1);
  SetProp(P_DRINK_DELAY,DRINK_DELAY-2);
  SetProp(P_ALCOHOL_DELAY,ALCOHOL_DELAY+1);

  SetProp(P_MAGIC_RESISTANCE_OFFSET,
        ([ MT_ILLUSION    : 200,
           MT_ANGRIFF     : 500,
	   MT_BEHERRSCHUNG: 500,
	   MT_PSYCHO      : 800 ]));

  /* Groesse wird nur einmal gesetzt */
  if(!QueryProp(P_SIZE)) {
    SetProp(P_SIZE,150+random(50));
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
  SetProp(P_DEFUEL_LIMIT_DRINK,30);
  SetProp(P_DEFUEL_TIME_FOOD,400);
  SetProp(P_DEFUEL_TIME_DRINK,250);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.45);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.4);
}

static void FinalSetup()
{
   if (!QuerySkill(SK_NIGHTVISION))
      ModifySkill(SK_NIGHTVISION, 5000, 0, "ANY");
}

string _query_race()
// nicht static, da sie manchmal auch so aufgerufen wird...
{
  return "Dunkelelf";
}

string _query_real_race()
{
  return "Dunkelelf";
}

static string _query_racedescr()
{
	  return
"\
Das Volk der Dunkelelfen lebt in einer grossen Hoehlenstadt gut versteckt\n\
hinter einem Wasserfall. Ueber kaum ein anderes Volk gibt es soviele\n\
Vorurteile wie ueber die Dunkelelfen, und so werden sie von allen misstrauisch\n\
beaeugt oder sogar bekaempft. In diesem Kampf, insbesondere gegen die Elfen,\n\
sind sie voellig auf sich allein gestellt, und so hat sich eine mehr oder\n\
minder autarke Gesellschaft entwickelt. Die Dunkelelfen haben eine eigene\n\
Kultur und eine eigene Goettin, der sie huldigen. Wie auch die Elfen\n\
verfuegen sie ueber ausserordenlich grosse magische Faehigkeiten, auch wenn\n\
sie sich mehr auf die schwarze Seite der Magie spezialisiert haben.\n";
}

// int QueryAllowSelect() { return 0; }
// Aktiviert am 28.07.03, Ark.
int QueryAllowSelect() { return 1; }

string *_query_racestring()
{
  if (QueryProp(P_GENDER)==2)
    return ({"Dunkelelfe","Dunkelelfe","Dunkelelfe","Dunkelelfe"});
  return ({"Dunkelelf","Dunkelelfen","Dunkelelf","Dunkelelf"});
}

static string _query_default_guild()
{
  return "dunkelelfen";
}

static int sun_in_room(object room)
{
  if (!room) return 0;
  closure qp=symbol_function("QueryProp", room);
  int lt=funcall(qp, P_LIGHT_TYPE);
  // (lt & LT_SUN) ist hier zunaechst _testweise_ drin. Die Rasse wurde
  // anders genehmigt. Sollte das im MG ueberhand nehmen und jeder Keller
  // nun sonnendurchflutet sein, dann wird das wieder ausgebaut!
  // 27.06.04 Padreic
  return ( (funcall(qp, P_INT_LIGHT)>0) &&
          ((lt & LT_SUN) || ((lt==LT_MISC) && !funcall(qp, P_INDOORS))));
}

protected void heart_beat()
{
  ::heart_beat();
  if (sun_in_room(environment()) &&
      QueryProp("Dunkelelfen:Outdoor")<random(100) && !QueryProp(P_GHOST)) {
     int hp;
     hp=QueryProp(P_HP)-1;
     SetProp(P_HP, hp);
     if (hp<0) {
       tell_object(ME,
         "Das war wohl zuviel fuer Dich. Das naechste mal solltest Du Dich "
        +"wohl besser\ngegen die Sonne schuetzen.\n");
       SetProp(P_KILL_NAME,"Zuviel Sonne");
       do_damage(999,ME);
       SetProp(P_KILL_NAME,0);
     }
     else tell_object(ME, /* die Info musste irgendwie in eine Zeile */
           "Die Sonne scheint gnadenlos auf Dein Haupt und schwaecht Dich.\n");
  }
}

static int _query_no_regeneration()
{
  if (sun_in_room(environment()))
     return NO_REG;
  return Query(P_NO_REGENERATION);
}

int StdSkill_Nightvision(object me, string sname, mixed sinfo)
{
  int last, light;

  if (!sinfo || !environment()) return 0;
  if (intp(sinfo)) sinfo=([SI_SKILLABILITY:sinfo]);
  if (!mappingp(sinfo)) return 0;
  light=(QueryProp(P_PLAYER_LIGHT)<=0 ? -1 : 1);
  if (last=sinfo[SI_USR]) { // letztes Lichtlevel != 0
     if (light==last) {
       if (sinfo[SI_LASTLIGHT]<=time())
          return sinfo[SI_SKILLABILITY]+1;
       return -1;
     }
     else {
       last=( MAX_ABILITY - sinfo[SI_SKILLABILITY] );
       last=(last/1000) + (last%1000 > random(1001) ? 1 : 0);
       if (light<0) last/=2; // an Dunkelkeit schneller gewoehnen...
       if(!this_interactive() || this_interactive()==this_object())
       {
	       ModifySkill(sname, ([SI_USR: light, SI_LASTLIGHT: time()+last]),
        	           0, sinfo[SI_GUILD]);
       }
       if (last<=0)
          return sinfo[SI_SKILLABILITY]+1;
       return -1;
     }
  }
  else { // Startwert...
    if(!this_interactive() || this_interactive()==this_object())
    {
    	ModifySkill(sname, ([SI_USR: light, SI_LASTLIGHT: 0]), 0, sinfo[SI_GUILD]);
    }
    return sinfo[SI_SKILLABILITY]+1;
  }
  return 0;
}

varargs int CannotSee(int silent)
{
  string is_blind;

  if ( is_blind = QueryProp( P_BLIND ) ) {
     if (!silent) {
       if (stringp(is_blind)) write(is_blind);
       else write("Du bist blind!\n");
     }
     return 1;
  }
  if (environment() && (!IS_LEARNER(ME) || !Query(P_WANTS_TO_LEARN))) {
     if (QueryProp(P_GHOST)) {
        if (StdSkill_Nightvision(ME, SK_NIGHTVISION, QuerySkill(SK_NIGHTVISION))>0)
            return 0;
     }
     else if (UseSkill(SK_NIGHTVISION)>0) return 0;
     if (!silent) {
        if (QueryProp(P_PLAYER_LIGHT)<=0)
          write("Du kannst nichts sehen, da sich Deine Augen noch nicht an die Dunkelheit\n"
               +"gewoehnt haben!\n");
        else write("Du bist von dem hellen Licht total geblendet und Du musst Dich erst langsam\n"
                  +"daran gewoehnen.\n");
     }
     return 1;
  }
  return 0;
}


/*
 * 2003-11-05, Zook:
 *
 *   Temporaere Funktion, die den Delfen es ermoeglicht, einen
 *   moeglicherweise falschen Raum (Indoor/Outdoor) zu melden.
 *
 */

static int _indoorbug(string key)
{
  if (!stringp(key))
    key= "";

  ERRORD->LogReportedError(
      ([ F_PROG: "unbekannt",
         F_LINE: 0,
         F_MSG: "Sonnenfehler: " + key,
         F_OBJ: environment(this_object())
      ])
      );

  write("Du hast einen fehlerhaften Innen-/Aussen-/Sonnenlichtraum gemeldet.\n");

  return 1;
}

static mixed _query_localcmds()
{
  return ({ ({"sonnenfehler", "_indoorbug", 0, 0 }) })
            + base::_query_localcmds();
}


/*
 * Goblin-Rassenshell
 * [/std/shells/goblin.c]
 * (c) 2007 nibel@mg.mud.de
 *
 * Werte von Ark abgesegnet am 11.12.2007
 */

#pragma strong_types,save_types

#include <attributes.h>
#include <health.h>
#include <new_skills.h>
#include <properties.h>
#include <wizlevels.h>
#include <defuel.h>
#include <moving.h>

inherit "/std/player/base.c";

static varargs int GoblinCmdWaaagh(string arg);

public void create() {
  mixed res;
  base::create();

  SetDefaultHome("/d/wald/kessa/waaagh/room/starthut/hut[" + 
    getuid(this_object()) +"]");
  SetDefaultPrayRoom("/d/wald/nibel/lichtung/room/lichtung_45");
  
  SetProp(P_AVERAGE_SIZE, 80);
  SetProp(P_AVERAGE_WEIGHT, 32000);
  SetProp(P_MATERIAL_KNOWLEDGE, ([MATGROUP_EATABLE:20, MATGROUP_DRUG:40,
    MATGROUP_PRECIOUS_METAL:25, MATGROUP_JEWEL:25]));

  SetProp(P_BODY, 15);
  SetProp(P_ATTRIBUTES_OFFSETS, ([A_STR:0, A_INT:1, A_DEX:2, A_CON:2]));
  SetProp(P_RESISTANCE_STRENGTHS, ([DT_FIRE:-0.15, DT_LIGHTNING:-0.15,
    DT_SOUND:0.1, DT_HOLY:0.1, DT_AIR:0.1, DT_ACID:0.1]));
  SetProp(P_MAGIC_RESISTANCE_OFFSET, ([MT_ANGRIFF:600, MT_ILLUSION:500,
    MT_VERWANDLUNG:-300, MT_HELLSICHT:-750, MT_BEHERRSCHUNG:250]));
  SetProp(P_SKILL_ATTRIBUTE_OFFSETS, ([SA_ENEMY_SAVE:103, SA_DAMAGE:107]));

  SetProp(P_SP_DELAY, HEAL_DELAY + 2);
  SetProp(P_HP_DELAY, HEAL_DELAY - 1);
  SetProp(P_ALCOHOL_DELAY, ALCOHOL_DELAY - 1);

  switch(QueryProp(P_SHELL_VERSION)) {
    case 0:
      if(!QueryProp(P_SIZE)) // Maennlein und Weiblein sind gleich "gross"
        SetProp(P_SIZE, 75 + random(11));
      if(QueryProp(P_WEIGHT) == 75000) // Dito Gewicht
        SetProp(P_WEIGHT, (QueryProp(P_SIZE) * 390) +
          random(QueryProp(P_SIZE) * 10));
      SetProp(P_MATERIAL,([MAT_MISC_LIVING:100]));
          
      Set(P_SIZE, SAVE, F_MODE_AS);
      Set(P_WEIGHT, SAVE, F_MODE_AS);
      Set(P_MATERIAL, SAVE, F_MODE_AS);

      if(!pointerp(res = QueryProp(P_HANDS)) || sizeof(res) < 3)
        res = ({" mit kleinen Faeustchen", 30, ({ DT_BLUDGEON })});
      SetProp(P_HANDS, res);

      if(!IS_SEER(this_object())) {
        SetProp(P_MSGIN, "flitzt herein");
        SetProp(P_MSGOUT, "flitzt");
      }
      SetProp(P_SHELL_VERSION, 1);
    case 1:
      // /std/player/base setzt ja schon P_WEIGHT...
      if(QueryProp(P_WEIGHT) == 75000) {
        SetProp(P_WEIGHT, (QueryProp(P_SIZE) * 390) +
          random(QueryProp(P_SIZE) * 10));
      }
      SetProp(P_SHELL_VERSION, 2);
    default: break;
  }
  
  SetProp(P_MAX_FOOD, 110);
  SetProp(P_MAX_DRINK, 80);
  SetProp(P_MAX_ALCOHOL, 125);
  SetProp(P_DEFUEL_LIMIT_FOOD, 60);
  SetProp(P_DEFUEL_LIMIT_DRINK, 60);
  SetProp(P_DEFUEL_TIME_FOOD, 400);
  SetProp(P_DEFUEL_TIME_DRINK, 360);
  SetProp(P_DEFUEL_AMOUNT_FOOD, 0.5);
  SetProp(P_DEFUEL_AMOUNT_DRINK, 0.4);
}

static void FinalSetup() {
  object o;
  if(QueryProp(P_LEVEL) > 5 || present("\nibel:waldlichtungskarte",
    this_object())) return;
  if(!catch(o = clone_object("/d/wald/nibel/lichtung/obj/karte")))
    o->move(this_object(), M_NOCHECK);
}

public int QueryAllowSelect() { return 1; }

public string _query_race() { return "Goblin"; }
public string _query_real_race() { return "Goblin"; }

public string _query_racedescr() {
  return break_string("Goblins sind winzige, gruenhaeutige Wesen, sogar "
    "noch kleiner als Hobbits. An ihren zu dick geratenen Koepfchen "
    "befinden sich lange, selten reglose, Ohren und eine grosse, krumme "
    "Nase. Ihre kleine Statur sollte jedoch nicht taeuschen, denn ihre "
    "fehlende Kraft machen sie mit Geschwindigkeit, Praezision und nicht "
    "zuletzt ihrer unbestrittenen Ruchlosigkeit alleweil wett. Obwohl "
    "fuer sie Pluendern, lautes Herumbruellen und die gemeinsten Streiche "
    "spielen zum Alltag gehoert, wuerde sie niemand als boesartig "
    "bezeichnen. Denn Goblins sind vieles, aber sicherlich nicht die "
    "intelligentesten Kreaturen. Durch ihren zaehen Willen und die dicke, "
    "lederne Haut sind sie aussergewoehnlich widerstandsfaehig, und, "
    "sofern funkelnde Beute winkt, fuer jedes Abenteuer zu haben.", 78);
}

public string *_query_racestring() {
  if(QueryProp(P_GENDER) == FEMALE)
    return ({"Goblinfrau", "Goblinfrau", "Goblinfrau", "Goblinfrau"});
  return ({"Goblin", "Goblins", "Goblin", "Goblin"});
}

public string _query_default_guild() {return "abenteurer";}

public string _query_visible_guild() {
  switch(lower_case(QueryProp(P_GUILD))) {
    case "abenteurer": return "abentoira";
    case "wipfellaeufer": return "wiffelloifa";
    case "chaos": return "kaos";
    case "zauberer": return "zaubara";
    case "bierschuettler": return "biaschuettla";
    case "katzenkrieger": return "kaznkriega";
	  case "tanjian": return "tanschan";
	  case "klerus": return "klerikae";	  	
	  case "dunkelelfen": return "dunklelfn";
	  case "kaempfer": return "kaempfa";
	  case "karate": return "karatae";
	  case "werwoelfe": return "weawoelf";
	  case "magus": return "magia";
    case "urukhai": return "urugai";
  }
  return QueryProp(P_GUILD);
}

public mixed RaceDefault(string arg) {
  if(!arg) return 0;
  switch(arg) {
    case P_HANDS:
      return ({" mit kleinen Faeustchen", 30, ({  DT_BLUDGEON })});
    case P_BODY:
      return 15;
  }
  return base::RaceDefault(arg);
}

static mixed _query_localcmds() {
  return ({({"waaagh", "GoblinCmdWaaagh", 0, 0})}) +
    base::_query_localcmds();
}

// "knuddel alle" ist deutlich teurer also who cares :-)
static varargs int GoblinCmdWaaagh(string arg) {
  object *obs;
  string s, w;
  
  if(!objectp(environment())) return 0;
  obs = filter(all_inventory(environment()) - ({this_object()}), #'living);
  obs = obs - filter_objects(obs, "QueryProp", P_INVIS);
  // levelabhaengige Anzahl aaaaaaa's
  w = "W"+ sprintf("%'a'"+ (QueryProp(P_LEVEL) / 10 + 3) +"s", "aaa") +"gh!";

  foreach(object o : obs)
  {
    string str=(break_string(Name(WER) +" ballt die Faeustchen und "
      "kreischt laut: "+ w +"\n"
      + capitalize(o->QueryDu(WER)) + " zuckst erschrocken zusammen.",
      78, 0, BS_LEAVE_MY_LFS));

    int res=o->ReceiveMsg(str,MT_LISTEN,MA_EMOTE,0,this_object());
    if (res<0)
    {
      obs-=({o}); // unten nicht mehr mit anzeigen.
      if (res==MSG_SENSE_BLOCK)
        ReceiveMsg(o->Name(WER) +" kann Dich nicht hoeren.",
                   MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_STORE,
                   MA_EMOTE,0,this_object());
      else
        ReceiveMsg(o->Name(WER) +" ignoriert Dich oder diesen Befehl.",
                   MT_NOTIFICATION|MSG_DONT_IGNORE|MSG_DONT_STORE,
                   MA_EMOTE,0,this_object());
    }
  }
  int anzahl=sizeof(obs);
  if(!anzahl)
  {
    ReceiveMsg("Du ballst die Faeustchen und kreischst laut: "
               + w, MT_NOTIFICATION|MSG_DONT_IGNORE,MA_EMOTE,0,this_object());
  }
  else
  {
    s = CountUp(map_objects(obs, "name", WER));
    ReceiveMsg(break_string("Du ballst die Faeustchen und kreischst laut: "
               + w +"\n"+ capitalize(s) +" zuck"
               +(anzahl > 1 ? "en" : "t") +" erschrocken zusammen.",
               78, 0, BS_LEAVE_MY_LFS),
               MT_NOTIFICATION|MSG_DONT_STORE|MSG_DONT_IGNORE,
               MA_EMOTE,0,this_object());
  }
  return 1;
}


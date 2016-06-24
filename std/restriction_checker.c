// MorgenGrauen MUDlib
//
// restriction_checker.c -- Beschraenkungen in Gilden, Spells und Skills
//
// $Id: restriction_checker.c 8754 2014-04-26 13:12:58Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
//#pragma range_check
#pragma no_shadow

inherit "/std/util/executer";

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <properties.h>
#include <new_skills.h>
#include <wizlevels.h>
#include <questmaster.h>

#define DEBUG(x) if (find_player("zesstra")) tell_object(find_player("zesstra"),x)

// erweitert um das <pl> argument und die extra-Argument, die bereits in <fun>
// angegeben wurden. (*grummel*)
protected mixed execute_anything(mixed fun, object pl, varargs mixed args)
{
  //DEBUG(sprintf("ex_any(): Fun: %O, Ob: %O, args: %O\n",fun,pl,args));
  // wenn in fun schon extraargumente stehen, dann pl einfuegen und args
  // dranhaengen.
  if (pointerp(fun) && sizeof(fun) > 2)
  {
    // args kann ({}) sein, wenn keine Extraargumente uebergeben wurden, macht
    // aber nix.
    args = ({pl}) + fun[2..] + args;
    fun = fun[0..1];
    return ::execute_anything(fun, args...);
  }
  // s.o.
  return ::execute_anything(fun, pl, args...);
}

mapping race_modifier(object pl, mapping map_ldfied) {
  mapping rmap,help;

  if (mappingp(map_ldfied) && member(map_ldfied,SM_RACE) && objectp(pl) &&
      mappingp(rmap=map_ldfied[SM_RACE]))
//    if (mappingp(help=rmap[pl->_query_real_race()]))
    if (mappingp(help=rmap[pl->QueryProp(P_REAL_RACE)]))
        return map_ldfied+help;
    else if(member(rmap,"*") && mappingp(help=rmap["*"]))
        return map_ldfied+help;
  return map_ldfied;
}

string
check_restrictions(object pl, mapping restr) {
  /* Folgende Einschraenkungen koennen geprueft werden:
   * - Mindestwerte bei allen Attributen und Properties Level, QP, XP
   * - bestimmte Rassen koennen ausgeschlossen werden bzw. erlaubt werden
   *   (SR_EXCLUDE_RACE ist ein Array von ausgeschlossenen Rassen,
   *    SR_INCLUDE_RACE eines mit eingeschlossenen)
   * - SR_RACE kann ein Mapping sein, dass Rassen ein eigenes
   *   Restriction-mapping zuordnet. "*" steht fuer alle anderen Rassen.
   */
  closure cl,cl2;
  string race, guild;

  if (!mappingp(restr) || !sizeof(restr)) return 0; // Keine Einschraenkungen
  if (!objectp(pl)) return "";
  if (pl->QueryDisguise())
    return "Zieh erst mal den Tarnhelm aus.\n";

  restr=race_modifier(&pl,&restr);

  cl=symbol_function("QueryProp",pl);
  cl2=symbol_function("QueryAttribute",pl);

  foreach(string to_check, mixed condition: restr)
  {
    switch(to_check)
    {
      case P_LEVEL:
        if (funcall(cl,P_LEVEL) < condition)
          return "Deine Stufe reicht dafuer nicht aus.\n";
        break;
      case P_GUILD_LEVEL:
        if (funcall(cl,P_GUILD_LEVEL) < condition)
          return "Deine Gildenstufe reicht dafuer nicht aus.\n";
        break;
      case SR_SEER: // das macht nun wahrlich nur bei interactives sinn!!!
        if (condition && query_once_interactive(pl) && !IS_SEER(pl))
          return "Du musst dazu erst Seher werden.\n";
        break;
      case P_XP:
        if (funcall(cl,P_XP) < condition)
          return "Du hast nicht genuegend Erfahrung dafuer.\n";
        break;
      case P_QP:
        if (funcall(cl,P_QP) < condition)
          return "Du hast nicht genuegend Aufgaben geloest.\n";
        break;
      case P_ALCOHOL:
        if (funcall(cl,P_ALCOHOL) >= condition)
          return "Du bist zu besoffen.\n";
        break;
      case P_DRINK:
        if (funcall(cl,P_DRINK) >= condition)
          return "Du hast zuviel getrunken.\n";
        break;
      case P_FOOD:
        if (funcall(cl,P_FOOD) >= condition)
          return "Du hast zuviel gegessen.\n";
        break;
      case P_DEAF:
        if (funcall(cl,P_DEAF))
          return "Du kannst nichts hoeren.\n";
        break;
      case P_BLIND:
        if (funcall(cl,P_BLIND))
          return "Du kannst nichts sehen.\n";
        break;
      case P_FROG:
        if (funcall(cl,P_FROG))
          return "Als Frosch kannst Du das leider nicht.\n";
        break;
      case A_INT:
        if (funcall(cl2,A_INT) < condition)
          return "Deine Intelligenz reicht dafuer nicht aus.\n";
        break;
      case A_DEX:
        if (funcall(cl2,A_DEX) < condition)
          return "Deine Geschicklichkeit reicht dafuer nicht aus.\n";
        break;
      case A_CON:
        if (funcall(cl2,A_CON) < condition)
          return "Deine Ausdauer reicht dafuer nicht aus.\n";
        break;
      case A_STR:
        if (funcall(cl2,A_STR) < condition)
          return "Deine Staerke reicht dafuer nicht aus.\n";
        break;
      case SR_BAD:
        if (funcall(cl,P_ALIGN) < condition)
          return "Du bist zu boese.\n";
        break;
      case SR_GOOD:
        if (funcall(cl,P_ALIGN) > condition)
          return "Du bist nicht boese genug.\n";
        break;
      case SR_MIN_SIZE:
        if (funcall(cl,P_SIZE) < condition)
          return "Du bist dafuer zu klein.\n";
        break;
      case SR_MAX_SIZE:
        if (funcall(cl,P_SIZE) > condition)
          return "Du bist dafuer zu gross.\n";
        break;
      case SR_FREE_HANDS:
        if (condition > (funcall(cl,P_MAX_HANDS)-funcall(cl,P_USED_HANDS)))
          return "Du hast nicht genug freie Haende dafuer.\n";
        break;
      case SR_EXCLUDE_RACE:
        if (IS_LEARNER(pl))
          race=funcall(cl,P_RACE);
        else
          race=funcall(cl,P_REAL_RACE); //race=pl->_query_real_race();
        if ( pointerp(condition) && member(condition,race)>=0 )
          return ("Als "+race+" kannst Du das nicht.\n");
        break;
     case SR_INCLUDE_RACE:
        if (IS_LEARNER(pl))
          race=funcall(cl,P_RACE);
        else
          race=funcall(cl,P_REAL_RACE); //race=pl->_query_real_race();
        if (pointerp(condition) && member(condition,race)<0)
          return ("Als "+race+" kannst Du das nicht.\n");
        break;
     case SR_EXCLUDE_GUILD:
        guild=funcall(cl,P_GUILD);
        if (pointerp(condition) && member(condition,guild)>=0)
          return ("Mit Deiner Gildenzugehoerigkeit kannst Du das nicht.\n");
        break;
      case SR_INCLUDE_GUILD:
        guild=funcall(cl,P_GUILD);
        if (pointerp(condition) && member(condition,guild)<0)
          return ("Mit Deiner Gildenzugehoerigkeit kannst Du das nicht.\n");
        break;
     case SR_FUN:
        string res=execute_anything(condition, pl);
        if (stringp(res))
          return res;
        break;
     case SR_PROP:
        foreach ( string propname, mixed value, string msg: condition ) {
          if ( funcall(cl, propname) != value )
            return ( stringp(msg) && sizeof(msg) ? msg :
              "Koennte es sein, dass Du vergessen hast, etwas zu "
              "erledigen?\n");
        }
        break;
     case SR_QUEST:
        if (pointerp(condition))
        {
          // geloeste Quests aus der Liste der Bedingungen entfernen
          condition -= filter(condition, "QueryQuest", pl);
          if ( sizeof(condition) )
            return ("Du musst zuerst das Abenteuer '"+condition[0]+
              "' loesen.\n");
        }
        break;
     case SR_MINIQUEST:
        if (pointerp(condition))
        {
          closure unsolved_MQs = function int (mixed mq_num)
            { return !(QM)->HasMiniQuest(pl, mq_num); };
          if ( sizeof(filter(condition, unsolved_MQs)) )
            return "Du hast eine benoetigte Miniquest noch nicht geloest.\n";
            // Der Name der MQ wird absichtlich nicht explizit ausgegeben.
        }
        break;
    } // switch
  } // foreach
  return 0;
}

varargs mixed
GetData(string dname, mapping map_ldfied, object pl) {
  mixed dat,res;

  if (!dname || !mappingp(map_ldfied)) return 0;
  if (closurep(dat=map_ldfied[dname]) && (res=funcall(dat,pl,map_ldfied)))
        return res;
  return dat;

}

varargs int
GetValue(string vname, mapping map_ldfied, object pl) {
  mixed dat,res;

  // printf("GetValue(%O): %O\n",vname,map_ldfied);
  if (!vname || !map_ldfied) return 0;
  if ((dat=map_ldfied[vname]) && (res=execute_anything(dat,pl,map_ldfied)))
        return res;
  // printf("Value: %O\n",dat);
  return intp(dat) ? dat : 0;
}

varargs int
GetFactor(string fname, mapping map_ldfied, object pl) {
  mixed res;

  // printf("GetFactor(%O):\n",fname);
  if (!fname  || !(res=GetValue(FACTOR(fname),map_ldfied,pl)))
        return 100;
  if (res<10) res=10;
  else if (res>1000) res=1000;
  // printf("Factor: %O\n",res);
  return res;
}

varargs int
GetOffset(string oname, mapping map_ldfied, object pl) {
  mixed res;

  // printf("GetOffset(%O):\n",oname);
  if (!oname  || !(res=GetValue(OFFSET(oname),map_ldfied,pl)))
        return 0;
  if (res<-10000) res=-10000;
  else if (res>10000) res=10000;
  // printf("Offset: %O\n",res);
  return res;
}

varargs int
GetFValue(string vname, mapping map_ldfied, object pl) {
  return (GetValue(vname,map_ldfied,pl)*GetFactor(vname,map_ldfied,pl))/100;
}

varargs int
GetValueO(string vname, mapping map_ldfied, object pl) {
  return GetValue(vname,map_ldfied,pl)+GetOffset(vname,map_ldfied,pl);
}

varargs int
GetFValueO(string vname, mapping map_ldfied, object pl) {
  return ((GetValue(vname,map_ldfied,pl)*GetFactor(vname,map_ldfied,pl))/100
                  + GetOffset(vname,map_ldfied,pl));
}

varargs int
GetRandFValueO(string vname, mapping map_ldfied, object pl) {
  return (random(GetValue(vname,map_ldfied,pl)*GetFactor(vname,map_ldfied,pl))/100
          + GetOffset(vname,map_ldfied,pl));
}

mapping AddSkillMappings(mapping oldmap, mapping newmap) {
  mapping res,t1,t2;
  mixed *inx,ind;
  int i;

  if (!mappingp(oldmap)) return newmap;
  if (!mappingp(newmap)) return oldmap;
  inx=({SI_SKILLRESTR_LEARN,SI_SKILLRESTR_USE,SM_RACE});
  res=oldmap+newmap;
  for (i=sizeof(inx);i--;) {
        ind=inx[i];
        t1=oldmap[ind];
        t2=newmap[ind];
        if (t1) {
          if (t2)
                res[ind]=t1+t2;
          else
                res[ind]=t1;
        } else {
          if (t2)
                res[ind]=t2;
        }
  }
  return res;
}


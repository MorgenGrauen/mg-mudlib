// MorgenGrauen MUDlib
//
// inpc/select.c -- Beste Ausruestung suchen
//
// $Id: select.c 6998 2008-08-24 17:17:46Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include <thing.h>
#include <living.h>
#include <inpc.h>
#include <properties.h>
#include <combat.h>
#include <language.h>

#define ME this_object()

mapping scan_objects(mixed src) {
  // Ermittelt zu jedem Typ (Waffe, Ruestungstyp...) alle Objekte diesen Typs
  // Gesucht wird: - Im Inventory, falls Objekt angegeben
  //               - Im Array, falls Array angegeben
  object *obs;
  mapping res;
  mixed x;
  int i,cost,cost_limit;

  if (mappingp(src))
    return src;
  res=([]);
  if (objectp(src))
    src=all_inventory(src);
  if (!pointerp(src))
    src=({src});
  
  cost_limit = get_eval_cost()/3;
  foreach(object ob: src)
  {
    if ( (cost=get_eval_cost()) < cost_limit )
    {
      log_file("rochus/raeuber_eval",
        ctime()+"select::scan_objects(). Rest "+to_string(cost)+
        ", i="+to_string(i)+", Size "+to_string(sizeof(src))+".\n");
      return res;
      break;
    }
    if (!objectp(ob))
      continue;
    if (x=ob->QueryProp(P_ARMOUR_TYPE))
      ;
    else if (ob->QueryProp(P_WEAPON_TYPE))
      x=OT_WEAPON;
    else if (ob->QueryProp(P_COMBATCMDS))
      x=OT_COMBAT_OBJECT;
    else
      x=OT_MISC;
    if (!pointerp(obs=res[x]))
      obs=({});
    obs+=({ob});
    res[x]=obs;
  }
  return res;
}

mixed *find_object_by_type(mixed from, mixed typ) {
  // Findet all Objekte eines Typs, z.B. alle Waffen im Monster
  // <from> kann auch ein Mapping sein, das schon die mit scan_objects
  // erstellt Klassifikation enthaelt, damit diese nicht mehrfach
  // erstellt werden muss.
  mixed res;

  if (!mappingp(from))
    from=scan_objects(from);
  if (!mappingp(from) ||
      !pointerp(res=from[typ]))
    return ({});
  return res;
}

int eval_weapon(object ob) {
  return ob->QueryProp(P_WC);
}

object find_best_weapon(mixed from) {
  // Findet die beste Waffe
  int i,wc,bwc,cost,cost_limit;
  object *res,bob;

  if (!pointerp(res=find_object_by_type(from, OT_WEAPON)))
    return 0;
  bwc=-1;bob=0;
  
  cost_limit = get_eval_cost()/3;
  for (i=sizeof(res);i--;)
  foreach(object ob: res)
  {
    if (!objectp(ob)) continue;
    wc=eval_weapon(ob);
    if (wc>bwc)
    {
      bob=ob;
      bwc=wc;
    }
    
    if ( (cost=get_eval_cost()) < cost_limit )
    {
      log_file("rochus/raeuber_eval",
        "Break in select::find_best_weapon(). Rest-Ticks "+to_string(cost)+
        ", i = "+to_string(i)+", Size "+to_string(sizeof(res))+".\n");
      return bob; // zurueckgeben, was bisher drinsteht.
      break;
    } 
  }
  return bob;
}

varargs int wield_best_weapon(mixed from) {
  // Zueckt die beste Waffe.
  // Sollte mit command("zuecke_beste_waffe") aufgerufen werden,
  // damit this_player() richtig gesetzt wird.
  object ob,old;

  // Einige NPC moegen keine Waffen. Zum Beispiel Karate-Gilden-NPC
  // Durch Setzen von INPC_DONT_WIELD_WEAPONS kann man das Zuecken verhindern
  if(QueryProp(INPC_DONT_WIELD_WEAPONS)) return 0;

  if (!from)
    from=ME;
  
  if (!objectp(ob=find_best_weapon(from)))
    return 0;
  if (objectp(old=QueryProp(P_WEAPON)) && old!=ob) {
    old->RemoveId(INPC_BEST_WEAPON_ID);
    old->DoUnwield();
  }
  if (!ob->id(INPC_BEST_WEAPON_ID))
    ob->AddId(INPC_BEST_WEAPON_ID);

  return ob->DoWield();
}

int eval_armour(object ob) {
  return ob->QueryProp(P_AC);
}

object find_best_armour(mixed from, mixed typ) {
  // Findet die beste Ruestung eines Typs
  int i,ac,bac;
  object *res,bob;
  
  if (!pointerp(res=find_object_by_type(from, typ)))
    return 0;
  bac=-1;
  bob=0;
  foreach(object ob: res)
  {
    if (!objectp(ob)) continue;
    ac=eval_armour(ob);
    if (ac>bac) 
    {
      bob=ob;
      bac=ac;
    }
  }
  return bob;
}

object *find_best_armours(mixed from) {
  // Findet die besten Ruestungen, die gleichzeitig getragen werden koennen
  mapping ol;
  object *res,ob;
  mixed *types;
  int i;
  
  if (!mappingp(ol=scan_objects(from)))
    return ({});
  if (!pointerp(res=ol[AT_MISC]))
    res=({});
  types=m_indices(ol);
  foreach(object typ: types)
  { 
    if (VALID_ARMOUR_CLASS[typ]) // anderer Armour-Typ ausser AT_MISC?
    {
      ob=find_best_armour(from,typ);
      if (objectp(ob))
	res+=({ob});
    }
  }
  return res;
}

varargs int wear_best_armours(mixed from) {
  // Die besten Ruestungen werden angezogen
  // Sollte mit command("trage_beste_ruestungen") aufgerufen werden,
  // damit this_player() richtig gesetzt wird.
  object *na,*oa,*diff;
  int i,cost,cost_limit;
  
  if (!from)
    from=ME;
  if (!pointerp(na=find_best_armours(from)))
    return 0;
  if (!pointerp(oa=QueryProp(P_ARMOURS)))
    oa=({});
  diff=oa-na;
  cost_limit = get_eval_cost()/3;
  foreach(object di: diff)
  {
    di->RemoveId(INPC_BEST_SHIELD_ID);
    di->DoUnwear();
    if ( (cost=get_eval_cost()) < cost_limit )
    {
      log_file("rochus/raeuber_eval",
        ctime()+"Break 1 in select::wear_best_armours(). Rest "+
	to_string(cost)+", i="+to_string(i)+", Size "+
	to_string(sizeof(diff))+".\n");
      return 1; // zurueckgeben, was bisher drinsteht.
      break;
    }
  }
  diff=na-oa;
  cost_limit = get_eval_cost()/3;
  foreach(object di: diff)
  {
    if ( di->QueryProp(P_ARMOUR_TYPE)==AT_SHIELD
         && !di->id(INPC_BEST_SHIELD_ID))
     di->AddId(INPC_BEST_SHIELD_ID);
     di->do_wear("alles");
     if ( (cost=get_eval_cost()) < cost_limit )
     {
       log_file("rochus/raeuber_eval",
         ctime()+"Break 2 in select::wear_best_armours(). Rest "+
	 to_string(cost)+", i="+to_string(i)+", Size "+
	 to_string(sizeof(diff))+".\n");
      return 1; // zurueckgeben, was bisher drinsteht.
      break;
    }
  }
  return 1;
}

int eval_combat_object(object ob, mapping vals, object enemy) {
  return 0;
}

varargs string
find_best_combat_command(mixed from, object enemy, mapping pref) {
  // Sucht den guenstigsten Befehl zur Benutzung einer Sonderwaffe heraus
  object *obs;
  mixed *ind,y,vul;
  mapping x;
  string best;
  int i,j,max,val,mhp;
  
  if (!from)
    from=ME;
  if (!enemy)
    enemy=SelectEnemy();
  if (!mappingp(pref)) // bevorzugte Eigenschaften
    pref=([C_MIN:5,
	   C_AVG:10,
	   C_MAX:2,
	   C_HEAL:10
	   ]);
  best=0;max=-1;
  if (!pointerp(obs=find_object_by_type(from,OT_COMBAT_OBJECT)))
    return best;
  mhp=QueryProp(P_MAX_HP)-QueryProp(P_HP);
  if (objectp(enemy))
    vul=enemy->QueryProp(P_RESISTANCE_STRENGTHS);
  if (mhp<0) mhp=0;
  foreach(object ob: obs)
  {
    if (!objectp(ob)) 
      continue;
    if (!mappingp(x=ob->QueryProp(P_COMBATCMDS)))
      continue;
    ind=m_indices(x);
    for (j=sizeof(ind)-1;j>=0;j--)
    {
      if (!stringp(ind[j])) continue;
      y=x[ind[j]];
      if (mappingp(y)) 
      {
	if (val=y[C_HEAL]) 
	{
	  if (val>mhp) val=mhp; // Nur MOEGLICHE Heilung beruecksichtigen
	  val*=pref[C_HEAL];
	} 
	else 
	{
	  val=y[C_MIN]*pref[C_MIN]
	    + y[C_AVG]*pref[C_AVG]
	    + y[C_MAX]*pref[C_MAX];
	  // auskommentiert, da eval_resistance() bisher nicht implementiert
	  // ist. Zesstra, 06.08.2007
	  //if (y[C_DTYPES] && vul) // Vulnerability des Gegners bedenken...
	  //  val=(int)(((float)val)*(1.0+eval_resistance(y[C_DTYPES],vul)));
	}
      } 
      else 
      {
	val=1;
      }
      val+=eval_combat_object(obs[i],y,enemy);
      if (val<max) continue;
      max=val;
      if (mappingp(y) && y[C_HEAL]>0)
        best=sprintf(ind[j],name(RAW)); // Heilung auf sich selbst
      else if (objectp(enemy))
        best=sprintf(ind[j],enemy->name(RAW)); // Schaden auf Gegner
    }
  }

  return best;
}

varargs int use_best_combat_command(mixed enemy, mixed from, mapping pref) {
  // Fuehrt moeglichst guten Kampfeinsatz mit einer Sonderwaffe aus
  string str;
    
  if (stringp(enemy) && environment())
    enemy=present(enemy,environment());
  if (str=find_best_combat_command(from,enemy,pref))
    return command(str);
  return 0;
}

void add_select_commands() {
  add_action("wield_best_weapon","zuecke_beste_waffe");
  add_action("wear_best_armours","trage_beste_ruestungen");
  add_action("use_best_combat_command","benutze_beste_sonderwaffe");
}

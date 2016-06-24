// MorgenGrauen MUDlib
//
// inpc/eval.c -- Einschaetzen von Gegnern
//
// $Id: eval.c 6371 2007-07-17 22:46:50Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <living.h>
#undef NEED_PROTOTYPES
#include <properties.h>
#include <thing.h>
#include <inpc.h>

varargs void SetEvalFactor(mixed what, int fac, int off) {
  mapping offs,facs;

  if (!mappingp(facs=QueryProp(P_EVAL_FACTORS))) facs=([]);
  if (!mappingp(offs=QueryProp(P_EVAL_OFFSETS))) offs=([]);
  facs[what]=fac;
  offs[what]=off;
  SetProp(P_EVAL_FACTORS,facs);
  SetProp(P_EVAL_OFFSETS,offs);
}

void SetDefaultEvalFactors() {
  SetEvalFactor(P_HP,150,-60);
  SetEvalFactor(P_TOTAL_WC,250,0);
  SetEvalFactor(P_TOTAL_AC,1000,-50);
  SetEvalFactor("abenteurer_pfeil",3,0);
  SetEvalFactor("abenteurer_feuerball",8,0);
}

varargs int
modify_eval(object ob, mixed what, mapping fac, mapping off, int val) {
  if (!val) val=QueryProp(what);
  val-=off[what];
  if (val<0) val=0;
  val=(val*fac[what])/10000;
  if (val>200) val=200;
  return val;
}

varargs int eval_enemy(object enemy, mapping fac, mapping off) {
  int res;
  string gilde;
  
  if (!objectp(enemy)) return 0;
  if (!mappingp(fac)) fac=QueryProp(P_EVAL_FACTORS);
  if (!mappingp(fac)) fac=([]);
  if (!mappingp(off)) off=QueryProp(P_EVAL_OFFSETS);
  if (!mappingp(off)) off=([]);

  res=0;
  res+=modify_eval(enemy,P_TOTAL_WC,fac,off);
  res+=modify_eval(enemy,P_TOTAL_AC,fac,off);
  res+=modify_eval(enemy,P_HP,fac,off);
  if (stringp(gilde=enemy->QueryProp(P_GUILD)))
    res+=call_other(this_object(),"eval_guild_"+gilde,enemy,fac,off,gilde);
  if (res<0) res=0;
  if (res>1000) res=1000;
  return res;
}

varargs mixed *eval_enemies(mapping fac, mapping off, object *here) {
  int i,sz;
  mixed *res;

  res=({});
  if (!here)
    here=PresentEnemies();
  if (!pointerp(here) || !(sz=sizeof(here)))
    return res;
  if (!mappingp(fac)) fac=QueryProp(P_EVAL_FACTORS);
  if (!mappingp(fac)) fac=([]);
  if (!mappingp(off)) off=QueryProp(P_EVAL_OFFSETS);
  if (!mappingp(off)) off=([]);

  for (i=sz-1;i>=0;i--)
    res+=({({here[i],eval_enemy(here[i],fac,off)})});
  return res;
}

varargs int sum_eval_enemies(mapping fac, mapping off, object *here) {
  int res,i;
  mixed *evals,x,y;

  res=0;
  if (!pointerp(evals=eval_enemies(fac,off,here)))
    return res;
  for (i=sizeof(evals)-1;i>=0;i--)
    if (pointerp(x=evals[i]) && sizeof(x)>=2 && intp(y=x[1]))
      res+=y;
  return res;
}

varargs object *minimize_prop_filt(object *here, mixed prop) {
  object *obs,ob;
  int i,mhp,hp,sz;
  
  obs=0;
  if (!pointerp(here))
    here=PresentEnemies();
  if (!prop)
    prop=P_HP;
  for (i=sizeof(here)-1;i>=0;i--) {
    if (objectp(ob=here[i])) {
      hp=ob->QueryProp(prop);
      if (!pointerp(obs) || hp<mhp) {
	obs=({ob});
	mhp=hp;
      } else if (hp==mhp) {
	obs+=({ob});
      }
    }
  }
  if (!pointerp(obs))
    obs=({});
  return obs;
}

varargs object *maximize_prop_filt(object *here, mixed prop) {
  object *obs,ob;
  int i,mwc,wc,sz;
  
  obs=0;
  if (!pointerp(here))
    here=PresentEnemies();
  if (!prop)
    prop=P_TOTAL_WC;
  for (i=sizeof(here)-1;i>=0;i--) {
    if (objectp(ob=here[i])) {
      wc=ob->QueryProp(prop);
      if (!pointerp(obs) || wc<mwc) {
	obs=({ob});
	mwc=wc;
      } else if (wc==mwc) {
	obs+=({ob});
      }
    }
  }
  if (!pointerp(obs))
    obs=({});
  return obs;
}

varargs object *player_filt(object *here) {
  object *obs,ob;
  int i;
  
  obs=({});
  if (!pointerp(here))
    here=PresentEnemies();
  for (i=sizeof(here)-1;i>=0;i--)
    if (objectp(ob=here[i]) && query_once_interactive(ob))
      obs+=({ob});
  return obs;
}

varargs object select_enemy_min_prop(object *here, mixed prop) {
  object *obs;
  int sz;

  if (pointerp(obs=minimize_prop_filt(here,prop)) && (sz=sizeof(obs)))
    return obs[random(sz)];
  return 0;
}

varargs object select_enemy_max_prop(object *here, mixed prop) {
  object *obs;
  int sz;

  if (pointerp(obs=maximize_prop_filt(here,prop)) && (sz=sizeof(obs)))
    return obs[random(sz)];
  return 0;
}

varargs object select_player_min_prop(object *here, mixed prop) {
  return select_enemy_min_prop(player_filt(here),prop);
}

varargs object select_player_max_prop(object *here, mixed prop) {
  return select_enemy_max_prop(player_filt(here),prop);
}

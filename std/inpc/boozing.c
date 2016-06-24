// MorgenGrauen MUDlib
//
// inpc/boozing.c -- Intelligentes Saufen
//
// $Id: boozing.c 8396 2013-02-18 21:56:37Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing.h>
#include <living.h>
#include <inpc.h>
#include <container/moneyhandler.h>
#include <properties.h>

#define DRINK_IDS 1
#define DRINK_COST 2
#define DRINK_HEAL 3
#define DRINK_ALC 4
#define DRINK_SAT 5

int Drink() {
  mixed drinks;
  object env;
  int i,max,mon,hdiff;
 
  if ((mon=QueryMoney())<=0
      || !(env=environment())
      || !pointerp(drinks=env->query_drink())
      || (hdiff=QueryProp(P_MAX_HP)-QueryProp(P_HP))<=0)
    return 0;
  max=-1;
  for (i=sizeof(drinks)-1;i>=0;i--) {
    if (drinks[i][DRINK_COST]>mon
        || ((drinks[i][DRINK_ALC]>0) &&
	    ((drinks[i][DRINK_ALC]+QueryProp(P_ALCOHOL))
	     > (100-QueryProp(P_I_HATE_ALCOHOL))))
        || drinks[i][DRINK_SAT]+QueryProp(P_DRINK)>100
        || drinks[i][DRINK_HEAL]<=0)
      continue;
    if (max<0
        || (drinks[i][DRINK_HEAL]>drinks[max][DRINK_HEAL] &&
            drinks[max][DRINK_HEAL]<hdiff)
        || (drinks[i][DRINK_HEAL]>=hdiff &&
            drinks[i][DRINK_COST]<drinks[max][DRINK_COST]))
      max=i;
  }
  if (max<0) return 0;
  command("bestell "+drinks[max][DRINK_IDS][0]);
  return 1;
}

void DrinkLoop() {
  while (remove_call_out("DrinkLoop")>=0);
  if (!Drink())
    return;
  call_out("DrinkLoop",0);
}


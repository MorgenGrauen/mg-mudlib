// MorgenGrauen MUDlib
//
// inpc/nobank.c -- Nieder mit Bankzweities!
//
// $Id: nobank.c 8966 2014-11-19 21:41:12Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/living/moneyhandler";

#include <properties.h>
#include <attributes.h>
#include <money.h>
#define ME this_object()

int DeepQueryMoney(object ob) {
  int res,i;
  object *obs;
  
  if (!ob) return 0;
  res=0;obs=deep_inventory(ob);
  for (i=sizeof(obs)-1;i>=0;i--)
    if ((ob=obs[i]) && load_name(ob)==GELD)
      res+=ob->QueryProp(P_AMOUNT);
  return res;
}

int is_bank(object ob) {
  int *stats,geld;
  
  if (!ob || !query_once_interactive(ob)) return 0;
  stats=({ob->QueryAttribute(A_STR),
	  ob->QueryAttribute(A_INT),
	  ob->QueryAttribute(A_DEX),
	  ob->QueryAttribute(A_CON),
	  0});
  stats[4]=stats[1]+stats[2]+stats[3];
  geld=DeepQueryMoney(ob);
  
  if (stats[0]>=10
      && stats[4]<stats[0]
      && geld>5000
      && ob->QueryProp(P_XP)<300000
      && ob->QueryProp(P_QP)<300)
    return geld;
  return 0;
}

int DeepTransferMoney(object pl, int amount) {
  object *obs,ob;
  int act,trans,i;
  
  if (!pl || amount<=0)
    return 0;
  if (pl->QueryMoney()>=amount) {
    pl->AddMoney(-1*amount);
    trans=amount;
  } else {
    obs=deep_inventory(pl);trans=0;
    for (i=sizeof(obs)-1;i>=0;i--) {
      if ((ob=obs[i]) && load_name(ob)==GELD) {
	act=ob->QueryProp(P_AMOUNT);
	if (act<=0) continue;
	if (act>=amount) { // mehr Geld als benoetigt?
	  ob->SetProp(P_AMOUNT,act-amount); // abziehen
	  trans+=amount; 
	  amount=0; // nichts mehr benoetigt
	  break;    // also auch ende
	}
	amount-=act;
	trans+=act;
	ob->remove(); // abziehen was da ist
      }
    }
  }
  log_file("stolen_money",
	   sprintf("%s %O: %O (%O)\n",ctime(time())[4..15],pl,trans,ME));
  AddMoney(trans);
  return trans;
}

varargs int TestAndTransfer(object ob, int trans, string msg) {
  int geld,abzug;

  if ((geld=is_bank(ob))<=0) return 0;
  if (trans<0)
    abzug=(geld*trans)/-100; // Prozentsatz
  else
    abzug=trans;             // fester Abzug
  if (abzug>geld) abzug=geld;
  if (abzug<=0) return 0;
  abzug=DeepTransferMoney(ob,abzug);
  if (abzug<=0) return 0;  
  if (msg && ob)
    tell_object(ob,sprintf(msg,abzug));
  return abzug;
}

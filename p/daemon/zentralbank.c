// MorgenGrauen MUDlib
//
// zentralbank.c -- Zentrale Geld-Verwaltung der Haendler im MG
// $Id: zentralbank.c 9223 2015-05-27 21:46:58Z Zesstra $
//
/*
 * $Log: zentralbank.c,v $
 * Revision 1.5  2003/12/30 11:17:34  Vanion
 * query_prevent_shadow fehlte. Danke fuer den Hinweis an Ogo.
 *
 * Revision 1.4  2003/12/30 11:12:02  Jof
 * Ich hab Dir das File mal geklaut.
 * Vanion
 *
 * Revision 1.3  1995/07/10  07:31:04  Jof
 * typo fixed
 *
 * Revision 1.2  1995/07/10  07:27:59  Jof
 * Use bank.h
 * _query_current_money
 *
 * Revision 1.1  1995/07/05  16:29:26  Jof
 * Initial revision
 *
 */
#pragma no_shadow
#pragma no_clone
#pragma no_inherit
#pragma strong_types,save_types

#include <wizlevels.h>
#include <properties.h>
#include <money.h>

#define NEED_PROTOTYPES
#include <bank.h>

#define TIME strftime("%H:%M",time())
#define LS 1000000

int allmoney,bank_default_percent,store_default_percent,shop_default_percent;
int falling;
// Geldmengen: in NPC, in Boersen, rumliegen, auf Seherkarten, Gesamtsumme,
// Zeitpunkt der Berechnung.
int *summen = ({0,0,0,0,0,0});

nosave int last_save;
nosave int counter;

void save_me()
{
  save_object("/p/daemon/save/zentralbank");
  last_save=time();
}

protected void create()
{
  seteuid(getuid());
  allmoney=500000;
  bank_default_percent=BANK_DEFAULT_PERCENT;
  store_default_percent=STORE_PERCENT_LEFT;
  shop_default_percent=SHOP_PERCENT_LEFT;
  restore_object("/p/daemon/save/zentralbank");
}

private void adjust_percents()
{
  if (allmoney<50000   && falling>=0) falling=-1;
  if (allmoney>1000000 && falling<=0) falling=1;
  if (allmoney<200000 && falling>0 ) falling=0;
  if (allmoney>80000  && falling<0 ) falling=0;
  if (falling<0 && !((++counter)%5))
  {
    bank_default_percent=bank_default_percent+random(3)+1;
    store_default_percent=store_default_percent+random(3)+1;
    shop_default_percent=shop_default_percent+random(3)+1;
    log_file("ZENTRALBANK",
	sprintf("%s: PERCENTS SET TO BANK %d STORE %d SHOP %d\n",
	  TIME,bank_default_percent,store_default_percent,
	  shop_default_percent),LS);
  }
  if (falling>0)
  {
    bank_default_percent/=2;
    store_default_percent/=2;
    shop_default_percent/=2;
    log_file("ZENTRALBANK",
	sprintf("%s: PERCENTS SET TO BANK %d STORE %d SHOP %d\n",
	  TIME,bank_default_percent,store_default_percent,
	  shop_default_percent),LS);
  }
  if (bank_default_percent<1) bank_default_percent=1;
  if (bank_default_percent>90) bank_default_percent=90;
  if (store_default_percent<1) store_default_percent=1;
  if (store_default_percent>90) store_default_percent=90;
  if (shop_default_percent<1) shop_default_percent=1;
  if (shop_default_percent>90) shop_default_percent=90;
  save_me();
}

void reset()
{
  adjust_percents();
}

private void real_log()
{
  log_file("ZBANKSTATUS",sprintf("%s: %d\n",dtime(time()),allmoney),LS);
}

private void log_state()
{
  // nur alle 15min nen Status-Logeintrag. Reicht voellig.
  if (find_call_out(#'real_log) == -1)
    call_out(#'real_log,900);
}

public varargs void PayIn(int amount, int percent)
{
  if (amount<=0) return;
  percent|=bank_default_percent;
  allmoney+=amount*percent/100;
  log_file("ZENTRALBANK",
      sprintf("%s: Einzahlung: %d (%d brutto) von %O\n",
	TIME,amount*percent/100,amount,previous_object()),LS);
  log_state();
}

public int WithDraw(int amount)
{
  int got;

  if (amount<=0) return 0;
  if (allmoney<0) allmoney=0;
  if (!allmoney)
    got=0;
  else
  {
    if (amount*3<allmoney)
      got=amount;
    else
    {
      got=allmoney/3;
      if (!got) got=1;
    }
    allmoney-=got;
  }
  log_file("ZENTRALBANK",sprintf("%s: Abhebung: %6d/%6d von %O\n",
	TIME,got, amount,previous_object()),LS);
  log_state();
  return got;
}

int _query_current_money()
{
  return allmoney;
}

int _query_bank_default_percent()
{
  return bank_default_percent;
}

int _query_shop_percent_left()
{
  return shop_default_percent;
}

int _query_store_percent_left()
{
  return store_default_percent;
}

// Geldmenge im Spiel
public int *geldmenge() {
  
  if (summen[5] > time()-3600 || !IS_ELDER(this_player()))
      return summen;

  summen = allocate(6);
  object *geld = clones(GELD,2);
  foreach(object ob: geld) {
    if (!environment(ob) || IS_LEARNER(environment(ob)))
      continue;
    if (environment(ob)->QueryProp(P_NPC))
      // in NPC
      summen[0]+=ob->QueryProp(P_AMOUNT);
    else if (load_name(environment(ob)) == BOERSE)
      // in boersen
      summen[1]+=ob->QueryProp(P_AMOUNT);
    else
      // sonst rumliegend
      summen[2]+=ob->QueryProp(P_AMOUNT);
  }
  // Seherkarten
  geld = clones(SEHERKARTE,2);
  mapping cards=m_allocate(sizeof(geld),1);
  foreach(object ob: geld) {
    cards[ob->query_owner()] = ob->QueryProp(P_AMOUNT);
  }
  foreach(string owner, int amount: cards)
    summen[3]+=amount;
  summen[4]=summen[0]+summen[1]+summen[2]+summen[3];
  summen[5]=time();
  return summen;
}

public void PrintGeldmenge() {
  int *sum = geldmenge();
  printf("NPC: %d\n"
         "Boersen: %d\n"
         "rumliegend: %d\n"
         "Seherkarten: %d\n"
         "Gesamt: %d\n"
         "Zeit: %s\n",
         sum[0],sum[1],sum[2],sum[3],sum[4],strftime(sum[5]));
}

void set_percents(int store,int shop,int bank)
{
  store_default_percent=store;
  shop_default_percent=shop;
  bank_default_percent=bank; 
  log_file("ZENTRALBANK",
      sprintf("%s: PERCENTS SET TO BANK %d STORE %d SHOP %d\n",
	TIME,bank_default_percent,store_default_percent,
	shop_default_percent),LS);
}

int remove()
{
  save_me();
  destruct(this_object());
  return 1;
}
   

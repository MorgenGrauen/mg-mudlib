// MorgenGrauen MUDlib
//
// trading_price.c -- Preisverhalten von Laeden und Haendlern
//
// $Id: trading_price.c,v 3.3 2002/06/09 19:51:40 Tilly Exp $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <bank.h>

#undef NEED_PROTOTYPES
#include <properties.h>

// TODO: langfristig zu private aendern?
nosave int buyfact, shop_percent_left, mymoney, wantto, last;
private nosave int wantthresh, maxnum;

varargs int SetTradingData(int money, int factor, int maxobs)
{
  if (extern_call() && previous_object() != this_object())
    log_file("ZENTRALBANK",sprintf("INITIALIZE: %O got (%d,%d,%d) from %O (%O)\n",
				   this_object(), money,factor,maxobs,
				   previous_object(),this_player()));

  if (money < 1000)
    money = 1000;

  last=wantto=mymoney=money;
  wantthresh=wantto/2;
  if (wantthresh < 5000)
    wantthresh = 5000;

  if (factor < 100)
    buyfact = 300;
  else
    buyfact = factor;
  
  if (maxobs < 1)
    maxnum=3;
  else
    maxnum=maxobs;
  
  return mymoney;
}

void SetBuyFact(int i)
{
  buyfact=i*100;
}

varargs int QueryBuyFact(object client)
{
  return buyfact;
}

int InflateSellValue(int value, int cnt)
{
  cnt-=maxnum;
  if (cnt<0) cnt=0;
  return value-(value*cnt)/(cnt+25);
}

int TruncateSellValue(int value)
{
  if (value > 500) {
    value-=500;
    value = ((1000*value)/(value+1000))+500;
  }
  return value;
}

int ObjectCount(object ob)
{
  return objectp(ob); // 0 oder 1
}

varargs int QueryValue(object ob, int value, object client)
{
  value=TruncateSellValue(InflateSellValue(value,ObjectCount(ob)));
  if (3*value>mymoney)
  {
    log_file("LADEN",sprintf("Out of money: %O for %O (%s)\n",this_object(),
			     this_player(),dtime(time())[5..]));
    value=mymoney/3;
  }
  if (value<0) value=0;
  
  /*   Preisverfall von beschaedigtenObjekten gleich hier festlegen  */

  if(value && ob->QueryProp(P_DAMAGED))  
  {                                      
    if(ob->QueryProp(P_WC) || ob->QueryProp(P_AC))
    {
      value = (6 * (value / 10));
    }
    else
    {
      value = (4 * (value / 10));
    }
  }
                                          
  return value;
}

varargs int QuerySellValue(object ob, object client)
{
  return QueryValue(ob,ob->QueryProp(P_VALUE),client);
}

varargs int QueryBuyValue(mixed ob, object client)
{
  // Mit Runden.
  return (ob->QueryProp(P_VALUE)*QueryBuyFact(client) + 50)/100;
}

protected void create()
{
  SetProp(P_SHOP_PERCENT_LEFT, 0);
  SetTradingData(50000,300,3);
}

protected void create_super() {
  set_next_reset(-1);
}

void reset()
{
  SetProp(P_SHOP_PERCENT_LEFT, 0);

  if (last/3>mymoney)
    wantto=wantto*110/100;
  else
    if (last/2<mymoney)
      wantto=wantto*85/100;
  if (wantto<wantthresh) wantto=wantthresh+500;
  if (wantto>1500000) wantto=1500000;
  if (wantto<mymoney)
  {
    ZENTRALBANK->PayIn(mymoney-wantto);
    mymoney=wantto;
  } else
    mymoney+=ZENTRALBANK->WithDraw(wantto-mymoney);
  last=mymoney;
}

void _add_money(int i) {
  if (extern_call() && previous_object() != this_object())
    log_file("ZENTRALBANK",sprintf("%s: TRANSFER: %O got %d from %O\n",
	strftime("%H:%M"), this_object(), i, previous_object()));
  mymoney+=i;
}

int _set_shop_percent_left(int dummy) {
  return shop_percent_left=ZENTRALBANK->_query_shop_percent_left();
}

int _query_shop_percent_left() {
  return shop_percent_left;
}

int _query_current_money() {
  return mymoney;
}

int _query_wantto() {
  return wantto;
}



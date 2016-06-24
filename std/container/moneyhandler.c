// MorgenGrauen MUDlib
//
// container/moneyhandler.c -- money handler for container
//
// $Id: moneyhandler.c 6738 2008-02-19 18:46:14Z Humni $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES

#include <properties.h>
#include <moving.h>
#include <money.h>

public int AddMoney( int amount )
{
  object ob;
  int ret;

  if ( !amount )
    return 1;
    
  ob = clone_object( GELD );
  ob->SetProp( P_AMOUNT, amount );

  ret=ob->move( this_object(), M_PUT|M_MOVE_ALL );
  // Bei fehlerhaftem move ggf. wieder zerstoeren.
  if ((ret != MOVE_OK)
      && ob)
  {
    ob->remove(1);
  }
  return ret;
}

public int QueryMoney()
{
  object money;
  int geld;

  if ( money = present_clone(GELD, this_object()) )
    geld = money->QueryProp(P_AMOUNT);

  if ( money = present(GELDBOERSE_MIT_GELD, this_object()) )
    geld += money->QueryProp(P_AMOUNT);

  return geld;
}

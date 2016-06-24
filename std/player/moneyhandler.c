// MorgenGrauen MUDlib
//
// player/moneyhandler.c -- money handler for players
// Nur noch aus Kompatibilitaetsgruenden vorhanden
//
// $Id: moneyhandler.c 9051 2015-01-11 20:28:00Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/container/moneyhandler";

#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES
#include <container/moneyhandler.h>
#include <wizlevels.h>
#include <properties.h>
#include <money.h>
#include <moving.h>

public int AddMoney( int amount )
{
  object ob;

  if ( !amount )
    return 1;

  int ret = moneyhandler::AddMoney(amount);

  // ggf. noch loggen
  if ( ret == MOVE_OK
       && objectp(ob = find_object("/p/daemon/moneylog"))
       && amount > 0
       // dieses muss leider drinbleiben, weil viele nicht-Spieler dieses
       // erben
       && query_once_interactive(this_object())
       && !IS_WIZARD(this_object())
       && !Query(P_TESTPLAYER) )
    ob->AddMoney( previous_object(), amount );

  return ret;
}

public int QueryMoney()
{
  object money = present(SEHERKARTEID_AKTIV, this_object());
  // zusaetzlich zu den anderen Geldquellen auch noch die Seherkarte pruefen.
  if (money)
    return moneyhandler::QueryMoney() + money->QueryProp(P_AMOUNT);

  return moneyhandler::QueryMoney();
}


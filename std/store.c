// MorgenGrauen MUDlib
//
// store.c -- Das Lager eines Ladens
//
// $Id: store.c 7100 2009-02-01 09:24:43Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define ANZ 3
#include <properties.h>
#include <bank.h>
#include <defines.h>

inherit "/std/thing/properties";
inherit "/std/room/items";

private nosave object *all;
private nosave mapping all2;
private nosave object shop;
private nosave int sum;
private nosave int store_percent_left;

static int _set_store_percent_left()
{
  return store_percent_left=ZENTRALBANK->_query_store_percent_left();
}

public void _register_shop(object ob)
{
  shop=ob;
}

protected void create()
{
  seteuid(getuid());
  properties::create();
  items::create();
  _set_store_percent_left();
  SetProp(P_MIN_STOCK, 20); // immer eine Reserve fuer unsere Anfaenger...
}

protected void create_super() {
      set_next_reset(-1);
}

public int MayAddObject(object ob)
{   return 1;   }

protected varargs void remove_multiple(int limit, mixed fun) {}

static int _query_store_consume()
// da 0 = 20 ist, ist kein default initialisieren im create noetig!!!
{
  int consum;
  consum=100-Query(P_STORE_CONSUME);
  if (consum<0 || consum >=100) // consum = 100, wenn P_STORE_CONSUM = 0
    return 70;
  return consum;
}

static int _set_store_consume(int consum)     /* 1 <= consum <= 100 */
{
  if (consum<1 || consum>100) return -1;
  return (100-(int)Set(P_STORE_CONSUME, consum));
}

static int _query_min_stock()
{
  int stock;
  stock=Query(P_MIN_STOCK);
  if (stock<0) return 0;
  return stock;
}

private void update_money() {
  if (sum) {
    if (!shop)
      ZENTRALBANK->PayIn(sum);
    else
      shop->_add_money(sum);
  }
  sum=0;
}

protected void RemoveObjectFromStore(object ob) {
  // Alle Funktionen die ausserhalb aufgerufen werden, werden "gecatcht"
  catch(sum+=ob->QueryProp(P_VALUE)*store_percent_left/100; publish);
  catch(ob->remove(); publish);
  if (ob) destruct(ob);  // Objekt auf jeden Fall zerstoeren
}

protected void aufraeumen() {
  int i, size;
  object ob;
  string element;

  if (!pointerp(all)) return;
  if (!mappingp(all2)) all2=([]);
  size=sizeof(all);
  for (i=(size<=50 ? 0 : size-50); i<size; i++) {
    if (!objectp(ob=all[i])) continue;
    if (object_name(ob)[0..2]=="/d/" || object_name(ob)[0..8]=="/players/")
      element=BLUE_NAME(ob);
    else
      catch(element=ob->short()+BLUE_NAME(ob); publish);
    if (all2[element]++>ANZ)
      RemoveObjectFromStore(ob);
  }
  if (size<=50) {
    all=0;  // Speicher freigeben...
    all2=0;
    update_money();
  } else {
    all=all[0..size-51];
    call_out(#'aufraeumen,2);
  }
}

void reset() {
  int i, to, stock;
  mixed *itemlist;
  
  items::reset();
  _set_store_percent_left();

  if (!(all=all_inventory()) || !sizeof(all)) {
    all=0; // Speicher freigeben
    return;
  }
  if (sizeof(itemlist=QueryProp(P_ITEMS))) {
    itemlist=filter(itemlist, #'[, 0);
    all-=itemlist;
    if (!sizeof(all)) {
      all=0; // Speicher freigeben
      return;
    }
  }

  i=sizeof(all)-1;
  to=i*QueryProp(P_STORE_CONSUME)/100;
  if ( to < (stock=QueryProp(P_MIN_STOCK)) )
    to=stock;
  else
    stock=0;

  // Hinterer Teil des Inventories wird zerstoert, also alle aelteren
  // und somit vermutlich selten gekaufte Objekte
  for (;i>=to;i--)
    RemoveObjectFromStore(all[i]);
  all2=([]);
  call_out(#'aufraeumen,random(10));
  update_money();
}

//dieser Raum wird sich nie per clean_up() zerstoeren, daher geben wir
//0 zurueck, damit der Driver clean_up() nicht mehr ruft.
int clean_up(int refcount) { return 0; }

static int _query_current_money()
{
  return sum;
}


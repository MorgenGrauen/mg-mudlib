/*
 * thing.c fuer Objekte, die sicherheitsrelevant sind.
 * Sollte von Dingen aus /secure oder Magiertools auf jeden
 * Fall statt /std/thing inherited werden.
 *
 * Der Unterschied zu /std/thing besteht darin, potentiell unsichere
 * Auswertungen wie process_string() (@@fun@@) zu verhindern - ansonsten
 * koennte jemand Magiern Code unterschieben.
 *
 */
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma no_shadow
#pragma range_check
#pragma pedantic

inherit "/std/thing";
#include <properties.h>

// int secure_level() // ist nun in simul_efun

varargs string long(int mode)
{
  return funcall(QueryProp(P_LONG));
}

string short()
{
  string sh;
  if( sh=QueryProp(P_SHORT) )
    return funcall(sh)+".\n";
}


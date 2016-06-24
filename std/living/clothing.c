// MorgenGrauen MUDlib
//
// living/clothing.c -- Modul fuer Bekleidungsfragen. ;-)
//
// $Id: armour.c,v 3.8 2003/08/25 09:36:04 Rikus Exp $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES
#include <living/clothing.h>
#include <living/combat.h>
#include <armour.h>

protected void create() {
  SetProp(P_CLOTHING, ({}));
  Set(P_CLOTHING, PROTECTED, F_MODE_AS);
}

public object *FilterClothing(closure filterfun, varargs mixed* extra) {
  if (!closurep(filterfun))
    return ({});
  return apply(#'filter, QueryProp(P_CLOTHING), filterfun, extra);
}

public object *FilterArmours(closure filterfun, varargs mixed* extra) {
  if (!closurep(filterfun))
    return ({});
  return apply(#'filter, QueryProp(P_ARMOURS)-({0}), filterfun, extra);
}

public int WearClothing(object ob) {
  object *clothing = QueryProp(P_CLOTHING);
  if (member(clothing, ob) != -1)
    return 0;
  clothing += ({ob});
  SetProp(P_CLOTHING, clothing);
  return 1;
}

public int WearArmour(object ob) {
  if (!VALID_ARMOUR_TYPE(ob->QueryProp(P_ARMOUR_TYPE)))
    return 0;

  object *armours = QueryProp(P_ARMOURS);
  if (member(armours, ob) != -1)
    return 0;

  armours += ({ob});
  SetProp(P_ARMOURS, armours);
  return 1;
}

public int Wear(object ob) {
  // reihenfolge ist wichtig! Ruestung sind _auch_ Kleidung, aber Kleidung
  // keine Ruestung.
  if (ob->IsArmour())
    return WearArmour(ob);
  else if (ob->IsClothing())
    return WearClothing(ob);
  return 0;
}

public int UnwearClothing(object ob) {
object *clothing = QueryProp(P_CLOTHING);
  if (member(clothing, ob) == -1)
    return 0;
  clothing -= ({ob});
  SetProp(P_CLOTHING, clothing);
  return 1;
}

public int UnwearArmour(object ob) {
  object *armours = QueryProp(P_ARMOURS);
  if (member(armours, ob) == -1)
    return 0;

  armours -= ({ob});
  SetProp(P_ARMOURS, armours);
  return 1;
}

public int Unwear(object ob) {
  // reihenfolge ist wichtig! Ruestung sind _auch_ Kleidung, aber Kleidung
  // keine Ruestung.
  if (ob->IsArmour())
    return UnwearArmour(ob);
  else if (ob->IsClothing())
    return UnwearClothing(ob);
  return 0;
}


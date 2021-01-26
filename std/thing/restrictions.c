// MorgenGrauen MUDlib
//
// thing/restrictions.c -- Gewicht u.ae. eines Objekts
//
// $Id: restrictions.c 6809 2008-03-29 21:54:04Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone

#include <defines.h>
#include <properties.h>

#define NEED_PROTOTYPES 
#include <thing/properties.h>

// Gewicht ist 1kg, wenn nicht anders bestimmt
protected void create() 
{  
  SetProp(P_WEIGHT, 1000);
}

protected void create_super() {
  set_next_reset(-1);
}

static int _set_weight(int weight)
{
  // Content-Cache invalidieren.
  object env = this_object();
  while ( objectp(env = environment(env)) )
      // Ja. Man ruft die _set_xxx()-Funktionen eigentlich nicht direkt auf.
      // Aber das Gewichtssystem ist schon *so* rechenintensiv und gerade der
      // P_LAST_CONTENT_CHANGE-Cache wird *so* oft benoetigt, dass es mir
      // da um jedes bisschen Rechenzeit geht.
      // Der Zweck heiligt ja bekanntlich die Mittel. ;-)
      //
      // Tiamak
      env->_set_last_content_change();

  // P_TOTAL_WEIGHT bei Nicht-Containern auf P_WEIGHT umleiten
  SetProp(P_TOTAL_WEIGHT, weight);
  return Set(P_WEIGHT, weight, F_VALUE);
}

// P_X_ATTR_MOD aendern (Attributaenderungen durch Ausruestung)
static mapping _set_extern_attributes_modifier(mapping xmod) 
{
  mapping res;

  // wenn Muell oder ein leeres Mapping uebergeben wurde, wird die prop
  // geloescht und ggf. das Objekt im Living abgemeldet.
  if (!mappingp(xmod)) xmod=([]);
  
  if (!sizeof(xmod)) {
 
    res=Set(P_X_ATTR_MOD, xmod);
  
    // wenn Prop geloescht wird, kann man das Item auch aus der Liste der
    // Statmodifizierer des Lebewesens austragen.
    if (living(environment())) {  
      environment()->deregister_modifier(ME);
      environment()->UpdateAttributes();
    }
  }
  else {
    // ok, Prop wird auf nen interessanten Wert gesetzt. ;-)

    // Damit Insert/RemoveSensitiveObject beim Move aufgerufen wird,
    // muss P_SENSITIVE gesetzt sein
    if (!QueryProp(P_SENSITIVE)) SetProp(P_SENSITIVE,({}));
  
    if (living(environment()) && 
        (!mappingp(res=QueryProp(P_X_ATTR_MOD)) || !sizeof(res))) {
      // Wenn dieses Objekt in einem Living ist und bisher die prop nicht
      // gesetzt war, muss sich das Objekt noch im Living als Statmodifizierer
      // registrieren.
      res=Set(P_X_ATTR_MOD, xmod);
      environment()->register_modifier(ME);
      environment()->UpdateAttributes(ME);
    }
    else if (living(environment())){
      // sonst reicht ein einfaches UpdateAttributes() um das Lebewesen zu
      // informieren.
      res=Set(P_X_ATTR_MOD, xmod);
      environment()->UpdateAttributes();
    }
    else {
      // wenn kein lebendes Env, reicht einfaches Setzen.
      res=Set(P_X_ATTR_MOD,xmod);
    }
  }

  return res;
}

// P_X_HEALTH_MOD aendern (LP/KP-Aenderung durch Ausruestung)
static mapping _set_extern_health_modifier(mapping xmod) {
  mapping res;

  // wenn Muell oder ein leeres Mapping uebergeben wurde, wird die prop
  // geloescht und ggf. das Objekt im Living abgemeldet.
  if (!mappingp(xmod)) xmod=([]);
  
  if (!sizeof(xmod)) {
 
    res=Set(P_X_HEALTH_MOD, xmod);
  
    // wenn Prop geloescht wird, kann man das Item auch aus der Liste der
    // Statmodifizierer des Lebewesens austragen.
    if (living(environment())) {  
      environment()->deregister_modifier(ME);
      environment()->UpdateAttributes();
    }
  }
  else {
    // ok, Prop wird auf nen interessanten Wert gesetzt. ;-)

    // Damit Insert/RemoveSensitiveObject beim Move aufgerufen wird,
    // muss P_SENSITIVE gesetzt sein
    if (!QueryProp(P_SENSITIVE)) SetProp(P_SENSITIVE,({}));
  
    if (living(environment()) && 
        (!mappingp(res=QueryProp(P_X_HEALTH_MOD)) || !sizeof(res))) {
      // Wenn dieses Objekt in einem Living ist und bisher die prop nicht
      // gesetzt war, muss sich das Objekt noch im Living als Statmodifizierer
      // registrieren.
      res=Set(P_X_HEALTH_MOD, xmod);
      environment()->register_modifier(ME);
      environment()->UpdateAttributes(ME);
    }
    else if (living(environment())){
      // sonst reicht ein einfaches UpdateAttributes() um das Lebewesen zu
      // informieren.
      res=Set(P_X_HEALTH_MOD, xmod);
      environment()->UpdateAttributes();
    }
    else {
      // wenn kein lebendes Env, reicht einfaches Setzen.
      res=Set(P_X_HEALTH_MOD,xmod);
    }
  }

  return res;

}

// P_X/M_ATTR_MOD werden nicht beruecksichtigt, da CUMULATIVE_ATTR_LIMIT
// ueberschritten ist
void NotifyXMAttrModLimitViolation()
{
}
  

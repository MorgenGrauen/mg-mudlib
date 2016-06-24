// MorgenGrauen MUDlib
//
// armour/wear.c -- armour standard object
//
// $Id: combat.c 6243 2007-03-15 21:10:21Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/clothing/wear";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <living/combat.h>
#include <living/attributes.h>
#include <language.h>
#include <defines.h>
#include <armour.h>

// Uebernimmt viele wesentliche Eigenschaften aus der Kleidung, daher werden
// hier auch nur div. Funktionen ueberschrieben, die sich leicht
// unterschiedlich zur Kleidung verhalten.

// Globale Variablen
private nosave int     logged;

protected void create() {
  ::create();
  // P_DAMAGED laesst sich zwar (noch) von aussen setzen, aber bitte nur ueber
  // die hier definierte Setmethode.
  // TODO: Direktes Setzen von P_DAMAGED entfernen.
  Set(P_DAMAGED, PROTECTED, F_MODE_AS);
}

/* Wenn eine Ruestung vom gleichen Typ im Array ist, gebe diese zurueck. */
private object TestType(object *armours) {
    mixed type;

    // Den eigenen Typ feststellen
    type = QueryProp(P_ARMOUR_TYPE);

    // Zerstoerte Objekte aussortieren
    armours-=({0});

    foreach(object armour: armours)
    {
      if (type==(armour->QueryProp(P_ARMOUR_TYPE)))
        {
            // Ruestung vom gleichen Typ gefunden -> zurueckgeben
            return armour;
        }
    }
    // Keine Ruestung vom gleichen Typ gefunden, also 0 zurueckgeben
    return 0;
}

// liefert 1 zurueck, wenn der Spieler die ruestung anziehen darf, 0 sonst.
protected int _check_wear_restrictions(int silent, int all) {
  mixed   type,res;
  object  *armours;

  type = QueryProp(P_ARMOUR_TYPE);

  // Das DoWear() der Kleidung prueft auf genuegend freie Haende,
  // prueft aber den Fall nicht ab, ob man hier ein Schild hat, was laut Prop
  // weniger als eine Hand belegt. Daher hier nachholen und korrigieren. Und
  // direkt mal loggen, damit das gefixt wird.
  if ( (type==AT_SHIELD) && QueryProp(P_NR_HANDS)<1 ) {
    SetProp(P_NR_HANDS,1);
    log_file("INVALID_SHIELD",
	sprintf("Fixme: AT_SHIELD item without valid P_NR_HANDS: %O\n",
	  object_name()));
  }

  armours=(object*)PL->QueryProp(P_ARMOURS) - ({0});

  // Von jedem Ruestungstyp ausser AT_MISC kann man immer nur ein
  // Teil tragen
  if ( (type!=AT_MISC) && (res=TestType(armours)) && objectp(res)) {
    msg(break_string(sprintf(
	  "Du traegst bereits %s als Schutz der Sorte %s.",
	  res->name(WEN,1), type),78,
	  (all?(Name(WER)+": "):0)), all);
    return(-1);
  }

  // Ruestungen vom Typ AT_ILLEGAL oder solche mit einem fuer ihren
  // Ruestungstyp zu hohen Schutzwert koennen nicht angezogen werden
  if ( (type==AT_ILLEGAL) || (QueryProp(P_AC)>VALID_ARMOUR_CLASS[type])) {
    write("Ungueltiger Ruestungstyp, bitte Erzmagier verstaendigen.\n");
        "/p/daemon/ruestungen"->RegisterArmour();      
    return(-2);
  }

  // Ruestungen, die ein oder mehrere Attribut veraendern und gegen
  // das gesetzte Limit verstossen, haben keine Wirkung bezueglich der
  // Attribute. Dies gibt aber nur ne Meldung aus, angezogen werden darf sie
  // trotzdem.
  if (mappingp(res=QueryProp(P_M_ATTR_MOD)) && PL->TestLimitViolation(res) ) {
    write(break_string(sprintf(
          "Irgendetwas an Deiner Ausruestung verhindert, dass Du Dich mit "
          "%s so richtig wohl fuehlst.",name(WEM,1)),78,
          (all?(Name(WER)+": "):0)));
  }

  // dann mal aus der Kleidung geerbte Pruefungen ausfuehren und Ergebnis
  // liefern.
  return ::_check_wear_restrictions(silent,all);
}

protected void _informwear(int silent, int all) {

  // Ruestungen koennen Resistenzen beeinflussen
  PL->AddResistanceModifier(QueryProp(P_RESISTANCE_STRENGTHS),
                            QueryProp(P_ARMOUR_TYPE));

  // Ruestungen koennen Attribute aendern/blockieren. Also muessen diese
  // nach dem Anziehen aktualisiert werden
  PL->register_modifier(ME);
  PL->UpdateAttributes();

  // P_TOTAL_AC im Traeger updaten (fuer Query()s)
  PL->QueryProp(P_TOTAL_AC);

  // Alle Ruestungen werden im awmaster registriert, sobald sie von
  // einem Spieler gezueckt werden
  if (!logged && query_once_interactive(PL)) {
    call_other("/secure/awmaster","RegisterArmour",ME);
    logged=1;
  }
  // noch das aus der Kleidung rufen, damit die Anziehmeldungen auch kommen.
  // ausserdem laeuft das Anstosses von InformWear() von dort.
  ::_informwear(silent,all);
}


protected int _check_unwear_restrictions(object worn_by, int silent, 
                                            int all) {
  // liefert >=0 zureck, wenn die Kleidung/Ruestung ausgezogen werden kann, 
  // <0 sonst.

  // Schilde belegen (mindestens) eine Hand. Hngl. Wenn man diesen bloeden
  // Check nicht machen muesste, koennte man sich die ganze Funktion sparen.
  // Hmpfgrmpfl. Achja, raise_error(), weil das eigentlich ja nicht vorkommen
  // sollte und gefixt werden soll. Das geht naemlich nur, wenn jemand diese
  // Prop geaendert, waehrend der Spieler das Schild getragen hatte.
  if ( (QueryProp(P_ARMOUR_TYPE)==AT_SHIELD) && 
      QueryProp(P_NR_HANDS)<1 ) {
    raise_error(sprintf("Fixme: AT_SHIELD beim Ausziehen ohne P_NR_HANDS: %O",
	  object_name()));
  }
  
  // Ausziehcheck der Kleidung machen
  return ::_check_unwear_restrictions(worn_by,silent,all);
}

protected void _informunwear(object worn_by, int silent, int all) {
  mixed res;
  // Gesetzte Resistenzen loeschen
  worn_by->RemoveResistanceModifier(res=QueryProp(P_ARMOUR_TYPE));
  
  // Ruestungen koennen Attribute aendern/blockieren. Also muessen diese
  // nach dem Ausziehen aktualisiert werden
  worn_by->deregister_modifiers(ME);
  worn_by->UpdateAttributes();

  // P_TOTAL_AC im Traeger updaten
  worn_by->QueryProp(P_TOTAL_AC);

  // die geerbte Funktion aus der Kleindung gibt noch meldungen aus und ruft
  // Informunwear().
  ::_informunwear(worn_by,silent,all);
}

// Funktion, die das "trage"/"ziehe * an"-Kommando auswertet
varargs int do_wear(string str, int silent) {  
  int all;  

  // Hat der Spieler "trage alles" eingegeben?
  all=(str=="alles" || str=="alle ruestungen");
  
  return(_do_wear(str,silent,all));
}

// Die Funktion, die das "ziehe * aus"-Kommando auswertet
varargs int do_unwear(string str, int silent) {
  int all;

  all=(str=="alles" || str=="alle ruestungen");
  return(_do_unwear(str,silent,all));
}


// Objekte, die die Beschaedigung einer Ruestung durch direktes Setzen von
// P_DAMAGED durchfuehren, werden im awmaster geloggt
static mixed _set_item_damaged(mixed arg) {
    if (arg && !intp(arg))
    {
        return Query(P_DAMAGED, F_VALUE);
    }

    if (previous_object(1))
      call_other("/secure/awmaster","RegisterDamager",
            previous_object(1),QueryProp(P_DAMAGED),arg); 

    return Set(P_DAMAGED, arg, F_VALUE);
}

// Will man eine Ruestung beschaedigen oder reparieren, so macht man das
// am besten ueber die Funktion Damage(argument). Positive Argumente
// bedeuten eine Beschaedigung, negative eine Reparatur. Der Rueckgabewert
// ist die wirklich durchgefuehrte Aenderung des Beschaedigungswertes
int Damage(int new_dam) {   
  int    ac,maximum,old_dam;
  object w;

  if (!new_dam || !intp(new_dam)) {      
    return 0;
  }

  if ( (ac=QueryProp(P_AC))<=0 && (new_dam>0) ) {
    // Sonst wuerde Beschaedigung zur Reparatur fuehren
    return 0;
  }

  // Min-AC und MAX_AC beachten
  if ((ac-new_dam) < MIN_ARMOUR_CLASS) {      
    new_dam = ac-MIN_ARMOUR_CLASS;
  }
  else if ((ac-new_dam) >
    (maximum=VALID_ARMOUR_CLASS[QueryProp(P_ARMOUR_TYPE)])) {      
    new_dam = ac-maximum;
  }

  // Nie mehr als 100% reparieren
  if ( ((old_dam=QueryProp(P_DAMAGED))<-new_dam) && (new_dam<0) ) {
    new_dam=-old_dam;
  }

  // Aenderungen an der Ruestungsklasse und dem Beschaedigungswert
  // durchfuehren
  SetProp(P_AC,(ac-new_dam));
  // eigene Set-Methode umgehen
  Set(P_DAMAGED,(old_dam+new_dam),F_VALUE);

  // P_TOTAL_AC im Traeger updaten, wenn vorhanden
  if (objectp(w=QueryProp(P_WORN)))
    w->QueryProp(P_TOTAL_AC);

  // Rueckgabewert: Durchgefuehrte Aenderung an P_DAMAGE
  return new_dam;
}

public status IsArmour() {return 1;}
public status IsClothing() {return 0;}


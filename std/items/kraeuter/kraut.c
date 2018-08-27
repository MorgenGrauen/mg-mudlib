// (c) September 2000 by Padreic (Padreic@mg.mud.de)

#pragma strong_types, save_types, rtt_checks

inherit "/std/thing";

#include <properties.h>
#include <items/kraeuter/kraeuter.h>

#define PLANT_LIFETIME  (24*3600)
#define FRESH_TIME      (6*3600)
// Die plantID wird fuer ungueltig erschaffene Kraeuter auf -1 gesetzt.

private int state;
// enthaelt die Nummer des Krauts
private int plantId;
// enthaelt den Pfad des clonenden Objekts
private string cloner;
// Qualitaet des Krautes.
private int quality=100;

// File kann ein name sein, dessen Eigenschaften der VC konfigurieren sein
// oder 0, wenn er selber ermitteln soll, was fuer ein File er gerade erzeugt
// hat.
// Sprich: real existierende Files auf der Platte muessen da ihren Namen
// angeben, Standardfall ist aber, dass man 0 angibt und der VC weiss, was er
// gerade erzeugt hat.
void customizeMe(string file)
{
  if (stringp(file)) 
    file=explode(file, "/")[<1];
  KRAEUTERVC->CustomizeObject(file);
}

protected void create()
{
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }
  ::create();

  Set(P_QUALITY, function int () { return quality; }, F_QUERY_METHOD);
  SetProp(P_WEIGHT, 120);
  SetProp(P_VALUE, 70);
  SetProp(P_MATERIAL, MAT_MISC_PLANT);
  // Zeitpunkt des Verfaulens
  set_next_reset(FRESH_TIME + PLANT_LIFETIME);
}

protected void create_super()
{
  set_next_reset(-1);
}

// wird gerufen, wenn sich der Zustand des Krautes (frisch, getrocknet,
// verfault) aendert. Kann genutzt werden, um die Beschreibung des Krautes zu
// aendern.
protected void ChangeState(int newstate)
{
  state = newstate;
  // Vorlaeufig muessen erbende Kraeuter die Beschreibung aendern.
  // Mittelfristig soll es dieses Objekt aber machen.
}

void reset()
{
  // wenn getrocknet oder schon verfault, wurden die Beschreibungen bereits
  // dabei geaendert und dieses reset() muss nix machen.
  if ( state == PLANT_DRIED || state == PLANT_EXPIRED )
      return;
  // Ist es (frisch) verfault?
  else if (object_time() + FRESH_TIME + PLANT_LIFETIME < time())
  {
    // ja, Zustand aendern
    ChangeState(PLANT_EXPIRED);
  }
  else
  {
    // noch frisch genug -> keine Aenderung.
    // Evtl. hat jemand am Reset rumgespielt oder ein Krautautor benutzt den
    // reset auch fuer andere Zwecke... Nix machen, aber sicherstellen, dass
    // der naechste Reset spaetestens am Ende der Lebensdauer passiert...
    set_next_reset(min(object_time() + FRESH_TIME + PLANT_LIFETIME,
                       query_next_reset()));
  }
}

public string short()
{
  string str=QueryProp(P_SHORT);
  if (!stringp(str)) 
      return 0;
  if (plantId==-1)
      return str+" (unwirksam).\n";
  else if (state==PLANT_DRIED)
     return str+" (getrocknet).\n";
  else if (state==PLANT_EXPIRED)
     return str+" (verfault).\n";
  return str+".\n";
}

// Liefert einen Skalierungsfaktor zurueck, der mit dem aktuellen Wert von
// P_QUALITY verrechnet wird. Rueckgabewerte:
// Pflanze getrocknet oder nicht aelter als 6 h : 100
// Pflanze aelter als 6, aber juenger als 30 h : 99...1
// Pflanze aelter als 30 h: 0.
// DryPlant() zerstoert das Kraut, wenn P_QUALITY unter 1 faellt.
public int PlantQuality()
{
  int factor;
  // schon getrocknet oder nicht aelter als 6 h? 
  // Dann keine (weitere) Reduktion.
  if ( state == PLANT_DRIED || object_time() + FRESH_TIME > time())
    factor = 100;
  // >30 Stunden nach dem Pfluecken ist das Kraut verschimmelt.
  else if ( object_time() + FRESH_TIME + PLANT_LIFETIME < time() )
    factor = 0;
  // Zeit zwischen 6 und 30 Stunden nach dem Pfluecken in 99 gleichmaessige
  // Abschnitte unterteilen. 24 h sind 86400 s, 86400/99 = 873.
  else
    factor=(time() - object_time() - FRESH_TIME)/873;

  return QueryProp(P_QUALITY)*factor/100;
}

// Wie lange (in Sekunden) ist das Kraut noch haltbar?
public int TimeToLive()
{
  if ( state == PLANT_DRIED )
    return __INT_MAX__;
  return object_time() + FRESH_TIME + PLANT_LIFETIME - time();
}

//TODO: vielleicht etwas zufall? Sonst Kraeuterqualitaet hieran ermittelbar.
static int _query_value()
{
  int val = Query(P_VALUE,F_VALUE)*PlantQuality()/100;
  if (plantId<=0 || val = 0)
    return 0;
  return val-val%10;
}

static string _query_nosell() 
{
  if (state != PLANT_DRIED)
    return "Mit ungetrockneten Kraeutern handele ich nicht. Die verderben "
      "immer so schnell im Lager, und dann werde ich sie nicht wieder los.";
  return 0;
}

// Mit DryPlant() wird die Pflanze getrocknet. Als Argument wird der Prozent-
// wert uebergeben, auf den die Qualitaet sinken soll. Als Ausgangswert
// dieser Berechnung wird der Rueckgabewert von PlantQuality() verwendet.
// Hintergrund: Die Qualitaet des Krauts sinkt im ungetrockneten Zustand
// ueber einen Zeitraum von 24 h kontinuierlich von 100 auf 0 ab, sobald
// es aelter als 6 Stunden ist. Danach ist es verschimmelt, was aber seiner
// verbleibenden "Wirkung" keinen Abbruch tut.
// Es wird die zum Zeitpunkt des Trocknungsvorganges gueltige Qualitaet
// also sozusagen "eingefroren" und entsprechend dem Rueckgabewert von
// PlantQuality() heruntergerechnet.

// Diese Funktion kann natuerlich auch ueberschrieben werden, wenn bestimmte
// Kraeuter erst durch trocknen in der Qualitaet steigen.

// TODO: Ist das Argument "qual" dabei prozentual aufzufassen, oder 
// soll nur ein noch zu bestimmender Festwert abgezogen werden?
// Arathorn: Es bleibt jetzt erstmal prozentual.

#define DRYING_ALLOWED ({PLANTMASTER, "/items/kraeuter/trockner"})

public void DryPlant(int qual) 
{
  // Keine mehrfache Trocknung zulassen.
  if ( state == PLANT_DRIED )
    return;

  // Nur bestimmte Objekte duerfen Trocknungen ausloesen.
  if ( member(DRYING_ALLOWED, load_name(previous_object())) == -1 )
    return;

  // Qualitaet auf 100 deckeln.
  if ( qual>100 )
    qual = 100;

  // Qualitaet mittels PlantQuality() runterskalieren.
  qual = PlantQuality()*qual/100;

  if ( qual < 1 ) {
    if(objectp(this_player()))
      tell_object(this_player(), 
        Name(WER,1)+" zerfaellt in unzaehlige feine Kruemel.\n");
    remove();
    return;
  }
  // Kraut als getrocknet kennzeichnen.
  ChangeState(PLANT_DRIED);
  quality = qual;
}

/* Funktionen zum Initialisieren der Pflanze */
// Der Kraeutermaster prueft den Clone auf Gueltigkeit. Eine einmal gesetzte
// ID kann auf diesem Weg nicht mehr geaendert werden.
nomask int SetPlantId(int new_id)
{
  if (plantId != 0)
    return -1;
  cloner=0;
  if (catch(cloner=call_other(PLANTMASTER, "CheckPlant", new_id)) || !cloner)
    new_id = -1;
  return (plantId=new_id);
}

nomask int QueryPlantId()
{
  return plantId;
}

nomask string QueryCloner()
{
  return cloner;
}

nomask int QueryDried()
{
  return (state == PLANT_DRIED);
}

// MorgenGrauen MUDlib
//
// thing/properties.c -- most general class (property handling)
//
// $Id: properties.c 6951 2008-08-09 23:08:31Z Zesstra $

// Properties.c -- Propertyverwaltung
// (c) 1993 Hate@MorgenGrauen, Mateese@NightFall
//          Idea and Code      Flames and Destructions
// -> *grin* thats the point actually :) 
//
// Ueberarbeitet von Jof       am 12.06.1994
// Ueberarbeitet von Mandragon am 11.05.2003

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include "/secure/wizlevels.h"


// the mapping where the actual properties are stored. Direct initialization.
private nosave mapping *prop = ({ ([]), ([]), ([]), ([]) });

// the mapping that is used for saving
private mapping properties;

// security-flag
private nosave int closure_call;

// Initialisierung der Props. Kann leider momentan nicht private sein, weil
// Padreic son komisches Objekt hat, was die Funktion hier ruft.
// TODO: irgendwann mal private machen.
// TODO: Da props jetzt einfach bei der Deklaration initlisiert wird,
// eruebrigt sich diese Funktion eigentlich. Bis auf Padreics Objekt...
protected void InitializeProperties() {
  prop = ({ ([]), ([]), ([]), ([]) });
  return;
}

// Props nur dann initialisieren, wenn sie es noch nicht sind
protected void create() {
  // Blueprints in /std benoetigenkeinen Reset ....
  if (object_name()=="/std/thing/properties")
    set_next_reset(-1);
}

protected void create_super() {
  set_next_reset(-1);
}

// Welche externen Objekte duerfen zugreifen?
nomask private int allowed()
{
    if ( (previous_object() && IS_ARCH(getuid(previous_object())) &&
          this_interactive() && IS_ARCH(this_interactive())) ||
         (previous_object() && getuid(previous_object()) == ROOTID &&
          geteuid(previous_object()) == ROOTID) )
        return 1;
    return 0;
}


// Set() -- provides direct access to a property, no filters
public varargs mixed Set( string name, mixed Value, int Type, int extern )
{

  if (!objectp(this_object()))
    return 0;

  // Properties, die SECURED oder PROTECTED sind, duerfen nur vom Objekt
  // selber, EM+ oder ROOT veraendert werden
  if ((prop[F_MODE][name]&(PROTECTED|SECURED))&&
      (closure_call||extern||extern_call()) &&
       previous_object() != this_object() && !allowed())
    return -1;
  
  // Das SECURED-Flag darf bei Properties nicht mehr geloescht werden
  if ((prop[F_MODE][name]&SECURED)&&
      (Type==F_MODE||Type==F_MODE_AD)&&(Value & SECURED))
    return -2;
  
  // Setzen duerfen das SECURED-Flag nur das Objekt selber, EM+ oder ROOT
  if ((Type==F_MODE||Type==F_MODE_AS)&&(Value&SECURED)&&
      (closure_call ||extern || extern_call()) &&
       previous_object() != this_object() && !allowed() )
    return -3;

  switch(Type)
  {
    // Je nach Modus Flags veraendern
    case F_MODE_AS:  prop[F_MODE][name]|= Value;
                     return prop[F_MODE][name];
    case F_MODE_AD:  prop[F_MODE][name]&= ~Value;
                     if (!prop[F_MODE][name]) prop[F_MODE]-=([name]);
                     return prop[F_MODE][name];
    case F_MODE:     prop[F_MODE][name]^= Value;
                     if (!prop[F_MODE][name]) prop[F_MODE]-=([name]);
                     return prop[F_MODE][name];
    case F_SET_METHOD:
      // -1 als Setz-Methode: Nosetmethod setzen
      if (Value == -1)
      {
        prop[F_SET_METHOD]-=([name]);
        prop[F_MODE][name] |= NOSETMETHOD;
        return 0;
      }
      // Kein break!
    case F_QUERY_METHOD:
      // Ungebundene Lambda_Closure? Binden!
      if (closurep(Value)&&!query_closure_object(Value))
      {
        if (extern_call() &&
             (getuid(previous_object()) != getuid()||
              geteuid(previous_object()) != geteuid()))
          return prop[Type][name];
        
        Value = bind_lambda( Value,this_object());
      }
      // Kein break!
    default:
      if (!Value) prop[Type]-=([name]);
      else prop[Type][name] = Value;
  }

  // Gesamtwert zurueckgeben
  return prop[Type][name];
}


// Direktes Auslesen der Property ohne Filter ...
public varargs mixed Query( string name, int Type )
{
    if (pointerp(prop)) return prop[Type][name];
    return 0;
}

// Property setzen unter Verwendung evtl. vorhandener Zugriffsfunktionen
public mixed SetProp( string name, mixed Value )
{
  closure func;
  mixed result;
   
  // nur fuer heute
  if (!objectp(this_object()))
    return 0;

  // NOSETMETHOD: Darf nicht gesetzt werden
  if (prop[F_MODE][name] & NOSETMETHOD ) return -1;

  // Set-Method abfragen, so vorhanden
  if (func=prop[F_SET_METHOD][name])
  {
    int flag;

    // Wert als Set-Method? gleich zurueckgeben
    if (!closurep(func)) return func;

    // An dieser Stelle muss func eine Closure sein. Da Set() ungebundene
    // Lambdas bindet, kann es auch nur eine gebundene Closure sein und das
    // Objekt existiert auch noch (sonst waere func == 0).

    // closure_call setzen, falls noch nicht gesetzt
    if ((flag=closure_call<time()))
      closure_call = time()+59;

    // Dann mal die Closure aufrufen. Bei Fehler selbige loeschen
    if (catch(result=funcall(func, Value, name);publish))
    {
      prop[F_SET_METHOD]-=([name]);
    }
      
    // Wenn closure_call gesetzt wurde, wieder loeschen
    if (flag) closure_call = 0;

    // Und zurueckgeben
    return result; 
  }

  // _set_*-Methode vorhanden? falls ja, aufrufen.i
  // TODO: Closurecache einfuehren und Funktionaufruf nur noch machen, wenn es
  // die _set_* auch gibt?
  if (call_resolved(&result,this_object(),"_set_"+name,Value ))
        return result;

  // Letzte Moeglichkeit: Muss eine 'normale' Property sein
  return Set( name, Value, F_VALUE, extern_call() );
}


// Property auslesen unter Verwendung evtl. vorhandener Zugriffsfunktionen
public mixed QueryProp( string name )
{
  closure func;
  mixed result;
 
  // nur fuer heute
  if (!objectp(this_object()))
    return;

  // Query-Methode vorhanden?
  if ( func = prop[F_QUERY_METHOD][name] )
  {
    int flag;

    // Wert als Query-Method? Gleich zurueckgeben ...
    if (!closurep(func)) return func;
 
    // An dieser Stelle muss func eine Closure sein. Da Set() ungebundene
    // Lambdas bindet, kann es auch nur eine gebundene Closure sein und das
    // Objekt existiert auch noch (sonst waere func == 0).
   
    // closure_call setzen, falls noch nicht gesetzt
    if ((flag=closure_call<time()))
      closure_call = time()+59;
    
    // Dann Mal die Closure aufrufen. Bei Fehler selbige loeschen
    if (catch(result=funcall(func);publish))
    {
      prop[F_QUERY_METHOD]-=([name]);
    }
    // Wenn closure_call gesetzt wurde, wieder loeschen    
    if (flag) closure_call = 0;
    
    // Und zurueckgeben
    return result;
  }
  
  // _query_*-Methode vorhanden? falls ja, aufrufen.
  // TODO: Closurecache und nur rufen, wenn es _query_* auch gibt?
  if (call_resolved(&result,this_object(),"_query_"+name))
    return result;
  
  // Hilft alles nichts. Es ist eine 'normale' Property ...
  return prop[F_VALUE][name];
}


// Das gesamte Property-Mapping auf einen Schlag setzen
public void SetProperties( mapping props )
{
  string *names;
  int i, j, same_object;
 
  // Kein Mapping? Schlecht ...
  if(!mappingp(props)) return;

  // Setzen wir selber?
  same_object = (!closure_call &&
                 (!extern_call()||previous_object()==this_object()||
                  allowed()));
  names = m_indices(props);
  
  // Das SECURED-Flag darf nur durch das Objekt selber gesetzt werden:
  // Alle SECURED-Flags aus props loeschen
  if (!same_object)
  {
    j=sizeof(names);
    while(j--) props[names[j], F_MODE] &= ~SECURED;
  }

  j=sizeof(names);
  while(j--)
  {
    // Properties, die schon SECURED oder PROTECTED sind, duerfen
    // nur vom Objekt selber manipuliert werden
    if (same_object||!(prop[F_MODE][names[j]] & (PROTECTED|SECURED)) )
    {
      i=4;
      while(i--)
      {
        if(props[names[j],i])
          prop[i][names[j]] = props[names[j], i];
        else
          prop[i]-=([names[j]]);
      }
    }
  }
  return;
}


// Ein Mapping mit allen Properties zurueckgeben
public mapping QueryProperties()
{
  mapping props;
  int i, j;
  string *names;

  props = m_allocate( 0, 4 );
  
  if (pointerp(prop))
  {
    i=4;
    while(i--)
    {
      names = m_indices(prop[i]);
      j=sizeof(names);
      while(j--) props[names[j], i] = prop[i][names[j]];
    }
  }
  return props;
}

// Die Properties als urspruengliches Array zurueckgeben
public mixed *__query_properties()
{
  if ( pointerp(prop) )
    return(deep_copy(prop));
  else
    return ({ ([]),([]),([]),([]) });
}


// mapping Properties setzen zum Speichern (per save_object())
// Aufruf nur aus simul_efun heraus
public void  _set_save_data(mixed data) { properties = data; }

// mapping Properties zum Restoren zurueckgeben
public mixed _get_save_data()           { return properties; }


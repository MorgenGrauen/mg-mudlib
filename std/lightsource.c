// MorgenGrauen MUDlib
//
// lightsource.c -- standard light source
//
// $Id: lightsource.c 7806 2011-07-11 20:28:17Z Zesstra $

// std/lightsource.c
//
// Original von Miril April '92
// Neufassung von Sir Dezember '92 ( unter Boings Aegide :)
// Version : 1.0
//
// Bugfixes von Sir Juni '93
// Version : 1.2
//
// Wichtige Funktionen, die man in create() aufrufen sollte :
//
// Setzen der Brennstoffmenge :  
// SetProp( P_FUEL, int )       int gibt die Brennzeit in Sekunden an
//
// Setzen der Beschreibung wenn Objekt angezuendet( default : "brennend" ) :
// SetProp( P_LIGHTDESC, string )
//
// Legt fest, ob ein Leuchtobjekt nach seinem Abbrennen destructed wird
// SetProp( P_DO_DESTRUCT, int )
//
// Legt fest, wie hell die Fackel leuchtet.
// SetProp( P_LIGHT, int )
//
// Ansonsten sind die Standardfuktionen wie z.B. SetProp( P_GENDER, MALE ) 
// aufzurufen

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "std/thing" ;

#include <properties.h>
#include <language.h>
#include <defines.h>

#define TRUE  1
#define FALSE 0
#define bool  int

#define DFLT_FUEL       20 // 20 Sekunden
#define DFLT_LIGHTDESC  "brennend"
#define DFLT_LIGHT      2

// So ergibt sich die Moeglichkeit sie wieder auzufuellen
#define CALL_OUT_TIME   100

#undef DEBUG

// TODO: Private waer langfristig nett...
nosave int  fuel, max_fuel;
nosave bool lighted ;
nosave int  call_time ;

void create()
{
  thing::create() ;
  
  SetProp( P_NAME, "Lichtquelle" ) ;
  SetProp( P_SHORT, "Eine Lichtquelle" ) ;
  SetProp( P_LONG, "Du siehst nichts besonderes.\n" ) ;
  SetProp( P_GENDER, FEMALE ) ;
  SetProp( P_FUEL, DFLT_FUEL ) ;
  SetProp( P_LIGHTDESC, DFLT_LIGHTDESC ) ;
  SetProp( P_LIGHT, DFLT_LIGHT );
  SetProp( P_DO_DESTRUCT, TRUE ) ;
  SetProp( P_VALUE, 5 ) ;
  
  AddId( ({ "lichtquelle", "\nlichtquelle" }) ) ;
  AddCmd( ({ "zuende", "zuend" }), "light" );
  AddCmd( ({ "loesche", "loesch" }), "extinguish" );
}

void test_remove()
{  if (QueryProp(P_DO_DESTRUCT)) remove();  }

/*
 * Lichtquelle anzuenden
 */
bool light(string str)
{
  string tmp ;
  object env;

  _notify_fail( "Zuende was an?\n" ) ;

  if( (!str) || (!sscanf( str, "%s an", tmp )) || (!id( tmp )) )
    return FALSE ;
  
  if( environment() != PL ) // Player hat es nicht
  {
    _notify_fail( "Erstmal musst Du " + name( WEN, 0 ) + " haben.\n" ) ;
    return FALSE ;
  }
  
  if( lighted )
  {
    _notify_fail( CAP( name( WER, 1 ) ) + " brennt schon.\n" ) ;
    return FALSE ;
  }

  if( fuel <= 0 )
  {
    _notify_fail( CAP( name( WER, 1 ) ) + " ist abgebrannt.\n" ) ;
    test_remove() ;
    return FALSE ;
  }

  lighted = TRUE ;
  env=this_object();
  while (objectp(env=environment(env)))
      // Ja. Man ruft die _set_xxx()-Funktionen eigentlich nicht direkt auf.
      // Aber das Lichtsystem ist schon *so* rechenintensiv und gerade der
      // P_LAST_CONTENT_CHANGE-Cache wird *so* oft benoetigt, dass es mir
      // da um jedes bisschen Rechenzeit geht.
      // Der Zweck heiligt ja bekanntlich die Mittel. ;-)
      //
      // Tiamak
      env->_set_last_content_change();
  call_time = (fuel < CALL_OUT_TIME)? fuel : CALL_OUT_TIME ;
  call_out( "out_of_fuel", call_time ) ;
  if( (int)PL->QueryProp(P_PLAYER_LIGHT) == 1 )
    write( "Du kannst wieder etwas sehen.\n" ) ;
  else write( "Ok.\n" ) ;
  say((string)PL->Name(WER)
      + " zuendet " + name( WEN, 0 ) + " an.\n" ) ;

  return TRUE ;
}

bool extinguish(string str)
{
  int ti;
  object env;

  _notify_fail( "Welche Lichtquelle moechtest Du loeschen?\n" ) ;

  if( (!str) || (!id( str )) )
    return FALSE ;
  
  if( !lighted )
  {
    _notify_fail( CAP( name( WER, 1 ) ) + " brennt gar nicht.\n" ) ;
    return FALSE ;
  }

  if( environment() != PL )
  {
    _notify_fail( "Erstmal musst Du " + name( WEN, 0 ) + " haben.\n" ) ;
    return FALSE ;
  }

  if( ( ti = remove_call_out( "out_of_fuel" ) ) == -1 )
    ti = 0 ;

  fuel -= (call_time - ti) ;
  lighted = FALSE ;
  env=this_object();
  while (objectp(env=environment(env)))
      // Ja. Man ruft die _set_xxx()-Funktionen eigentlich nicht direkt auf.
      // Aber das Lichtsystem ist schon *so* rechenintensiv und gerade der
      // P_LAST_CONTENT_CHANGE-Cache wird *so* oft benoetigt, dass es mir
      // da um jedes bisschen Rechenzeit geht.
      // Der Zweck heiligt ja bekanntlich die Mittel. ;-)
      //
      // Tiamak
      env->_set_last_content_change();
  if ( this_player()->QueryProp(P_PLAYER_LIGHT) == 0 )
  {
    write( "Es wird dunkel.\n" ) ;
    say((string)PL->Name(WER) 
	+ " macht das Licht aus.\n" ) ;
  }
  else
  {
    write( "Ok.\n" ) ;
    say((string)PL->Name(WER)
	+ " loescht " + name( WEN, 0 ) + " aus.\n" ) ;
  }

  if( fuel <= 0 ) test_remove() ;
  return TRUE ;
}

bool unlight()
{
  int ti;
  object env;

  if( !lighted )
    return FALSE ;

  if( ( ti = remove_call_out( "out_of_fuel" ) ) == -1 )
    ti = 0 ;

  fuel -= (call_time - ti) ;
  lighted = FALSE ;
  env=this_object();
  while (objectp(env=environment(env)))
      // Kommentar siehe oben ;^)
      env->_set_last_content_change();
  if( fuel <= 0 ) test_remove() ;
  return TRUE ;
}

void out_of_fuel()
{
  int i;
  object *inv, env;
  
  fuel -= call_time ;

  if (fuel>0) {
    call_out( "out_of_fuel", call_time ) ;
    return ;
  }
  lighted=FALSE;
  env=this_object();
  while (objectp(env=environment(env)))
      // Immer noch nicht wirklich sauber. Aber Begruendung siehe oben.
      env->_set_last_content_change();

  if (environment())
  {
    if ( living(environment()) && environment(environment()) )
    {
      inv=(users() & all_inventory(environment(environment())))-({ environment() });
      for (i=sizeof(inv)-1; i>=0; i--) 
        if (inv[i]->QueryProp(P_PLAYER_LIGHT)<=0)
          tell_object(inv[i], "Es wird dunkel als " + environment()->name(WESSEN) +
            " " + QueryProp(P_NAME) + " erlischt.\n" ) ;
        else tell_object(inv[i], CAP( name( WER, 0 ) ) + " erlischt.\n" ) ;
      if (environment()->QueryProp(P_PLAYER_LIGHT)<=0)
        tell_object(environment(), 
          CAP( name( WER, 1 ) ) + " erlischt und es wird dunkel.\n" ) ;
      else tell_object(environment(), CAP( name( WER, 1 ) ) + " erlischt.\n" ) ;    
    }
    else
    {
      inv=(users() & all_inventory(environment()));
      for (i=sizeof(inv)-1; i>=0; i--) 
        if (inv[i]->QueryProp(P_PLAYER_LIGHT)<=0)
          tell_object(inv[i], "Es wird dunkel als " + name(WER,1)
          + " erlischt.\n" ) ;
        else tell_object(inv[i], CAP( name( WER, 0 ) ) + " erlischt.\n" ) ;
    }
  }  
  test_remove() ;
}

// Brennstoff nachfuellen
void AddFuel(int f)
{  fuel += f ;  }

// Property - Funktionen

static void _set_lighted(bool l)
{  lighted = l ;  }

bool _query_lighted()
{  return lighted ;  }

static void _set_fuel(int f)
{
  if (f>max_fuel) max_fuel=f;
  fuel = f ;
}

static int _query_fuel()
{
  int tmp;

  if ((tmp=find_call_out("out_of_fuel"))>=0)
    return fuel-call_time+tmp;
  else return fuel;
}

static string _query_lightdesc()
{
  string *tmp;
  int n,i;
  
  tmp=Query(P_LIGHTDESC);
  if (!pointerp(tmp)) return (string)tmp;
  n=sizeof(tmp);
  i=n*_query_fuel()/max_fuel;
  if (i>=n) i=n-1;
  return tmp[i];
}

static int _query_light()
{   return (lighted ? Query(P_LIGHT) : 0);  }

/*
 * short() ueberladen
 */
string short()
{
  string s;
  string desc;

  if(!(s=QueryProp(P_SHORT)))
    return 0;
  return s + ( (lighted)? (" ("+QueryProp( P_LIGHTDESC ) +").\n") : ".\n" );
}

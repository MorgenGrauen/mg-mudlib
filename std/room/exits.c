// MorgenGrauen MUDlib
//
// room/exits.c -- room exits handling 
//
// $Id: exits.c 9497 2016-02-21 14:20:03Z Zesstra $

/*
 * Exits of the room (obvious ones, doors, and special ones)
 * we define the following function for easy reference:
 * GetExits() - return a string containing an "Obvious Exits" Statement
 *
 * The exits are implemented as properties P_EXITS
 * They are stored locally (_set_xx, _query_xx)
 * as mapping to speed up the routines in this module.
 *
 */

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <moving.h>
#include <room/exits.h>
#include <hook.h>
#include <exploration.h>
#undef NEED_PROTOTYPES

#include <sys_debug.h>
#include <config.h>
#include <properties.h>
#include <defines.h>
#include <daemon.h>
#include <doorroom.h>
#include <routingd.h>

#define NUMBERS ({ "zwei", "drei", "vier", "fuenf", "sechs", "sieben", "acht" })


// Hilfsfunktion, die bei kaputten Exits eine Notrettung betreibt, aber
// trotzdem auf Debug eine Meldung macht.
static mapping rescueExit()
{
  catch(raise_error(sprintf(
	  "room/exits.c: Forgotten ::create()? "
	  "P_EXITS in %O is 0!\n", this_object()));publish); 

  return ([:2]);
}


static mapping _set_exits( mapping map_ldfied ) 
{
    if( mappingp(map_ldfied) )
        return Set( P_EXITS, map_ldfied );
    return 0;
}


static mapping _query_exits() 
{
    if( (!previous_object() || object_name(previous_object()) != DOOR_MASTER)
        && QueryProp(P_DOOR_INFOS) )
        call_other( DOOR_MASTER, "init_doors" );

    mapping exits = Query(P_EXITS) || rescueExit();

    return filter(exits, function int (string key, mixed val)
        {return stringp(val[0]);} );
}


static int _set_special_exits( mapping map_ldfied )
{
    return -1;
}


static mapping _query_special_exits() 
{
    mapping exits = Query(P_EXITS) || rescueExit();

    return filter(exits, function int (string key, mixed val)
        {return closurep(val[0]);} );
}


void reset()
{}


protected void create()
{
  offerHook(H_HOOK_EXIT_USE, 1);
  SetProp( P_EXITS, ([:2]) );
  // Falls dieser Raum ein Haltepunkt von Transportern ist, werden diese
  // Transporter informiert, dass dieser Raum neugeladen wurde - evtl. will
  // der Transporter jetzt diesen Raum ansteuern, wenn er pausiert war.
  object *trans = TRAVELD->HasTransporter(this_object());
  if (pointerp(trans))
  {
    foreach (object t : trans)
    {
      t->Continue();
    }
  }
}

protected void create_super() {
  set_next_reset(-1);
}

protected void _AddExit(string|string* cmd, string|closure room,
                        string message)
{
  mapping exita;

  exita = Query(P_EXITS) || rescueExit();

  if ( !closurep(room) )
  {
    object router;

    room = _MakePath(room);

    if ( !clonep(this_object()) && objectp(router = find_object(ROUTER)) )
    {
      router->RegisterExit( object_name(this_object()), cmd, room );
    }
  }

  if( stringp(cmd) )
  {
    exita += ([ cmd : room; message ]);
  }
  else
  {
    foreach(string c : cmd)
    {
      if (stringp(c))
        exita += ([ c : room; message ]);
    }
  }

  Set( P_EXITS, exita );
}

void AddExit(string|string* cmd, closure|string dest)
{
  string msg;
  if ( stringp(dest) )
  {
    int s;
    if( (s = member(dest, '#')) != -1 )
    {
      msg  = dest[0..s-1];
      dest = dest[s+1..];
    }
  }
  _AddExit(cmd, dest, msg);
}

void RemoveExit(string|string* cmd )
{
    mapping exita;

    if ( !cmd ) {
        SetProp(P_EXITS, ([:2]) );
        return;
    }

    if ( stringp(cmd) )
        cmd = ({ cmd });

    exita = Query(P_EXITS, F_VALUE) || rescueExit();
    foreach(string c : cmd)
      m_delete( exita, c );

    Set( P_EXITS, exita, F_VALUE );
}


void AddSpecialExit(string|string* cmd, string|closure functionname )
{

    if ( stringp(functionname) )
        functionname = symbol_function( functionname, this_object() );

    if ( !closurep(functionname) )
    {
        catch(raise_error(sprintf( "method %O doesn't exist\n",
                          functionname)); publish);
        return;
    }

    AddExit( cmd, functionname );
}


void RemoveSpecialExit(string|string* cmd)
{
    RemoveExit( cmd );
}


varargs string GetExits( object viewer ) 
{
    string *indices, *hidden;
    string exits;

    if ( QueryProp(P_DOOR_INFOS) )
        call_other( DOOR_MASTER, "init_doors" );

    indices = m_indices( Query(P_EXITS) || rescueExit() );
    
    if ( pointerp(hidden = QueryProp(P_HIDE_EXITS)) )
        indices -= hidden;

    int n=sizeof(indices);
    switch (n) {
    case 0:
        return "Es gibt keine sichtbaren Ausgaenge.\n";
        
    case 1:
        return "Es gibt einen sichtbaren Ausgang: " + indices[0] + ".\n";
        
    case 2: case 3: case 4: case 5: case 6: case 7: case 8:
        exits = "Es gibt " + NUMBERS[n-2] + " sichtbare Ausgaenge: ";
        break;
        
    default:
        exits = "Es gibt viele sichtbare Ausgaenge: ";
    }
    exits += CountUp(indices);
    return break_string( exits+".", 78 );
}


// Richtungsbefehle nur interpretieren, wenn der Spieler *im* Raum steht und
// nicht davor (Transporter etc.)/o
void init()
{
    if ( environment(this_player()) == this_object() )
        add_action( "_normalfunction", "", 1 );
}


/* not only normal exits are handled here */

int _normalfunction()
{
  int ret;
  mapping exits = Query(P_EXITS, F_VALUE) || ([:3]);
  if (!member(exits,query_verb()))
    return 0;

  string verb = query_verb();
  string destroom = exits[query_verb(),0];
  string message = exits[query_verb(),1];

  mixed hres = HookFlow(H_HOOK_EXIT_USE, ({verb, destroom, message}));
  if(hres && pointerp(hres) && sizeof(hres)>H_RETDATA)
  {
    if(hres[H_RETCODE]==H_CANCELLED)
    {
      return 1;
    }
    else if(hres[H_RETCODE]==H_ALTERED
            && pointerp(hres[H_RETDATA])
            && sizeof(hres[H_RETDATA]) >= 3)
    {
      <string|closure>* hdata = hres[H_RETDATA];
      if (!stringp(hdata[0])
          || (!stringp(hdata[1]) && !closurep(hdata[1]))
          || (hdata[2] && !stringp(hdata[2])) )
        raise_error(sprintf("Invalide Daten aus H_HOOK_EXIT_USE: %.150O\n",
                            hdata));
      verb = hdata[0];
      destroom = hdata[1];
      message = hdata[2];
    }
  }

  if( closurep(destroom) )
  {
    ret = funcall( destroom, verb );

    if(ret==MOVE_OK)
    {
      GiveEP( EP_EXIT, verb );
    }

    return ret;
  }

  if (!stringp(message))
  {
    if( member( ({ "sueden", "suedwesten", "westen","nordwesten", "norden",
        "nordosten", "osten","suedosten" }), verb ) != -1 )
    {
      message = "nach " + capitalize(verb);
    }
    else if ( member( ({ "oben", "unten" }), verb ) != -1 )
    {
      message = "nach " + verb;
    }
    else
    {
      message = verb;
    }
  }

  ret = this_player()->move( destroom, M_GO, message );

  if (ret==MOVE_OK)
  {
    GiveEP( EP_EXIT, verb );
  }

  return ret;
}

static string _MakePath( string str )
{
  string *comp;

  comp = explode( object_name(this_object()), "/" ) - ({""});
  
   switch( str[0] ){
   case '.':
       str = "/" + implode( comp[0..<2], "/" ) + "/" + str;
       break;
       
   case '~':
       str = "/" + comp[0] + "/" + (comp[0] == "d" ? comp[1] + "/" : "")
           +REAL_UID(this_object()) + str[1..];
       break;
   }
   
   return MASTER->_get_path( str, getuid(this_object()) );
}


// MorgenGrauen MUDlib
//
// player/moving.c -- player moving
//
// $Id: moving.c 9434 2016-01-12 12:34:05Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone

inherit "std/living/moving";

#define NEED_PROTOTYPES
#include <player/base.h>
#include <living/description.h>
#include <player/viewcmd.h>
#include <player.h>
#undef NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <language.h>
#include <defines.h>
#include <moving.h>
#include <wizlevels.h>
#include <events.h>
#include <pathd.h>

private nosave string *connections;

public void create()
{
    SetProp( P_MSGIN,  "kommt an" );
    SetProp( P_MSGOUT, "geht" );
    SetProp( P_MMSGIN, "tritt aus einer Schwefelwolke" );
    SetProp( P_MMSGOUT, "verschwindet mit Knall und Schwefelduft" );
    Set( P_MSGIN,   SAVE, F_MODE );
    Set( P_MSGOUT,  SAVE, F_MODE );
    Set( P_MMSGIN,  SAVE, F_MODE );
    Set( P_MMSGOUT, SAVE, F_MODE );
    connections = ({});

    ::create();
}


static mixed _to_remove( object ob )
{
    return ob->QueryProp(P_AUTOLOADOBJ) || ob->QueryProp(P_NODROP);
}


// autoload and nodrop object may not fall into the environment
static varargs int remove( int silent )
{
    object* dest_obj = filter( deep_inventory(ME) - ({0}), "_to_remove" );
    filter_objects( dest_obj, "remove" );
    filter( dest_obj - ({0}), #'destruct );

    if ( !QueryProp(P_INVIS) && !silent )
        catch( say( name(WER, 1) + " verlaesst diese Welt.\n", ME );publish );
    
    if ( ME && !silent )
        tell_object( ME, "Bis bald!\n" );

    // Im Falle des Resets ist previous_object() == ME, aber
    // previous_object(0) == 0. Ausserdem ist der Caller-Stack leer. Also
    // schauen, ob es ein PO gibt, was nicht gleich dem Objekt selber ist, TI
    // pruefen und 
    if ( this_interactive() != ME
        && objectp(previous_object()) && previous_object() != ME
        && object_name(previous_object())[0..7] != "/secure/"
        && member(object_name(ME), ':') > -1 )
      log_file( "PLAYERDEST",
                sprintf( "%s: %O vernichtet von PO %O, TI %O, TP %O\n",
                         dtime(time()), ME, previous_object(),
                         this_interactive(), this_player() ) );

    // Logout-event ausloesen
    EVENTD->TriggerEvent(EVT_LIB_LOGOUT, ([
	    E_OBJECT: ME,
	    E_PLNAME: getuid(ME),
	    E_ENVIRONMENT: environment() ]) );

    return ::remove();
}

public string NotifyDestruct(object caller) {
  
  if (previous_object() != master()
      || object_name(this_object()) == __FILE__[..<3])
    return 0;

  // Das Zerstoeren von Spielern wird ggf. geloggt.
  if ( objectp(caller) && caller != this_object()
       && getuid(caller) != ROOTID ) {
      log_file( "PLAYERDEST",
		sprintf( "%s: %O VERNICHTET von PO %O, TI %O, TP %O\n",
			 dtime(time()), this_object(), caller,
			 this_interactive(), this_player() ) );
  }
  // erstmal nix weiter tun, destruct gestatten.
  return 0;
}

protected int PreventMove(object dest, object oldenv, int method) {
  string hcroom;
  mixed res;

  // gestorbene HC-Spieler duerfen nicht mehr aus dem Nirvana, nicht umgehbar
  // durch M_NOCHECK
  if ( interactive(ME) && (query_hc_play()>1) ) {
    if (objectp(dest))
      hcroom=object_name(dest);
    if (sizeof(hcroom)<7 || hcroom[0..5]!="/room/") { 
      return ME_CANT_BE_INSERTED;
    }
  }

  // alle anderen Pruefungen sind mit M_NOCHECK egal.
  if ( (method & M_NOCHECK) )
    return(::PreventMove(dest,oldenv,method));

  // Spieler duerfen in Raeume mit gesetztem P_NO_PLAYERS nicht hinein
  if ( dest->QueryProp(P_NO_PLAYERS) && interactive(ME) &&
	!(method & M_NOCHECK) &&
        !IS_LEARNER(ME) && (!stringp(res = QueryProp(P_TESTPLAYER))
                             || !IS_LEARNER( lower_case(res) )) ){
      tell_object( ME, "Da darfst Du als Spieler nicht hin.\n" );
      return ME_NOT_ALLOWED;
  }

  return ::PreventMove(dest,oldenv,method);
}

// Fuck. Ausnahmsweise wegen VC brauch ich nen anderes BLUE_NAME
#define OLD_BLUE_NAME(ob) (explode(object_name(ob),"#")[0])
// Krams nach dem Move machen und nebenbei zum Ueberschreiben.
protected void NotifyMove(object dest, object oldenv, int method) {

  // erstmal ggf. Rauminhalt an Spieler ausgeben.
  if ( interactive(ME) && !(method & M_NO_SHOW) ) {
      if (!CannotSee(1))
          tell_object( ME, "" + env_descr(1) );
      else if ( QueryProp(P_BRIEF) < 2 )
          tell_object( ME, "Finsternis.\n" );
  }

  //dann geerbten Kram machen
  ::NotifyMove(dest,oldenv,method);

  // Schlussendlich noch fuer den PathD bewegung protokollieren.
  // (dest nicht geprueft, da ein Spieler nicht ausserhalb jedes Env bewegt
  // werden kann)
  if ( interactive() && environment() && query_verb() && objectp(oldenv)) {
      connections += ({ ({ OLD_BLUE_NAME(oldenv), OLD_BLUE_NAME(dest),
                        query_verb() + " " + (_unparsed_args(2) || ""),
                        method, dest->QueryProp(P_PARA) }) });

      if ( sizeof(connections) > 30
           && find_call_out("flush_connections") == -1 )
            call_out( "flush_connections", 0, connections );
  }
}

public void flush_connections() {
    catch(PATHD->add_paths(connections);publish);
    connections = ({});
}

/*** Query-Methoden fuer Froesche... ;^) ***/
static string _query_msgin()
{
    return QueryProp(P_FROG) ? "huepft herein" : Query(P_MSGIN);
}


static string _query_msgout()
{
    return QueryProp(P_FROG) ? "huepft" : Query(P_MSGOUT);
}

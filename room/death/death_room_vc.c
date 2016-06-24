// MorgenGrauen MUDlib
//
// death_room_vc.c -- Das Standardobjekt fuer den VC-Todesraum
//
// $Id: death_room_vc.c 8988 2014-12-31 13:10:19Z Zesstra $


#pragma strict_types

#include <defines.h>
#include <properties.h>
#include <moving.h>
#include <language.h>
#include <wizlevels.h>
#include <daemon.h>
#include <new_skills.h>

inherit "/std/room";

public int no_attack();
public int no_poison(int val);

public void create()
{
  if (IS_BLUE(this_object())) return;
    ::create();
    
    SetProp( P_GENDER, MALE );
    SetProp( P_ARTICLE, 0 );
    SetProp( P_LIGHT,1 );
    SetProp( P_NO_TPORT, NO_TPORT_OUT );
    SetProp( P_LOG_FILE, "TOD/Todesraum" );
    SetProp( P_INT_SHORT, "Arbeitszimmer des Todes" );
    SetProp( P_INT_LONG, break_string(
             "Ein dunkler Raum, erleuchtet von dunklem Licht, das sich der "
             "Dunkelheit nicht so sehr zu widersetzen scheint, indem es "
             "leuchtet, als dass es der dunkelste Punkt in einer weniger "
             "dunklen Umgebung ist. Im seltsamen Licht erkennst Du einen "+
             "zentral aufgestellten Schreibtisch, der mit Diagrammen und "
             "Buechern bedeckt ist. Die Waende verschwinden hinter Regalen, "
             "die gefuellt sind mit in Leder gebundenen, dunklen Waelzern, "
             "von denen geheimnisvolle Runen leuchten.\nTod.", 78, 0, 1 ) );
    previous_object()->CustomizeObject();
    call_other("/room/death/death_room","???");
}

void test_remove()
{
  if (!sizeof(all_inventory(this_object())&users()))
    if (!this_object()->remove()) destruct(this_object());
}

public void reset()
{
  ::reset();
  test_remove();
  return;
}

public void exit(object liv)
{
  call_out("test_remove",2);
  return;
}


static void deep_destruct( object ob )
{
    if ( objectp(ob) && environment(ob) == this_object() )
        filter( deep_inventory(ob) + ({ ob }),
                      lambda( ({'x/*'*/}), ({#'destruct, 'x}) ) );
}

public void init()
{  
  string prayroom;
  int res;

  ::init();
  if ( !query_once_interactive(this_player()) ){
      call_out( "deep_destruct", 0, this_player() );
      return;
  }

  if ( !(prayroom = (string) this_player()->QueryPrayRoom()) )
      prayroom = "/room/pray_room";
  
  if ( !this_player()->QueryProp(P_GHOST) )
    {
      if ( IS_WIZARD(this_player()) &&
           this_player()->QueryProp(P_WANTS_TO_LEARN) )
	{
          if ( !this_player()->QueryProp(P_INVIS) )
              tell_room( this_object(),
                         "Der Tod sagt: WAS WILLST DU HIER, "+
                         upperstring((string) this_player()->name())+"?\n"+
                         "Der Tod sagt: DU BIST UNSTERBLICH, DU HAST HIER "
                         "NICHTS ZU SUCHEN!\n\n" );
	}
      else 
	{
          write("Der Tod sagt: WAS TUST DU HIER? DEINE ZEIT IST NOCH "
                "NICHT REIF!\n\n");
          
	  if ( catch(res = (int) this_player()->move( prayroom,M_GO|M_SILENT|M_NOCHECK ))
               || res < 1 && environment(this_player()) == this_object() )
              this_player()->move( "/room/pray_room", M_GO|M_SILENT|M_NOCHECK );
	}
      return;
    }

  if ( !IS_DEPUTY(this_player()) ){
    add_action( "filter_ldfied", "", 1 );
    this_player()->Set( P_TMP_MOVE_HOOK,
                        ({ time()+31536000, 
                           find_object("/room/death/death_room"), 
                           "hier_geblieben" }) );
  }

  this_player()->Set( P_NO_ATTACK, #'no_attack, F_QUERY_METHOD );
  this_player()->Set( P_POISON, #'no_poison, F_SET_METHOD );
  
  "/room/death/death_room"->add_player(this_player());
}

// public, da command_me() sonst buggt (Wurzels Speedy o.ae.)
public int filter_ldfied( string str )
{
    // Description:   Filter out relevant commands.
    string verb;
    
    verb=query_verb();
    
    if ( (verb == "schlafe" || verb == "schlaf") && str == "ein" ){
        write("DU KANNST DEM TOD NICHT ENTRINNEN!\n");
        return 0;
    }
    
    if ( verb == "typo" || verb == "fehler" || verb == "bug" || verb == "idee" )
        return 0;

    write( "Dein Koerper gehorcht Dir nicht !\n" );
    return 1;
}

public int no_attack()
{
  int i;

  // Spieler haengt noch in der Todessequenz
  if( present(previous_object()) )
    return 1;
  else
    previous_object()->Set( P_NO_ATTACK, 0, F_QUERY_METHOD );

  return 0;
}

public int no_poison(int val)
{
  if ( val != 0 )
  {
    catch( raise_error("Poisoning dead players is illegal. Calling object "
      "was "+object_name(previous_object(1))+"\n");publish );
  }
  return 0;
}


#pragma strong_types,save_types

#include <defines.h>
#include <moving.h>
#include <properties.h>
#include <language.h>

// 
// Da der Shadow sich nicht im Player befindet, wird ein
// Zusatzobjekt benoetigt.
// 7.10.97 - Die Hook sollte in den Verzeichnissen der Benniequest
//           zu finden sein. - Rumata

#define HOOK "d/wald/kassandra/bennie/obj/pony_hook"

static object pl;

void create()
{
 if( IS_BLUE(ME) ) return;
  shadow( PL, 1);
 pl=PL;
 //tell_object(this_player(),"Du besteigst das Pony samt Schatten.\n");
  clone_object( HOOK ); // Steuerung des shadows ueber die hook
}

string _query_title()
{ 
 if( pl->QueryProp(P_GENDER) == FEMALE )
     return "die Ponyreiterin";
 return "der Ponyreiter";
}

string _query_msgin() { return "reitet herein"; }
string _query_msgout() { return "reitet"; }

varargs int move(mixed dest, int method, string dir, string textout,string textin)
{
 if( !(method & M_NOCHECK) && dest->QueryProp(P_INDOORS) )
 {
  write( "Das Pony weigert sich, dorthin zu traben.\n" );
  return 1;
 }
 return pl->move( dest, method, dir, textout, textin );
}

int _inventory(string str)
{
 if( !environment() || set_light(0)>0 )
  write( "Du sitzt auf einem Pony.\n" );
 return pl->_inventory(str);
}

void absteigen() {
  unshadow();
  destruct(ME);
}

int QueryReiter() { return 1; }

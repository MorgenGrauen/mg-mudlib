inherit "std/room";
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>

void create()
{
  ::create();
  SetProp(P_LIGHT, 1 );
  SetProp(P_INT_SHORT, "Bei den inaktiven Spielern :)" );
  SetProp(P_INT_LONG,
    "Hier ist absolut nichts zu sehen, die, die sich hier normalerweise\n"
   +"aufhalten, sind eh noninteractive...\n");
  AddCmd( "", "onlywelt", 1 );
}

int onlywelt( string s ) {
  if( query_verb()=="welt" ) {
    this_player()->move("/gilden/abenteurer", M_TPORT);
    return 1;
  }
  if( IS_LEARNER(this_player()) ) return 0;
  write(
   "Wenn Du das magische Kommando 'welt' gibst, wirst Du wieder in Deine Welt\n"
   +"zurueckversetzt werden.\n"
  );
  return 1;
}

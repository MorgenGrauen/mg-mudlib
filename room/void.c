inherit "std/room";
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>

void create()
{
  ::create();
  SetProp(P_LIGHT, 1 );
  SetProp(P_INT_SHORT, "Das Nichts" );
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_LONG,
    "Du schwebst im absoluten, ewigen und leeren Nichts umher. Hier kommen\n"
   +"all die hin, deren Welt, in der sie sich befanden, zerstoert worden ist.\n"
   +"Wenn Du das magische Kommando 'welt' gibst, wirst Du wieder in Deine Welt\n"
   +"zurueckversetzt werden.\n"
  );
  AddCmd( "", "onlywelt", 1 );
}

int onlywelt( string s ) {
  if( query_verb()=="welt" ) {
    this_player()->move("/gilden/abenteurer", M_TPORT);
    return 1;
  }
  if( IS_LEARNER(this_player()) ) return 0;
  write(
    "Du schwebst im absoluten, ewigen und leerem Nichts umher. Hier kommen\n"
   +"all die hin, deren Welt, in der sie sich befanden, zerstoert worden ist.\n"
   +"Wenn Du das magische Kommando 'welt' gibst, wirst Du wieder in Deine Welt\n"
   +"zurueckversetzt werden.\n"
  );
  return 1;
}


// Sonst zerstoert sich das Void brav selbst - an NotifyDestruct vorbei. 
public varargs int remove()
{
  if (!ARCH_SECURITY || process_call())
    return 0;
  return ::remove();
}

// Nicht jeder Magier muss das Void entsorgen koennen.
string NotifyDestruct(object caller) {
    if( (caller!=this_object() && !ARCH_SECURITY) || process_call() ) {
      return "Du darfst das Void nicht zerstoeren!\n";
    }
}


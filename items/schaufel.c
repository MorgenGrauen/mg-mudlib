#pragma strong_types,rtt_checks

inherit "/std/thing";

#include <properties.h>
#include <language.h>

void create()
{
  ::create();
  SetProp(P_SHORT,"Eine Schaufel");
  SetProp(P_LONG,"Du kannst versuchen, mit dieser Schaufel zu graben.\n");
  AddId("schaufel");
  AddId("\nschaufel");
  SetProp(P_NAME, "Schaufel");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_VALUE, 50);
  SetProp(P_WEIGHT, 1200);
	AddCmd( ({"grab","grabe","buddel","buddele"}), "grab" );
}

int grab(string str) 
{
  _notify_fail("Du kannst hier nicht graben.\n");
  return 0;
}

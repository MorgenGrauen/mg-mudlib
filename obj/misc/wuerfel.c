inherit "std/thing";

#include <language.h>
#include <properties.h>

void create()
{
  // Short und Value werden von Laeden in der Blueprint erwartet
  SetProp(P_SHORT, "Ein Wuerfel");
  SetProp(P_VALUE, 1);
  
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_LONG, "Ein Wuerfel mit 6 Seiten.\n");
  SetProp(P_NAME, "Wuerfel");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 1);
  AddId("wuerfel");
	AddCmd("wuerfel","wuerfel");
}

int wuerfel(string str)
{
  int zahl;

  zahl = random(6) + 1;
  write("Du wuerfelst eine " + zahl + ".\n");
  say(this_player()->name() + " wuerfelt eine " + zahl + ".\n");
  return 1;
}

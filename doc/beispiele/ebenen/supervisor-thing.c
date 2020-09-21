#pragma strong_types,rtt_checks

inherit "/std/thing";

// Standard-Ebenen-Supervisor erben, Variablen nosave, die sollen hier nicht
// gespeichert werden, falls jemand save_object() ruft.
nosave variables inherit "/std/channel_supervisor";

#include <properties.h>
#include <language.h>

protected void create()
{
  ::create();
  SetProp(P_SHORT,"Eine kleine Knolle namens Lilly.");
  SetProp(P_LONG,"Die Knolle ist bestimmt magisch.\n");
  AddId("lilly");
  SetProp(P_NAME, "Lilly");
  SetProp(P_GENDER, FEMALE);
  // Das systemweite Init-File einlesen und die Zugriffsrechte fuer die Ebenen
  // merken, in denen dieses Objekt SV ist.
  ch_read_init_file();
}

public varargs string name(int casus,int demon)
{
  return thing::name(casus, demon);
}

public varargs string Name(int casus, int demon)
{
  return thing::Name(casus, demon);
}


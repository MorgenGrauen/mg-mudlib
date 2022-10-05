inherit "/std/room";

#include <properties.h>
#define BS(x)          break_string(x,79)

protected void create() {
  ::create();
  SetProp(P_HAUS_ERLAUBT,1);
  SetProp(P_INT_SHORT,"Der Abstellraum fuer heimatlose Seherhaeuser");
  SetProp(P_INT_LONG,BS("Hier landen Seherhaeuser, die keinen Platz mehr haben. Wenn "
  +"Dein Haus hier steht, wende Dich am besten bald an den zustaendigen Magier fuer die "
  +"Haeuser.\n"));
  AddDetail("magier","Zook kuemmert sich momentan um die Haeuser.\n");
  AddExit("norden","/gilden/abenteurer");
}

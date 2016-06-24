inherit "/std/room";
#include <properties.h>
#include <moving.h>

static string x;
static string *xx;

void create() {
  ::create();
  x="----------";
  xx=({
    "/d/ebene/room/ak_str7",
    "/d/ebene/room/dra_str3",
    "/d/ebene/room/hp_str2c",
    "/d/ebene/room/o_wa1d",
    "/d/ebene/room/waldweg8"
  });
  SetProp(P_LIGHT,1);
  SetProp(P_INT_LONG,
"Dieser Raum ist fuer Leute, die TF spielen lassen.\n\
Es gibt nur einen Weg heraus...\n");
  AddCmd("","raus",1);
}

void init() {
  int i;
  
  ::init();
  for (i=0;i<10;i++)
    x[i]='a'+random(26);
}

varargs string GetExits(object viewer) {
  return sprintf("Es gibt einen sichtbaren Ausgang: %s.\n",x);
}

varargs int raus(string s) {
  if (query_verb()!=x)
    return 0;
  notify_fail("Etwas hat nicht funktioniert. Bitte verstaendige einen Magier.\n");
  return ((this_player()->move(xx[random(sizeof(xx))],M_GO))>0);
}

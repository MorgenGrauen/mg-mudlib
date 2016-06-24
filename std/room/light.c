// MorgenGrauen MUDlib
//
// container/light.c -- Lichtsystemkomponenten fuer Raeume
//
// $Id: description.c 6986 2008-08-22 21:32:15Z Zesstra $

inherit "/std/container/light";

#pragma strict_types
#pragma save_types,rtt_checks
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES

#include <thing/description.h>
#include <room/description.h>
#include <container.h>

protected void create()
{
  ::create();
  SetProp(P_LIGHT_ABSORPTION, 1);
}

protected void create_super() {set_next_reset(-1);}

/*
   // das wird selten benutzt und ist zur zeit funktionsunfaehig, da die
   // interne Speicherung der Prop sich geaendert hat.
static int _set_int_light(int *light)
{
   int tmp;

   // zur Sicherheit
   if (!pointerp(light)) return -1;
   if (light[0]>QueryProp(P_LIGHT)) {
      // Licht verlaeuft sich in einem grossen Raum, daher Modifier abfragen...
      tmp=light[0]-QueryProp(P_LIGHT_ABSORPTION);
      // wenn sich das Vorzeichen geaendert hat, auf 0 setzen.
      light[0]=((tmp^light[0]) & 0x80000000 ? 0 : tmp);
   }
   if (light[1]<QueryProp(P_LIGHT) && light[1]<0) {
      // Licht verlaeuft sich in einem grossen Raum, daher Modifier abfragen...
      tmp=light[1]+QueryProp(P_LIGHT_ABSORPTION);
      // wenn sich das Vorzeichen geaendert hat, auf 0 setzen.
      light[1]=((tmp^light[1]) & 0x80000000 ? 0 : tmp);
   }
   light[2]=light[0]+light[1];
   Set(P_INT_LIGHT, light, F_VALUE);
   // diese Prop setzen kaum Leute (offiziell gehts ja auch gar nicht. Keiner
   // davon erwartet nen Rueckgabewert. Daher wird hier 0 zurueckgeben, statt
   // des aufwaendig berechneten QueryProp(P_INT_LIGHT).
   // Achja. Der Rueckgabewert vom Set() waere ein int*, was nicht geht, weil
   // diese Funktion nur int zurueckgeben darf.
   return 0;
}
*/

#include <items/kraeuter/kraeuterliste.h>
#include <items/kraeuter/kraeuter.h>
#include "../files.h"

inherit STDPLANT;

void create()
{
   ::create();
   customizeMe(WASSER_HAHNENFUSS_WEISS);
}

/*varargs int move(mixed dest, int method)
{
   // die Einwohner des Zauberwalds sehen es nicht gerne, wenn jemand ihre
   // magischen Pflanzen pflueckt...
   int res;
   res=(int)::move(dest, method);
   if (res!=1) return res;
   if (environment()) 
     environment()->SetProp(ZAUBERWALD, time()+AGGRESSIVE_TIME);
   return 1;
}*/

protected void NotifyMove(object dest, object oldenv, int method) {
  if ( !oldenv && objectp(dest) && query_once_interactive(dest) &&
       strstr(object_name(environment(dest)),"zauberwald")>-1) {
    dest->SetProp(ZAUBERWALD, time()+AGGRESSIVE_TIME);
  }
  return ::NotifyMove(dest, oldenv, method);
}


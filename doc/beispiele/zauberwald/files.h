// (c) by Padreic (Padreic@mg.mud.de)

#define HOME(x) "/doc/beispiele/"+x
#define WALD(x) HOME("zauberwald/"+x)
#define ROOM(x) WALD("room/"+x)
#define NPC(x)  WALD("npc/"+x)
#define OBJ(x)  WALD("obj/"+x)

#include <defines.h>
#include <properties.h>
#include "/p/service/padreic/mnpc/mnpc.h"

#define BS(x) break_string(x, 78)
#define PO    previous_object()
#define DEBUG(x) tell_object(find_player("padreic"), x);

#define SAECKCHEN "/d/wald/padreic/obj/saeckchen.c"


#define ZAUBERWALD "\npadreiczauberwald" /* gesetzt wenn man im wald ist */
#define AGGRESSIVE_TIME 900              /* zeit bis zur befriedung */
#define ZAUBERWALDNPC WALDID("npc")
#define AUSGANG     "\npadreicausgang"   /* die letzte Bewegung */
#define WALDID(x)   ZAUBERWALD+x
#define EXTRA_LONG   "\npadreiczusatzlong" /* zur ergaenzung zur VC-long */

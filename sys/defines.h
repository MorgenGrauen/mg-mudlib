// MorgenGrauen MUDlib
//
// defines.h -- Nuetzliche und oft gebrauchte Definitionen
//
// $Id: defines.h 7265 2009-08-21 18:13:22Z Zesstra $

#ifndef _DEFINES_H_
#define _DEFINES_H_

// defines.h darf leider keine Standard-Includeverzeichnisse benutzen, weils
// es vom Master selber benutzt wird.
#include "/sys/break_string.h"

#ifndef ME
#define ME this_object()
#endif

#ifndef PL
#define PL this_player()
#endif

#ifndef RPL
#define RPL this_interactive()
#endif

#ifndef CAP
#define CAP(str) capitalize(str)
#endif

#ifndef QPP
#define QPP QueryPossPronoun
#endif

#define REAL_UID(x)   (explode(getuid(x),  ".")[<1])
#define REAL_EUID(x)  (explode(geteuid(x), ".")[<1])

// obsolet, nur aus Kompatibilitaetsgruenden noch vorhanden
#ifndef IS_CLONE
#define IS_CLONE(ob) (clonep(ob))
#endif

// obsolet, nur aus Kompatibilitaetsgruenden noch vorhanden
#ifndef IS_BLUE
#define IS_BLUE(ob) (!clonep(ob))
#endif

#ifndef BLUE_NAME
#define BLUE_NAME(ob) load_name(ob)
//#define BLUE_NAME(ob) (explode(object_name(ob),"#")[0])
#endif

#ifndef IS_PARA
#define IS_PARA(ob) (sizeof(regexp(({to_string(ob)}),"\\^[1-9][0-9]*$")))

#endif

// obsolet, nur aus Kompatibilitaetsgruenden noch vorhanden
#ifndef MAX
#define MAX(a,b) max(a,b)
#endif

#endif /* _DEFINES_H_ */


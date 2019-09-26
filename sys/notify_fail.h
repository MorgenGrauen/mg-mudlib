// MorgenGrauen MUDlib
//
// notify_failh -- sym. Konstanten fuer notify_fail()
//
// $Id: defines.h 6415 2007-08-10 19:24:04Z Zesstra $

#ifndef _NOTIFY_FAIL_H_
#define _NOTIFY_FAIL_H_

// Prioritaeten / 'Nice levels' von notify_fail()s:
// Wie 'nice' bin ich? Je groessere Zahl/Prioritaet, desto wenig lieb. ;-)
#define NF_NL_NONE   -1   //max 'niceness', wird von allem ueberschrieben
#define NF_NL_OWN    100  //eigenes Objekt (soul verben) ueberschreiben kaum was
#define NF_NL_THING  100000
#define NF_NL_ROOM   1000000 // Raeume ueberschreiben sonstigen Krams
#define NF_NL_LIVING 10000000 // Lebewesen ueberschreiben auch Raeume
#define NF_NL_MAX    __INT_MAX__ //hoechste Prioritaet, ueberschreibt alles

#endif /* _NOTIFY_FAIL_H_ */


// MorgenGrauen MUDlib
/** \file /std/util/cidr.c
* Funktionen zur Interpretation und Umrechnung von IP-Adressen in CIDR-Notation.
* \author Zesstra
* \date 05.02.2010
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma no_shadow
#pragma pedantic
#pragma range_check

// Never do this at home. ;-)
// Der Hintergrund ist, dass der Master moeglichst wenig Code von ausserhalb
// /secure/ erben/inkludieren sollte, ich aber nur an einer Stelle Aenderungen
// vornehmen moechte...
#define private public
#include "/secure/master/cidr.c"
#undef private

//string IPv4_test(string addr) {return (IPv4_int2addr(IPv4_addr2int(addr)));}


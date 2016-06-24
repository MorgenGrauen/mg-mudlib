// MorgenGrauen MUDlib
//
// weapon/description.h -- Description for damaged Weapons
//
// $Id: description.h 6081 2006-10-23 14:12:34Z Zesstra $

#ifndef __WEAPON_DESCRIPTION_H__
#define __WEAPON_DESCRIPTION_H__

// properties
#define P_DAM_DESC           "dam_desc"

// Default-Messages
#define DFLT_DAM_DESC ({"ist in einem hervorragenden Zustand",\
                        "ist in einem sehr guten Zustand",\
                        "ist in einem guten Zustand",\
                        "sieht noch ganz brauchbar aus",\
                        "wirkt etwas abgenutzt",\
                        "wirkt stark abgenutzt",\
                        "ist gerade noch zu gebrauchen",\
                        "wird bald schrottreif sein",\
                        "ist nur noch Schrott" })

#endif // __WEAPON_DESCRIPTION_H__

#ifdef NEED_PROTOTYPES

#ifndef __WEAPON_DESCRIPTION_H_PROTO__
#define __WEAPON_DESCRIPTION_H_PROTO__

// prototypes
string dam_descr();

#endif // __WEAPON_DESCRIPTION_H_PROTO__

#endif // // NEED_PROTOYPES

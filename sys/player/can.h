// MorgenGrauen MUDlib
//
// player/can.h -- various abitlities
//
// $Id: can.h 7924 2011-09-26 10:02:26Z Zesstra $

#ifndef __CAN_H__
#define __CAN_H__

// properties
#define P_CAN_FLAGS          "can_flags"

// bitmasks
#define CAN_EMOTE  0x01
#define CAN_ECHO   0x02
#define CAN_REMOTE 0x04
#define CAN_PRESAY 0x08
#define CAN_REPORT_SP     0x10     // Statusupdates fuer KP
#define CAN_REPORT_POISON 0x20     // Statusupdates fuer Gift
#define CAN_REPORT_WIMPY  0x40     // Statusupdates fuer Vorsicht
#define CAN_REPORT_WIMPY_DIR 0x80  // Statusupdates fuer Vorsichtrichtung

#endif // __CAN_H__

#ifdef NEED_PROTOTYPES

#ifndef __CAN_H_PROTO__
#define __CAN_H_PROTO__

// no prototypes

#endif // __CAN_H_PROTO__

#endif	// NEED_PROTOYPES

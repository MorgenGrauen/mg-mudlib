// MorgenGrauen MUDlib
//
// room/description.h -- standard room header
//
// $Id: description.h 9190 2015-04-02 14:29:36Z Arathorn $
 
#ifndef __ROOM_DESCRIPTION_H__
#define __ROOM_DESCRIPTION_H__

// properties

#define P_INT_SHORT          "int_short"
#define P_INT_LONG           "int_long"

#define P_ROOM_MSG           "room_msg"
#define P_FUNC_MSG           "func_msg"
#define P_MSG_PROB           "msg_prob"

#define P_HAUS_ERLAUBT       "haus_erlaubt"

#define P_NO_TPORT           "tport"
#define P_TPORT_COST_IN      "tport_cost_in"
#define P_TPORT_COST_OUT     "tport_cost_out"
#define P_INDOORS            "indoors"

#define P_NEVER_CLEAN        " never clean "
#define P_ROOM_TYPE          "room_type"
#define P_NO_PLAYERS         "no_players"

#define P_NO_PARA_TRANS      "no_para_trans"
#define P_KEEPER             "shop_keeper"    // Eigentuemer von Pub oder Shop
#define P_BUY_ONLY_PLANTS    "lib_p_buy_only_plants"
#define P_ORAKEL             "orakel"

#define P_LIGHT_ABSORPTION   "light_absorption" // Licht das der Raum schluckt

#define P_DOMAIN             "lib_p_domain"   // Domain, in der der Raum ist.

#endif // __ROOM_DESCRIPTION_H__

#ifdef NEED_PROTOTYPES

#ifndef __ROOM_DESCRIPTION_H_PROTO__
#define __ROOM_DESCRIPTION_H_PROTO__

// prototypes
// no prototypes

#endif // __ROOM_DESCRIPTION_H_PROTO__

#endif	// NEED_PROTOYPES

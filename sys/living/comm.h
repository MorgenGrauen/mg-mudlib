// MorgenGrauen MUDlib
//
// living/comm.h -- communication module properties for Livings
//
// $Id: comm.h,v 3.3 2000/02/09 15:56:25 Padreic Exp $

#ifndef __LIVING_COMM_H__
#define __LIVING_COMM_H__

#include <break_string.h>

// Flags fuer ReceiveMsg().
// The BS-Flags MUST always be equal to the one defined for break_string!
#define MSG_BS_LEAVE_LFS    BS_LEAVE_MY_LFS    //1
#define MSG_BS_SINGLE_SPACE BS_SINGLE_SPACE    //2
#define MSG_BS_BLOCK        BS_BLOCK           //4
#define MSG_BS_NO_PARINDENT BS_NO_PARINDENT    //8
#define MSG_BS_INDENT_ONCE  BS_INDENT_ONCE     //16
#define MSG_BS_PREP_INDENT  BS_PREPEND_INDENT  //32
// Flags < 1024 (2^10) reserviert fuer BS
#define MSG_DONT_BUFFER  1024
#define MSG_DONT_STORE   2048
#define MSG_DONT_WRAP    4096
#define MSG_DONT_IGNORE  8192
// Flags < 1048576 (2^20) reserviert fuer Flags
#define MSG_ALL_BS_FLAGS (BS_LEAVE_MY_LFS | BS_SINGLE_SPACE | BS_BLOCK\
                          | BS_NO_PARINDENT | BS_INDENT_ONCE\
                          |BS_PREPEND_INDENT)
#define MSG_ALL_FLAGS (MSG_DONT_BUFFER | MSG_DONT_STORE | MSG_DONT_WRAP\
                      | MSG_DONT_IGNORE | MSG_ALL_BS_FLAGS)

// Nachrichtentypen, Werte ab 2^20
#define MT_UNKNOWN    0
#define MT_LOOK       1048576
#define MT_LISTEN     2097152
#define MT_FEEL       4194304
#define MT_TASTE      8388608
#define MT_SMELL      16777216
#define MT_MAGIC      33554432
#define MT_NOTIFICATION 67108864
#define MT_COMM       134217728
#define MT_FAR        268435456
#define MT_DEBUG      536870912
#define MT_NEWS       1073741824

// Aktionen
// Bei MA_UNKNOWN wird die Aktion aus query_verb() geraten. Wenn das nicht
// existiert, bleibt die Aktion MA_UNKNOWN.
#define MA_UNKNOWN    0
#define MA_PUT        "wirf"
#define MA_TAKE       "nimm"
#define MA_GIVE       "gib"
#define MA_MOVE_IN    "move_in"
#define MA_MOVE_OUT   "move_out"
#define MA_MOVE       "move"
#define MA_FIGHT      "toete"
#define MA_WIELD      "zuecke"
#define MA_UNWIELD    "stecke"
#define MA_WEAR       "trage"
#define MA_UNWEAR     "ziehe"
#define MA_EAT        "iss"
#define MA_DRINK      "trinke"
#define MA_SPELL      "zaubere"
#define MA_LOOK       "untersuche"
#define MA_LISTEN     "lausche"
#define MA_FEEL       "taste"
#define MA_SMELL      "rieche"
#define MA_SENSE      "erspueren"
#define MA_READ       "lies"
#define MA_USE        "benutze"
#define MA_SAY        "sage"
#define MA_REMOVE     "remove"
#define MA_CHANNEL    "ebenen"
#define MA_EMOTE      "emote"
#define MA_SHOUT      "rufe"
#define MA_SHOUT_SEFUN "shout"
#define MA_TEAMRUF    "teamruf"
#define MA_TELL       "teilemit"

// Rueckgabewerte von ReceiveMsg()
#define MSG_DELIVERED    1
#define MSG_BUFFERED     2
#define MSG_FAILED      -1
#define MSG_IGNORED     -2
#define MSG_VERB_IGN    -3
#define MSG_MUD_IGN     -4
#define MSG_SENSE_BLOCK -5
#define MSG_BUFFER_FULL -6

#endif // __LIVING_COMM_H__

#ifdef NEED_PROTOTYPES

#ifndef __LIVING_COMM_H_PROTO__
#define __LIVING_COMM_H_PROTO__

public varargs int ReceiveMsg(string msg, int msg_type, string msg_action,
                              string msg_prefix, object origin);

#endif // __LIVING_COMM_H_PROTO__

#endif // NEED_PROTOYPES


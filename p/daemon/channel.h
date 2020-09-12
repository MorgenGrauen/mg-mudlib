// ch.h - channel daemon defines
// --
// $timestamp::$

#ifndef __DAEMON_CHANNEL_H__
#define __DAEMON_CHANNEL_H__

#define P_CHANNELS        "channels"
#define P_STD_CHANNEL     "std_channel"
#define P_SWAP_CHANNELS   "swap_channels"

#define CHMASTER          "/p/daemon/channeld"
#define CMNAME            "<MasteR>"
#define DEFAULTSV         "/p/daemon/channel-sv"
#define DEFAULTSVNAME     "Merlin"

// Message types
#define MSG_SAY           0
#define MSG_EMOTE         1
#define MSG_GEMOTE        2
#define MSG_EMPTY         3

// Errors
#define E_ACCESS_DENIED  -1
#define E_ALREADY_JOINED -2
#define E_NOT_MEMBER     -3
#define E_EMPTY_MESSAGE  -4
#define E_UNKNOWN_TYPE   -5

// Commands
#define C_NEW	            "new"
#define C_JOIN            "join"
#define C_LEAVE           "leave"
#define C_SEND            "send"
#define C_LIST            "list"
#define C_FIND            "find"

// Ebenen-Flags, die Verhalten von Ebenen steuern (vom CHANNELD verwendet)
#define CHF_FIXED_SUPERVISOR 1   // Kein Wechsel des SV erlaubt

// Flags fuer Zugriffsverwaltung, nur benutzt von den Ebenen-Supervisoren
// F_WIZARD kennzeichnet reine Magierebenen
#define CH_ACCESS_WIZARD 1
// Ebenen, auf denen keine Gaeste erlaubt sind, sind mit F_NOGUEST markiert.
#define CH_ACCESS_NOGUEST 2

#endif //__DAEMON_CHANNEL_H__

// prototypes
#ifdef NEED_PROTOTYPES

#ifndef __CHANNEL_H_PROTO__
#define __CHANNEL_H_PROTO__
public varargs int new(string ch_name, object owner, string|closure desc,
                       int channel_flags);
public varargs int send(string chname, object sender, string msg, int type);

#endif //__CHANNEL_H_PROTO__

#endif //NEED_PROTOTYPES

// ch.h - channel daemon defines
// --
// $timestamp::$

#ifndef __DAEMON_CHANNEL_H__
#define __DAEMON_CHANNEL_H__

#define P_CHANNELS		"channels"
#define P_STD_CHANNEL		"std_channel"

#define CHMASTER		"/p/daemon/channeld"

// Message types
#define MSG_SAY                 0
#define MSG_EMOTE               1
#define MSG_GEMOTE              2
#define MSG_EMPTY               3

// Errors
#define E_ACCESS_DENIED         -1
#define E_ALREADY_JOINED        -2
#define E_NOT_MEMBER            -3
#define E_EMPTY_MESSAGE         -4
#define E_UNKNOWN_TYPE          -5

// Commands
#define C_NEW			"new"
#define C_JOIN			"join"
#define C_LEAVE			"leave"
#define C_SEND			"send"
#define C_LIST			"list"
#define C_FIND			"find"

// definition of the list mapping entry 
// ([ channelname : ({ I_MEMBER, I_ACCESS, I_INFO, I_MASTER, I_NAME }) ])
#define I_MEMBER                0
#define I_ACCESS                1
#define I_INFO                  2
#define I_MASTER		3
#define I_NAME			4

#endif //__DAEMON_CHANNEL_H__

// prototypes
#ifdef NEED_PROTOTYPES

#ifndef __CHANNEL_H_PROTO__
#define __CHANNEL_H_PROTO__
varargs int new(string ch, object pl, mixed info);
varargs int send(string ch, object pl, string msg, int type);

// ok, keine Prototypen, aber trotzdem nur fuer channeld.c interessant.
#define MAX_HIST_SIZE   200
#define MAX_CHANNELS    90

#endif //__CHANNEL_H_PROTO__

#endif //NEED_PROTOTYPES


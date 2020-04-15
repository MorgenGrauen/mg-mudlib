// MorgenGrauen MUDlib
//
// player/comm.h -- communication module properties
//
// $Id: comm.h,v 3.3 2000/02/09 15:56:25 Padreic Exp $

#ifndef __PLAYER_COMM_H__
#define __PLAYER_COMM_H__

// living/comm.h brauchen wir auch.
#include <living/comm.h>

// properties

#define P_INTERMUD           "intermud" // Does she listen to it ?
#define P_BUFFER             "buffer"   // ignore-, tell-buffer
#define P_DEAF               "deaf"     // Taubheit
#define P_PERM_STRING        "perm_string" // Fuer Sprachflueche
#define P_MESSAGE_BEEP       "message_beep" // wenn gesetzt 
                                            // Anzahl der Sekunden dazwischen
#define P_MESSAGE_PREPEND  "message_prepend"
// Will der Magier MT_DEBUG sehen?
#define P_WIZ_DEBUG          "std_p_wizdebug"

#define MESSAGE_BEEP "\007"

// definitions for Message()

#define MSGFLAG_CHANNEL 0x0001	// messages from channeld.c
#define MSGFLAG_CHIST	0x0002
#define MSGFLAG_SOUL	0x0004	// messages from soul.c
#define MSGFLAG_REMOTE	0x0008	// remote emotes
#define MSGFLAG_SHOUT	0x0010	// communication commands
#define MSGFLAG_TELL	0x0020
#define MSGFLAG_SAY	0x0040
#define MSGFLAG_WHISPER 0x0080
#define MSGFLAG_MECHO	0x0100
#define MSGFLAG_RTELL	0x0200	// tell from other mud

#define MAX_SAVED_MESSAGES  50
#define MAX_SAVED_CHATS     20

// Was geht in den Kobold?
#define MSGFLAG_CACHE	(MSGFLAG_TELL | MSGFLAG_REMOTE | MSGFLAG_WHISPER | MSGFLAG_RTELL)

#define MSGFLAG_DEAFCHK (MSGFLAG_SHOUT | MSGFLAG_TELL | MSGFLAG_SAY | MSGFLAG_WHISPER | MSGFLAG_CHANNEL | MSGFLAG_RTELL)

// Rueckgabewerte von Message()
#define MESSAGE_OK	     1	// alles klar
#define MESSAGE_CACHE	     0	// Meldung steht im Kobold
#define MESSAGE_IGNORE_YOU  -1	// Meldung/Sender wird ignoriert
#define MESSAGE_IGNORE_VERB -2
#define MESSAGE_IGNORE_MUD  -3
#define MESSAGE_DEAF	    -4	// Empfaenger hoert nicht zu
#define MESSAGE_CACHE_FULL  -5	// Kobold kann nichts mehr merken

// Flags fuer Statusreports
#define DO_REPORT_HP         0x1
#define DO_REPORT_SP         0x2
#define DO_REPORT_POISON     0x4
#define DO_REPORT_WIMPY      0x8
#define DO_REPORT_WIMPY_DIR  0x10

#endif // __PLAYER_COMM_H__

#ifdef NEED_PROTOTYPES

#ifndef __PLAYER_COMM_H_PROTO__
#define __PLAYER_COMM_H_PROTO__

// prototypes

protected varargs int _recv(object ob, string message,
                            int flag, string indent);

varargs int Message(string msg, int flag, string indent,
                    string cname, mixed sender);

protected void status_report(int type, mixed val);
protected void _notify(string msg, string action);
static void modify_prompt();

#endif // __PLAYER_COMM_H_PROTO__

#endif // NEED_PROTOYPES


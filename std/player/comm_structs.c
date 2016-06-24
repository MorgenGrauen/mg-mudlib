// MorgenGrauen MUDlib
//
// player/comm.c-- basic player communiction commands
//
// $Id: comm.c 6918 2008-08-07 21:13:16Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

struct msg_s {
  string msg;       // Inhalt der Nachricht
  int type;         // Messagetyp fuer ReceiveMsg
  string action;    // Messageaction fuer ReceiveMsg
  string prefix;    // Einrueckung der Nachricht bei Darstellung (msg_prefix)
  string sendername;// Ursprung der Nachricht
};

struct stored_msg_s (msg_s) {
  int timestamp;    // Zeitstempel der Nachricht
};

struct msg_buffer_s {
  //struct msg_s *buf;
  mixed *buf;
  int index;
};

struct chat_s {
  string uid;           // UID des Gespraechspartners
  int time_first_msg;   // Zeit der ersten Nachricht
  int time_last_msg;    // Zeit der letzen Nachricht
  int sentcount;        // Anzahl gesendeter Nachrichten
  int recvcount;        // Anzahl empfangener Nachrichten
  mixed msgbuf;         // Array von msg_s (Art Ringpuffer)
  int ptr;              // Pointer auf die naechste zu ueberschreibende msg_s
                        // in msgbuf
};


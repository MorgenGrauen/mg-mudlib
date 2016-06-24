// MorgenGrauen MUDlib
//
// npc/comm.c -- Basiskommunikation fuer NPCs
//
// $Id: comm.c 9358 2015-10-22 18:35:04Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/living/comm";

#include <language.h>
#include <living/comm.h>
#define NEED_PROTOTYPES
#include <thing/description.h>


void create() {
  add_action( "sage", "sag", 1 );
  add_action( "echo", "echo" );
  add_action( "emote", "emote" );
}

int echo( string str ) {
  say( str + "\n" );
  return 1;
}

int sage( string str ) {
  say( break_string(str, 78, capitalize(name(WER,2))+" sagt: "));
  return 1;
}

int emote( string str ) {
  say( capitalize(name(WER,2))+" "+str+"\n" );
  return 1;
}

// zum ueberschreiben - DEPRECATED! USE ReceiveMsg()!
public void catch_msg(mixed *arr, object obj) {}
public void catch_tell(string str) {}

// by default, the msg is delivered to catch_tell() for compatibility reasons
// and otherwise ignored.
public varargs int ReceiveMsg(string msg, int msg_typ, string msg_action,
                              string msg_prefix, object origin)
{
  // compatibility...
  if (msg_typ & MSG_DONT_WRAP)
    catch_tell(sprintf("%s%s", msg_prefix||"", msg));
  else
    catch_tell(sprintf("%s%s\n", msg_prefix||"", msg));
  return MSG_DELIVERED;
}


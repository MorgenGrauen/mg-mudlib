// channel-sv.c
//
// Standard-SV der Mudlib

#pragma strong_types,rtt_checks
#pragma no_shadow,no_clone, no_inherit

#include "channel.h"

inherit "/std/channel_supervisor";

protected void create()
{
  ::create();
  // Namen setzen, der auf den Ebenen genannt werden soll.
  ch_set_sv_name(DEFAULTSVNAME);
  // Das systemweite Init-File einlesen und die Zugriffsrechte fuer die Ebenen
  // merken, in denen dieses Objekt SV ist.
  ch_read_init_file();
}


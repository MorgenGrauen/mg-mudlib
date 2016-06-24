// MorgenGrauen MUDlib
//
// virtual_compiler.c -- Virtueller Compiler fuer die Todesraeume
//
// $Id.$


#pragma strong_types

inherit "/std/virtual/v_compiler";

#include <defines.h>
#include <v_compiler.h>

int NoParaObjects() {return 1;}

void create() 
{
  if (IS_CLONE(this_object())) return;
 ::create();
 SetProp(P_STD_OBJECT, "/room/death/death_room_vc");
 seteuid(getuid());
 return;
}

string Validate(string file) {
  string base, room, who;

 file = ::Validate(file);
 if(sscanf(file, "death_room_%s",who)!=1 ||
    ((who[0..3]!="gast") &&
    (!"/secure/master"->get_userinfo(who)))) return 0;
 return file;
}

mixed CustomizeObject()
{
 string base,room,who,file,fun;

 if(!(file = ::CustomizeObject()) ||
    !Validate(file)) return 0;
 return file;
}


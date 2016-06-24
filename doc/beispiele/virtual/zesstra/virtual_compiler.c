inherit "/std/virtual/v_compiler.c";

#pragma strong_types,rtt_checks,save_types

#include <thing/properties.h>

#define NEED_PROTOTYPES
#include <v_compiler.h>
#undef NEED_PROTOTYPES

protected void create() {
    ::create();

    // jeder Spieler kriegt eine "Kopie" von std_arena als Raum.
    SetProp(P_STD_OBJECT, __DIR__"std_arena");
    SetProp(P_COMPILER_PATH, __DIR__);
}

public string Validate(string file)
{
    string raum, spieler;
    //.c abschneiden
    file=::Validate(file);
    // wenn das gewuenscht file dem Schema "arena|spielername" folgt, ist es
    // zulaessig.
    if(sscanf(file,"%s|%s",raum,spieler)==2 && raum=="arena")
       return file;

    // nicht zulaessig.
    return 0;
}


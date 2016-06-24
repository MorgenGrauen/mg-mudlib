#include <defines.h>
#include <v_compiler.h>
inherit "/std/virtual/v_compiler";

void create() {
  ::create();
  SetProp(P_STD_OBJECT, "/room/welcome/std");
}

string Validate(string file) {

  file=::Validate(file);

  // keine Unterverzeichnisse, gueltige Charnamen
  if (strstr(file,"/") == -1
      && sizeof(file) <= 11 // charnamen <=11 zeichen
      && (regmatch(file,"[a-zA-Z]+") == file // nur A-Z,a-z
          || strstr(file,"gast") == 0 // oder Gaeste
          )
     )
  {
    // Eigentlich P_COMPILER_PATH, aber hier geht jetzt auch __DIR__
    return __DIR__ + file;
  }
  return 0;
}


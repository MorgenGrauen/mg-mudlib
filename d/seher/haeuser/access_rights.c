/* 15.03.2008, Arathorn
   - Wargon und Wurzel ausgetragen in Abstimmung mit Zook */
#include "haus.h"

int access_rights(string euid, string file)
{
  // return ( euid == "vanion" );

  // Die Maintainer dieses Pakets sind im Define MAINTAINER 
  // in /d/seher/haeuser/haus.h zentral gepflegt
  return ( member(MAINTAINER, euid) !=-1);
}


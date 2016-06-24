/*
 * kommentar.c -- DIE Ergaenzung zum Bausparvertrag
 *
 * (c) 1994 Beldin@MorgenGrauen
 *
 * $Date: 1994/10/13 14:14:06 $
 * $Revision: 1.2 $
 * $Log: kommentar.c,v $
 * Revision 1.2  1994/10/13  14:14:06  Wargon
 * Ins Seherverzeichnis verlegt, angepasst und "reindented".
 *
 * Revision 1.1  1994/10/13  14:10:02  Wargon
 * Initial revision
 *
 */

inherit "std/thing";

#include <properties.h>
#include <language.h>
#include "haus.h"

create()
{
  ::create();
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Kommentar");
  SetProp(P_WEIGHT, 500);
  SetProp(P_VALUE, 0);
  SetProp(P_NAME,"Kommentar");
  SetProp(P_LONG,
     "\nDies ist ein Kommentar. Er besteht aus 100%-recyclefaehigem Altpapier, und\n"
    +"ist chlorfrei gebleicht. Das Papier scheint duenn zu sein.\n"
    +"Vielleicht solltest Du ihn schnell lesen!\n\n"
     );

  AddId( ({"kommentar", "kommentierung"}));
  AddCmd("lies","lesen");
}

rem()
{
  call_out("remove", 2);
  tell_object(environment(), "\n\nDer Kommentar loest sich in Wohlgefallen auf.\n\n");
  return 1;
}

lesen(str)
{
  string komment;
  komment=PATH+"txt/kommentar.txt";
  if (!str) return 0;
  if (id(str)) {
    this_player()->More(komment,1,#'rem);
    return 1;
    }
  else {
    notify_fail("Was willst Du lesen???\n");
    return 0;
  }
}


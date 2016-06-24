// (c) 2001 by Padreic (Padreic@mg.mud.de)
// beschreibung magdalena@morgengrauen.de - 8.12.2003

#pragma strong_types,rtt_checks

#include <properties.h>
#include <items/kraeuter/kraeuter.h>
#include <items/kraeuter/kraeuterliste.h>

inherit STDPLANT;

#define BS(x) break_string(x, 78)


void create()
{
  ::create();
  customizeMe(GLOCKENBLUME);
  SetProp(P_NAME,     "Glockenblume");
  SetProp(P_NAME_ADJ, "schoen");
  SetProp(P_GENDER,   FEMALE);
  SetProp(P_LONG, BS(
    "Die Bluetensterne in blau-violett sind ein toller Blickfang, wie "
   +"sie am langen Stiel im Wind hin- und hernicken. Die "
   +"glockenfoermige Blumenkrone mit ihren fuenf breiten, nicht bis zur "
   +"Mitte reichenden Zipfeln erfreut Dich mit ihrer Schoenheit."));
  SetProp(PLANT_ROOMDETAIL, 
    "Eine anmutige, schoene Glockenblume nickt dir auffordernd zu. Ob man sie "
   +"pfluecken kann?\n");
  SetProp(P_SHORT,    "Eine Glockenblume");
  AddId(({ "blume", "glockenblume" }));
  
  AddDetail(({"blickfang","schoenheit"}),BS(
     "Diese Glockenblume besticht durch ihre Schoenheit. Ihre Farben sind " 
    +"sind ein wahrer Blickfang.\n"));
  AddDetail(({"farben","blueten"}),BS(
     "Die Blueten der Glockenblume sind blau-violett.\n"));
  AddDetail("blumenkrone",BS(
     "Die Blume ist glockenfoermig. Daher hat die Pflanze ihren Namen.\n"));
  AddDetail("pflanze",BS(
     "Diese Pflanze ist eindeutig eine Glockenblume.\n"));
  AddDetail(({"bluetensterne","sterne","zipfel","zipfeln"}),BS(
     "Bemerkenswert an der Pflanze sind die Bluetensterne. Die blau-violetten "
    +"Zipfel bilden die Bluete der Pflanze, was sehr auffaellig ist. Diese "
    +"Bluetensterne gaben der Blume ihren Namen.\n"));

}



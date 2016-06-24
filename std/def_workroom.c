// MorgenGrauen MUDlib
//
// def_workroom.c -- standard workroom for new wizards
//
// $Id: def_workroom.c 7423 2010-02-07 22:56:38Z Zesstra $

#pragma strong_types,save_types,rtt_checks
#pragma pedantic, range_check

inherit "std/room";

#include <properties.h>

void create()
{
  string WIZNAME;

  ::create();
  WIZNAME = capitalize(getuid(this_object()));

  SetProp( P_LIGHT, 1 );
  SetProp( P_INT_SHORT, "Der Arbeitsraum von "+WIZNAME );
  SetProp( P_INT_LONG,
   "Dieses ist der Arbeitsraum von "+WIZNAME+". Es liegen vollgeschriebene\n"
  +"Zettel auf dem Boden, die Waende sind mit Kreidezeichen zugekritzelt.\n"
  +"Man merkt, dass dieser Raum dafuer da ist, dass "+WIZNAME+" fuer sich\n"
  +"ungestoert arbeiten kann. Ein magisches Portal fuehrt in die Welt\n"
  +"der Sterblichen.\n"
	  );
  SetProp(P_INDOORS,1);
  AddDetail( ({ "portal", "portale" }), 
	    "Das Portal fuehrt in die Abenteurer-gilde.\n" );
  AddDetail( ({ "raum", "arbeitsraum" }),
	     "Du befindest Dich mittendrin.\n" );
  AddDetail( "zettel",
	     "Die Zettel sind mit unleserlichen Zeichen vollgeschrieben.\n" );
  AddDetail( ({ "boden", "fussboden" }),
	     "Der Fussboden ist bedeckt mit vollgeschriebenen Zetteln.\n" );
  AddDetail( ({ "wand", "waende" }),
	     "An den Waenden befinden sich Kreidezeichen.\n" );
  AddDetail( ({ "kreide", "kreidezeichen", "zeichen" }),
	    "Die Zeichen ergeben nur fuer " + WIZNAME + " einen Sinn.\n" );
  AddDetail( "portal",
	     "Das Portal fuehrt in die Welt der Sterblichen.\n" );
  AddExit("gilde","in die Gilde#/gilden/abenteurer");
}

#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <language.h>
inherit "/std/room";
create(){
   ::create();
   SetProp(P_INT_SHORT,"Das juengste Geruecht");
   SetProp(P_INT_LONG,
	   "Dies ist ein Raum, den die Spieler gerne und oft als \n"
	   +"'das juengste Geruecht' bezeichnen. Hier werden naem-\n"
	   +"lich wichtige Entscheidungen im Morgengrauen disku-\n"
	   +"tiert und beschlossen. Demzufolge ist dies ein reiner\n"
	   +"Magierraum. Man kann sich hier treffen, reden oder\n"
	   +"einfach herumidlen.\n");
   AddExit("unten", "/gilden/abenteurer");
   SetProp(P_LIGHT,100);
   SetProp(P_INDOORS,1);
}

exit() // War schon oefter so, das Magier hier walking monsters erzeugt
       // haben, die dann in die Gilde gehopst sind ... die exits
			 // zu sperren hat auch keinen Sinn - Sirs MNPC moved sich dann
			 // doch an seinen Startpunkt. Naja, machen wir es eben SO - Monster
			 // haben hier eh nix verloren.
{
	object tp;

	tp=previous_object();
	if (!query_once_interactive(tp))
		call_out("do_destruct",1,tp);
}

do_destruct(ob)
{
	if (ob)
	  ob->remove();
	if (ob)
		destruct(ob);
}

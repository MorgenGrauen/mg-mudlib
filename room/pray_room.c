inherit "std/room";

#include <properties.h>

void create()
{
	::create();
	SetProp(P_LIGHT,1);
	SetProp(P_INT_LONG,
   "Du befindest Dich in einer kleinen Kapelle, in der Du beten kannst, um\n"+
   "wieder Deine natuerliche Gestalt zu erhalten. Am suedlichen Ende steht\n"+
   "ein Altar, auf dem sich ein grosses Kreuz befindet. In den Waenden siehst\n"+
   "Du herrliche Fenster, durch die tagsueber das Licht bunt hereinscheint.\n"+
   "Im Norden befindet sich eine Tuer, die zur Gilde fuehrt.\n");
  SetProp(P_INT_SHORT,"Kapelle");
  AddDetail("altar", "Er ist aus Stein und nicht besonders schoen.\n");
  AddDetail("kreuz", "Ein schlichtes Holzkreuz.\n");
  AddDetail("fenster", "Die Fenster sind wunderschoen.\n");
  AddDetail(({"wand", "waende"}), "An den Waenden siehst Du nichts besonderes.\n");
  AddCmd("bete","bete");
  AddExit("norden","/gilden/abenteurer");
}

static int bete() {
   if (this_player()->QueryProp(P_GHOST))
   {
        write("Du haeltst eine kurze Andacht.\n");
	write("Eine sehr starke Kraft saugt Dich auf und spuckt Dich wieder aus.\n");
	say(this_player()->name()+" erscheint in koerperlicher Form.\n");
	this_player()->SetProp(P_GHOST,0);
	return 1;
   }
   notify_fail("Du haeltst eine kurze Andacht.\n");
   return 0;
}



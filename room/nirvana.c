#include <properties.h>
#include <moving.h>

inherit "std/room";

create() {
  ::create();
  SetProp(P_INDOORS, 1);
  SetProp(P_LIGHT,1);
  SetProp(P_INT_LONG,
"Du befindest Dich im Nirvana!\n"+
"Es sieht anders aus als Du es Dir vorgestellt hast. Eine riesige Halle \n"+
"mit Waenden aus Nebel, einem Boden aus Wasser und einer Decke aus Feuer\n"+
"bildet das Heim der ewig Verstorbenen.\n"+
"Stille herrscht hier. Grosse Stille. STILLE sozusagen.\n"+
"Rauch durchzieht die Luft. Er entspringt einer Raeucherschale, welche auf\n"+
"einem riesigen Altar mitten im Nirvana steht.\n"+
"Du bist nun wohl wirklich tot.\n");
  SetProp(P_INT_SHORT,"Im Nirvana");
  SetProp(P_NO_TPORT,NO_TPORT);
  SetProp(P_NO_PARA_TRANS);
  
  AddDetail("altar",
"Der Altar der verzweifelten Toten. Er besteht vollstaendig aus grinsenden "+
"Totenschaedeln und bietet keinerlei Hoffnung.\nEine Raeucherschale steht auf dem Altar.\n");

  AddDetail(({"tote","toten","verstorbene"}),
"Dies hier ist das Heim der Toten.\nDu wohnst hier.\n");

  AddDetail(({"hoffnung"}),
"HAHA, KEINE HOFFNUNG!\n");

  AddDetail(({"schaedel","totenschaedel"}),
"Sie grinsen Dich haemisch an als wollten sie dich verspotten.\n"+
"Auf einem Schadel erkennst Du eine Inschrift.\n");

  AddDetail(({"inschrift"}),
"Du kannst sie lesen.\n");

  AddReadDetail(({"inschrift"}),
"DIES IST KEIN APRILSCHERZ.\n");

  AddDetail(({"schale","raeucherschale"}),
"Aus der Schale entsteigt dunkler Rauch, der die Luft unheilvoll schwaengert.\n");


  AddDetail(({"luft"}),
"Luft war einmal lebensnotwendig. Du brauchst sie nicht mehr.\n");

  AddDetail(({"rauch"}),
"Der Rauch verdunkelt die Luft. Er wuerde jeden Lebenden ersticken, aber das schreckt Dich nicht mehr.\n");

  AddDetail(({"stille"}),
"Sie hoert sich irgendwie tot und endgueltig an.\n");

  AddDetail(({"nirvana","raum"}),
"Schau Dich nur um.\n");

  AddDetail(({"lebende"}),
"Zu denen gehoerst Du nicht mehr.\n");

  AddDetail(({"nebel"}),
"Der Nebel begrenzt das Nirvana seitlich. Er formt die Waende.\n");

  AddDetail(({"wasser"}),
"Der Boden des Nirvanas scheint aus Wasser zu bestehen. Du bist tot und gehst daher nicht unter.\n");

  AddDetail(({"feuer"}),
"Die Decke des Nirvanas lodert Dir feurig entgegen. Waerest Du am Leben, wuerde Dich das beunruhigen.\n");

  AddDetail(({"halle"}),
"Die Halle der Toten. Das Nirvana.\n");

  AddDetail(({"heim"}),
"Das Heim der Toten ist das Nirvana. Dein Heim.\n");


  AddDetail(({"boden"}),
"Der Boden wabert unter Dir. Manchmal meinst Du das Gesicht Jofs zu erkennen.\n");

  AddDetail(({"gesicht"}),
"Gesichter sind fuer Dich nicht mehr wichtig.\n");

  AddDetail(({"jof"}),
"Er schlaeft.\n");


  AddDetail(({"decke"}),
"Die Decke lodert Dir entgegen. Abundzu meinst Du Rumata zu erkennen.\n");

  AddDetail(({"rumata"}),
"Er ist wohl doch nicht da. Niemand wird Dir helfen. Es ist aus.\n");

  AddDetail(({"wand","waende"}),
"Der Nebel wabert auf und ab. Manchmal scheint es so, als wandere Zook durch den Nebel.\n");


  AddDetail(({"zook"}),
"Er grinst Dich an. Das ist wohl das Ende der Hoffnung.\nWeit hinter Zook erkennst Du eine weitere Gestalt.\n");

  AddDetail(({"gestalt"}),
"Es scheint sich um Boing zu handeln.\n");

  AddDetail(({"boing"}),
"Boing schaut Dich traurig an.\n");


  AddDetail(({"ende"}),
"Du hast Dein Ende erreicht. Du bist tot.\n");

  AddDetail(({"leben"}),
"Dein Leben ist zu Ende. Du bist tot.\n");

  AddCmd("bete","bete");
  AddItem("/obj/zeitungsautomat", REFRESH_REMOVE);
}

bete() {
  write("Du haeltst eine kurze Andacht.\n");
  if (this_player()->QueryProp(P_GHOST) && this_player()->query_hc_play()>1) {
	write("Eine sehr starke Kraft saugt Dich auf und spuckt Dich wieder aus.\nEs scheint sich aber nichts veraendert zu haben.\n");
	say(this_player()->name()+" erscheint nicht in koerperlicher Form.\n");
	this_player()->SetProp(P_GHOST,0);
  }
  else
  {
        write("Fuer Dich ist dieser Altar nicht gedacht!\n");
  }
  return 1;
}



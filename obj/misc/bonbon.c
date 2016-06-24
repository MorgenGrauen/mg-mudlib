// Asrahel

inherit "std/thing" ;

#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <language.h>

void geschmack()
{
	int num;
	num = random (8);
	switch(num)
	{
		case 0 :	
			SetProp(P_SHORT,"Ein Orangenbonbon");
			SetProp(P_LONG,"Ein gefuelltes Bonbon mit Orangengeschmack.\n");
			SetProp(P_NAME,"Orangenbonbon");
			AddId(({"orangenbonbon"}));
			break;
		case 1 :
			SetProp(P_SHORT,"Ein Zitronenbonbon");
			SetProp(P_LONG,"Ein gefuelltes Bonbon mit Zitronengeschmack.\n");
			SetProp(P_NAME,"Zitronenbonbon");
			AddId(({"zitronenbonbon"}));
			break;
                case 2 :
			SetProp(P_SHORT,"Ein Schokoladenbonbon");
			SetProp(P_LONG,"Ein Schokoladenbonbon, pass auf, dass es nicht schmilzt.\n");
			SetProp(P_NAME,"Schokoladenbonbon");
			AddId(({"schokobonbon","schokoladenbonbon"}));
			break;
                case 3 :
			SetProp(P_SHORT,"Ein Pfefferminzbonbon");
			SetProp(P_LONG,"Pfefferminze befreit den Hals und kitzelt in der Nase.\n");
			SetProp(P_NAME,"Pfefferminzbonbon");
			AddId(({"pfefferminzbonbon","minze","pfefferminz"}));
			break;
                case 4 :
			SetProp(P_SHORT,"Ein Sahnebonbon");
			SetProp(P_LONG,"Sehr suess und sehr klebrig.\n");
			SetProp(P_NAME,"Sahnebonbon");
			AddId(({"sahnebonbon"}));
			break;
                case 5 :
			SetProp(P_SHORT,"Ein Karamelbonbon");
			SetProp(P_LONG,"Ein Karamelbonbon, der Rest von Karneval?\n");
			SetProp(P_NAME,"Karamelbonbon");
			AddId(({"karamelbonbon"}));
			break;
		case 6 :
			SetProp(P_SHORT,"Ein Anananasbonbon");
			SetProp(P_LONG,"Ein dreieckiges Anananasbonbon, das schmeckt bestimmt ganz ananassig.\n");
			SetProp(P_NAME,"Ananasbonbon");
			AddId(({"ananasbonbon"}));
			break;
		case 7 :
			SetProp(P_SHORT,"Ein Himbeerdrops");
			SetProp(P_LONG,"Dieses himbeerige Fruchtbonbon ist viel groesser als eine Himbeere.\n");
			SetProp(P_NAME,"Himbeerdrops");
			AddId(({"drops","himbeerdrops","fruchtbonbon"}));
			break;
                default :
			SetProp(P_SHORT,"Ein Multivitaminbonbon");
			SetProp(P_LONG,"Ein Multivitaminbonbon, sehr gesund (besonders bei dem Wetter in Muenster).\n");
			SetProp(P_NAME,"Multivitaminbonbon");
			AddId(({"vitaminbonbon","multivitaminbonbon"}));
			break;
    }
}

void create()
{
  if (!clonep(this_object())) return;
	::create();
	SetProp(P_WEIGHT, 25);
	SetProp(P_VALUE, 0);
	SetProp(P_GENDER, NEUTER );
	AddId("bonbon");
	geschmack();
}


void init()
{
	::init();
	add_action("essen","esse");
	add_action("essen","iss");
	add_action("essen","lutsche");
}

int essen(string str)
{
	notify_fail("Was moechtest Du essen?\n");
	if ( !str || !id(str) )
	    return 0;
	write("Du lutschst genuesslich " + name(WEN,0) + ".\n");
	say( PL->Name(WER,2) + " lutscht " + name(WEN,0) + ".\n");
	remove();
	return 1;
}

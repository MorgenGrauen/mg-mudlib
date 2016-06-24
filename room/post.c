inherit "/std/post";
#include <properties.h>

create()
{
	::create();
        AddExit("osten","/gilden/abenteurer");
	SetProp(P_INT_LONG,"Dies ist das Hauptpostamt von MorgenGrauen.\n"+
               "Von hier aus kannst Du Briefe an Deine Mitspieler schicken und Briefe von\n"+
               "ihnen lesen. Wenn Du das willst, tippe \"post\".\n"+
               "Der einzige sichtbare Ausgang fuehrt nach Osten in die Gilde.\n");
}             

inherit "/std/pub";
#include <rooms.h>
#include <properties.h>

create(){
   replace_program("std/pub.c");
   ::create();

   AddDrink("Bier nach dem Reinheitsgebot","bier",12,0,2,1,
            ({"Der Kellner bringt Dein Bier. Du fuehlst Dich gut.",
              "&& trinkt ein eiskaltes Bier."}));

   AddDrink("Franks Spezial-Schnaps",
            ({"schnaps","spezial","spezial-schnaps"}),
            50,10,8,0,
            ({"Der Kellner bringt Deinen Schnaps.\n"+
              "Du fuehlst ein Kribbeln im ganzen Koerper.",
              "&& schuettet einen Spezial-Schnaps herunter."}));

   AddDrink("Ein beruehmter Rachenputzer",({"putzer","rachenputzer"}),
            150,25,12,0,
            ({"Du bekommst Deinen weltberuehmten Rachenputzer.\n"+
              "Eine Schockwelle rast mit der Gewalt eines Baseball-Schlaegers durch\n"+
              "Deinen Koerper und trifft mit voller Wucht Dein Gehirn.",
              "&& torkelt, als &! einen Rachenputzer saeuft."}));

   AddDrink("Eine Tasse Kaffee",({"kaffee","tasse","tasse kaffee"}),
            20,0,-2,3,
            ({"Baeh! Schmeckt ja widerlich!",
              "&& verzieht angewidert das Gesicht, als &! einen Kaffee schluckt."}));

   AddExit("westen","room/gilde");

   SetProp(P_INT_SHORT,"Franks Abenteurer-Kneipe");
   SetProp(P_INT_LONG,"Du bist in Franks Abenteurer-Kneipe.\n"+
               "Wie immer ist hier mal wieder die Hoelle los. Flaschen und Glaeser\n"+
               "fliegen durch den Raum, und du musst aufpassen, dass Du nicht in\n"+
               "die Schlaegerei zwischen einem Kerl, der aussieht wie Sylvester\n"+
               "Stallone und einem auch nicht gerade harmlosen Zauberer hinein-\n"+
               "gezogen wirst.\n\n"+
               "Wenn Du wissen willst, was Du hier trinken kannst, tippe: 'menue'.\n");	
   SetProp(P_LIGHT,1);
}


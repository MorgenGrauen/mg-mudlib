inherit "std/thing";
#include <properties.h>
#include <language.h>

void create()
{
   // Laeden erwarten Short und Value in der Blueprint.
   SetProp(P_SHORT, "Ein Lolli");
   SetProp(P_VALUE, 5);
   
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Lolli");
   SetProp(P_NAME, "Lolli");
   SetProp(P_GENDER, MALE);
   SetProp(P_LONG, "Ein leckerer Lolli, Du kannst ihn schlecken.\n");
   AddId("lolli");
   SetProp(P_WEIGHT, 10);
   AddCmd( ({"schleck","schlecke","leck","lecke"}), "schleck" );
}

int schleck(string str)
{
   notify_fail("Was moechtest Du schlecken?\n");
   if (!str || !id(str)) return 0;
   write("Du schleckst an Deinem Lolli. Er schmeckt ausgezeichnet.\n");
   say(this_player()->name()+" schleckt an "+this_player()->QueryPossPronoun(this_object(), WEM)+" Lolli.\n");
   return 1;
}

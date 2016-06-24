#include <properties.h>
#include <language.h>

inherit "obj/tools/MGtool";
inherit "std/thing";

void create()
{
  if(!clonep(this_object()))
    return;
  thing::create();
  ::create();
  SetProp(P_NAME,"Ankh");
  SetProp(P_GENDER,NEUTER);
  SetProp(P_IDS,({"ankh","xxtool"}));
}

string _query_short()
{
  ::_query_short();
  return "Ein Ankh am Halsband.\n";
}

string _query_long()
{
  ::_query_long();
  return
    "Ein Halsband, an dem ein Ankh haengt.\n"
    +	"Ein Ankh sieht ungefaehr so aus wie ein Kreuz aber eben nicht ganz.\n";
}

string name( int d1, int d2 )
{
  return thing::name(d1,d2);
}

int id( string str )
{
  ::id(str);
  return member( ({"ankh","xxtool"}),str ) != -1;
}

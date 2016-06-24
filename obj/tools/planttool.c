inherit "/std/secure_thing";

#include <properties.h>
#include <wizlevels.h>
#define PLANTMASTER "/secure/krautmaster"

protected void create() 
{
   if (!clonep(this_object())) 
     return;
   ::create();
   
   SetProp(P_SHORT, "Das Kraeutertool");
   Set(P_LONG, function string () { return 
     "Das Kraeutertool hat folgende Befehle:\n"
    +"- showplant [kategorie] (listet alle Pflanzen einer Kategorie auf.)\n"
    +(IS_ARCH(this_player())?
    "Folgende Befehle sind nur fuer EM+ verfuegbar:\n"
    +"- showrooms [ID|all] (Listet alle Raeume auf, in denen das Kraut <ID>\n"
    +"  zu finden ist, oder alle Raeume, in denen ein Kraut waechst.)\n"
    +"- refresh (updatet alle Files).\n"
    +"- addroom <plantId> <loadname> (traegt einen neuen Fundort ein).\n"
    +"- delroom <loadname> (loescht alle Kraeuter fuer einen Fundort).\n"
    +"- createfile <plantId> (erzeugt das Kraut als File).\n"
    +"- cloneplant <plantId|plantFile> (clont das Kraut inv Inv).\n":
    "");}, F_QUERY_METHOD);
   SetProp(P_NAME, "Planttool");
   SetProp(P_GENDER, NEUTER);
   SetProp(P_AUTOLOADOBJ, 1);
   SetProp(P_NODROP, 1);
   SetProp(P_NEVERDROP, 1);
   AddId(({"tool", "kraeutertool", "planttool"}));
   AddCmd(({"cloneplant", "showplant", "showrooms", "refresh",
            "addroom", "delroom", "createfile"}), "_plantmaster");
}

static int _plantmaster(string str)
{
   notify_fail("Unknown Function in plantmaster: _"+query_verb()+"()\n");
   return call_other(PLANTMASTER, "_"+query_verb(), str);
}

// (c) by Padreic (Padreic@mg.mud.de)

inherit "std/room";

#include "../files.h"
#include <moving.h>

static string sounds();

void create()
{
   ::create();
   SetProp(P_NO_TPORT, NO_TPORT);
   AddSmells(SENSE_DEFAULT,
     "Der Duft vom Harz der Baeume und einiger sonstiger Pflanzen liegt in der Luft.\n"
    +"Man merkt sofort, Du stehst mittem im Wald.\n");
   AddSounds(SENSE_DEFAULT, #'sounds);
   AddSounds(({"voegel", "voegeln", "zwitschern"}),
     "Das herrliche zwitschern der Voegel, ist das einzige das die Ruhe im Wald\n"
    +"durchbricht.\n");
}

static string sounds()
{
   if (sizeof(filter_objects(all_inventory(), "InFight")))
      return "Der Laerm des Kampfes stoert die Ruhe des Waldes und uebertoent alles.\n";
   if (sizeof(filter(all_inventory(), #'query_once_interactive))>1)
      return "Es ist unheimlich still hier im Wald und nur das zwitschern einiger Voegel\n"
            +"ist zu hoeren.\n";
   return "Du hast das Gefuehl, als waerst Du ganz allein hier im Wald. Alles ist ruhig\n"
         +"und nur hier und da vernimmst Du das zwitschern einiger Voegel.\n";
}

varargs void delay_reset(int time)
// NPCs koennen boing resets verhindern
{
   if (time)
      set_next_reset(time);
   else set_next_reset(3600);
}

int _normalfunction()
{
  mixed z;
  int i;
  z=this_player()->QueryProp(AUSGANG);
  this_player()->SetProp(AUSGANG, query_verb());
  i=(int)::_normalfunction();
  if (!i) this_player()->SetProp(AUSGANG, z);
  return i;
}
              
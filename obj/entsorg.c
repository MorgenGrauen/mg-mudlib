#pragma strict_types

inherit "/std/npc";

#include <properties.h>
#include <moving.h>

void create()
{
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_SHORT,"Ein Muellschlucker");
  SetProp(P_LONG,break_string(
    "Der Muellschlucker gehoert zu Familie der Kleinsaurier und zeichnet "+
    "sich durch ein sehr grosses Maul und nahezu unbegrenzten Appetit aus. "+
    "Das einzige, was er nicht mag, sind Dinge, die materiellen Wert haben. "+
    "Alles andere kann man ihm mit 'fuettere muellschlucker mit <name>' "+
    "in den Rachen stopfen.",78));
  SetProp(P_INFO,break_string(
    "Dieser Muellschlucker verschlingt auch lose herumliegende Dinge. Also "+
    "pass auf, was Du wegwirfst.",78));
  SetProp(P_NAME,"Muellschlucker");
  SetProp(P_GENDER, MALE);
  SetProp(P_RACE,"saurier");
  SetProp(P_ARTICLE,1);
  AddId(({"muellschlucker","schlucker","kleinsaurier","saurier"}));
  create_default_npc( 1 );
  SetProp(P_XP,0);
  SetProp(P_BODY,100);
  SetProp(P_HANDS,({" mit dem alles verschlingenden Gebiss",20}));
  SetProp(P_ALIGN, 0);
  seteuid(getuid(this_object()));
  SetProp(P_DEFAULT_INFO,"grunzt nur vor sich hin.\n");
  SetProp(P_MSGIN,"rollt herein");
  SetProp(P_MSGOUT,"rollt");
  SetProp(P_MAX_WEIGHT, 200000);
  AddCmd(({"fuetter","fuettere"}),"haps");
  enable_commands();
}

int haps(string str)
{
  string s1,s2;
  object ob;
  notify_fail("Syntax: fuettere muellschlucker mit <name>\n");
  if(!str)return 0;
  if(sscanf(str,"%s mit %s",s1,s2)!=2)return 0;
  if(!id(s1))return 0;
  notify_fail("So etwas hast Du nicht.\n");
  if((!ob=present(s2,this_player())) && 
     (!ob=present(s2,environment()))) return 0;
  if(ob->QueryProp(P_NODROP) || ob->QueryProp(P_NEVERDROP)){
    write("Das kannst Du nicht wegwerfen.\n");
    return 1;
  }
  if(living(ob)){
    write("Der Muellschlucker frisst keine Lebewesen.\n");
    return 1;
  }
  if(ob->QueryProp(P_VALUE)){
    write("Das ist zu wertvoll, das vertraegt der Muellschlucker nicht.\n");
    return 1;
  }
  if (ob->QueryProp(P_CURSED)) {
    write("Verfluchte Dinge bekommen dem Muellschlucker nicht!\n");
    return 1;
  }
  ob->remove();
  if(ob){
    write("Aus irgendeinem Grund scheint das dem Muellschlucker nicht "+
    "zu schmecken.\n");
    return 1;
  }
  write("Mit einem lauten Ruelpsen verschlingt der Muellschlucker alles.\n");

  return 1;
}

void reset()
{
  int i;
  object *inv;
  ::reset();
  
  inv=all_inventory(this_object());
  if(inv && sizeof(inv))
    for(i=sizeof(inv)-1;i>=0;i--)
      destruct(inv[i]);
  command("nimm alles");
}


// tragbare Seher-FAQ, gibt es im Seherladen zu kaufen bzw. automatisch zur Seherwerdung
// Rikus@mg.mud.de

inherit "std/thing";

#include <properties.h>
#include <wizlevels.h>
#include "../haus.h"

#define FAQ(f) (SPECIALPATH+"faq/"+(f))

static string *seite;

mapping synonym = ([]);

create()
{
//  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT,"Die SeherFAQ");
  SetProp(P_NAME,"SeherFAQ");
  SetProp(P_GENDER,FEMALE); 
  SetProp(P_LONG,"Dies ist die SeherFAQ. Wenn Du weitere Fragen oder "+
          "Anregungen zu dieser FAQ\nhast, sei so nett und schicke Sting "+
          "einen kurzen Brief. Zum Inhalt der FAQ\nkommst Du mit "+
          ">lies inhalt<\n");
  SetProp(P_WEIGHT,100);
  SetProp(P_VALUE,1000);
  AddId(({"faq","seherfaq"}));
  AddCmd("lies","lesen");
  restore_object(FAQ("faq"));
  seite = get_dir(FAQ("*"))-({".","..","faq.o"});
}

init()
{
  object o;
  ::init();
  if ((o=environment()) && query_once_interactive(o) && !IS_SEER(o))
     call_out("wech",0);
}

wech()
{
   write("Die Seher-FAQ lacht Dich aus und loest sich in Wohlgefallen auf.\n");
   remove();
}

int lesen(string str)
{
  if (!str || str == "") {
    notify_fail("Was willst Du denn lesen?\n");
    return 0;
  }
  if (environment()!=this_player()) 
  {
     notify_fail("Du musst die FAQ schon erst nehmen!\n");
     return 0;
  }
  str = lower_case(this_player()->_unparsed_args());
  if (member(seite,str)<0) {
    if (!member(synonym,str)) {
      notify_fail("Diese Seite gibt es nicht...\n");
      return 0;
    }
    else 
      str = synonym[str];
  }
  this_player()->More(FAQ(str),1);
  return 1;
}

string _query_extralook()
{
   object o;
   if ((o=environment()) && (o->QueryProp(P_GENDER)==FEMALE))
      return "Mit ihrer Seher-FAQ outet sie sich als Wurstseherin.\n";
   return "Mit seiner Seher-FAQ outet er sich als Wurstseher.\n";
}

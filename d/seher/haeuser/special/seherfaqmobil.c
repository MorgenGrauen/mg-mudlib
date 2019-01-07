// tragbare Seher-FAQ, gibt es im Seherladen zu kaufen bzw. automatisch zur Seherwerdung
// Rikus@mg.mud.de

inherit "/std/thing";

#include <properties.h>
#include <wizlevels.h>
#include "../haus.h"

#define FAQ(f) (SPECIALPATH+"faq/"+(f))

static string *seite;

mapping synonym = ([
  "neuigkeiten":"0",
  "transport":"1",
  "transporter":"1",
  "transportmoeglichkeit":"1",
  "transportmoeglichkeiten":"1",
  "portale":"1",
  "seherportale":"1",
  "sehertore":"1",
  "quest":"2",
  "quests":"2",
  "seherquest":"2",
  "seherquests":"2",
  "fluchtrichtung":"3",
  "level":"4",
  "seherlevel":"4",
  "kommunikation":"5",
  "kugel":"6",
  "magische kugel":"6",
  "seherkugel":"6",
  "kristallkugel":"6",
  "seherladen":"7",
  "designer-seher":"8",
  "designerseher":"8",
  "fehlermeldung":"9",
  "paralleldimension":"10",
  "portal":"11",
  "paraportal":"11",
  "para-portal":"11",
  "finger":"12",
  "finger-befehl":"12",
  "fingerbefehl":"12",
  "echo":"13",
  "remote":"13",
  "r-emote":"13",
  "haus":"14",
  "haeuser":"14",
  "seherhaus":"14",
  "seherhaeuser":"14",
  "nachwort":"15",
  "danksagung":"16",
  "danksagungen":"16",
]);

protected void create() {
  if ( !clonep() ) {
    set_next_reset(-1);
    return;
  }
  ::create();
  
  SetProp(P_SHORT,"Die SeherFAQ");
  SetProp(P_NAME,"SeherFAQ");
  SetProp(P_GENDER,FEMALE); 
  SetProp(P_LONG, break_string(
    "Dies ist die SeherFAQ. Wenn Du weitere Fragen oder Anregungen zu "
    "dieser FAQ hast, sei so nett und schicke dem Regionsmagier Seher oder "
    "einem Erzmagier einen kurzen Brief. Zum Inhalt der FAQ kommst Du mit "
    "dem Befehl 'lies inhalt'",78));
  SetProp(P_WEIGHT,100);
  SetProp(P_VALUE,1000);
  AddId(({"faq","seherfaq"}));
  AddCmd("lies","lesen");
  seite = get_dir(FAQ("*"))-({".","..","faq.o"});
}

public varargs void init(object oldenv) {
  ::init(oldenv);
  object o = environment();
  if ( objectp(o) && query_once_interactive(o) && !IS_SEER(o) )
    call_out("wech",0);
}

void wech() {
  write("Die Seher-FAQ lacht Dich aus und loest sich in Wohlgefallen auf.\n");
  remove();
}

int lesen(string str) {
  if (!str || str == "") {
    notify_fail("Was willst Du denn lesen?\n");
    return 0;
  }
  if (environment()!=this_player()) {
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

string _query_extralook() {
  object owner = environment();
  return sprintf("Mit %s Seher-FAQ outet %s sich als Wurstseher%s.\n",
           owner->QueryPossPronoun(this_object(), WEM, SINGULAR),
           owner->QueryPronoun(WER),
           owner->QueryProp(P_GENDER)==FEMALE ? "in" : "" );
}

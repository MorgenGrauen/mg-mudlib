// (c) by Padreic (Padreic@mg.mud.de)

#include <properties.h>
#include <v_compiler.h>
#include "../files.h"
#include "/p/service/padreic/kraeuter/plant.h"

inherit "std/virtual/v_compiler";

create()
{
  ::create();
  SetProp(P_STD_OBJECT, ROOM("stdlichtung"));
  SetProp(P_COMPILER_PATH, ROOM(""));
}

string Validate(string file)
{
  file=::Validate(file);
  switch(file) { 
     case "lichtungso":
     case "lichtungo":
     case "lichtungno":
     case "lichtungn":
     case "lichtungs":
     case "lichtungnw":
     case "lichtungw":
     case "lichtungsw":  return file;
     default:
  }
}
         
mixed CustomizeObject()
{
  string file;
  file=Validate(::CustomizeObject());
  if (!random(3)) PO->AddItem(NPC("laufeiche"), REFRESH_DESTRUCT, ([MNPC_HOME: ROOM(file)]) );
  switch(file) {
     case "lichtungno":
       PO->AddExit("westen", ROOM("lichtungn"));
       PO->AddExit("sueden", ROOM("lichtungo"));
       PO->AddExit("suedwesten", ROOM("tuempel"));
       PO->AddExit("suedosten", ROOM("weg2"));
       PO->SetProp(EXTRA_LONG,
         " Im Suedosten befindet sich der Waldweg, ueber den Du den Wald wieder "
        +"verlassen kannst.");
       return file;

     case "lichtungo":
       PO->AddExit("norden", ROOM("lichtungno"));
       PO->AddExit("sueden", ROOM("lichtungso"));
       PO->AddExit("westen", ROOM("tuempel"));
       PO->AddExit("osten", ROOM("weg2"));
       PO->SetProp(EXTRA_LONG,
         " Im Osten befindet sich der Waldweg, ueber den Du den Wald wieder "
        +"verlassen kannst.");
       return file;

     case "lichtungso":
       PO->AddExit("westen", ROOM("lichtungs"));
       PO->AddExit("norden", ROOM("lichtungo"));
       PO->AddExit("nordwesten", ROOM("tuempel"));
       PO->AddExit("nordosten", ROOM("weg2"));
       PO->SetProp(EXTRA_LONG,
         " Im Nordosten befindet sich der Waldweg, ueber den Du den Wald wieder "
        +"verlassen kannst.");
       PO->AddItem(NPC("riese"), REFRESH_DESTRUCT);
       return file;

     case "lichtungs":
       PO->AddExit("westen", ROOM("lichtungsw"));
       PO->AddExit("osten", ROOM("lichtungso"));
       PO->AddExit("norden", ROOM("tuempel"));
       PO->SetProp(EXTRA_LONG,
         " In der Wiese siehst Du an einer Stelle etwas Klee wachsen.");
       PO->AddPlant(BITTERKLEE);
       // "klee" _nach_ dem AddPlant einfuegen, damit die restlichen Details
       // wie Bitterklee trotzdem noch eingefuegt werden
       PO->RemoveDetail(({"klee"}));
       PO->AddDetail(({"stelle", "klee"}),
         "Bei der Stelle scheint es sich unzweifelhaft um Bitterklee zu handeln.\n");
       PO->AddCmd("sueden", "cmd_sueden");
       return file;

     case "lichtungn":
       PO->AddExit("westen", ROOM("lichtungnw"));
       PO->AddExit("osten", ROOM("lichtungno"));
       PO->AddExit("sueden", ROOM("tuempel"));
       PO->SetProp(EXTRA_LONG, " Am Rand der Wiese entdeckt Du einige kleine Pilze im Gras.");
       PO->AddCmd(({"esse", "iss", "pflueck", "pfluecke"}), "cmd_pilze");
       PO->AddDetail(({"pilz", "pilze", "schimmer"}),
         "Die Pilze haben einen aeusserst seltsamen metallischem Schimmer und\n"
        +"verbreiten einen sehr angenehmen suesslichen Duft im Raum.\n");
       PO->AddSmells(({"pilz", "pilze"}),
         "Die Pilze verbreiten einen sehr suesslichen Duft.\n");
       PO->AddSmells("duft", "Der Duft ist eigentlich sehr befreiend und angenehm.\n");
       return file;

     case "lichtungnw":
       PO->AddExit("westen", ROOM("schule"));
       PO->AddExit("osten", ROOM("lichtungn"));
       PO->AddExit("sueden", ROOM("lichtungw"));
       PO->AddExit("suedosten", ROOM("tuempel"));
       PO->SetProp(EXTRA_LONG,
         " Nach Westen kannst Du ein Stueck in den Wald hinein gehn.");
       return file;

     case "lichtungw":
       PO->AddExit("norden", ROOM("lichtungnw"));
       PO->AddExit("sueden", ROOM("lichtungsw"));
       PO->AddExit("osten", ROOM("tuempel"));
       PO->SetProp(EXTRA_LONG, " Am Waldrand stehen einige blaeuliche Farne.");
       PO->AddCmd(({"esse", "iss", "pflueck", "pfluecke"}), "cmd_farne");
       PO->AddDetail(({"farn", "farne"}),
         "Die blaeulichen Farne sind wirklich sehr merkwuerdig. soetwas hast Du bisher\n"
        +"noch nirgendwo gesehn. Irgendetwas besonderes hat es sicherlich damit auf sich.\n");
       return file;

     case "lichtungsw":
       PO->AddExit("osten", ROOM("lichtungs"));
       PO->AddExit("norden", ROOM("lichtungw"));
       PO->AddExit("nordosten", ROOM("tuempel"));
       PO->AddExit("suedwesten", ROOM("stein"));
       PO->SetProp(EXTRA_LONG,
         " Nach Suedwesten kannst Du ein Stueck in den Wald hinein gehn.");
       return file;

     default: return 0;
  }
  return file;
}

int NoParaObjects() { return 1; }

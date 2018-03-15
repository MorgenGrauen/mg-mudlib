inherit "/std/thing";
#include <properties.h>
#include <moving.h>
#include <language.h>

#pragma strict_types,save_types,rtt_checks

protected void create()
{
   ::create();

   SetProp(P_GENDER, NEUTER);
   SetProp(P_NAME, "Gebuesch");
   SetProp(P_SHORT,"Ein Gebuesch");
   SetProp(P_LONG,"Du stehst vor einem gaanz normalen Gebuesch. Es traegt "
                  "Fruechte. Und eventuell\nkann man darin etwas finden.\n");
   SetProp(P_NOGET,"Es ist festgewachsen, eventuell kann man es ausgraben?\n");

   SetProp(P_WEIGHT,5000);
   SetProp(P_SIZE,100);

   AddId(({"busch","gebuesch"}));

   AddDetail(({"fruechte","frucht"}),
     "Es scheinen Beeren zu sein. Vielleicht kannst Du sie pfluecken?\n");
   AddDetail("beeren",
     "Pflueck sie doch, vielleicht bringt das ja was.\n");

   // Nur wenn die Syntax erfuellt ist, wird cmd_pfluecken() aufgerufen.
   // "pfluecke beeren von busch" etc.
   AddCmd("pfluecke|ernte&fruechte|beeren&@ID","cmd_pfluecken",
          "Was willst Du denn @verben?|Wo willst Du denn die Beeren @verben?");
   //      ^- Fehler fuer 'fruechte'   ^- Fehler fuer '@ID'

   // suchen gibt nur eine Meldung aus. Dafuer braucht man kuenftig keine
   // Funktionen mehr.
   AddCmd("such|suche|durchsuch|durchsuche&@ID&\nimpossible",0, 
          "Wo willst Du @VERBen?|Du durchsuchst das Gebuesch, findest aber nichts."
   //      ^- Fehler fuer '@ID'  ^- Fehler fuer unerfuellbare Regel '\nimpossible'
          "^@WER1 durchsucht ein Gebuesch, findet aber nichts.");
   //      ^- return 1-Flag, mit Raummeldung

   // Graben geht eh nicht. Daher nur Fehlermeldungen.
   // Das ^ sagt, dass hier ein return1 zurueckgegeben wird. Es gibt aber keine 
   // Raummeldung.
   AddCmd("grab|grabe&@ID&aus@\nimpossible",0,
          "Was willst Du graben?|Du willst das Gebuesch ausgraben?|"
   //      ^- Fehler fuer '@ID'  ^- Fehler fuer 'aus'
          "Die Wurzeln scheinen tief zu rechen. Das wird nichts.^");
   //      ^- Fehler fuer unerfuellbare Regel '\nimpossible'    ^- return 1-Flag
   //                                                              keine Raum
}

int cmd_pfluecken(string arg, mixed *param)
{
  object obj = clone_object(__DIR__"obst");
  string msg = "Verwundert pflueckst Du "+(obj->name())+" vom Busch. "
               "Komisch.";

  this_player()->ReceiveMsg(msg, MT_NOTIFICATION);
  if(this_player()->pick(obj))
    return 1;

  if(obj->move(environment(), M_PUT) == MOVE_OK)
    this_player()->ReceiveMsg(obj->Name(WER)+" faellt zu Boden.",
                              MT_LOOK|MT_LISTEN);
  else {
    this_player()->ReceiveMsg(obj->Name(WER, 1)+" zerplatzt.",
                              MT_LOOK|MT_LISTEN);
    obj->remove(1);
  }
  return 1;
}

#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"katzenhai","hai","schmirgelpapier","\nkhaut"}));
  SetProp(P_NAME,"Katzenhai" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Katzenhai" );
  SetProp(P_LONG,"Ein kleiner Katzenhai, er hat eine aeusserst raue Haut.\n");
  SetProp(P_VALUE,100);
  SetProp(P_WEIGHT, 200);
  SetEatMessage("Du haeutest den Katzenhai und isst ihn.");
  SetCorpseObject(ANGELOBJ("khaut"));
}

inherit "/std/thing";

#include <language.h>
#include <properties.h>

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"haut","katzenhaihaut","schmirgelpapier","\nkhaut"}));
  SetProp(P_NAME, "Haut");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_SHORT, "Etwas Haut eines Katzenhaies" );
  SetProp(P_LONG, "Ein Stueck der rauen Haut eines Katzenhaies.\n");
  SetProp(P_MATERIAL,MAT_SKIN);
  SetProp(P_VALUE,10);
  SetProp(P_NOBUY,1);
  SetProp(P_WEIGHT,2 );
}

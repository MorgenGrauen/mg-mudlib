#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"seewolf","wolf"}));
  SetProp(P_NAME,"Seewolf" );
  SetProp(P_SHORT,"Ein Seewolf" );
  SetProp(P_LONG,
  "Ein haesslicher Seewolf mit scharfen, gebogenen Zaehnen. Er sieht\n"
  +"richtig boesartig aus.\n");
  SetProp(P_VALUE,100);
  SetProp(P_WEIGHT, 600);
}

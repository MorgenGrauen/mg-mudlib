//--------------------------------------------------------------------------------
// Name des Objects:    Schraenger-Gutschein
// Letzte Aenderung:    05.05.2001
// Magier:              Swift
//--------------------------------------------------------------------------------

#include "schrankladen.h"
#include <ansi.h>
inherit "std/thing";

create()
{
  if (!clonep(TO)) return;
  ::create();
  SetProp(P_SHORT, "Ein Gutschein");
  SetProp(P_LONG, "@@longtext@@");
  AddId(({"gutschein"}));
  SetProp(P_NAME, "Gutschein");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 5);            // Gewicht 5g
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_MATERIAL, ({MAT_PAPER}) );
  SetProp(P_INFO, "Dieser Gutschein ist nur fuer einen Seher mit Eigenheim von Interesse!\n");
  SetProp(P_AUTOLOADOBJ, 1);
}

string longtext()
{
  string str,aussteller,inhaber,text;
  int wert;
  aussteller="Seleven";
  inhaber="Filli";
  wert=5000;
  text="";
  str="***********************************************************************\n\n"
     +"  D E R   S C H R A E N K E R\n\n"
     +"  E I N K A U F S - G U T S C H E I N    .\n\n"
     +"  Ausgestellt von "+aussteller+" fuer "+inhaber+".\n\n"
     +"  Wert: "+to_string(wert)+" Muenzen.\n\n"
     +text
     +"***********************************************************************\n";
  return str;
}

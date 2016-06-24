//--------------------------------------------------------------------------------
// Name des Objects:    Muellcontainer
// Letzte Aenderung:    17.03.2001
// Magier:              Swift
//--------------------------------------------------------------------------------

#include "schrankladen.h"
inherit LADEN("swift_std_container");

#define VERSION_OBJ ".003"

create()
{
  if (!clonep(TO)) return;
  swift_std_container::create();
  SetProp(P_SHORT, "Ein Muellcontainer");
  SetProp(P_LONG, BS(
     "Ein grosser Container aus Stahl, aus dem es nicht gerade angenehm duftet. "
    +"Offensichtlich werden dort alle Sachen entsorgt, die Seher nicht gerne in "
    +"ihrem Haus oder auf der Strasse herumliegen sehen.")
    +"@@cnt_status@@");
  AddId(({"muellcontainer", "container"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Muellcontainer");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 100);     // Mehr sollte nicht sein, lt. Tiamak.
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_STEEL, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");

  AddSmells(({"container", "duft", "gestank"}), BS("Aus "+name(WEM,1)+" stinkt es "
    +"schon ganz schoen, aber Deine Nase ist ja allerhand gewoehnt."));

  AddCmd(({"wirf", "werf", "werfe"}), "wegwerfen");
}

int wegwerfen(string str)
{
  string was,inwas;
  notify_fail("Was willst Du wo hinein werfen?\n");
  if(!str) return 0;
  if( sscanf(str, "%s in %s", was, inwas) != 2)
    return 0;
  if( present(inwas)==TO )   // Dies Objekt ist gemeint!
  {
    TP->command_me("stecke "+str);
    return 1;
  }
  return 0;  
}

varargs int PreventInsert(object ob)
{
  if( living(ob) )
  {
    write( BS("Lebewesen kannst Du nicht in "+name(WEN,1)+" werfen!"));
    return 1;
  }
  else if( ob->QueryProp(P_VALUE) )
  {
    write( BS("Nur Dinge ohne materiellen Wert lassen sich in "
      +name(WEN,1)+" werfen."));
    return 1;
  }
  else if( ob->QueryProp(P_CURSED) )
  {
    write( BS("Verfluchte Dinge kannst Du nicht in "+name(WEN,1)+" werfen!"));
    return 1;
  }
  else
    return ::PreventInsert(ob);
}

void reset()
{
  int i;
  object *inv;
  ::reset();
  inv=all_inventory(TO);
  if(inv && sizeof(inv))
  {
    for(i=sizeof(inv)-1;i>=0;i--)
      destruct(inv[i]);
    tell_room(ETO, BS("Aus "+name(WEM,1)+" ertoent kurz ein Knistern und Knacken, "
           +"dann ist wieder Ruhe."));
  }
}

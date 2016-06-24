//--------------------------------------------------------------------------------
// Name des Objects:    Tresor
// Letzte Aenderung:    17.03.2001
// Magier:              Swift
//--------------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"
inherit LADEN("swift_std_container");

#define VERSION_OBJ "3"

protected void create()
{
  if (!clonep(TO)) return;
  swift_std_container::create();
  SetProp(P_SHORT, "Ein Tresor");
  SetProp(P_LONG, BS(
     "Ein sehr stabiler und feuersicherer Tresor, an dem sich sicher schon so "
    +"mancher Einbrecher eine Abfuhr geholt hat. Das magische Feld, das den "
    +"Tresor umgibt, verhindert wohl auch Attacken magischer Art. So wuchtig "
    +"und wichtig, wie dieser Klotz aussieht, vermutest Du wohl zu Recht, "
    +"dass sich unsagbare Reichtuemer in seinem Inneren verbergen muessen.")
    +"@@cnt_status@@");
  AddId(({"tresor"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Tresor");
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

  AD(({"feld"}), BS("Ein starkes magisches Feld umgibt "+name(WEN,1)+"."));
  AD(({"klotz"}), BS(Name(WER,1)+" ist wahrlich ein Klotz, ausgesprochen wuchtig."));
}

varargs int PreventInsert(object ob)
{
  // Zuerst mal: Dinge, deren Wert >= 5000 Muenzen ist, duerfen rein, solange
  // es keine Ruestungen oder Waffen sind!
  if( ob->QueryProp(P_VALUE) >= 5000 && !ob->QueryProp(P_ARMOUR_TYPE) &&
     !ob->QueryProp(P_WEAPON_TYPE) )
        return ::PreventInsert(ob);
  if( ob->id("\ngeld") ||                                     // Geld?
      ob->QueryMaterialGroup(MATGROUP_PRECIOUS_METAL) ||      // Edelmetall?
      ob->QueryMaterialGroup(MATGROUP_JEWEL) ||               // Edelstein?
      ob->QueryMaterial(MAT_CRYSTAL) ||                       // Kristall?
      ob->QueryMaterial(MAT_IVORY) )                          // Elfenbein?
  {
    if(!ob->QueryProp(P_WEAPON_TYPE) )    // Auf keinen Fall Waffen!
    {
      if( ob->QueryProp(P_ARMOUR_TYPE ) ) // Falls Ruestungsteil...
      {
        if( ob->QueryProp(P_ARMOUR_TYPE)==AT_RING ||
            ob->QueryProp(P_ARMOUR_TYPE)==AT_AMULET ||
            ob->QueryProp(P_ARMOUR_TYPE)==AT_MISC )
          return ::PreventInsert(ob);
        else
          write(BS("Nur wertvolle Ringe und Amulette kannst Du in "+name(WEN,1)
            +" stecken, evtl. noch andere wertvolle Sachen, aber auf keinen "
            +"Fall Ruestungen oder Waffen!"));
      }
      else
        return ::PreventInsert(ob);
    }
    else
      write("Waffen kannst Du nicht in "+name(WEN,1)+" tun!\n");
  }
  write( BS("In "+name(WEN,1)+" kannst Du nur Geld und Edelsteine legen, evtl. auch "
           +"wertvolle Objekte, z.B. aus Edelmetallen, Kristall oder Elfenbein!"));
  return 1;
}

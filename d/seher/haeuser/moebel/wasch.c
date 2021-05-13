// Name des Objects:    Waschobjekt
// Letzte Aenderung:    01.04.2005
// Magier:              Seleven
// Dieses File gibt dem Spieler die Moeglichkeit, eine Waschgelegenheit in
// seinem Seherhaus aufzustellen.
// 01.07.2007 (Zesstra): GetOwner() definiert.
//----------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"
inherit LADEN("swift_std_container");

string *objekte =
  (({"\nstinkobj","\nzesstra_asche","seleve\nscheisse","\npopc",
     "\nsspbollen","seleve\nstinkobj","\nq2p_gestank","ber\tram_skot",
     "bambi\nschmutzigehaende"}));
/* Wurzel(Klo/Imbiss), Zesstra(Vulkan/Illuminatos), Seleven(Klo/Nebelelfen),
 * Seleven(Popcorn/Nebelelfen), Boing(Schleimschmeisser,SSP), STANDART
 */
#define XLOOK_IDS ({"saitha_unterwelt_staub_haende",\
 "saitha_unterwelt_staub_ueberall","saitha_unterwelt_schimmel",\
 })

static void wasch_fun();

protected void create()
{
  if (!clonep(this_object()))
  {
      set_next_reset(-1);
      return;
  }
  ::create();

  AddId(({"duschkabine","kabine","dusche","waschgelegenheit"}));
  SetProp(P_NAME,"Duschkabine");
  SetProp("cnt_version_obj",2);

  SetProp("obj_cmd",
  "wasch(e), dusch(e)\n"+
  "   Damit lassen sich verschiedene Verunreinigungen von Deinem Koerper\n"
  "   abwaschen.\n");

  SetProp(P_SHORT,"Eine Duschkabine");
  SetProp(P_LONG,BS("Die Duschkabine hat einen Rahmen aus hellem Holz. "
   "Dazwischen wurden matte Scheiben eingelassen, damit man niemanden beim "
   "Duschen beobachten kann."));
  AddDetail("rahmen","Der Rahmen ist aus hellem Holz.\n");
  AddDetail(({"scheiben","scheibe"}),"Die Scheiben der Duschkabine sind "+
   "matt und absolut undurchsichtig!\n");
  AddDetail(({"duschen","beim duschen"}),
   "Man kann niemanden beim Duschen beobachten.\n");

  SetProp(P_GENDER, FEMALE);
  SetProp(P_WEIGHT, 25000);        // Gewicht 25 Kg
  SetProp(P_MAX_WEIGHT, 0);        // Soll nur n pseudo-Container
  SetProp(P_MAX_OBJECTS, 0);       // Kein Container :)
  SetProp(P_VALUE, 0);             // Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Falls doch: zerstoeren!
  SetProp(P_NOGET,
    "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ([MAT_MISC_WOOD:20, MAT_GLASS: 30,
                        MAT_MARBLE: 40,
                        MAT_MISC_MAGIC:5, MAT_BRASS:5]) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");

  AddCmd(({"wasche","wasch","dusche","dusch"}),"cmd_waschen");
  RemoveCmd(({"oeffne","oeffn","schliesse","schliess"}));
}

// Zesstra, 1.7.07, fuers Hoerrohr
string GetOwner() {return "seleven";}

varargs int PreventInsert(object ob) {return 1;} //Das ist kein Container :)

static void wasch_fun()
{
  if(!TP) return;
  foreach(string obj_id : objekte)
  {
    object ob;
    while(ob = present(obj_id,TP))
    {
      ob->remove(1);
    }
  }
  foreach(string xlid: XLOOK_IDS)
  {
    TP->RemoveExtraLook(xlid);
  }
}

int cmd_waschen(string str)
{
  if( !erlaubt() )
  {
      write("Du solltest den Besitzer vielleicht um Erlaubnis bitten.\n");
      return 1;
  }
  write("Du springst schnell unter die Dusche und waeschst Dich gruendlich.\n");
  say(this_player()->Name(WER)+" springt schnell unter die Dusche.\n");
  wasch_fun();
  return 1;
}

// oeffnen und schliessen ausbauen.
varargs int skommandos(string str)
{
  notify_fail( "Fehler: Dieser Befehl benoetigt eine gueltige Objekt-Id als Parameter.\n"
    +"Beispiel: skommandos "+QueryProp(P_IDS)[1]+"\n");
  if(!str) return 0;
  if(present(str)!=TO ) // Bin ich gemeint?
    return 0;
  write("=============================================================================\n");
  write("Aktuelle Version: "+QueryProp("cnt_version_std")+QueryProp("cnt_version_obj")+"\n");
  write( BS(Name(WER,1)+" kann nur in diesem Seherhaus verwendet werden, "
    "da "+QueryPronoun(WER)+" speziell dafuer gekauft wurde. "+CAP(QueryPronoun(WER))
    " verfuegt ueber folgende Kommandos:")
    "-----------------------------------------------------------------------------\n"
    "serlaube [Objekt-Id] [Spielername|\"hausfreunde\"|\"zweities\"]\n"
    "  Erlaubt Personen, "+name(WEN,1)+" mitzubenutzen.\n"
    "  serlaube + Objekt-Id (ohne Spielername/hausfreunde)\n"
    "  listet alle Personen mit Zugriff auf "+name(WEN,1)+"\n\n"
    "verschiebe [Objekt-Id] nach [Ausgang]\n"
    "  Damit kannst Du "+name(WEN,1)+" innerhalb Deines Seherhauses verschieben.\n\n"
    "sverstecke [Objekt-Id]\n"
    "  Damit machst Du "+name(WEN,1)+" unsichtbar.\n"
    "shole [Objekt-Id] hervor\n"
    "  Damit machst Du "+name(WEN,1)+" wieder sichtbar.\n"
    "zertruemmer [Objekt-Id]\n"
    "  Damit zerstoerst Du "+name(WEN,1)+".\n\n");


  if( QueryProp("obj_cmd") )
    write( QueryProp("obj_cmd")+"\n" );

  write("[Objekt-Id] muss eine gueltige Id sein, in diesem Fall z.B. "
      +QueryProp(P_IDS)[1]+"\n");
  write("=============================================================================\n");


  return 1;
}


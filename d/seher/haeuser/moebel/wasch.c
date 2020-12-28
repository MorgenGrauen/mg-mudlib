// Name des Objects:    Waschobjekt
// Letzte Aenderung:    01.04.2005
// Magier:              Seleven
// Dieses File gibt dem Spieler die Moeglichkeit, eine Waschgelegenheit in
// seinem Seherhaus aufzustellen. Das beim Kommando wasch(e), dusch(e) laesst
// sich die Medung ueberschreiben (Test per raum->QueryProp("h_commands"))
// 01.07.2007 (Zesstra): GetOwner() definiert.
//----------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"
inherit LADEN("swift_std_container");

string *wcmd;
string *objekte =
  (({"\nstinkobj","\nzesstra_asche","seleve\nscheisse","\npopc",
     "\nsspbollen","seleve\nstinkobj","\nq2p_gestank","ber\tram_skot",
     "bambi\nschmutzigehaende"}));
/* Wurzel(Klo/Imbiss), Zesstra(Vulkan/Illuminatos), Seleven(Klo/Nebelelfen),
 * Seleven(Popcorn/Nebelelfen), Boing(Schleimschmeisser,SSP), STANDART
 */

static void wasch_fun();

protected void create()
{
  if (!clonep(this_object())) return;
  ::create();

  wcmd = (({"wasche","wasch","dusche","dusch"}));
  AddId(({"duschkabine","kabine","dusche","waschgelegenheit"}));
  SetProp(P_NAME,"Duschkabine");
  
  SetProp("cnt_version_obj",1);

  SetProp("obj_cmd",
  "wasch(e), dusch(e)\n"+
  "   Damit laesst sich das Stinken aus dem Imbissklo beseitigen.\n"+
  "   Wenn kein Befehl 'wasch(e)' im Raum beschrieben ist, erfolgt eine\n"+
  "   Standartmeldung, selbiges gilt fuer 'dusch(e)'.\n"+
  "ssetz(e) waschbefehl\n"+
  "   Damit kann man die Befehlsliste erweitern, die das Waschen \n"+
  "   ausloesen\n"+
  "sloesch(e) waschbefehl\n"+
  "   Damit kann man einen Befehl aus der Liste loeschen\n"+
  "waschbefehlsliste, wbl"+
  "   Zeigt eine Liste der Befehle an, die das Waschen ausloesen.\n");

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
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 0);        // Soll nur n pseudo-Container
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 0);       // Kein Container :)
  SetProp(P_VALUE, 0);             // Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Fals doch: zerstoeren!
  SetProp(P_NOGET,
    "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_MISC_WOOD, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");

  AddCmd(wcmd,"cmd_waschen");
  RemoveCmd(({"oeffne","oeffn","schliesse","schliess"}));
  AddCmd(({"ssetze","ssetz"}),"cmd_setzen");
  AddCmd(({"sloesche","sloesche"}),"cmd_loeschen");
  AddCmd(({"wbl","waschbefehlsliste"}),"liste");
}

// Zesstra, 1.7.07, fuers Hoerrohr
string GetOwner() {return "seleven";}

varargs int PreventInsert(object ob) {return 1;} //Das ist kein Container :)

static void wasch_fun()
{
  int i;
  object ob;
  i = sizeof(objekte);
  i--;
  while(i > -1)
  {

    if(!TP) return;
    ob = present(objekte[i],TP);
    while(ob)
    {
      if(query_once_interactive(ob))
	  {
	    tell_object(TP,BS("Du scheinst jemanden in Deinem Inv zu haben!\n"
	     "Das sollte sich vielleicht ein Magier anschauen."));
	    i--;
	    return;
      }
      ob->remove(1);
      if(present(objekte[i],TP));
       ob = present(objekte[i],TP);
    }
    i--;

  }
}

int cmd_waschen(string str)
{
  string verb;
  verb = query_verb();
  if(!environment()) return 0;
  if( !erlaubt() )
  {
      write("Du solltest den Besitzer vielleicht um Erlaubnis bitten.\n");
      return 1;
  }
  if(member(environment()->QueryProp("h_commands"),verb) == 0)
  {
    write(
     "Du springst schnell unter die Dusche und waeschst Dich gruendlich.\n");
    say(this_player()->Name(WER)+" springt schnell unter die Dusche.\n");
    wasch_fun();
    return 1;
  }
  wasch_fun();
  return 1;
}


static int cmd_setzen(string str)
{
  string bef;
  notify_fail("Moechtest Du einen Waschbefehl setzen?\n"+
    "Syntax: ssetz(e) waschbefehl [befehl]\n");
  if(!str || !stringp(str) || sizeof(str) == 0) return 0;
  sscanf(str,"waschbefehl %s",bef);
  if(!bef) return 0;
  if(bef[sizeof(bef) .. sizeof(bef)-1] == "e")
   bef = bef[sizeof(bef) .. sizeof(bef)-2];
  notify_fail("Der Befehl steht schon auf der Liste.\n");
  if(member(wcmd,bef) != -1) return 0;
  wcmd += (({bef,bef+"e"}));
  AddCmd(wcmd,"cmd_waschen");
  write(BS("Du hast die Befehle "+bef+" und "+bef+"e an die Liste "
   "angefuegt."));
  return 1;
}

static int cmd_loeschen(string str)
{
  string bef;
  notify_fail("Moechtest Du einen Waschbefehl loeschen?\n"+
    "Syntax: sloesch(e) waschbefehl [befehl]\n");
  if(!str || !stringp(str) || sizeof(str) == 0) return 0;
  sscanf(str,"waschbefehl %s",bef);
  if(!bef) return 0;
  notify_fail("Der Befehl steht gar nicht auf der Liste.\n");
  if(member(wcmd,bef) == -1) return 0;
  wcmd -= (({bef}));
  RemoveCmd(bef);
  write(BS("Du hast die Befehle "+bef+" und "+bef+"e von der Liste "
   "entfernt."));
  return 1;
}

static int liste(string str)
{
  write("Du hast folgende Befehle auf der Waschliste stehen:\n"+
   break_string(implode(wcmd,", "),78,3));
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
    +"da "+QueryPronoun(WER)+" speziell dafuer gekauft wurde. "+CAP(QueryPronoun(WER))
    +" verfuegt ueber folgende Kommandos:")
    +"-----------------------------------------------------------------------------\n"
    +"serlaube [Objekt-Id] [Spielername|\"hausfreunde\"|\"zweities\"]\n"
    +"  Erlaubt Personen, "+name(WEN,1)+" mitzubenutzen.\n"
    +"  serlaube + Objekt-Id (ohne Spielername/hausfreunde)\n"
    +"  listet alle Personen mit Zugriff auf "+name(WEN,1)+"\n\n"
    +"verschiebe [Objekt-Id] nach [Ausgang]\n"
    +"  Damit kannst Du "+name(WEN,1)+" innerhalb Deines Seherhauses verschieben.\n\n"
  //Seleven 07.12.2005
    +"sverstecke [Objekt-Id]\n"
    +"  Damit machst Du "+name(WEN,1)+" unsichtbar.\n"
    +"shole [Objekt-Id] hervor\n"
    +"  Damit machst Du "+name(WEN,1)+" wieder sichtbar.\n"
  // Bis hier
    +"zertruemmer [Objekt-Id]\n"
    +"  Damit zerstoerst Du "+name(WEN,1)+".\n\n");


  if( QueryProp("obj_cmd") )
    write( QueryProp("obj_cmd")+"\n" );

  write("[Objekt-Id] muss eine gueltige Id sein, in diesem Fall z.B. "
      +QueryProp(P_IDS)[1]+"\n");
  write("=============================================================================\n");


  return 1;
}


#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include <bank.h>
#include "haus.h"

inherit "std/room";

create()
{
  ::create();

  SetProp(P_INDOORS, 1);
  SetProp(P_LIGHT, 1);
  SetProp(P_INT_SHORT, "Am Ausgabeschalter" );
  SetProp(P_INT_LONG,
     "Am Ausgabeschalter. Wenn Du alle Raten fuer Deinen Vertrag eingezahlt hast,\n"
    +"kannst Du nun Dein Haus abholen. Dazu musst Du einfach nur Deinen Vertrag\n"
    +"vorlegen (mit 'lege vertrag vor').\n"
    +"Der Schemen am Schalter koennte ein Beamter sein, aber auch etwas viel, viel\n"
    +"Schlimmeres...\n" );

  AddDetail( ({ "boden" }),
     "Der tiefschwarze Boden gibt Dir das Gefuehl, im absoluten Nichts zu stehen.\n" );
  AddDetail( ({ "wand", "waende" }),
     "Dunkle Schatten tanzen ueber die Waende.\n" );
  AddDetail( ({ "decke" }),
     "Sie scheint Dir in dieser unheimlichen Umgebung auf den Kopf fallen zu\n"
    +"wollen.\n" );
  AddDetail( ({ "nichts" }),
     "Du siehst nichts.\n" );
  AddDetail( ({ "schatten" }),
     "Die Schatten irritieren Dich sehr.\n" );
  AddDetail( ({ "schalter", "ausgabeschalter" }),
     "Du stehst direkt vor dem Ausgabeschalter.\n" );
  AddDetail( ({ "beamte", "beamter", "beamten", "schalterbeamter", "schalterbeamte", "schalterbeamten", "schemen", "schlimmeres" }),
     "Der Schalterbeamte schaut Dich mit aufgesetztem Laecheln an.\n" );

  AddCmd( ({ "leg", "lege" }), "legen" );
  AddExit( "raus", PATH+"seherbank" );
}

void
BecomesNetDead(object pl)
{
  pl->move(PATH+"seherbank", M_GO, 0, "wird rausgeworfen", "schlafwandelt herein");
}

int
legen(string str)
{
  object vertrag, tp;

  tp = this_player();
  vertrag = present("sehe\rvertrag", tp);

  if (!str || str != "vertrag vor")
    return 0;

  if (!IS_SEER(tp)) {
    write( "Der Schalterbeamte blickt Dich scharf an. Er sagt: Nur SeherInnen\n"
	  +"koennen hier ein Haus abholen!\n" );
    HLOG( "BANK.LOG", "Ausgabe: "+getuid(tp)+" ist kein Seher.\n" );
  }
  else if (tp->QueryProp(P_KILLS))
    write( "Der Schemen faucht: Verbrecher werden hier nicht bedient!\n" );
  else if (!vertrag) {
    write( "Der Schemen bemerkt: Sie haben doch gar keinen Vertrag!\n" );
    HLOG( "BANK.LOG", "Ausgabe: "+getuid(tp)+" hatte keinen Vetrag.\n" );
  }
  else if (old_explode(object_name(vertrag),"#")[0] != PATH+"bausparvertrag") {
    HLOG( "BANK.LOG", "Ausgabe: "+getuid(tp)+" Falscher Vertrag!\n");
    HLOG( "SCHUMMEL", sprintf("Ausgabe: TP: %O, V: %O\n",
		      this_player(),
		      vertrag->QueryProp(P_CLONER)));
  }
  else if (!(vertrag->QueryProp(P_AUTOLOADOBJ)[1] & V_COMPL)) {
    write( "Der Schalterbeamte weist Dich darauf hin, dass Du noch nicht genug\n"
	  +"auf den Vertrag eingezahlt hast!\n" );
    HLOG( "BANK.LOG", "Ausgabe: "+getuid(tp)+" hatte zu wenig eingezahlt.\n" );
  }
  else if (ZENTRALBANK->_query_current_money() < 30000) {
    write( "Der Schalterbeamte weist Dich darauf hin, dass die Zentralbank im\n"
	  +"Moment leider nicht ueber genug Bargeldreserven verfuegt, um ihren\n"
	  +"Teil der Abmachung zu erfuellen. Er vertroestet Dich auf spaeter.\n" );
  }
  else if (vertrag->QueryProp(P_AUTOLOADOBJ)[1] & V_RAUM) {
    VERWALTER->NeuerRaum(getuid(this_player()));
    write( "Der Schemen hinter dem Schalter streckt einen langen, schattigen Finger\n"
	  +"nach Deinem Vertrag aus. Wie von Geisterhand bewegt gleitet der Vertrag\n"
	  +"hinueber. Der Schemen scheint den Vertrag ausgiebig zu pruefen, dann\n"
	  +"zerreisst er ihn, wedelt einmal mit seinen schattenhaften Haenden und\n"
	  +"beginnt mit Grabesstimme zu sprechen:\n"
	  +"Der neue Raum ist in ihrem Haus installiert! Hinfort mit Ihnen!\n" );
    HLOG( "BANK.LOG", "Ausgabe: "+getuid(tp)+" bekam einen neuen Raum.\n" );
    HLOG( getuid(tp), "Ausgabe: "+getuid(tp)+" bekam einen neuen Raum.\n" );
    "/p/daemon/zentralbank"->WithDraw(30000);
    vertrag->remove();
  }
  else {
    write( "Der Schemen hinter dem Schalter streckt einen langen, schattigen Finger\n"
	  +"nach Deinem Vertrag aus. Wie von Geisterhand bewegt gleitet der Vertrag\n"
	  +"hinueber. Der Schemen scheint den Vertrag ausgiebig zu pruefen, dann\n"
	  +"zerreisst er ihn, wedelt einmal mit seinen schattenhaften Haenden und\n"
	  +"vor Dir erscheint ein kleines, tragbares Haus. Zoegernd nimmst Du es in\n"
	  +"Deine Haende, da erschallt eine grollende Stimme:\n"
	  +"Das Haus gehoert Ihnen. Beehren sie uns bald wieder! Har, har, har!!!\n" );
    clone_object(PATH+"traghaus")->move(tp, M_NOCHECK);
    HLOG( "BANK.LOG", "Ausgabe: "+getuid(tp)+" bekam das Haus.\n" );
    HLOG( getuid(tp), "Ausgabe: "+getuid(tp)+" bekam das Haus.\n" );
    "/p/daemon/zentralbank"->WithDraw(30000);
    vertrag->remove();
  }
  return 1;
}

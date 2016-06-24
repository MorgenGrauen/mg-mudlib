#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include "haus.h"

inherit "std/room";

create()
{
  ::create();

  SetProp( P_INDOORS, 1 );
  SetProp( P_LIGHT, 1 );
  SetProp( P_INT_SHORT, "Am Einzahlungsschalter" );
  SetProp( P_INT_LONG,
     "Du stehst hier am Einzahlungsschalter der Bank. Um eine Rate einzuzahlen,\n"
    +"musst Du einfach nur Deinen MG MASTER-BLOCK vorlegen. Gib dazu einfach nur\n"
    +"'lege block vor' ein.\n"
    +"Der Blick eines schemenhaft erkennbaren Beamten scheint Dich selbst durch\n"
    +"die dicke Rauchglasscheibe hindurch zu durchbohren.\n" );

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
  AddDetail( ({ "scheibe", "rauchglas", "glas", "glasscheibe", "rauchglasscheibe" }),
     "Hinter ihr scheint sich ein Beamter zu verbergen.\n" );
  AddDetail( ({ "schalter" }),
     "An diesem Schalter kannst Du eine Rate fuer Dein Haus einzahlen.\n" );
  AddDetail( ({ "beamte", "beamter", "beamten", "schalterbeamte", "schalterbeamter", "schalterbeamten" }),
     "Der Schalterbeamte durchbohrt Dich mit seinen Blicken.\n" );

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
  object tp, block, vertrag;
  int *al, *vl, rate;

  if (!str || str != "block vor")
    return 0;

  tp = this_player();
  block = present( "\n block", tp);
  vertrag = present( "sehe\rvertrag", tp );

  if (!IS_SEER(tp))
    write( "Der Schalterbeamte weist Dich darauf hin, dass hier nur Seher be-\n"
	  +"dient werden.\n" );
  else if (tp->QueryProp(P_KILLS))
    write( "Ein bohrender Blick verkuendet: Verbrecher werden hier nicht bedient!\n" );
  else if (!block)
    write( "Du hast keinen Block, den Du vorlegen koenntest!\n" );
  else if (!vertrag) {
    write( "Der Schalterbeamte sagt: Sie haben keinen Bausparvertrag! Wozu wollen\n"
	  +"Sie dann eine Rate einzahlen? Wie sind Sie ueberhaupt an den Block\n"
	  +"gekommen?\n" );
    HLOG( "BANK.LOG", "Einzahlung: "+getuid(tp)+" Block ohne Vertrag!\n" );
  }
  else if (old_explode(object_name(block),"#")[0] != PATH+"block" ||
	   old_explode(object_name(vertrag),"#")[0] != PATH+"bausparvertrag") {
    write( "Der Schalterbeamte faehrt Dich an: DU WILLST SCHUMMELN???\n" );
    HLOG( "BANK.LOG", "Einzahlung: "+getuid(tp)+" Falscher Block/Vertrag!\n");
    HLOG( "SCHUMMEL", sprintf("Einzahlung: TP: %O, B: %O, V: %O\n",
		      this_player(),
		      block->QueryProp(P_CLONER),
		      vertrag->QueryProp(P_CLONER)));
  }
  else {
    al = block->Query(P_AUTOLOADOBJ);
    rate = RATENHOEHE;
    if (al[V_FLAGS] & B_EXTEND)
      rate += (6*RATENHOEHE)/10;

    if (al[V_MONEY] < rate) {
      write( "Der Schalterbeamte weist Dich darauf hin, dass Du noch nicht genug Erfahrung\n"
	    +"fuer die Rate gelassen hast.\n" );
      HLOG( getuid(tp), "Einzahlung: "+getuid(tp)+" hatte nicht genug ("+al[V_MONEY]+"/"+rate+").\n");
    }
    else {
      vl = vertrag->Query(P_AUTOLOADOBJ);
      rate = (al[V_FLAGS] & B_FAST) ? SUMME_S : SUMME_L;
      if (vl[1] & V_RAUM)
	rate = rate * 4 /10;
      write( "Du zahlst Deine Rate ein.\n" );
      block->remove();
      if (vertrag->Einzahlung() >= rate) {
	  write( "Der Schalterbeamte sagt: Sie haben es geschafft! Sie haben nun genug\n"
		+"Erfahrung eruebrigt. ");
	if (vl[1] & V_RAUM)
	  write( "Am Ausgabeschalter koennen Sie nun den neuen Raum\n"
		+"eintragen lassen.\n" );
	else
	  write( "Am Hausausgabeschalter koennen Sie nun Ihr neues\n"
		+"Heim in Empfang nehmen.\n" );
	vertrag->Sign( vl[1] | V_COMPL );
      }
      else {
	write( "Der Schalterbeamte beschwoert Blitz und Donner vom Himmel herab. Ein\n"
	      +"lauter Knall, und ein neuer Ratenblock erscheint in Deinem Inventar.\n" );
	(block = clone_object(PATH+"block"))->move(tp, M_NOCHECK);
	block->SetProp("schwer", vl[1] & V_FAST);
	al = block->Query(P_AUTOLOADOBJ);
	al[V_FLAGS] |= B_ACTIVE;
	block->Set(P_AUTOLOADOBJ, al);
      }
      HLOG(getuid(tp), "Einzahlung: "+getuid(tp)+" zahlte ein.\n" );
      tp->force_save();
    }
  }
  return 1;
}


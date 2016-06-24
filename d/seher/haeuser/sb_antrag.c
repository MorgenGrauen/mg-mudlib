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
  SetProp( P_INT_SHORT, "Am Antragsschalter" );
  SetProp( P_INT_LONG,
     "Du stehst am Antragsschalter der Bank. Hinter einer Rauchglasscheibe scheint\n"
    +"sich ein Schalterbeamter zu befinden, Du kannst aber nichts genaueres er-\n"
    +"kennen.\n"
    +"Wenn Du Dich traust, kannst Du hier einen Bausparvertrag beantragen und einen\n"
    +"beantragten Vertrag unterschreiben.\n"
    +"Die Syntax dazu lautet:\n"
    +"beantrage vertrag (fuer einen Bausparvertrag)\n"
    +"beantrage ausbauvertrag (fuer den Ausbau des Hauses)\n"
    +"unterschreibe vertrag (um den jeweiligen Vertrag zu unterschreiben)\n" );

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
  AddDetail( ({ "schalter" }),
     "An diesem Schalter kannst Du einen Bausparvertrag beantragen und unter-\n"
    +"schreiben.\n" );
  AddDetail( ({ "beamte", "beamter", "beamten", "schalterbeamte", "schalterbeamten", "schalterbeamter" }),
     "Du kannst ihn nur schemenhaft hinter der Rauchglasscheibe des Schalters er-\n"
    +"kennen. Die Umrisse scheinen allerdings zu keiner Lebensform zu gehoeren,\n"
    +"derer Du Dich erinnern koenntest.\n" );
  AddDetail( ({ "scheibe", "glasscheibe", "rauchglas", "rauchglasscheibe" }),
     "Ein schemenhafter Beamter sitzt dahinter.\n" );
  AddDetail( ({ "umrisse" }),
     "Sie sind aeusserst beunruhigend.\n" );

  AddCmd( ({ "beantrage" }), "beantragen" );
  AddCmd( ({ "unterschreib", "unterschreibe" }), "unterschreiben" );
  AddExit( "raus", PATH+"seherbank" );
}

void
BecomesNetDead(object pl)
{
  pl->move(PATH+"seherbank", M_GO, 0, "wird rausgeworfen", "schlafwandelt herein");
}

int
beantragen(string str)
{
  object tp, vertrag;
  int haus;

  tp = this_player();

  if (!str || (haus = member( ({"ausbauvertrag", "vertrag", "bausparvertrag"}), str)) <0) {
    notify_fail( "Versuch mal 'beantrage vertrag'!\n" );
    return 0;
  }

  if (!IS_SEER(tp)) {
    notify_fail( "Nur Seher koennen einen Bausparvertrag beantragen!\n" );
    return 0;
  }
  if (tp->QueryProp(P_KILLS)) {
    write("Verbrecher werden hier nicht bedient!\n");
    return 1;
  }
  if (vertrag = present("sehe\rvertrag", tp))
    write( "Der Beamte schnarrt: Aber Sie haben doch schon einen Vertrag!\n" );
  else if (haus && (present("sehe\rhaus", tp) || VERWALTER->FindeHaus(getuid(tp)))) {
    write( "Der Beamte schnarrt: Sie duerfen nur EIN Haus besitzen!\n" );
    HLOG( "BANK.LOG", "Antrag: "+getuid(tp)+" stellte zweiten Antrag.\n" );
  }
  else if (!haus && !VERWALTER->FindeHaus(getuid(tp))) {
    write( "Der Beamte faucht: Haben sie denn ueberhaupt schon ein Haus aufgestellt?\n"
	  +"Vorher koennen sie natuerlich keine Erweiterungen beantragen!\n" );
  }
  else {
    if (!haus && VERWALTER->HausProp(getuid(tp),HP_ROOMS) == 9) {
      write( "Der Beamte droehnt: Ihr Haus hat schon die hoechste Ausbaustufe erreicht!\n");
      return 1;
    }
    (vertrag = clone_object(PATH+"bausparvertrag"))->move(tp, M_NOCHECK);

    write( "Die Schatten verdichten sich zu einem Blatt Papier, welches in Deine Haende\n"
	  +"geweht wird. Mit droehnender Stimme erklaert der Schalterbeamte:\n"
	  +"Dies ist Ihr Bausparvertrag. Bitte lesen Sie ihn sich gruendlich durch, be-\n"
	  +"vor Sie ihn unterschreiben! Wenn Sie ihn erst einmal unterschrieben haben,\n"
	  +"koennen sie nicht wieder davon zuruecktreten (es sei denn, Sie zerreissen\n"
	  +"ihn oder ueberziehen die Ratenzahlungen)! Sie sind nicht gezwungen, den Ver-\n"
	  +"trag sofort zu unterschreiben. Im Hauptraum der Bank befindet sich ein In-\n"
	  +"formationsstand, an dem Sie Kommentare zu diesem Vertrag kaeuflich erwerben\n"
	  +"koennen. Wenn Ihnen einige Passagen des Vertrags unverstaendlich sind, wird\n"
	  +"Ihnen in den Kommentaren sicherlich geholfen.\n"
	  +"Das Leisten der Unterschrift ist jedoch nur hier an diesem Schalter\n"
	  +"moeglich!\n" );
    if (!haus)
      vertrag->Sign(V_RAUM);
    HLOG( "BANK.LOG", "Antrag: "+getuid(tp)+" bekam einen "+(haus ? "Haus" : "Raum")+"vertrag.\n" );
  }
  return 1;
}

int
unterschreiben(string str)
{
  object tp, vertrag;
  int flag;

  tp = this_player();

  notify_fail( "Was willst Du denn unterschreiben?\n" );
  if (!str || !(vertrag = present(str, tp)) || !vertrag->id("sehe\rvertrag"))
    return 0;

  if ((flag = vertrag->QueryProp(P_AUTOLOADOBJ)[1]) & V_SIGNED) {
    write( "Der Schalterbeamte sagt: Aber der Vertrag ist doch schon unterschrieben!\n" );
    return 1;
  }

  write( "Der Schalterbeamte sagt: Fuer welche Vertragsvariante haben sie sich\n"
	+"entschieden? Tippen Sie 'sanft' fuer den sanften Weg zum eigenen Heim\n"
	+"oder 'schnell' fuer den schnellen Weg.\n] " );
  input_to("sign", 0, vertrag, flag|V_EP);
  return 1;
}

void
sign(string str, object vertrag, int flag)
{
  object block;
  string s;
  int *al;

  if (str == "sanft" || str == "schnell") {
    write( "Der Schalterbeamte gibt Dir einen Fuellfederhalter sowie ein Skalpell.\n"
	  +"Schwungvoll setzt Du Deine Unterschrift unter den Vertrag. Dann piekst\n"
	  +"Du Dir mit dem Skalpell in den Finger und bestaetigst die Unterschrift\n"
	  +"mit einem dunklen Tropfen Deines Blutes.\n"
	  +"Ploetzlich zucken Blitze um Dich herum auf, und der Schalterbeamte lacht\n"
	  +"droehnend auf. Du hast das Gefuehl, dass hier etwas nicht ganz mit rechten\n"
	  +"Dingen vor sich geht. Dann bestaetigt der Beamte die Unterschrift und\n"
	  +"uebergibt Dir den ersten Block, auf dem Deine Raten vermerkt werden.\n"
	  +"Der Beamte erklaert: Damit auf den Block auch eingezahlt wird, muss man\n"
	  +"ihn wie eine Ruestung anziehen!\n" );

    this_player()->reduce_hit_points(50);
    vertrag->Sign(flag | (str == "sanft" ? V_SIGNED : V_SIGNED | V_FAST));
    (block = clone_object(PATH+"block"))->move(this_player(), M_NOCHECK);
    block->SetProp("schwer", V_FAST & vertrag->QueryProp(P_AUTOLOADOBJ)[1]);
    al = block->QueryProp(P_AUTOLOADOBJ);
    al[V_FLAGS] |= B_ACTIVE;
    block->SetProp(P_AUTOLOADOBJ, al);
    s = "Antrag: "+getuid(this_player())+" unterschrieb den Vertrag ("+str+").\n";
    HLOG( "BANK.LOG", s );
    HLOG( getuid(this_player()), s );
  }
  else {
    write( "Der Schalterbeamte sagt: Nur 'sanft' oder 'schnell', bitte!\n" );
    input_to("sign", 0, vertrag, flag);
  }
}

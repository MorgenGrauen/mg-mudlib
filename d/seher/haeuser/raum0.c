//
//  raum0.c  -- Der Eingangsraum des Hauses
//
//  Grundobjekt (c) 1994 Boing@MorgenGrauen
//  Abschliessen und Rauswerfen von Jof
//  Fuer Aenderungen ab dem 06.10.94 verantwortlich: Wargon
//  Fuer Aenderungen ab dem 01.01.04 verantwortlich: Vanion
//
// $Date: 1996/02/21 18:13:41 $
// $Revision: 2.5 $
/* $Log: raum0.c,v $
 * Revision 2.5  1996/02/21  18:13:41  Wargon
 * *** empty log message ***
 *
 * Revision 2.4  1995/06/29  08:54:57  Wargon
 * Neuer Befehl: "spion" zeigt, wer vor der Tuer steht.
 *
 * Revision 2.3  1995/06/28  08:59:25  Wargon
 * init() meldet dem Hausbesitzer Spielerrueckmeldungen.
 *
 * Revision 2.2  1995/02/22  21:30:12  Wargon
 * "uebersicht" jetzt auch fuer Wargon ueberall moeglich. ;)
 *
 * Revision 2.1  1995/02/04  14:31:47  Wargon
 * AddItem(truhe) rausgenommen. Wird nun in Load() erledigt (siehe raum.c).
 *
 * Revision 2.0  1995/02/01  20:38:24  Wargon
 * Von raum.c abgekoppelt.
 *
 */
/*
Letzte Aenderung: Vanion, 29. Feb 2004
                 - "uebersicht erlaubt" zeigt nun nur an, wer erlaubt ist.
*/

#include "haus.h"
#include <wizlevels.h>
#include <properties.h>
#include <moving.h>

inherit RAUM;

void create()
{
  if (!clonep(this_object()) && object_name(this_object())==(PATH+"raum0")) {
    set_next_reset(-1);
    return;
  }
  ::create();

  Set(H_REPORT, 0);
  Set(H_REPORT, SAVE, F_MODE);

  AddCmd( "erlaube", "erlauben" );
  AddCmd( "verbiete", "verbieten" );
  AddCmd( "oeffne", "oeffne" );
  AddCmd( ({ "schliess", "schliesse" }), "schliesse" );
  AddCmd( "spion", "spion" );
}

void notifyRep(int r)
{
  printf("Seit Deinem letzten Besuch gab es %s Rueckmeldung%s von anderen Spielern\nin Deinem Haus.\n", (r==1) ? "eine" : to_string(r), (r==1) ? "" : "en");
}

public varargs void init(object origin)
{
  int r;

  ::init();

  if (tp_owner_check() && r=Query(H_REPORT)) {
    call_out("notifyRep", 0, r);
    Set(H_REPORT, 0);
    Save();
  }
}

int oeffne(string str)
{
  return (HAUSNAME(QueryOwner()))->oeffne(str);
}

int schliesse(string str)
{
  return (HAUSNAME(QueryOwner()))->schliesse(str);
}

varargs int
uebersicht(string str, string pre)
{
  if ( !(tp_owner_check() || IS_MAINTAINER(this_player())) )
    return 0;

  pre = "\n"+arr_out(VERWALTER->HausProp(QueryOwner(), HP_ALLOWED),
	    "Nur Du kannst Dein Haus auf- und abschliessen.\n",
	    "Du und %s koennen Dein Haus auf- und abschliessen.\n",
	    "Neben Dir koennen noch folgende Personen Dein Haus auf- und abschliessen:\n", 1 );

  if (str=="erlaubt")
  {
     this_player()->More(pre);
     return 1;
  }
  
  return ::uebersicht(str, pre);
}

int
erlauben(string str)
{
  string *erlaubt;

  if (!tp_owner_check()) {
    notify_fail( "Du kannst hier gar nichts erlauben!\n" );
    return 0;
  }

  if (!(str=UP_ARGS(this_player())) || str == "") {
    notify_fail( "Wem willst denn das Auf- und Abschliessen Deines Hauses erlauben?\n" );
    return 0;
  }

  erlaubt = VERWALTER->Erlaube(QueryOwner(),
			       map((regexplode(str, "[, ]")-({","," ",""})), #'capitalize));
  arr_out(erlaubt, "Nur Du kannst Dein Haus auf- und abschliessen.\n",
		   "Du und %s koennen Dein Haus auf- und abschliessen.\n",
		   "Neben Dir koennen noch folgende Personen Dein Haus auf- und abschliessen:\n" );
  return 1;
}

int
verbieten(string str)
{
  string *erlaubt;

  if (!tp_owner_check()) {
    notify_fail( "Du kannst hier gar nichts verbieten!\n" );
    return 0;
  }

  if (!(str=UP_ARGS(this_player())) || str == "") {
    notify_fail( "Wem willst denn das Auf- und Abschliessen Deines Hauses verbieten?\n" );
    return 0;
  }
  erlaubt = VERWALTER->Verbiete(QueryOwner(),
				map((regexplode(str, "[, ]")-({","," ",""})), #'capitalize));
  arr_out(erlaubt, "Nur Du kannst Dein Haus auf- und abschliessen.\n",
		   "Du und %s koennen Dein Haus auf- und abschliessen.\n",
		   "Neben Dir koennen noch folgende Personen Dein Haus auf- und abschliessen:\n" );
  return 1;
}

int
spion(string str)
{
  object env, *inv;
  string ow;

  ow = QueryOwner();
  str = VERWALTER->HausProp(ow, HP_ENV);
  call_other(str, "???");
  env = find_object(str);
  inv = all_inventory(env);
  if (env = present("\n"+ow+"haus", env))
    inv -= ({ env });
  if (sizeof(inv))
    printf( "Vor dem Haus siehst Du:\n%s", make_invlist(this_player(), inv));
  else
    printf( "Vor dem Haus ist nichts und niemand zu sehen.\n" );

  return 1;
}

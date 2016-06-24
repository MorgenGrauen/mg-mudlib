/*
 * traghaus.c -- Wie suess! ;)
 *
 * (c) 1994 Boing@MorgenGrauen
 * Be- und ueberarbeitet von Jof und Wargon.
 *
 * HINWEIS: Um einen Bauplatz zu kennzeichnen, muss dort die
 *  Propertie "haus_erlaubt" auf 1 gesetzt werden!
 *
 * 17.11.2003 Feigling - Bugfix: Fehlenden Regionsmagier abgefangen
 *
 * $Date: 1994/10/24 08:25:18 $
 * $Revision: 1.5 $
 * $Log: traghaus.c,v $
 * Revision 1.5  1994/10/24  08:25:18  Wargon
 * Macht jetzt Gebrauch von VERWALTER->Unbebaubar() und dem geaenderten
 * VERWALTER->NeuesHaus().
 *
 * Revision 1.4  1994/10/19  10:03:29  Wargon
 * Hinweis eingebaut (s.o.)
 * Beschraenkung auf ein Seherhaus pro Raum rausgenommen.
 *
 * Revision 1.3  1994/10/09  20:17:03  Wargon
 * In blas() haus->Save() eingefuegt.
 *
 * Revision 1.2  1994/10/07  22:13:17  Wargon
 * Etwas aufgeraeumt.
 * Text beim Giessen geaendert.
 * Falls in dem Raum schon ein Seherhaus steht, kann kein weiteres mehr
 * aufgestellt werden.
 *
 * Revision 1.1  1994/10/07  14:40:54  Wargon
 * Initial revision
 *
 */
#include <properties.h>
#include <moving.h>
#include "haus.h"

inherit "std/thing";

static string owner;

void create()
{
   if (!clonep(this_object())) return;
   ::create();
   owner = getuid(this_player());
   SetProp(P_SHORT, "Ein tragbares Instanthaus");
   SetProp(P_LONG, "Dies ist Dein Haus. Du kannst es abstellen und giessen, dann wird daraus ein\n\
richtiges Seherhaus, in das Du sofort einziehen kannst. Es ist allerdings\n\
noetig, dass der Magier, der den Raum erschaffen hat, in dem Du es abstellen\n\
moechtest, dem Bau eines Seherhauses in diesem Raum zustimmt.\n");
   SetProp(P_NAME, "Haus");
   SetProp(P_NEVERDROP, 1);
   SetProp(P_GENDER, 0);
   AddId(({"haus", "sehe\rhaus"}));
   SetProp(P_VALUE, 0);
   SetProp(P_WEIGHT, 0);
   AddCmd(({"gies", "giess", "giesse"}), "blas");
}

string SetOwner(string o)
{
   return owner = o;
}

string QueryOwner()
{
  return owner;
}

string get_mag(mixed dest)
{
  mapping reg;
  string *mag;
  string ret;
  int i, j;

  catch(reg = "/obj/mliste"->QueryProp("Magierliste"));

  if (mappingp(reg) && member(reg, getuid(dest)))
    mag = reg[getuid(dest)];
  else
    return capitalize(getuid(dest));


  if (!sizeof(mag)) 
    return capitalize(getuid(dest)); //Hinzugefuegt Feigling
  
  if ((i=sizeof(mag)) == 1)
    return capitalize(mag[0]);
  else {
    ret = capitalize(mag[0]);
    j = 1;
    while (j < i-1) {
      ret += ", "+capitalize(mag[j]);
      j++;
    }
    ret += " und "+capitalize(mag[<1]);
  }
  return ret;
}

int move(mixed dest, int method)
{
  int nix;

  if ((method & M_GIVE) == M_GIVE)
  {
    write("Dein sauer verdientes Seherhaus weggeben? NIEMALS!\n");
    return 0;
  }
  if ((method & M_PUT) == M_PUT)
  {
    if ((nix = VERWALTER->Unbebaubar(dest)))
    {
      write( ({ "Hier ist das Bauen von Haeusern untersagt!\n",
		"Du kannst das Haus nicht in geschlossenen Raeumen abstellen.\n",
		"Dieser Platz ist nicht als Bauplatz gekennzeichnet, Du solltest mal\nmit dem verantwortlichen Magier reden. Die Zustaendigkeit fuer diesen Ort\nliegt bei "+get_mag(dest)+".\n"
	     })[nix-1]);
      return 0;
    }
    return ::move(dest, method);
  }
  if ((method & M_GET) == M_GET)
  {
    if (getuid(this_player())!=owner)
    {
      write("Dies ist nicht Dein Haus.\n");
      return 0;
    }
  }
  return ::move(dest, method);
}

int blas(string str)
{
  object haus;

  notify_fail("Was moechtest Du giessen?\n");
  if (!str || str!="haus")
    return 0;
  if (living(environment()))
  {
    write("Du solltest das Haus vielleicht erst mal abstellen.\n");
    return 1;
  }
  if (getuid(this_player())!=owner)
  {
    write("Machst Du immer fremde Haeuser nass?\n");
    return 1;
  }
  if (!environment()->QueryProp(P_HAUS_ERLAUBT))
  {
    write("Dieser Platz ist nicht als Bauplatz gekennzeichnet, Du solltest mal\n\
mit dem entsprechenden Magier reden.\nFrag mal "+capitalize(getuid(environment()))+".\n");
    return 0;
  }
  write( "Voller Vorfreude greifst Du nach Deiner Giesskanne und beginnst, Dein mueh-\n\
sam erspartes Instanthaus zu begiessen. Moment... Giesskanne? Reichlich ver-\n\
dutzt schaust Du auf die kleine gruene Kanne in Deinen Haenden. Wo mag sie\n\
nur hergekommen sein? Doch kaum erreichen die ersten Tropfen das Instanthaus,\n\
wird Deine Aufmerksamkeit auch schon wieder von der Kanne abgelenkt:\n\
Das Trockenhaus beginnt zu schaeumen und sich ueber den Boden auszubreiten.\n\
In dem Schaum entstehen und vergehen Waende, Decken und Fenster, als ob sie\n\
sich noch nicht recht entscheiden koennten, wie sich sich zu einem Ganzen zu-\n\
sammenfuegen sollen.\n\
Dann erhebt sich dort, wo Du das Trockenhaus abgestellt hattest, eine grosse,\n\
schaumige Blase! Sie wird groesser und groesser, bis sie Dich schliesslich\n\
streift. Dabei platzt sie mit einem leisen *PLOPP* - und es bleibt ein be-\n\
zugsfertiges Seherhaus zurueck!\n\
Du wirfst noch einmal einen verstohlenen Blick auf die Giesskanne, musst aber\n\
feststellen, dass sie verschwunden ist und auch verschwunden bleibt.\n" );

  say(capitalize((string)(this_player()->name()))+" verwandelt mit einem Tropfen Wasser "+
      this_player()->QueryPossPronoun(this_object(), WEN) + " Haus.\n");

  VERWALTER->NeuesHaus(owner, environment(this_object()));
  remove();
  return 1;
}

static string _query_autoloadobj()
{
  return owner;
}

#pragma strong_types,save_types,rtt_checks

inherit "/std/thing";

#include <properties.h>
#include <language.h>
#include <defines.h>
#include <sys_debug.h>

#define P_SEITENZAHL "seitenzahl"
#define P_SEITE      "buchseite"
#define P_BUCH       "buchdir"

string *load(string file) { return explode(read_file(file), "@seite@\n"); }

protected void create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Buch");
  SetProp(P_SEITE, 0);
  SetProp(P_BUCH, "/items/buch/buch.info");
  SetProp(P_SEITENZAHL, sizeof(load(QueryProp(P_BUCH))));
  SetProp(P_SHORT, "Wie man ein Buch schreibt!");
  SetProp(P_LONG,
	"Du haeltst ein leinengebundenes Buch in Deinen Haenden. Der Titel lautet:\n"
  +QueryProp(P_SHORT)+"\n");
  SetProp(P_NAME,"Buch");
  AddId("buch");
  SetProp(P_GENDER, NEUTER);
  SetProp(P_WEIGHT,80);
  SetProp(P_NOBUY, 1);
  AddCmd(({"lies","lese"}), "lies");
  AddCmd("oeffne","oeffne");
  AddCmd("schliesse","schliesse");
  AddCmd("blaettere","blaettere");
}

int oeffne(string str) {
  if (!str || !id(str)) return 0;
  if (QueryProp(P_SEITE)) write("Es ist schon geoeffnet.\n");
  else {
	  SetProp(P_SEITE, 1);
	  write("Du oeffnest das Buch auf Seite 1.\n");
	  say(PL->name() + " oeffnet ein Buch.\n",PL);
  }
  return 1;
}

int schliesse(string str) {
  string was;

  if (!str || !id(str)) return 0;
  if (!QueryProp(P_SEITE)) write("Es ist schon geschlossen.\n");
  else {
    SetProp(P_SEITE, 0);
	  write("Du schliesst das Buch wieder.\n");
	  say(PL->name() + " schliesst ein Buch wieder.\n",PL);
  }
  return 1;
}

int blaettere(string str) {
  int seite;

  notify_fail("Zu welcher Seite moechtest Du blaettern?\n");
  if (!str || !(seite = QueryProp(P_SEITE))) return 0;
  if (sscanf(str,"zu seite %d", seite)<1 &&
      ((seite < 1) || (seite > sizeof(load(QueryProp(P_BUCH)))))) return 0;
  write("Du blaetterst zu Seite "+seite+".\n");
  say(PL->name() + "blaettert in einem Buch.\n");
  SetProp(P_SEITE, seite);
  return 1;
}

int lies(string str)
{
  string was;
  int seite;

  notify_fail("Was willst Du lesen ? Syntax: LIES <ETWAS>.\n");
  if(!str || !id(str) || sscanf(str,"%s",was)!=1) return 0;
  say(PL->name() + " liest in einem Buch.\n",PL);

  seite = QueryProp(P_SEITE);
	PL->More(load(QueryProp(P_BUCH))[seite++]);
  if (!QueryProp(P_SEITE)) return 1;
	if (seite >= sizeof(load(QueryProp(P_BUCH)))) {
    SetProp(P_SEITE, 0);
		write("Du hast das Buch ausgelesen und schliesst es.\n");
		say(PL->name() + " hat ein Buch ausgelesen und schliesst es.\n",PL);
	} else {
    SetProp(P_SEITE, seite);
	  write("Du blaetterst um.\n");
		say(PL->name() + " blaettert um.\n",PL);
  }
	return 1;
}


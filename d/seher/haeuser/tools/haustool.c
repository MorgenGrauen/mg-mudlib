#include "../haus.h"
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>

inherit "std/secure_thing";

private int secure()
{
  return (this_interactive() && (IS_ARCH(this_interactive()) ||
				 getuid(this_interactive()) == "wurzel"));
}

protected void create()
{
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_SHORT, "Ein Adressverzeichnis" );
  SetProp(P_LONG,
     "Mit diesem Adressverzeichnis koennen die richtigen Leute Ordnung in den\n"
    +"Wust der Seherhausangelegenheiten bringen.\n"
    +"Die richtigen Leute?\n"
    +"Ja: Man muss schon Erzmagier sein oder Wurzel heissen! ;)\n"
    +"Du kannst es uebrigens lesen!\n" );
  SetProp(P_NAME, "Adressverzeichnis");
  SetProp(P_READ_MSG,
     "Es stehen folgende Befehle zur Verfuegung:\n"
    +"checked <name> [+/-] <nr>\n"
    +"rep\n"
    +"mkmail <name>\n"
    );
  SetProp(P_GENDER, NEUTER);
  SetProp(P_WEIGHT, 100);
  SetProp(P_AUTOLOADOBJ, 1);
  SetProp(P_NEVERDROP, 1);
  SetProp(P_NODROP, 1);

  AddId( ({ "verzeichnis", "adressverzeichnis", "haustool" }) );

  AddCmd("checked", "checked");
  AddCmd("rep", "report");
  AddCmd("mkmail", "mkmail");
}

#define CHECKED PATH+"log/CHECKED"
static int
checked(string str)
{
  mapping drin;
  string name, *lines, *ind, pm;
  int nr, i;
  closure hp;

  if (!secure())
    return 0;

  notify_fail("Syntax: checked <name> [+/-] <nr>\n" );
  if (!str || str == "")
    return 0;

  if (file_size(CHECKED) > 0)
    lines = explode(read_file(CHECKED), "\n") - ({""});
  else
    lines = ({});

  for (drin = ([]), i=sizeof(lines)-1; i>=0; i--) {
    if (sscanf(lines[i], "%s%t%d", name, nr) == 2)
      drin += ([ name : nr-1 ]);
  }
  lines = ({ });
  pm = "";
  hp = symbol_function("HausProp", VERWALTER);

  if (sscanf(str, "%s %d", name, nr)==2 || sscanf(str, "%s %s %d", name, pm, nr) == 3) {
    if (!funcall(hp, name, HP_ENV)) {
      write(capitalize(name)+" hat gar kein Haus!\n");
      return 1;
    }
    if (nr < 0) {
      write("Zum Abziehen nimm bitte 'checked <name> - <nr>'!\n");
      return 1;
    }
    if (member(drin, name))
      i = drin[name];
    switch(pm) {
      case "+":
	if (i+nr > funcall(hp, name, HP_ROOMS)) {
	  printf("%d Raeume? Wie soll das gehen?\n", i+nr);
	  return 1;
	}
	drin[name] += nr;
	break;
      case "-":
	if (i-nr < 0) {
	  write("Soviele Raeume kannst Du "+capitalize(name)+" gar nicht abziehen...\n");
	  return 1;
	}
	drin[name] -= nr;
	break;
      default:
	if (nr > funcall(hp, name, HP_ROOMS)) {
	  printf("%s nat keine %d Raeume!\n",capitalize(name),nr);
	  return 1;
	}
	drin[name] = nr;
    }
    rm(CHECKED);
    for (i=sizeof(ind=sort_array(m_indices(drin),#'<))-1; i>=0; i--)
      write_file(CHECKED, sprintf("%s %d\n",ind[i], 1+drin[ind[i]]));
    return 1;
  }
  else
    return 0;
}

static int
report()
{
  string *rep, *head, name;
  int i, lp, nr;

  if (file_size("/log/report/wurzel.rep")<=0) {
    write("Kein wurzel.rep (Jippieh! ;)\n");
    return 1;
  }

  rep = old_explode(read_file("/log/report/wurzel.rep"),"\n");
  lp = sizeof(PATH)-1;

  for (i=sizeof(rep)-2; i>=0; i-=2) {
    head = old_explode(rep[i], " ");
    if (head[4][0..lp] == PATH) {
      name = old_explode(head[4],"/")[<1];
      nr = to_int(name[<1..<1]);
      name = name[0..<6];
      head[2] = capitalize(head[2]);
      write_file(PATH+"rep/"+name,
		 sprintf("%s in Raum %d:\n%s\n",
			 implode(head[0..2]," "),nr,rep[i+1]));
    }
    else
      write_file(PATH+"rep/wurzel.rep",sprintf("%s\n%s\n", rep[i], rep[i+1]));
  }
  return 1;
}

static int
mkmail(string str)
{
  string f,fm;

  if (!str || str == "") {
    notify_fail( "Syntax: mkmail <name>\n");
    return 0;
  }

  if (!(f=(VERWALTER)->HausProp(str, HP_ENV))) {
    printf("%s hat kein Haus!\n", str);
    return 1;
  }
  fm = PATH+str+".mail";

  write_file(fm, "\
  Huhu!\n\
\n\
Da Du Magier bist, brauchst Du auch kein Seherhaus mehr. Damit Deine\n\
Beschreibungen aber nicht verloren sind, findest Du die Files Deines\n\
Hauses in /players/"+str+"/seherhaus/.\n\
\n\
Soll Dein Haus weiterhin an der jetzigen Stelle stehen bleiben\n\
("+f+"),\n\
so klaere das bitte mit dem dort zustaendigen Magier sowie den zustaen-\n\
digen Regionsmagiern (weil es sich jetzt um den Anschluss von Files an\n\
ein Gebiet handelt).\n\
\n\
!! Das Haus wird auf jeden Fall nach Ende der Haussperre abgerissen !!\n\
\n\
  Wargon.\n");

  return 1;
}

//
// Projecttool
//

#include "/p/service/mandragon/projektbrett/projectmaster.h"
#include "/secure/wizlevels.h"
#include <moving.h>

inherit "/std/thing";
string hilfe;
static int changetime;
static int suppressinfos;

void create()
{
	if (!IS_CLONE(TO)) return;
	::create();
	if (!find_object(PROJECTMASTER)) call_other(PROJECTMASTER,"???");
        
	SetProp(P_NAME,"Schreibfeder");
	SetProp(P_NODROP,"Du moechtest Deine Feder dann doch lieber behalten.\n");
	SetProp(P_NEVERDROP,1);
	SetProp(P_GENDER,FEMALE);
	SetProp(P_SHORT,"Eine kleine magische Schreibfeder");
        if (!changetime) changetime=1;
        hilfe=BS("-----------------------------------------------------------------------------\n"
                 "Diese Feder ermoeglicht es Dir nicht nur, von ueberall her auf die "
                 "allseits beliebten Schwarzen Bretter fuer Projekte zuzugreifen, "
                 "sondern ermoeglicht es Dir auch, selber Projekte dort zu "
                 "veroeffentlichen.\n"
                 "Folgende Kommandos koennen Dir dabei helfen:\n\n"
                 "federhilfe              - Diese Uebersicht\n"
                 "federdoku               - Dokumentation aller Befehle\n"
                 "projekthilfe            - Uebersicht ueber die Brettbefehle\n"
                 "AddProject              - Projekt anlegen\n"
                 "RemoveProject <Nummer>  - Projekt entfernen\n"
                 "ModifyProject <Nummer>  - Projekt aendern\n"
                 "projektinfos ein|an|aus - Aenderungen (nicht mehr) anzeigen\n\n"
                 "Bei Fragen, Bugs und/oder Typos wende Dich bitte vertrauensvoll an den\n"
                 "Mandragon Deiner Wahl.\n"
                 "---------------------------------------------------------------------------");
	SetProp(P_LONG,
		BS("Diese kleine, weisse, magisch glaenzende Feder liegt sanft " +
	           "in Deiner Hand. Du spuerst foermlich, wie sie danach dringt, " +
		   "Neues zu schaffen und die Welt zu vergroessern.\n\n")+hilfe);
        AddId(({"schreibfeder","feder","projekttool"}));
	AddCmd("federhilfe","hilfe_fun");
	AddCmd("projekthilfe","board_help");
	AddCmd("projektliste","board_list");
        AddCmd("federdoku","doku_fun");
	AddCmd(({"projektdetail","projektdetails"}),"board_long");
	AddCmd(({"addproject","AddProject","ap"}),"addproj_fun");
	AddCmd(({"removeproject","RemoveProject","rp"}),"remproj_fun");
	AddCmd(({"modifyproject","ModifyProject","mp"}),"modproj_fun");
        AddCmd("projektinfos","watch_fun");
	TO->move(TP,M_NOCHECK);
	return;
}

void _set_autoloadobj(mixed val)
{
  if (intp(val)) changetime=time();
  else if (pointerp(val)&&sizeof(val)==2)
  {
    changetime=val[0];
    suppressinfos=val[1];
  }
  return;
}

int *_query_autoloadobj() { return ({ changetime, suppressinfos }); }

int doku_fun(string arg)
{
  TP->More(PROJECTDOKU,1);
  return 1;
}

int board_help(string arg)
{
  PROJECTMASTER->BoardHelp();
  return 1;
}

int board_list(string arg)
{
  arg=TP->_unparsed_args(1);
  if (!sizeof(arg)) (PROJECTMASTER)->ShowList();
  else PROJECTMASTER->ParseArgs(arg);
  changetime=time();
  return 1;
}

int board_long(string arg)
{
  int number;
  arg=TP->_unparsed_args(1);
  notify_fail("Syntax: projektdetail <PROJEKTNUMMER>\n");
  if (!sizeof(arg)) return 0;
  if (!(number=to_int(arg))||(arg!=to_string(number))) return 0;
  notify_fail("Projektnummern koennen nur Zahlen groesser Null sein.\n");
  if (number<1) return 0;
  PROJECTMASTER->ShowLong(number);
  return 1;
}

int hilfe_fun(string arg)
{
  tell_object(TP,hilfe);
  return 1;
}

int board_fun(string arg)
{
  PROJECTMASTER->ParseArgs(TP->_unparsed_args(1));
  return 1;
}

int addproj_fun(string arg)
{
  if (!IS_LEARNER(TP))
  {
    tell_object(TP,"Wie bist DU denn an die Feder gekommen?\n");
    TO->remove();
    return 1;
  }
  PROJECTMASTER->ChangeData(0);
  return 1;
}

int remproj_fun(string arg)
{
  if (!IS_LEARNER(TP))
  {
    tell_object(TP,"Wie bist DU denn an die Feder gekommen?\n");
    TO->remove();
    return 1;
  }
  notify_fail("Syntax: removeproject NUMMER\n");
  if (!sizeof(arg)||!intp(to_int(arg))||(arg!=to_string(to_int(arg)))) return 0;
  PROJECTMASTER->DeleteData(to_int(arg));
  return 1;
}

int modproj_fun(string arg)
{
  if (!IS_LEARNER(TP))
  {
    tell_object(TP,"Wie bist DU denn an die Feder gekommen?\n");
    TO->remove();
    return 1;
  }
  notify_fail("Syntax: modifyproject NUMMER\n");
  if (!arg||!intp(to_int(arg))||(arg!=to_string(to_int(arg)))) return 0;
  notify_fail("Es gibt kein Projekt mit der Nummer 0.\n");
  if (to_int(arg)==0) return 0;
  PROJECTMASTER->ChangeData(to_int(arg));
  return 1;
}

int watch_fun(string arg)
{
  if (!IS_LEARNER(TP))
  {
    tell_object(TP,"Wie bist DU denn an die Feder gekommen?\n");
    TO->remove();
    return 1;
  }
  if (!sizeof(arg))
  {
    tell_object(TP,BS(sprintf("Du bekommst derzeit %sInformationen "
                              "ueber Aenderungen am Projektbrett.",
                              suppressinfos?"keine ":"")));
    return 1;
  }
  notify_fail("Syntax: projektinfos <ein|an|aus>\n");
  if (arg!="ein"&&arg!="an"&&arg!="aus") return 0;
  if (arg=="aus")
  {
    suppressinfos=1;
    tell_object(TP,BS("Du wirst jetzt keine Meldungen mehr ueber "
                      "Aenderungen am Projektbrett erhalten.\n"));
    return 1;
  }
  suppressinfos=0;
  tell_object(TP,BS("Du bekommst ab sofort Meldungen ueber Aenderungen "
                    "am Projektbrett.\n"));
  return 1;
}

void init()
{
  if (!IS_LEARNER(TP)) {
    if (find_call_out("remove") == -1)
      call_out("remove",0);
  }
  else ::init();
  return;
}

void reset()
{
  if (!environment())
  {
    destruct(TO);
    return;
  }
  if (!suppressinfos&&changetime<(PROJECTMASTER->ChangeTime()))
    tell_object(environment(),"Intuitiv spuerst Du, dass sich "
                "die Aushaenge am Projektbrett geaendert haben.\n");
  return;
}

/*
 *  The new ultimate quest administration tool
 */

/* AENDERUNGEN
 * 2006-09-29 : Zook
 *              Questtool an neue Felder "Attribute" und "Klasse"
 *              angepasst.
 * 2000-11-18 : Zook:
 *		Questtool um neues Feld "Wartender Magier" des Questmaster
 *		erweitert.
 * 2000-11-18 : Zook:
 *		Questtool um neues Feld "Gruppeneinordnung" des Questmaster
 *		erweitert.
 */

inherit "std/thing";
inherit "std/more";

#include <defines.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include "/secure/questmaster.h"

#define UA this_interactive()->_unparsed_args()

#define QNAME	0
#define QP	1
#define XP	2
#define ALLOW	3
#define INFO	4
#define LEVEL	5
#define CLASS	6
#define ATTR	7
#define WIZ	8
#define SCNDWIZ 9
#define END    10

#pragma strong_types

private void Load_NumKey();
int  Add_Quest	     ( string str );
void ReadNewQuest    ( string str );
int  Remove_Quest    ( string str );
int  Change_Quest    ( string str );
int  Restore_Quest   ( string str );
int  Set_Quest	     ( string str );
int  Remove_Quest    ( string str );
int  Query_Quest     ( string str );
int  Query_Keys      ( string str );
int  Query_Quests    ( string str );
int  ActivateQuest   ( string str );
int  DeactivateQuest ( string str );
int  GetReturn	     ( string str );

string name;
int    qp;
int    xp;
int    level;
int    need;
string *allowed;
string info;
string wizard;
string scndwizard;
int    group;
mapping num_key;

object owner;
int    counter;
mixed  *savequest;
string savekey;
int    do_change;
string changekey;

void create()
{
  string str;

  if(IS_BLUE(ME)) return;
  thing::create();
  SetProp(P_GENDER,NEUTER);
  owner = PL;
  AddId("questtool");
  AddId("qt");
  SetProp(P_ARTICLE,0);
  SetProp(P_GENDER, NEUTER);
  str=capitalize(((str=owner->query_real_name())[<1]=='s'||str[<1]=='x'||str[<1]=='z')?str+="'":str+="s");
  SetProp(P_NAME, str+" QuestTool");
  SetProp(P_SHORT, str+" QuestTool");
  SetProp(P_LONG,
   "Das QuestTool dient der Administration von Quests. (by Kirk, 10.11.94)\n"
  +"implementierte Kommandos:\n"
  +"  AddQ			    : Neuen Questeintrag einfuegen\n"
  +"  RemoveQ	  <quest>	    : Questeintrag loeschen\n"
  +"  ChangeQ	  <quest>	    : Questeintrag aendern\n"
  +"  ReStoreQ			    : Zuletzt geloeschten oder geaenderten\n"
  +"				    : Questeintrag restaurieren\n"
  +"  SetQ	  <spieler> <quest> : Quest als 'geloest' markieren\n"
  +"  DelQ	  <spieler> <quest> : Quest als 'nicht geloest' markieren\n"
  +"  QueryQ	  <quest>	    : Questeintrag anzeigen\n"
  +"  QueryK			    : Alle Keys und Indices anzeigen\n"
  +"  QueryAllQ 		    : Alle Eintraege vollstaendig anzeigen\n"
  +"  ActiVateQ   <quest>	    : Quest im Questmaster aktivieren\n"
  +"  DeactiVateQ <quest>	    : Quest im Questmaster deaktivieren\n\n"
  +"  fuer <quest> kann der Index verwendet werden, den QueryK liefert!\n\n"
  +"  (Alle Kommandos koennen abgekuerzt werden.)\n"
	+"   -> Beispiel: DeactiVateQ == DVQ == dvq == deactivateq\n");

  SetProp(P_NEVERDROP, 1);
  SetProp(P_NODROP, 
  "Mit Hilfe des QuestTools koennen Quests eingetragen, oder geloescht\n"+
  "werden. Du brauchst es vielleicht noch, deshalb wirfst Du es nicht weg.\n");
  SetProp(P_NOBUY, 1);
  AddCmd(({ "AddQ", "addq", "AQ", "aq" }),		   "Add_Quest");
  AddCmd(({ "RemoveQ", "removeq", "RQ", "rq" }),	   "Remove_Quest");
  AddCmd(({ "ChangeQ", "changeq", "CQ", "cq" }),	   "Change_Quest");
  AddCmd(({ "ReStoreQ", "restoreq", "RSQ", "rsq" }),	   "Restore_Quest");
  AddCmd(({ "SetQ", "setq", "SQ", "sq" }),		   "Set_Quest");
  AddCmd(({ "DelQ", "delq", "DQ", "dq" }),		   "Del_Quest");
  AddCmd(({ "QueryQ", "queryq", "QQ", "qq" }),		   "Query_Quest");
  AddCmd(({ "QueryK", "queryk", "QK", "qk" }),		   "Query_Keys");
  AddCmd(({ "QueryAllQ", "queryallq", "QAQ", "qaq" }),	   "Query_Quests");
  AddCmd(({ "ActiVateQ", "activateq", "AVQ", "avq" }),	   "ActivateQuest");
  AddCmd(({ "DeactiVateQ", "deactivateq", "DVQ", "dvq" }), "DeactivateQuest");
  counter = QNAME;
  savekey = "";
  do_change = 0;
  num_key = ([]);
}

int _query_autoloadobj() { return 1; }

void init()
{
  object tp;

  if(!(tp = PL) || tp != environment())
    return;

  thing::init();

  if ( !IS_WIZARD(tp) || tp != owner )
    return call_out("do_remove",1);
}

static void do_remove()
{
  write ( ME->QueryProp(P_SHORT)+" zerfaellt zu Staub.\n");
  say ( ME->QueryProp(P_SHORT)+" zerfaellt in "+PL->name(WESSEN)+
       " unbefugten Haenden zu Staub.\n");
  call_out("remove",1);
}

private void Load_NumKey()
{
  string *keys;
  int i;

  if ( !(keys = (string*) QM->QueryAllKeys()) ) return;
  keys=sort_array(keys,#'>);//')
  for ( i = 0, num_key = ([]); i < sizeof(keys); i++ )
    num_key += ([(i+1)+"":keys[i]]);
}

int Add_Quest(string str)
{
  counter = QNAME;
  write ( "Neue Quest: (Abbruch mit '.' oder '~q')\n" );
  write ( "Key :\n" ); 
  input_to ( "ReadNewQuest" );
  return 1;
}

void ReadNewQuest (string str)
{
  mixed *oldquest;
  int errstat;
  int active;

  if ( str == "~q" || str == "." )
  {
    counter = QNAME;
    if(do_change)
    {
      do_change = 0;
      changekey = "";
      savekey = "";
      savequest = ({});
    }
    return;
  }

  switch ( counter )
  {
    case QNAME:
      if((!str||!sizeof(str)) && do_change)
	name = savekey;
      else
	name = str;
      counter++;
      write ( sprintf("Punkte (>0) [%d]:\n",
		      (do_change?(int)savequest[0]:10)) ); 
      input_to ( "ReadNewQuest" );
      break;
    case QP:
      if((!str||!sizeof(str)))
	if(do_change)
	  qp = (int) savequest[Q_QP];
	else
	  qp = 10;
      else
	sscanf ( str, "%d", qp );
      counter++;
      write ( sprintf("Erfahrung (>=0) [%d]:\n",
		      (do_change?(int)savequest[1]:qp*1000)));
      input_to ( "ReadNewQuest" );
      break;
    case XP: 
      if((!str||!sizeof(str)))
	if(do_change)
	  xp = (int) savequest[Q_XP];
	else
	  xp = qp * 1000;
      else
	sscanf ( str, "%d", xp );
      counter++;
      write ( sprintf("Filenamen file1[,file2,...]) %s:\n",
		      (do_change?"["+implode((string *)savequest[2],",")+"]":
		       "")) );
      input_to ( "ReadNewQuest" );
      break;
    case ALLOW:
      if((!str||!sizeof(str)) && do_change)
	allowed = (string *) savequest[Q_ALLOWED];
      else
	allowed = old_explode(implode(old_explode(str,".c"),""),",");
      counter++;
      write ( sprintf("Info %s:\n", (do_change?
				     "["+break_string("\b\b\b\b\b\b"+(string)
						      savequest[3]+"]",78,6)
				     :"")) ); 
      input_to ( "ReadNewQuest" );
      break;
    case INFO:
      if((!str||!sizeof(str)) && do_change)
	info = (string) savequest[Q_HINT];
      else
	info = str;
      counter++;
      write ( sprintf("Stufe (-1 <=lev<=10) (-1 ist eine Seherquest) [%d]:\n",
		      (do_change?(int)savequest[4]:0)) );
      input_to ( "ReadNewQuest" );
      break;
    case LEVEL:
      if((!str||!sizeof(str)))
	if(do_change)
	  level = (int) savequest[Q_DIFF];
	else
	  level = 0;
	sscanf ( str, "%d", level );
      counter++;
      write ( sprintf("Klasse ([012345]) [%d]:\n",
		      (do_change?(int)savequest[5]:0)) ); 
      input_to ( "ReadNewQuest" );
      break;
    case CLASS:
      if((!str||!sizeof(str)))
	if(do_change)
	  need = (int) savequest[Q_CLASS];
	else
	  need = 0;
      else
	sscanf ( str, "%d", need );
      counter++;
      write ( sprintf("Attribut [01234]\n(1=fleissig,2=heroisch,3=episch,4=legendaer) %s:\n",
		      (do_change?"["+(string)savequest[9]+"]":"")) );
      input_to ( "ReadNewQuest" );
      break;
    case ATTR:
      if ((!str||!sizeof(str)))
	if (do_change)
	  group = (int) savequest[Q_ATTR];
	else
	  group = 0;
      else
	sscanf (str, "%d", group);
      counter++;
      write ( sprintf("Magier %s:\n",
		      (do_change?"["+(string)savequest[7]+"]":"")) );
      input_to ( "ReadNewQuest" );
    break;	
    case WIZ:
      if((!str||!sizeof(str)) && do_change)
	wizard = (string) savequest[Q_WIZ];
      else
	wizard = str;
      wizard = lower_case(wizard);
      counter++;
      write ( sprintf("Wartender Magier %s:\n",
		      (do_change?"["+(string)savequest[8]+"]":"")) );
      input_to ( "ReadNewQuest" );
      break;
    case SCNDWIZ:
      if ((!str||!sizeof(str)) && do_change)
	scndwizard = (string) savequest[Q_SCNDWIZ];
      else
	scndwizard = str;
      scndwizard = lower_case(scndwizard);
      counter++;
      write ( "Eintragen (j/n)?\n" );
      input_to ( "ReadNewQuest" );
      break;
    case END:
      counter=QNAME;

      if ( str != "j" && str != "ja" && str != "y" && str != "yes" )
	return;

      active = 0;
      if ( do_change && changekey && sizeof(changekey) )
      {
	oldquest = (mixed *) QM->QueryQuest ( changekey );

	if ( !pointerp ( oldquest ) || !sizeof ( oldquest ) )
	{
	  write ( "Alten Eintrag nicht gefunden.\n" );
	  return;
	}

	errstat = (int) QM->RemoveQuest( changekey );

	do_change = 0;
	changekey = "";

	switch ( errstat )
	{
	  case	0: write ( "Zugriff auf alten Eintrag verweigert.\n" ); return;
	  case -1: write ( "Parameterfehler beim Loeschen.\n" ); return;
	  case -2: write ( "Alten Eintrag nicht gefunden.\n" ); return;
	  default: write ( "Alter Eintrag geloescht.\n" );
	}
	active = oldquest[6];
	savequest = oldquest;
	savekey   = changekey;
      }

      // add new Quest deactivated by default and keep old active flag
      // if changing an existing entry
      errstat= (int)QM->AddQuest(name,qp,xp,allowed,info,level,need,active,
				 wizard, scndwizard, group);

      switch ( errstat )
      {
	case  0: write ( "Zugriff verweigert.\n" ); break;
	case -1: write ( "Key ungueltig oder sizeof(key) < 5.\n" ); break;
	case -2: write ( "QP ungueltig oder < 1.\n" ); break;
	case -3: write ( "XP ungueltig.\n" ); break;
	case -4: write ( "Filename(n) ungueltig.\n" ); break;
	case -5: write ( "Info ungueltig.\n" ); break;
	case -6: write ( "Stufe ungueltig oder < 0 oder > 20.\n" ); break;
	case -7: write ( "Aktiv-Flag < 0 oder > 1.\n" ); break;
	case -8: write ( "Magiername ungueltig.\n" ); break;
	case -9: write ( "Magiername des wartenden Magiers ungueltig.\n"); break;
	case -10: write ("Falsche Gruppeneinordnung.\n"); break;
	default: write ( "Eintrag eingefuegt.\n" );
      }
    default:
      return;
  }
}

int Remove_Quest ( string str )
{
  mixed *oldquest;
  string newstr;
  int errstat;

  if ( !(str=UA) )
  {
    write ( "Syntax: RemoveQ <name>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
    newstr = str;

  oldquest = (mixed *) QM->QueryQuest ( newstr );

  if ( !pointerp ( oldquest ) || !sizeof ( oldquest ) )
  {
    write ( "Keine Quest dieses Namens gefunden.\n" );
    return 1;
  }

  errstat = (int) QM->RemoveQuest( newstr );

  switch ( errstat )
  {
    case  0: write ( "Zugriff verweigert.\n" ); break;
    case -1: write ( "Parameterfehler.\n" ); break;
    case -2: write ( "Quest nicht gefunden.\n" ); break;
    default: write ( "Quest entfernt.\n" );
  }

  savequest = oldquest;
  savekey   = newstr;

  return 1;
}

int Change_Quest ( string str )
{
  mixed *oldquest;
  string newstr;

  if ( !(str=UA) )
  {
    write ( "Syntax: ChangeQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
    newstr = str;

  oldquest = (mixed *) QM->QueryQuest ( newstr );

  if ( !pointerp( oldquest ) || !sizeof ( oldquest ) )
  {
    write ( "Keine Quest dieses Namens gefunden.\n" );
    return 1;
  }

  do_change = 1;
  changekey = newstr;
  savekey = newstr;
  savequest = oldquest;

  write ( "Aktueller Eintrag:\n");
  write ( "Key              : "+newstr+"\n" );
  write ( "Punkte           : "+oldquest[Q_QP]+"\n" );
  write ( "Erfahrung        : "+oldquest[Q_XP]+"\n" );
  write ( "Filenamen        : "+implode(oldquest[Q_ALLOWED],",")+"\n" );
  write ( "Info             : "+oldquest[Q_HINT]+"\n" );
  write ( "Stufe            : "+oldquest[Q_DIFF]+"\n" );
  write ( "Klasse           : "+QCLASS_STARS(oldquest[Q_CLASS])+"\n" );
  write ( "Attribut         : "+QATTR_STRINGS[oldquest[Q_ATTR]]+"\n" );
  write ( "Magier           : "+capitalize(oldquest[Q_WIZ])+"\n" );
  write ( "Wartender Magier : "+capitalize(oldquest[Q_SCNDWIZ])+"\n" );


  write ( "\nNeue Quest:     (mit '.' oder '~q' kann abgebrochen werden)\n" );
  write ( sprintf("Key [%s]:\n", savekey) );
  input_to ( "ReadNewQuest" );

  return 1;
}

int Restore_Quest ( string str )
{
  int errstat;

  if ( !savekey || !sizeof(savekey) )
  {
    write ( "\nTut mir leid!\n" );
    write ( "Nichts zum Restaurieren gefunden ...\n" );
    return 1;
  }

  errstat = 
	(int)QM->AddQuest( savekey, (int) savequest[0], (int) savequest[1],
			  (string *) savequest[2], (string) savequest[3],
			  (int) savequest[4], (int) savequest[5],
			  (string) savequest[6], (int) savequest[7],
			  (string) savequest[8] );

  switch ( errstat )
  {
    case  0: write ( "Zugriff verweigert.\n" ); break;
    case -1: write ( "Key ungueltig oder sizeof(key) < 5.\n" ); break;
    case -2: write ( "QP ungueltig oder < 1.\n" ); break;
    case -3: write ( "XP ungueltig.\n" ); break;
    case -4: write ( "Filename(n) ungueltig.\n" ); break;
    case -5: write ( "Info ungueltig.\n" ); break;
    case -6: write ( "Stufe ungueltig oder < 0 oder > 20.\n" ); break;
    case -7: write ( "Aktiv-Flag < 0 oder > 1.\n" ); break;
    case -8: write ( "Magiername ungueltig.\n" ); break;
    case -9: write ( "Magiername ungueltig.\n"); break;
    case -10: write ("Ungueltige Gruppennummer.\n"); break;
    default: write ( "'"+savekey+"' restauriert.\n" );
  }
  savekey = "";
  return 1;
}

int Set_Quest ( string str )
{
  string quest, player, newquest;
  int created;
  object ob;

  if ( !(str=UA) || sscanf( str, "%s %s", player, quest ) != 2 )
  {
    write ( "Syntax: SetQ <player> <quest>\n" );
    return 1;
  }

  created=0;
  Load_NumKey();

  if ( !(newquest = num_key[quest+""]) )
    newquest = quest;

  ob=find_player(player);
  if(!ob)
  {
    ob=find_netdead(player);
    if(!ob)
    {
      created=1;
      ob=__create_player_dummy(player);
    }
  }
  if(!ob)
  {
    write("Kein solcher Spieler gefunden.\n!");
    return 1;
  }
  
  write(ERRNO_2_STRING("GQ",(int)ob->GiveQuest(newquest,"__silent__"))+"\n");
  if(created)
  {
    ob->save_me(0);
    if(!(ob->remove()))
    {
    	destruct(ob);
    }
  }
  return 1;
}

int Del_Quest ( string str )
{
  string quest, player, newquest;
  int created;
  object ob;

  if ( !(str=UA) || sscanf( str, "%s %s", player, quest ) != 2 )
  {
    write ( "Syntax: DelQ <player> <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newquest = num_key[quest+""]) )
    newquest = quest;

  created=0;
  ob=find_player(player);
  if(!ob)
  {
    ob=find_netdead(player);
    if(!ob)
    {
      created=1;
      ob=__create_player_dummy(player);
    }
  }
  if(!ob)
  {
    write("Kein solcher Spieler gefunden.\n!");
    return 1;
  }
  
  write(ERRNO_2_STRING("DQ",(int) ob->DeleteQuest ( newquest ))+"\n");
  if(created)
  {
    ob->save_me(0);
    if(!(ob->remove()))
    {
    	destruct(ob);
    }
  }
  return 1;
}

int Query_Quest ( string str )
{
  mixed *quest;
  string newstr;
  int i;

  if ( !(str=UA) )
  {
    write ( "Syntax: QueryQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
	newstr = str;

  quest = (mixed *) QM->QueryQuest( newstr );

  if ( !pointerp( quest ) || !sizeof ( quest ) )
  {
    write ( "Keine Quest dieses Namens gefunden.\n" );
    return 1;
  }

  write ( "Aktueller Eintrag:\n");
  write ( "Key              : "+newstr );
  if(quest[Q_ACTIVE])
    write (" (aktiviert)\n");
  else
    write (" (deaktiviert)\n");
  write ( "Punkte           : "+quest[Q_QP]+"\n" );
  write ( "Erfahrung        : "+quest[Q_XP]+"\n" );
  write ( break_string ( implode( quest[Q_ALLOWED], " " ), 65, "Filenamen	 : " ) );
  write ( break_string ( quest[Q_HINT], 65, "Info             : " ) );
  write ( "Stufe            : "+quest[Q_DIFF]+"\n" );
  printf("Stufe (avg)      : %.2f (%d)\n", quest[Q_AVERAGE][0], 
        quest[Q_AVERAGE][1]);
  write ( "Klasse           : "+ QCLASS_STARS(quest[Q_CLASS])+"\n");
  write ( "Attribute        : "+ QATTR_STRINGS[quest[Q_ATTR]]+"\n");
  write ( "Magier           : "+capitalize(quest[Q_WIZ])+"\n" );
  write ( "Wartender Magier : "+capitalize(quest[Q_SCNDWIZ])+"\n");
  return 1;
}

int Query_Keys ( string str )
{
  string *keys, active;
  mixed *quest;
  int i;

  if ( !(keys = (string *) QM->QueryAllKeys()) )
    return 1;

  write ( "\n" );
  keys=sort_array(keys,#'>);//')
  for ( i = 0; i < sizeof(keys); i++ )
  {
    quest = (mixed *) QM->QueryQuest(keys[i]);
    if(quest[6])
      active="*";
    else
      active=" ";
    write(sprintf("%2d%s %-33s%s",i+1,quest[6]?"*":" ",keys[i],
		      !(i%2)?" ":"\n"));
  }
  if(i%2)
    write("\n");

  return 1;
}

int Query_Quests ( string str )
{
  mixed *quest;
  string *keys, rstr;
  int i;

  if ( !(keys = (mixed *) QM->QueryAllKeys()) )
    return 1;

  keys=sort_array(keys,#'>);//')
  for ( i = 0, rstr = ""; i < sizeof(keys); i++ )
  {
    quest = (mixed *) QM->QueryQuest(keys[i]);
      write ( "\nKey              : "+keys[i] );
  if(quest[Q_ACTIVE])
    write (" (aktiviert)\n");
  else
    write (" (deaktiviert)\n");
  write ( "Punkte           : "+quest[Q_QP]+"\n" );
  write ( "Erfahrung        : "+quest[Q_XP]+"\n" );
  write ( break_string ( implode( quest[Q_ALLOWED], " " ), 78, "Filenamen	 : " ) );
  write ( break_string ( quest[Q_HINT], 78, "Info		 : " ) );
  write ( "Stufe            : "+quest[Q_DIFF]+"\n" );
  write ( "Klasse           : "+ QCLASS_STARS(quest[Q_CLASS])+"\n");
  write ( "Attribute        : "+ QATTR_STRINGS[quest[Q_ATTR]]+"\n");
  write ( "Magier           : "+capitalize(quest[7])+"\n" );
  write ( "Wartender Magier : "+capitalize(quest[8])+"\n");
  }
  More( rstr, 0 );
  return 1;
}

int ActivateQuest( string str )
{
  mixed *quest;
  string newstr;
  int errstat;

  if ( !(str=UA) )
  {
    write ( "Syntax: ActiVateQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
	newstr = str;

  switch( QM->SetActive( newstr, 1 ) )
  {
    case -3: write ( "Ungueltiger Flag.\n" ); break;
    case -2: write ( "Quest war bereits aktiviert.\n" ); break;
    case -1: write ( "Keine Quest dieses Namens gefunden.\n" ); break;
    case  0: write ( "Zugriff verweigert\n" ); break;
    case  1: write ( "Quest '"+newstr+"' aktiviert.\n" ); break;
    default: write ( "unbekannter Fehlerstatus.\n" );
  }
  return 1;
}

int DeactivateQuest( string str )
{
  mixed *quest;
  string newstr;
  int errstat;

  if ( !(str=UA) )
  {
    write ( "Syntax: DeactiVateQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
	newstr = str;

  switch( QM->SetActive( newstr, 0 ) )
  {
    case -3: write ( "Ungueltiger Flag.\n" ); break;
    case -2: write ( "Quest war bereits deaktiviert.\n" ); break;
    case -1: write ( "Keine Quest dieses Namens gefunden.\n" ); break;
    case  0: write ( "Zugriff verweigert\n" ); break;
    case  1: write ( "Quest '"+newstr+"' deaktiviert.\n" ); break;
    default: write ( "unbekannter Fehlerstatus.\n" );
  }
  return 1;
}



#include "../haus.h"

inherit "/p/service/wargon/sm/statemachine";

#include "/p/service/wargon/sm/statemachine.h"
#include <properties.h>

#define STATE_IDLE           0
#define STATE_HOUSE          1
#define STATE_ROOM           2
#define STATE_CHEST          3
#define STATE_DETAIL         4
#define STATE_RDETAIL        5
#define STATE_COMMAND        6
#define STATE_FINALIZE       7
#define STATE_EXITS          8

#define SIG_INITIALIZE       0
#define SIG_HOUSE            1
#define SIG_HOUSE_DONE       2
#define SIG_ROOM             3
#define SIG_DO_CHEST         4
#define SIG_DO_DETAIL        5
#define SIG_DETAIL_DONE      6
#define SIG_RDETAIL_DONE     7
#define SIG_COMMAND_DONE     8
#define SIG_DO_EXITS         9
#define SIG_ROOM_DONE       10

private nosave string file;
private nosave int    final;
private nosave object raum;
private nosave int    nr;
private nosave object hausmeister;
private nosave mixed  hmargs;

string dump(object env, int final, mixed args);

private string special(string str);
private void removeOutputFile();
private void dumpHouse();
private void dumpRoomHeader();
private void dumpChest();
private void dumpDetails();
private void dumpReadDetails();
private void dumpCommands();
private void doFinalize();
private void dumpExits();
private void dumpRoomTrailer();
private void doReturn();
private void sProp(string file, string pName, string pStr);
static void mProp(string file, string pName, mixed map_ldfied, int pos, int signal);
static void cProp(string file, string pName, mixed cmd, int pos, int signal);
private void prettyString(string file, string *str, int indent);
private void prettyArray(string file, string *arr, int sep);
private void headerRoom();
private void trailerRoom();
private void headerHouse();
private void trailerHouse();

create()
{
  ::create();
  seteuid(getuid());
  
  AddState(STATE_IDLE, 0, 0);
  AddState(STATE_HOUSE,    #'dumpHouse, 0);
  AddState(STATE_ROOM,     #'dumpRoomHeader, 0);
  AddState(STATE_CHEST,    #'dumpChest, 0);
  AddState(STATE_DETAIL,   #'dumpDetails, 0);
  AddState(STATE_RDETAIL,  #'dumpReadDetails, 0);
  AddState(STATE_COMMAND,  #'dumpCommands, 0);
  AddState(STATE_FINALIZE, #'doFinalize, 0);
  AddState(STATE_EXITS,    #'dumpExits, 0);

  AddTransition(SM_INVALID, SIG_INITIALIZE, STATE_IDLE, 0);

  // Zustandsuebergaenge zum Schreiben des Hauses
  AddTransition(STATE_IDLE, SIG_HOUSE, STATE_HOUSE, #'removeOutputFile);
  AddTransition(STATE_HOUSE, SIG_HOUSE_DONE, STATE_IDLE, #'doReturn);

  // Zustandsuebergaenge zum Schreiben eines Raums
  AddTransition(STATE_IDLE,     SIG_ROOM,      STATE_ROOM, #'removeOutputFile);
  AddTransition(STATE_ROOM,     SIG_DO_CHEST,  STATE_CHEST);
  AddTransition(STATE_ROOM,     SIG_DO_DETAIL, STATE_DETAIL);
  AddTransition(STATE_CHEST,    SIG_DO_DETAIL, STATE_DETAIL);
  AddTransition(STATE_DETAIL,   SIG_DETAIL_DONE,  STATE_RDETAIL);
  AddTransition(STATE_RDETAIL,  SIG_RDETAIL_DONE, STATE_COMMAND);
  AddTransition(STATE_COMMAND,  SIG_COMMAND_DONE, STATE_FINALIZE);
  AddTransition(STATE_FINALIZE, SIG_DO_EXITS,  STATE_EXITS);
  AddTransition(STATE_FINALIZE, SIG_ROOM_DONE, STATE_IDLE, #'doReturn);
  AddTransition(STATE_EXITS,    SIG_ROOM_DONE, STATE_IDLE, #'dumpRoomTrailer);
  
  StateTransition(SIG_INITIALIZE);
}


// Erzeuge normalen LPC-Quellcode aus einem Haus oder Raum.
//
// Parameter:
//   env: Das Haus oder der Raum, aus dem Quellcode erzeugt werden soll
//   fin: Falls gleich 0, so wird Quellcode fuer Typofixes o.ae.
//        erzeugt; das erzeugte File heisst immer 'fixed.c'.
//        Andernfalls wird Quellcode fuer ein eigenstaendiges Objekt
//        erzeugt. Dieses kann z.B. zum Anschluss als "Magierhaus"
//        verwendet werden. Die erzeugten Files heissen
//        'rep/<name>raum<nr>.c' bzw. 'rep/<name>haus.c'. Befindet sich
//        in dem Raum die Truhe, wird zusaetzlich die Datei
//        'rep/<name>truhe.c' geschrieben.
//
// Rueckgabewert:
//   Der Dateiname, in den die Daten geschrieben wurden.

string
dump(object env, int fin, mixed args)
{
  string out;
  int num;

  hausmeister = previous_object();
  hmargs = args;
  
  if (QueryState() != STATE_IDLE)
  {
    write("Der Generator arbeitet noch...\nBitte warten!\n");
    return "<keine Datei generiert>\n";
  }
  
  // Falls ein Raum generiert wird, erhaelt <nr> die Raumnummer.
  // Bei der Aussenansicht des Hauses wird <nr> auf -1 gesetzt.
  if (sscanf(object_name(env), "%sraum%d", out, num) < 2)
  {
    num = -1;
  }
  
  if (fin)
  {
    out = PATH+"rep/"+env->QueryOwner();

    if (num >= 0)
    {
      out += "raum"+num+".c";
    }
    else
    {
      out += "haus.c";
    }
  }
  else
  {
    out = PATH+"fixed.c";
  }

  // Informationen fuer die Statemachine setzen
  file = out;
  final = fin;
  raum = env;
  nr = num;

  if (num >= 0)
  {
    StateTransition(SIG_ROOM);
  }
  else
  {
    StateTransition(SIG_HOUSE);
  }
  return out;
}

// Sonderzeichen \, " und TAB korrekt umwandeln
private string special(string s)
{
  s = implode(explode(s, "\\"), "\\\\");
  s = implode(explode(s, "\t"), "\\t");
  s = implode(explode(s, "\""), "\\\"");

  return s;
}

/*** Funktionen zum Generieren einer Datei aus den Properties des Hauses ***/

#define EXIT_TMPL "\"/players/%s/seherhaus/%sraum%d\""

private void
removeOutputFile()
{
  catch(rm(file));
}

private void
dumpHouse()
{
  headerHouse();
  
  sProp(file, "P_SHORT", raum->QueryProp(P_SHORT));
  write_file(file, "\n");
  sProp(file, "P_LONG", raum->QueryProp(P_LONG));
  write_file(file, "\n");

  if (final)
  {
    mixed n;

    write_file(file, "  SetProp(P_NOGET, 1);\n");
    write_file(file,
               "  SetProp(P_GENDER, "+
               ({"NEUTER","MALE","FEMALE"})[raum->QueryProp(P_GENDER)]+
               " );\n  SetProp(P_NAME, ");
    n = raum->QueryProp(P_NAME);

    if (stringp(n))
    {
      write_file(file, "\""+n+"\"");
    }
    else
    {
      prettyArray(file, n, 0);
    }
    write_file(file, " );\n\n  AddId( ");
    prettyArray(file,
                raum->QueryProp(P_IDS)-({ "sehe\rhaus",
                                          "\n"+raum->QueryOwner()+"haus" }),
                0);
    write_file(file, " );\n");
  }
  trailerHouse();

  StateTransition(SIG_HOUSE_DONE);
}

private void
dumpRoomHeader()
{
  headerRoom();
  
  sProp(file, "P_INT_SHORT", raum->QueryProp(P_INT_SHORT));
  write_file(file, "\n");
  sProp(file, "P_INT_LONG", raum->QueryProp(P_INT_LONG));
  write_file(file, "\n");

  if (final && raum->QueryProp(H_CHEST))
  {
    StateTransition(SIG_DO_CHEST);
  }
  else
  {
    StateTransition(SIG_DO_DETAIL);
  }
}

private void
dumpChest()
{
  mixed n;
  string cfile;
  object chest;

  cfile = PATH+"rep/"+raum->QueryOwner()+"truhe.c";
  chest = present(TRUHE, raum);
  
  catch(rm(cfile));

  write_file(cfile,
             "#include <properties.h>\n#include <moving.h>\n\n"
             "inherit \"/d/wueste/durian/behaelter\";\n\n"
             "void create()\n{\n  if(!clonep(this_object())) return;\n\n"
             "  ::create();\n\n" );
  sProp(cfile, "P_SHORT", chest->QueryProp(P_SHORT));
  sProp(cfile, "P_LONG",  chest->QueryProp(P_LONG));

  write_file(cfile, "  SetProp(P_NOGET, 1);\n");
  write_file(cfile,
             "  SetProp(P_GENDER, "+
             ({"NEUTER","MALE","FEMALE"})[chest->QueryProp(P_GENDER)]+
             " );\n  SetProp(P_NAME, ");

  n = chest->QueryProp(P_NAME);
  if (stringp(n))
  {
    write_file(cfile, "\""+n+"\"");
  }
  else
  {
    prettyArray(file, n, 0);
  }
  write_file(cfile, " );\n\n  AddId( ");
  prettyArray(cfile, chest->QueryProp(P_IDS)-({ TRUHE }), 0);
  write_file(cfile, " );\n");

  if ((n=chest->QueryProp(P_ADJECTIVES)) && sizeof(n))
  {
    write_file(cfile, "\n  AddAdjective( ");
    prettyArray(cfile, n, 0);
    write_file(cfile, " );\n");
  }
  if ((n=chest->QueryProp(P_NAME_ADJ)) && sizeof(n))
  {
    write_file(cfile, "\n  SetProp(P_NAME_ADJ, ");
    prettyArray(cfile, n, 0);
    write_file(cfile, " );\n");
  }
  write_file(cfile, "}\n");

  StateTransition(SIG_DO_DETAIL);
}

private void
dumpDetails()
{
  mapping prop;
  mixed det;
  
  prop = raum->QueryProp(P_DETAILS);

  if (sizeof(prop))
  {
    det = VERWALTER->PCrunch(prop);
    // next state transition happens when do_mProp is done
    mProp(file, "AddDetail", det, sizeof(det)-1, SIG_DETAIL_DONE);
  }
  else
  {
    StateTransition(SIG_DETAIL_DONE);
  }
}

private void
dumpReadDetails()
{
  mapping prop;
  mixed rdet;
  
  prop = raum->QueryProp(P_READ_DETAILS);

  if (sizeof(prop))
  {
    rdet = VERWALTER->PCrunch(prop);
    // next state transition happens when do_mProp is done
    mProp(file, "AddReadDetail", rdet, sizeof(rdet)-1, SIG_RDETAIL_DONE);
  }
  else
  {
    StateTransition(SIG_RDETAIL_DONE);
  }
}

private void
dumpCommands()
{
  mapping comm;
  mixed cmd;
  
  comm = raum->QueryProp(H_COMMANDS);

  if (sizeof(comm))
  {
    cmd = VERWALTER->PCrunch(comm);
    // next state transition happens when do_cProp is done
    cProp(file, "AddUserCmd", cmd, sizeof(cmd)-1, SIG_COMMAND_DONE);
  }
  else
  {
    StateTransition(SIG_COMMAND_DONE);
  }
}

private void
doFinalize()
{
  if (final)
  {
    StateTransition(SIG_DO_EXITS);
  }
  else
  {
    StateTransition(SIG_ROOM_DONE);
  }
}

private void
dumpExits()
{
  string *k, o;
  mapping prop;
  int i, num;

  prop = raum->QueryProp(P_EXITS);
  k = m_indices(prop);
  if (member(k, "raus") >= 0)
  {
    k -= ({ "raus" });
    write_file(file, sprintf("  AddExit( \"raus\", \"%s\");\n", prop["raus"]));
  }
  
  for (i=sizeof(k)-1; i>=0; i--)
  {
    if (sscanf(prop[k[i]], PATH+"%sraum%d", o, num) != 2)
    {
      printf("Komischer Exit (%O)\n%s -> %s\n", raum, k[i], prop[k[i]]);
    }
    else
    {
      if (o == raum->QueryOwner())
      {
        o = sprintf(EXIT_TMPL, o, o, num);
        write_file(file, sprintf("  AddExit( \"%s\", %s);\n", k[i], o));
      }
      else
      {
        write_file(file,
                   sprintf("  AddExit( \"%s\", \"%s\");\n", k[i], prop[k[i]]));
        printf("Exit von %O nach %s!\n", raum, prop[k[i]]);
      }
    }
  }
  StateTransition(SIG_ROOM_DONE);
}

private void
dumpRoomTrailer()
{
  trailerRoom();
  doReturn();
}

private void
doReturn()
{
  if (hausmeister != 0)
  {
    apply(#'call_other, hausmeister, "GenerationDone", hmargs);
  }
  destruct(this_object());
}

private void
sProp(string f, string pName, string pStr)
{
  string *str;

  write_file(f, "  SetProp( "+pName+",");

  if (!pStr)
  {
    write_file(f, "0 );\n");
    return;
  }

  pStr = special(pStr);

  if (sizeof(str=old_explode(pStr,"\n")) > 1)
  {
    prettyString(f, str, 4);
  }
  else
  {
    if (sizeof(str)==0)
    {
      str = ({""});
    }
    write_file(f, "\n     \""+str[0]);

    if (pStr[<1] == '\n')
    {
      write_file(f, "\\n\"");
    }
    else
    {
      write_file(f, "\"");
    }
  }
  write_file(f, " );\n");
}

static void
mProp(string file, string pName, mixed cmd, int pos, int signal)
{
  int i;
  string *eq, t1;
  
  for (i = pos; (i >= 0) && (get_eval_cost() > 10000); --i)
  {
    write_file(file, "  "+pName+"(\n    ");
    eq = cmd[i][0];
    t1 = cmd[i][1];

    prettyArray(file, eq, 0);
    write_file(file, ", ");

    prettyString(file, old_explode(special(t1), "\n"), 6);
    write_file(file, " );\n");
  }

  // Falls wir die eval_cost ausgereizt haben, aber noch nicht
  // alle Einträge bearbeitet wurden, wird jetzt die naechste
  // Runde gestartet
  if (i >= 0)
  {
    call_out("mProp", 1, file, pName, cmd, i, signal);
  }
  else
  {
    // Ansonsten wechseln wir jetzt in den naechsten Zustand
    write_file(file, "\n");
    StateTransition(signal);
  }
}

static void
cProp(string file, string pName, mixed cmd, int pos, int signal)
{
  string t1, t2;
  mixed  eq;
  int i;

  for (i = pos; (i >= 0) && (get_eval_cost() > 10000); --i)
  {
    write_file(file, "  "+pName+"(\n    ");
    eq = cmd[i][0];
    t1 = cmd[i][1];
    t2 = cmd[i][2];

    prettyArray(file, eq, 1);
    write_file(file, ", 0, ");

    prettyString(file, old_explode(special(t1), "\n"), 4);
    write_file(file, ", ");
    
    if (t2)
    {
      prettyString(file, old_explode(special(t2), "\n"), 4);
    }
    else
    {
      write_file(file, "0");
    }
    
    write_file(file, " );\n");
  }

  // Falls wir die eval_cost ausgereizt haben, aber noch nicht
  // alle Einträge bearbeitet wurden, wird jetzt die naechste
  // Runde gestartet
  if (i >= 0)
  {
    call_out("cProp", 1, file, pName, cmd, i, signal);
  }
  else
  {
    // Ansonsten wechseln wir jetzt in den naechsten Zustand
    write_file(file, "\n");
    StateTransition(signal);
  }
}

private void
prettyString(string f, string *str, int indent)
{
  string ind;
  int i;

  ind = extract("\n          ",0,indent);

  if (!sizeof(str))
  {
    write_file(f, ind+" \"\\n\"");
    return;
  }

  write_file(f, ind+" \""+str[0]+"\\n\"");

  for (i=1; i<sizeof(str); i++)
  {
    write_file(f, ind+"+\""+str[i]+"\\n\"");
  }
}

private void
prettyArray(string f, string *arr, int sep)
{
  int i,j;
  string res, t1, t2;

  write_file(f, "({");

  if (sizeof(arr))
  {
    t1 = ("\""+arr[0]+"\"");
    res = " "+t1;
    t2 = "";

    for (i=1, j=sizeof(arr); i<j; i++)
    {
      t2 = "\""+arr[i]+"\"";
      if (!sep)
      {
	if ((sizeof(t1) + sizeof(t2)) > 69)
        {
	  res += (",\n       "+t2);
	  t1 = t2;
	  t2 = "";
	}
	else {
	  t1 += (", "+t2);
	  res += (", "+t2);
	}
      }
      else {
	res += (",\n       "+t2);
      }
    }
  }
  write_file(f, res + " })" );
}

private void
headerRoom()
{
  if (final)
  {
    write_file(file,
               "#include <properties.h>\n\n"
               "inherit \"std/room\";\n"
               "inherit \""+PATH+"modules/usercmd\";\n\n"
               "create()\n"
               "{\n"
               "  room::create();\n"
               "  usercmd::create();\n\n"
               "  SetProp(P_LIGHT, 1);\n"
               "  SetProp(P_INDOORS, 1);\n\n");
  }
  else
  {
    write_file(file,
               "#include \"haus.h\"\n"
               "#include <properties.h>\n\n"
               "inherit RAUM;\n"
               "inherit \"/std/thing/moving\";\n\n"
               "create()\n"
               "{\n"
               "  if (!clonep(this_object())) return;\n"
               "  ::create();\n\n"
               "  SetOwner(\""+raum->QueryOwner()+"\", "+nr+");\n"
               "  Load();\n\n"
               "  SetProp(P_DETAILS, ([]));\n"
               "  SetProp(P_READ_DETAILS, ([]));\n\n");
  }
}

private void
trailerRoom()
{
  if (final)
  {
    write_file(file, "}\n");
  }
  else
  {
    write_file(file,
               "  Save(1);\n\n"
               "  { object raum;\n"
               "    if (raum = find_object(RAUMNAME("
               "\""+raum->QueryOwner()+"\", "+nr+")))\n"
               "      raum->Load();\n"
               "  }\n\n"
               "  call_out(\"remove\",0);\n"
               "}\n");
  }
}

private void
headerHouse()
{
  if (final)
  {
    write_file(file,
               "#include <properties.h>\n"
               "#include <moving.h>\n"
               "#include \""+PATH+"haus.h\"\n\n"
               "inherit \"std/thing\";\n"
               "inherit HAUSTUER;\n\n"
               "create()\n"
               "{\n"
               "  thing::create();\n"
               "  haustuer::create();\n\n");
  }
  else
  {
    write_file(file,
               "#include \"haus.h\"\n"
               "#include <properties.h>\n\n"
               "inherit HAUS;\n"
               "inherit \"/std/thing/moving\";\n\n"
               "create()\n"
               "{\n"
               "  if (!clonep(this_object())) return;\n"
               "  ::create();\n\n"
               "  SetOwner(\""+raum->QueryOwner()+"\"\n"
               "  Load();\n\n");
  }
}

private void
trailerHouse()
{
  if (final)
  {
    write_file(file, read_file(PATH+"tools/haus.apx"));
    write_file(file, "  this_player()->move(");
    write_file(file, sprintf(EXIT_TMPL,
                             raum->QueryOwner(), raum->QueryOwner(), 0));
    write_file(file,
               ",\n\t\t\tM_GO, 0, \"betritt \"+name(WEN,1), \"kommt herein\");\n"
               "  return 1;\n}\n");
    if (!raum->QueryProp(P_SHORT))
    {
      write_file(file,
                 "\nstring short()\n"
                 "{\n"
                 "  string ret;\n\n"
                 "  ret = ::short();\n"
                 "  if (previous_object() != environment() && !ret)\n"
                 "    ret =\"\";\n\n"
                 "  return ret;\n"
                 "}\n");
    }
  }
  else
  {
    write_file(file,
               "  Save(1);\n\n"
               "  { object raum;\n"
               "    if (raum = find_object(HAUSNAME("
               "\""+raum->QueryOwner()+"\")))\n"
               "      raum->Load();\n"
               "  }\n\n"
               "  call_out(\"remove\",0);\n}\n");
  }
}

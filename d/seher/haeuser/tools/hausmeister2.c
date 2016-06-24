#include "../haus.h"
#include <properties.h>
#include <moving.h>

inherit "std/thing";

static void do_gen(string ow, object env, int nr, int fin, int all);
private string special(string str);
private string dump(int haus, int final);
private void header(string f, int final, object haus, int nr);
private void trailer(string f, int final, object haus, int nr);
private void sProp(string f, string pName, string pStr);
private void mProp(string f, string pName, mapping pMap);
private void cProp(string f, string pName, mapping cMap);
private void prettyString(string f, string *str, int indent);
private void prettyArray(string f, string *arr, int sep);

create()
{
  if (!clonep(this_object())) return;
  ::create();

  SetProp( P_NAME, "Hausmeister");
  SetProp( P_SHORT, "Ein Hausmeister" );
  SetProp( P_LONG,
     "Dies ist der Hausmeister aller Seherhaeuser. Er hilft Wurzel bei der\n"
    +"Verwaltung der Haeuser.\n"
    +"Der Hausmeister hat einen blauen Kittel an und eine Abrissbirne gezueckt.\n" );
  SetProp( P_GENDER,MALE);
  SetProp( P_NOGET, 1);

  AddId( "hausmeister" );

  AddCmd( "generiere", "generieren" );
  AddCmd( ({ "reiss", "reisse" }), "abreissen" );
  AddCmd( "verlege", "verlegen" );
}

static int
bewege(mixed dest, int flags, string msg1, string msg2)
{
  int ret;

  tell_room(environment(), capitalize(name(WER))+" "+msg1+".\n");
  if ((ret = move(dest, flags)) == 1)
    tell_room(environment(), capitalize(name(WER))+" "+msg2+".\n");

  return ret;
}

static int generieren(string str)
{
  object env;
  int nr, fin, all;
  string ow, *parm;

  env = environment(this_player());

  notify_fail( "Syntax: generiere <name> [<nr>] [soft | ganz]\n" );

  if (!str || str == "")
    return 0;

  parm = old_explode(str, " ");
  fin = 1;

  switch (sizeof(parm)) {
    case 3:
      if (parm[2] == "soft")
	fin = 0;
    case 2:
      if (parm[1] == "soft")
	fin = 0;
      else if (parm[1] == "ganz") {
	ow = parm[0];
	nr = (VERWALTER)->HausProp(ow, HP_ROOMS);
	str = ow+"raum"+nr;
	all = 1;
	break;
      }
      else {
	nr = to_int(parm[1]);
	str = parm[0]+"raum"+parm[1];
	ow = parm[0];
	break;
      }
    case 1:
      ow = parm[0];
      nr = -1;
      str = ow+"haus";
      break;
    default:
      return 0;
  }
  if (file_size(HAUSSAVEPATH+ow+".o")<0) {
    if (nr >= 0 && file_size(HAUSSAVEPATH+ow+nr+".o")<0) {
      write( "Es gibt kein '"+str+"'!\n");
      return 1;
    }
  }
  do_gen(ow, env, nr, fin, all);
  return 1;
}

static void do_gen(string ow, object env, int nr, int fin, int all)
{
  string str, out;

  if (nr >= 0)
    str = ow+"raum"+nr;
  else
    str = ow+"haus";

  bewege(PATH+str, M_NOCHECK, "geht zur Arbeit", "kommt an");
  tell_room(environment(), Name(WER)+" zueckt einen Block, sieht sich alles genau an und macht\n"
    +"sich hin und wieder Notizen. Dann nickt er zufrieden und steckt seinen\nBlock wieder weg.\n" );
  out = dump(nr, fin);
  bewege(env, M_NOCHECK, "geht zu seinem Auftraggeber zurueck", "kommt an");
  write( "Der Hausmeister sagt: Ich habe '"+str+"' in die Datei\n"
	+"Der Hausmeister sagt: '"+out+"' geschrieben, Chef!\n");

  if (all) {
    if (nr > 0)
      call_out("do_gen", 1, ow, env, nr-1, fin, all);
    else
      call_out("do_gen", 1, ow, env, -1, fin);
  }
}

static int abreissen(string str)
{
  object haus, env;
  string ich, msg;

  if (!str || str == "" || sscanf(str, "%s ab",str) != 1) {
    notify_fail( "Syntax: reisse <name> ab\n" );
    return 0;
  }
  str = lower_case(str);
  if (!(haus = VERWALTER->FindeHaus(str))) {
    write( capitalize(str)+" hat kein Haus!\n" );
    return 1;
  }
  ich = capitalize(name(WER));
  env = environment();
  bewege(environment(haus), M_NOCHECK, "geht zur Arbeit", "kommt an");
  msg = haus->Name(WER);

  if (VERWALTER->LoescheHaus(str) == -1) {
    tell_room(environment(), ich+" versucht vergeblich, ein Haus abzureissen.\n" );
    msg = "Der Abrissversuch ist fehlgeschlagen, Chef!\n";
  }
  else {
    tell_room(environment(), ich+" betaetigt eine Sirene.\n"+msg+" wird von "+name(WEM)+" eingeebnet.\n");
    msg = msg+" ist abgerissen, Chef!\n";
    if (haus) haus->remove();
  }
  bewege(env, M_NOCHECK, "geht zu seinem Auftraggeber zurueck", "kommt an");
  write( "Der Hausmeister sagt: "+msg);
  return 1;
}

static int verlegen(string str)
{
  string name, von, nach;
  object haus;
  int ret;

  if (!str || sscanf(str, "%s nach %s", name, nach) != 2) {
    notify_fail( "Syntax: verlege <name> nach <ziel>\n" );
    return 0;
  }
  if (nach == "hier")
    nach = object_name(environment(this_player()));

  name = lower_case(name);

  if (!(haus = VERWALTER->FindeHaus(name))) {
    write( "Der Hausmeister sagt: "+capitalize(name)+" hat kein Haus!\n" );
    return 1;
  }
  von = object_name(environment(haus));
  ret = VERWALTER->VerlegeHaus(name, von, nach);
  write( "Der Hausmeister sagt: " );
  switch(ret) {
    case -111:
      write( "Das hast Du nicht zu bestimmen!\n" );
      break;
    case -3:
      write( "Der Zielraum laesst sich nicht laden! Verlegen abgebrochen!\n" );
      break;
    case -4:
      write( "Im Zielraum kann nicht gebaut werden!\n" );
      break;
    case -5:
      write( "Im Haus von "+capitalize(name)+" gibt es noch Ausgaenge in andere Haeuser!\n");
      break;
    case 1:
      write( "OK, Chef, ich habe das Haus verlegt!\n" );
      break;
  }
  return 1;
}

/*** Funktionen zum Generieren einer Datei aus den Properties des Hauses ***/

#define EXIT_TMPL "\"/players/%s/seherhaus/%sraum%d\""

private void dump_chest(string out, object chest)
{
  mixed n;

  catch(rm(out));

  write_file(out, "#include <properties.h>\n#include <moving.h>\n\n"
		  "inherit \"/d/wueste/durian/behaelter\";\n\n"
		  "void create()\n{\n  if(!clonep(this_object())) return;\n\n"
		  "  ::create();\n\n" );
  sProp(out, "P_SHORT", chest->QueryProp(P_SHORT));
  sProp(out, "P_LONG",  chest->QueryProp(P_LONG));

  write_file(out, "  SetProp(P_NOGET, 1);\n");
  write_file(out, "  SetProp(P_GENDER, "+
		    ({"NEUTER","MALE","FEMALE"})[chest->QueryProp(P_GENDER)]+
		    " );\n  SetProp(P_NAME, ");

  n = chest->QueryProp(P_NAME);
  if (stringp(n))
    write_file(out, "\""+n+"\"");
  else
    prettyArray(out, n, 0);
  write_file(out, " );\n\n  AddId( ");
  prettyArray(out, chest->QueryProp(P_IDS)-({ TRUHE }), 0);
  write_file(out, " );\n");

  if ((n=chest->QueryProp(P_ADJECTIVES)) && sizeof(n)) {
    write_file(out, "\n  AddAdjective( ");
    prettyArray(out, n, 0);
    write_file(out, " );\n");
  }
  if ((n=chest->QueryProp(P_NAME_ADJ)) && sizeof(n)) {
    write_file(out, "\n  SetProp(P_NAME_ADJ, ");
    prettyArray(out, n, 0);
    write_file(out, " );\n");
  }
  write_file(out, "}\n");
}

private string dump(int nr, int final)
{
  string out;
  object env;
  mapping prop;

  env = environment(this_object());

  if (final && env->QueryProp(H_CHEST))
    dump_chest(PATH+"rep/"+env->QueryOwner()+"truhe.c", present(TRUHE, env));

  if (final) {
    out = PATH+"rep/"+env->QueryOwner();
    if (nr < 0)
      out += "haus.c";
    else
      out += "raum"+nr+".c";
  }
  else
    out = PATH+"fixed.c";

  catch(rm(out));

  header(out, final, env, nr);
  if (nr<0) {
    sProp(out, "P_SHORT", env->QueryProp(P_SHORT));
    write_file(out, "\n");
    sProp(out, "P_LONG", env->QueryProp(P_LONG));
    write_file(out, "\n");

    if (final) {
      mixed n;

      write_file(out, "  SetProp(P_NOGET, 1);\n");
      write_file(out, "  SetProp(P_GENDER, "+
		      ({"NEUTER","MALE","FEMALE"})[env->QueryProp(P_GENDER)]+
		      " );\n  SetProp(P_NAME, ");
      n = env->QueryProp(P_NAME);
      if (stringp(n))
	write_file(out, "\""+n+"\"");
      else
	prettyArray(out, n, 0);
      write_file(out, " );\n\n  AddId( ");
      prettyArray(out, env->QueryProp(P_IDS)-({ "sehe\rhaus", "\n"+env->QueryOwner()+"haus" }), 0);
      write_file(out, " );\n");
    }
  }
  else {
    sProp(out, "P_INT_SHORT", env->QueryProp(P_INT_SHORT));
    write_file(out, "\n");
    sProp(out, "P_INT_LONG", env->QueryProp(P_INT_LONG));
    write_file(out, "\n");
    if (sizeof(prop = env->QueryProp(P_DETAILS))) {
      mProp(out, "AddDetail", prop);
      write_file(out, "\n");
    }
    if (sizeof(prop = env->QueryProp(P_READ_DETAILS))) {
      mProp(out, "AddReadDetail", prop);
      write_file(out, "\n");
    }
    if (sizeof(prop = env->QueryProp(H_COMMANDS)))
      cProp(out, "AddUserCmd", prop);

    write_file(out, "\n");
    if (final) {
      string *k, o;
      int i, num;

      prop = env->QueryProp(P_EXITS);
      k = m_indices(prop);
      if (member(k, "raus") >= 0) {
	k -= ({ "raus" });
	write_file(out, sprintf("  AddExit( \"raus\", \"%s\");\n", prop["raus"]));
      }
      for (i=sizeof(k)-1; i>=0; i--) {
	if (sscanf(prop[k[i]], PATH+"%sraum%d", o, num) != 2)
	  printf("Komischer Exit (%O)\n%s -> %s\n", env, k[i], prop[k[i]]);
	else {
	  if (o == env->QueryOwner()) {
	    o = sprintf(EXIT_TMPL, o, o, num);
	    write_file(out, sprintf("  AddExit( \"%s\", %s);\n", k[i], o));
	  }
	  else {
	    write_file(out, sprintf("  AddExit( \"%s\", \"%s\");\n", k[i], prop[k[i]]));
	    printf("Exit von %O nach %s!\n", env, prop[k[i]]);
	  }
	}
      }
    }
  }
  trailer(out, final, env, nr);

  return out;
}

private void header(string f, int final, object haus, int nr)
{
  if (final) {
    write_file(f, "#include <properties.h>" );
    if (nr >= 0) {
      write_file(f, "\n\ninherit \"std/room\";\ninherit \""+PATH+"modules/usercmd\";");
      write_file(f, "\n\ncreate()\n{\n  room::create();\n  usercmd::create();\n\n");
      write_file(f, "  SetProp(P_LIGHT, 1);\n  SetProp(P_INDOORS, 1);\n\n");
    }
    else {
      write_file(f, "\n#include <moving.h>\n#include \""+PATH+"haus.h\"\n\n" );
      write_file(f, "inherit \"std/thing\";\ninherit HAUSTUER;\n\n" );
      write_file(f, "create()\n{\n  thing::create();\n  haustuer::create();\n\n");
    }
  }
  else {
    write_file(f, "#include \"haus.h\"\n#include <properties.h>\n\ninherit ");
    write_file(f, ((nr < 0) ? "HAUS" : "RAUM" ) );
    write_file(f, ";\ninherit \"/std/thing/moving\";\n\n");
    write_file(f, "create()\n{\n  if (!clonep(this_object())) return;\n  ::create();\n\n");
    write_file(f, "  SetOwner(\""+haus->QueryOwner()+"\"");
    if (nr >= 0)
      write_file(f, ", "+nr);
    write_file(f, ");\n  Load();\n\n");
    if (nr >= 0)
      write_file(f, "  SetProp(P_DETAILS, ([]));\n  SetProp(P_READ_DETAILS, ([]));\n\n");
  }
}

private void trailer(string f, int final, object haus, int nr)
{
  if (final) {
    if (nr >= 0)
      write_file(f, "}\n");
    else {
      write_file(f, read_file(PATH+"tools/haus.apx"));
      write_file(f, "  this_player()->move(");
      write_file(f, sprintf(EXIT_TMPL, haus->QueryOwner(), haus->QueryOwner(), 0));
      write_file(f, ",\n\t\t\tM_GO, 0, \"betritt \"+name(WEN,1), \"kommt herein\");\n"
		    "  return 1;\n}\n");
      if (!haus->QueryProp(P_SHORT))
	write_file(f, "\nstring short()\n{\n  string ret;\n\n"
		      "  ret = ::short();\n"
		      "  if (previous_object() != environment() && !ret)\n"
		      "    ret =\"\";\n\n  return ret;\n}\n");
    }
  }
  else {
    write_file(f, "  Save(1);\n\n");
    write_file(f, "  { object raum;\n    if (raum = find_object(");
    if (nr >= 0)
      write_file(f, "RAUMNAME(\""+haus->QueryOwner()+"\", "+nr+")))\n");
    else
      write_file(f, "HAUSNAME(\""+haus->QueryOwner()+"\")))\n");
    write_file(f, "      raum->Load();\n  }\n\n  call_out(\"remove\",0);\n}\n");
  }
}

private string special(string s)
{
  s = implode(explode(s, "\\"), "\\\\");
  s = implode(explode(s, "\t"), "\\t");
  s = implode(explode(s, "\""), "\\\"");

  return s;
}

private void sProp(string f, string pName, string pStr)
{
  string *str;
  write_file(f, "  SetProp( "+pName+",");
  if (!pStr) {
    write_file(f, "0 );\n");
    return;
  }
  pStr = special(pStr);
  if (sizeof(str=old_explode(pStr,"\n")) > 1)
    prettyString(f, str, 4);
  else {
    if (sizeof(str)==0)
      str = ({""});
    write_file(f, "\n     \""+str[0]);
    if (pStr[<1] == '\n')
      write_file(f, "\\n\"");
    else
      write_file(f, "\"");
  }
  write_file(f, " );\n");
}

private void mProp(string f, string pName, mapping pMap)
{
  string *eq, t1;
  mixed cmd;
  int i,j;

  if (sizeof(pMap) == 0)
    return;

  cmd = VERWALTER->PCrunch(pMap);

  for (i=sizeof(cmd)-1; i>=0; i--) {
    write_file(f, "  "+pName+"(\n    ");
    eq = cmd[i][0];
    t1 = cmd[i][1];

    prettyArray(f, eq, 0);
    write_file(f, ", ");

    prettyString(f, old_explode(special(t1), "\n"), 6);
    write_file(f, " );\n");
  }
}

private void cProp(string f, string pName, mapping cMap)
{
  string t1, t2;
  mixed cmd, eq;
  int i,j;

  cmd = VERWALTER->PCrunch(cMap);

  for( i=sizeof(cmd)-1; i>=0; i--) {
    write_file(f, "  "+pName+"(\n    ");
    eq = cmd[i][0];
    t1 = cmd[i][1];
    t2 = cmd[i][2];

    prettyArray(f, eq, 1);
    write_file(f, ", 0, ");

    prettyString(f, old_explode(special(t1), "\n"), 4);
    write_file(f, ", ");
    if (t2)
      prettyString(f, old_explode(special(t2), "\n"), 4);
    else
      write_file(f, "0");
    write_file(f, " );\n");
  }
}

private void prettyString(string f, string *str, int indent)
{
  string ind;
  int i;

  ind = extract("\n          ",0,indent);

  if (!sizeof(str)) {
    write_file(f, ind+" \"\\n\"");
    return;
  }
  write_file(f, ind+" \""+str[0]+"\\n\"");
  for (i=1; i<sizeof(str); i++) {
    write_file(f, ind+"+\""+str[i]+"\\n\"");
  }
}

private void prettyArray(string f, string *arr, int sep)
{
  int i,j;
  string res, t1, t2;

  write_file(f, "({");

  if (sizeof(arr)) {
    t1 = ("\""+arr[0]+"\"");
    res = " "+t1;
    t2 = "";

    for (i=1, j=sizeof(arr); i<j; i++) {
      t2 = "\""+arr[i]+"\"";
      if (!sep) {
	if ((sizeof(t1)+sizeof(t2))>69) {
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


#include "../haus.h"

inherit "std/thing";
inherit "/p/service/wargon/sm/statemachine";

#include "/p/service/wargon/sm/statemachine.h"
#include <properties.h>
#include <moving.h>

#define STATE_IDLE       0
#define STATE_GENERATING 1

#define SIG_INIT      0
#define SIG_GENERATE  1
#define SIG_DONE      2

private string dump(int haus, int final);
private void enterGenerating(string ow, object env, int nr, int fin, int all);
private void doGenerating(string ow, object env, int nr, int fin, int all);

create()
{
  if (!clonep(this_object())) return;
  thing::create();
  statemachine::create();
  
  SetProp( P_NAME, "Hausmeister");
  SetProp( P_SHORT, "Ein Hausmeister" );
  SetProp( P_LONG,
     "Dies ist der Hausmeister aller Seherhaeuser. Er hilft Wurzel bei der\n"
    +"Verwaltung der Haeuser.\n"
    +"Der Hausmeister hat einen blauen Kittel an und eine Abrissbirne gezueckt.\n" );
  SetProp( P_GENDER,MALE);
  SetProp( P_NOGET, 1);

  AddId( "hausmeister" );

  AddState(STATE_IDLE, 0, 0);
  AddState(STATE_GENERATING, #'doGenerating, 0);

  AddTransition(SM_INVALID, SIG_INIT, STATE_IDLE);
  AddTransition(STATE_IDLE, SIG_GENERATE, STATE_GENERATING, #'enterGenerating);
  AddTransition(STATE_GENERATING, SIG_GENERATE, STATE_GENERATING, #'enterGenerating);
  AddTransition(STATE_GENERATING, SIG_DONE, STATE_IDLE);

  StateTransition(SIG_INIT);
  
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
    write( "Es gibt kein '"+str+"'!\n");
    return 1;
  }
  StateTransition(SIG_GENERATE, ({ow, env, nr, fin, all }) );
  return 1;
}

static void enterGenerating(string ow, object env, int nr, int fin, int all)
{
  string str;
  
  if (nr >= 0)
    str = ow+"raum"+nr;
  else
    str = ow+"haus";

  bewege(PATH+str, M_NOCHECK, "geht zur Arbeit", "kommt an");
  tell_room(environment(),
            Name(WER)+" zueckt einen Block, sieht sich alles genau an und macht\n"
            +"sich hin und wieder Notizen. Dann nickt er zufrieden und steckt seinen\n"
            "Block wieder weg.\n" );
}

static void doGenerating(string ow, object env, int nr, int fin, int all)
{
  object gen;
  string str, out;

  gen = clone_object(PATH+"tools/generator");
  out = gen->dump(environment(), fin, ({ ow, env, nr, fin, all }));
  
  if (nr >= 0)
    str = ow+"raum"+nr;
  else
    str = ow+"haus";

  bewege(env, M_NOCHECK, "geht zu seinem Auftraggeber zurueck", "kommt an");
  write( "Der Hausmeister sagt: Ich habe '"+str+"' in die Datei\n"
	+"Der Hausmeister sagt: '"+out+"' geschrieben, Chef!\n");
}

void
GenerationDone(string ow, object env, int nr, int fin, int all)
{
  if (all)
  {
    if (nr > 0)
    {
      call_out("StateTransition", 1,
               SIG_GENERATE, ({ ow, env, nr-1, fin, 1 }) );
    }
    else
    {
      call_out("StateTransition", 1,
               SIG_GENERATE, ({ ow, env, -1, fin, 0 }) );
    }
  }
  else
  {
    StateTransition(SIG_DONE, ({ ow, env, -1, fin, 0 }) );
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


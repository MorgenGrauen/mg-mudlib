#include "haus.h"
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>

inherit "/d/wueste/durian/behaelter";
inherit "/p/service/wargon/mat_ed";
inherit "/mail/nedit";

static string owner;

void create()
{
  if (!clonep(this_object())) return;

  ::create();

  Set(P_NAME, SAVE, F_MODE_AS);
  Set(P_SHORT, SAVE, F_MODE_AS);
  Set(P_LONG, SAVE, F_MODE_AS);
  Set(P_GENDER, SAVE, F_MODE_AS);
  Set(P_IDS, SAVE, F_MODE_AS);
  Set(P_INVIS, SAVE, F_MODE_AS);
  Set(P_NAME_ADJ, SAVE, F_MODE_AS);
  Set(P_MATERIAL, SAVE, F_MODE_AS);
  Set(H_DOORLSTAT, SAVE, F_MODE_AS);

  SetProp( P_NAME, "Truhe" );
  SetProp( P_SHORT, "Eine Truhe" );
  SetProp( P_LONG,
     "Eine stabile Holztruhe mit einem Sicherheitsschloss. Nur ausgewaehlte\n"
    +"Personen duerfen die Truhe oeffnen und Sachen in ihr lagern oder aus\n"
    +"ihr nehmen.\n" );
  SetProp( P_GENDER, FEMALE );
  SetProp( P_NOGET, 1 );
  SetProp( P_IDS, ({ "truhe","holztruhe", TRUHE}) );
  SetProp( P_MATERIAL, ([ MAT_MISC_WOOD : 100 ]) );
  SetProp( H_DOORLSTAT, ({ "geschlossen", "geoeffnet"}) );

  AddCmd( ({ "schieb","schiebe"}), "schieben" );
  AddCmd( ({ "beschreib", "beschreibe" }), "beschreiben" );
  AddCmd( ({ "aender", "aendere" }), "aendern" );
}

mixed
_set_owner(string ow)
{
  string *na, *a;
  int i;

  if (ow && stringp(ow))
    restore_object(HAUSSAVEPATH+ow+"truhe");

  if ((na = QueryProp(P_NAME_ADJ)) && (i=sizeof(na))) {
    for (a = ({ }), i-=1; i>=0; i--)
      a += ({ DeclAdj(na[i], WER)[0..<2], DeclAdj(na[i], WEM)[0..<2], DeclAdj(na[i],WEN)[0..<2] });

    a = m_indices(mkmapping(a));
    if (sizeof(a))
      AddAdjective(a);
  }

  SetZu();
  return owner=ow;
}

string
short()
{
  int open;
  string sh, *stat;

  open = (QueryProp(P_CNT_STATUS) == CNT_STATUS_OPEN);
  sh = QueryProp(P_SHORT);
  stat = QueryProp(H_DOORLSTAT);
  if (!stat)
    stat = ({ "", ""});
  else if (strstr(sh, "%s") == -1)
    sh += " (%s)";
  if(QueryProp(P_INVIS)) return "";
  return sprintf(sh, stat[offen])+".\n";
}

oeffne(string str)
{
  object env;

  notify_fail("WAS moechtest Du oeffnen?\n");

  // bin ich gemeint?
  if (!str || present(str, environment()) != this_object())
    return 0;

  if ((env=environment()) && env->id("sehe\rhaus")) {
    if (!env->allowed_check(this_player())) {
      write("Du darfst "+name(WEN)+" nicht oeffnen!\n" );
      return 1;
    }
  }
  return ::oeffne(str);
}

int MayAddWeight(int w)
{
  return 0;
}

static int
schieben(string richtung)
{
  string *aus;
  mapping exits;

  if (!environment() || (!environment()->id("sehe\rhaus")) ||
      !environment()->tp_owner_check())
    return 0;

  notify_fail( "Syntax: schiebe <truhe> nach <richtung>\n");

  if (!richtung || richtung == "")
    return 0;

  aus = old_explode(richtung, " ");
  richtung = aus[<1];
  if (sizeof(aus) < 3 || !id(implode(aus[0..<3]," ")))
    return 0;

  if (member(aus = m_indices(exits = environment()->QueryProp(P_EXITS)),richtung)==-1)
    write( "Du kannst "+name(WEN,1)+" nicht nach '"+richtung+"' schieben!\n");
  else if (!exits[richtung]->tp_owner_check())
    write( "Du kannst "+name(WEN,1)+" nicht aus Deinem Haus hinausschieben!\n");
  else {
    write( "Du schiebst "+name(WEN,1)+" in einen anderen Raum. Dann kehrst\n"
	  +"Du wieder zurueck.\n");
    say(capitalize(this_player()->name(WER))+" schiebt "+name(WEN)+" in einen anderen\n"
	+"Raum, kehrt aber selber wieder.\n");
    environment()->Set(H_CHEST,0);
    environment()->Save();
    move(exits[richtung], M_NOCHECK);
    environment()->Set(H_CHEST,1);
    environment()->Save();
  }
  return 1;
}

private string *brk(string str)
{
  int i;
  string *t1, *t2;

  t1 = regexplode(str, ", *");
  for (t2 = ({}), i=sizeof(t1)-1; i>=0; i-=2)
    t2 = ({t1[i]}) + t2;

  return t2;
}

static void getShort();

static int beschreiben(string str)
{
  string *x;

  if (!str || getuid(this_player()) != owner)
    return 0;

  x = old_explode(str, " ");

  if (x[<1] == "status" && !id(implode(x[0..<2]," ")))
    return 0;
  else if (!id(str) && (str != "truhe"))
    return 0;

  write( "Beschreibung "+name(WESSEN,1)+"\n"
	+"Wenn die Vorgaben nicht geaendert werden sollen, einfach weiter\n"
	+"mit <RETURN>.\n\n");
  getShort();
  return 1;
}

static void getShort()
{
  write("Geschlecht (m, w, s): ["
	+({ "s","m","w" })[QueryProp(P_GENDER)]+"]\n]" );
  input_to("gotGender");
}

static void gotGender(string str)
{
  int gen;
  mixed name;

  if (!str || str == "")
    write("Geschlecht nicht geaendert.\n");
  else if ((gen = member(({'s','m','w'}),str[0])) == -1) {
    write( "Bitte nur 'm', 'w' oder 's' eingeben!\nGeschlecht:\n]");
    input_to("gotGender");
    return;
  }
  else {
    SetProp(P_GENDER, gen);
    write( "Das Geschlecht ist jetzt "+QueryGenderString()+".\n");
  }

  name = QueryProp(P_NAME);
  write("Nun musst Du den Namen eingeben. Sollte der Name unregelmaessig dekliniert\n"
       +"werden, musst Du den Namen in allen vier Faellen in der Reihenfolge Nomi-\n"
       +"nativ, Genitiv, Dativ, Akkusativ und durch Kommata getrennt eingeben.\n"
       +"Name: ["+(pointerp(name) ? implode(name, ", ") : name)+ "]\n]");
  input_to("gotName");
}

static void gotName(string str)
{
  string *n;

  if (!str || member(({ "", ".", "**", "~q"}), str) >= 0)
    write("Name wurde nicht geaendert.\n");
  else {
    n = brk(str);

    if (sizeof(n) == 1)
      SetProp(P_NAME, n[0]);
    else {
      if (sizeof(n) > 4)
	n = n[0..3];
      else
	while (sizeof(n) != 4)
	  n = n + ({ n[0] });
      SetProp(P_NAME,n);
    }
    printf("Der Name ist jetzt '%s'\n", implode(n, ", "));
  }
  write("Du kannst jetzt noch Namensadjektive angeben. Hier darfst Du nur den Wort-\n"
       +"stamm angeben, da die Adjektive automatisch dekliniert werden (also fuer\n"
       +"eine 'alte Truhe' muesstest Du 'alt' angeben).\n"
       +"Du kannst auch mehrere Adjektive, durch Kommata getrennt, angeben.\n"
       +"Um alle Namensadjektive zu loeschen, gib bitte 'nix' ein.\n"
       +"Namensadjektive: ["+implode(QueryProp(P_NAME_ADJ),", ")+"]\n");
  input_to("gotAdj");
}

static void gotAdj(string str)
{
  string *n;

  if (!str || member(({ "", ".", "**", "~q"}), str) >= 0)
    write("Adjektive nicht geaendert.\n");
  else {
    n = brk(str);

    if (member(n, "nix") == -1) {
      SetProp(P_NAME_ADJ, n);
      printf("Namensadjektive lauten %s.\n", implode(n,", "));
    }
    else {
      SetProp(P_NAME_ADJ, ({}) );
      write("Namensadjektive geloescht.\n");
    }
  }
  SetProp(P_SHORT, capitalize(name(WER)));
  write( "Die Kurzbeschreibung lautet damit:\n"+short());
  write( "Falls das Ergebnis nicht Deinen Vorstellungen entspricht, gelangst Du\n"
	+"mit 'n' wieder in die Geschlechtsabfrage, ansonsten geht es in der\n"
	+"Beschreibunssequenz weiter.\n"
	+"Stimmt die Kurzbeschreibung so? (j/n)\n]");
  input_to("gotKurz");
}

static void gotKurz(string str)
{
  string art, nam, *p;
  int gen;

  if (str && sizeof(str) && str[0] == 'n') {
    write( "Neues Spiel, neues Glueck... ;)\n");
    getShort();
    return;
  }
  write("\nIDs (durch Kommata getrennt):\n["+implode(QueryProp(P_IDS)-({TRUHE}), ",")+"]\n]");
  input_to("gotIDs");
}

static void gotIDs(string str)
{
  if (!str || member( ({ "", ".", "**", "~q" }), str) >= 0 )
    write("IDs nicht geaendert.\n");
  else {
    str = implode(old_explode(lower_case(str)," ")-({""})," ");
    str = implode(old_explode(str,", "),",");
    SetProp(P_IDS, old_explode(str, ",")+({TRUHE}));
    write("OK, IDs lauten '"+str+"'\n");
  }
  write("\nBitte Langbeschreibung eingeben (Abbruch mit ~q, Ende mit . oder **):\n");
  nedit("gotLang");
}

static void gotMat(mapping mat);

static void gotLang(string str)
{
  if (!str || str == "")
    write("Die Langbeschreibung wurde nicht veraendert.\n");
  else
    SetProp(P_LONG,implode(old_explode(str, "@@"),"**"));

  write("\nBitte das Material "+name(WESSEN,1)+" bestimmen:\n"
	"(<RETURN> falls nichts geaendert werden soll)\n\n");

  EditMaterial(QueryProp(P_MATERIAL), #'gotMat, 0, ({MATGROUP_GAS,MATGROUP_EXPLOSIVE}));
}

static void gotMat(mapping mat)
{
  if (mappingp(mat)) {
    SetProp(P_MATERIAL, mat);
    write("OK.\n");
  }
  else
    write("Das Material wurde nicht veraendert.\n");

  save_object(HAUSSAVEPATH+owner+"truhe");
}

static int aendern(string str)
{
  if (getuid(this_player())!=owner || !id(str))
    return 0;

  write("Bitte Langbeschreibung aendern.\n(Abbruch mit ~q, Ende mit . oder **, Hilfe mit ~h)\n");
  nedit("gotLang", QueryProp(P_LONG));
  return 1;
}

void Save()
{
  save_object(HAUSSAVEPATH+owner+"truhe");
}

int SmartLog(string ofile, string typ, string msg, string date)
{
  object home;
  string foo, bar;

  write_file(REPFILE(owner), sprintf("%s von %s an der Truhe (%s):\n%s\n",
				     typ,
				     capitalize(getuid(this_player())),
				     date,
				     break_string(msg,78)));

  if (IS_LEARNER(owner)) {
    log_file("report/"+owner+".rep",
	     sprintf("MELDUNG von %s im Seherhaus, an der Truhe (%s):\n"
		     +"Bitte zur Kenntnis nehmen! (Mit dem Befehl 'meldungen')  -Wargon\n",
		     capitalize(getuid(this_player())),
		     date));
  }

  home = load_object(RAUMNAME(owner,0));
  home->Set(H_REPORT, home->Query(H_REPORT)+1);
  home->Save();

  return 1;
}


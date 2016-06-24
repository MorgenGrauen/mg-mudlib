// MorgenGrauen MUDlib
//
// explorer.c -- Tool zur FP-Verwaltung
//
// $Id: explorer.c 8357 2013-02-09 11:16:14Z Zesstra $

inherit "/std/secure_thing";

#include <properties.h>
#include <exploration.h>
#include <wizlevels.h>

create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Der Erforscher");
  SetProp(P_NAME, "Erforscher");
  SetProp(P_GENDER, MALE);
  SetProp(P_LONG, "Dies ist der beruehmte Erforscher des MorgenGrauens. Er\n"
	  +"stellt folgende Befehle zur Verfuegung:\n"
	  +"  epadd <was>: <bonus> <typ> <keys>\n"
	  +"  epchange <was>: <wie> <neu>\n"
	  +"  epdel <was>\n"
	  +"  epinfo <was>\n"
	  +"  epcount <spielername>                   - Anzahl der EPs des Spielers\n"
	  +"  epscan <spielername> [filenamenpattern] - Zeige dessen EPs gefiltert an\n"
	  +"  eppladd <spielername> <ep-anzahl>       - Gib ihm ep-anzahl EPs (zufaellig)\n"
	  +"  eppldel <spielername> <ep-anzahl>       - Loesche ihm ep-anzahl EPs (zufael.)\n"
	  +"  epplset <spielername> <ep-nummer>       - Setze ihm EP ep-nummer\n"
	  +"  epplclr <spielername> <ep-nummer>       - Loesche ihm EP ep-nummer\n"
	  +"Dabei bedeutet:\n"
	  +"<was>   - Das Objekt (eine ID bei Objekten in der Naehe, hier oder\n"
	  +"          here fuer den aktuellen Raum, oder ein Dateiname)\n"
	  +"<bonus> - n fuer normale EPs, b fuer Bonus-EPs (z.B. Para-EPs)\n"
	  +"<typ>   - detail, rdetail, exit/ausgang, cmd/befehl/kommando, info, pub, misc,\n"
	  +"          smell/geruch, sound/noise, taste/touch\n"
	  +"<key>   - Liste der Schluesselwoerter, mit Kommata getrennt\n"
	  +"<wie>   - obj, key, bonus oder typ 1\n"
	  +"<neu>   - Je nach <wie>; siehe <was>, <key>, <typ>\n"
	  );
  SetProp(P_NODROP, 1);
  SetProp(P_NEVERDROP, 1);
  SetProp(P_AUTOLOADOBJ,1);
  AddId(({"explorer","erforscher"}));

  AddCmd(({"epadd"}), "add");
  AddCmd(({"epchange"}), "change");
  AddCmd(({"epdel"}), "del");
  AddCmd(({"epinfo"}), "info");
  AddCmd(({"epcount"}), "epcount");
  AddCmd(({"epscan"}), "epscan");
  AddCmd(({"eppladd"}), "eppladd");
  AddCmd(({"eppldel"}), "eppldel");
  AddCmd(({"epplset"}), "epplset");
  AddCmd(({"epplclr"}), "epplclr");
}

static string strArr(string *s)
{
  string ret;
  int i;

  ret = ("\"" + s[<1] + "\"");
  for (i=sizeof(s)-2; i>=0; i--)
    ret += (", \""+s[i]+"\"");

  return ret;
}

static int getType(string s)
{
  switch(s[0..2]) {
  case "det":
    return EP_DETAIL;
  case "rde":
    return EP_RDET;
  case "aus":
  case "exi":
    return EP_EXIT;
  case "cmd":
  case "bef":
  case "kom":
    return EP_CMD;
  case "inf":
    return EP_INFO;
  case "pub":
    return EP_PUB;
  case "mis":
  case "ver":
    return EP_MISC;
  case "sme":
  case "ger":
    return EP_SMELL;
  case "sou":
  case "noi":
    return EP_SOUND;
  case "tas":
  case "tou":
    return EP_TOUCH;
  }
  return -1;
}

static object getOb(string str)
{
  object ob;

  if (str == "hier" || str == "here" )
    return environment(this_player());

  ob = present(str, environment(this_player()));
  if (!ob)
    ob = present(str, this_player());

  if (!ob) {
    str = "/secure/master"->_get_path(str, getuid(this_player()));
    catch(call_other(str, "???"));
    ob = find_object(str);
  }
  return ob;
}

static string *getKeys(string str)
{
  int i;
  string *t1, *t2;

  t1 = regexplode(str, ", *");
  for (t2 = ({}), i=sizeof(t1)-1; i>=0; i-=2)
    t2 = ({ t1[i] }) + t2;
  return t2;
}

static string errMsg(int code)
{
  if (code >= 0 || code < EPERR_INVALID_ARG)
    return "Unbekannter Fehler";

  code = -(code+1);
  return ({ "Du bist kein Erzmagier", "Ungueltiges Objekt",
	    "Objekt steht nicht in der Liste", "Ungueltiges Argument"})[code];
}

static int add(string str)
{
  string was, t, k, *keys, b;
  int type, bonus;
  object ob;

  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }
  notify_fail("Syntax: epadd <was>: <typ>\n");
  if (!(str = this_player()->_unparsed_args()))
    return 0;

  if (sscanf(str, "%s: %s %s %s", was, b, t, k) !=4)
    return 0;

  if (!(ob = getOb(was))) {
    printf("Kann '%s' nicht finden!\n", was);
    return 1;
  }
  if ((type = getType(t)) < 0) {
    write("Ungueltiger Typ!\n");
    return 1;
  }
  if (b=="n") bonus=0; else if (b=="b") bonus=1;
  else {
    write("Ungueltige Bonusart!\n");
    return 1;
  }
  keys = getKeys(k);

  type = EPMASTER->AddEPObject(ob, keys, type, bonus);
  if (type < 0)
    printf("Fehler: %s\n", errMsg(type));
  return 1;
}

static int change(string str)
{
  string was, wie, neu;
  object ob;
  int type;
  mixed new;

  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }

  notify_fail("Syntax: epchange <was>: <wie> <neu>\n");
  if (!(str = this_player()->_unparsed_args()))
    return 0;

  if (sscanf(str, "%s: %s %s", was, wie, neu) != 3)
    return 0;

  if (!(ob = getOb(was))) {
    printf( "Kann '%s' nicht finden!\n", was);
    return 1;
  }
  switch(wie[0..2]) {
  case "obj":
    type = CHANGE_OB;
    new = getOb(neu);
    if (!new) {
      printf( "Kann '%s' nicht finden!\n", neu);
      return 1;
    }
    break;
  case "key":
    type = CHANGE_KEY;
    new = getKeys(neu);
    break;
  case "typ":
    type = CHANGE_TYPE;
    if ((new = getType(neu)) < 0) {
      write("Ungueltiger Typ!\n");
      return 1;
    }
    break;
  case "bon":
    type = CHANGE_BONUS;
    if (neu=="n") new=0; else if (neu=="b") new=1;
    else {
      write("Ungueltige Bonusart!\n");
      return 1;
    }
    break;
  default:
    write("Das laesst sich nicht aendern...\n");
    return 1;
    break;
  }
  type = EPMASTER->ChangeEPObject( ob, type, new );
  if (type < 0)
    printf("Fehler: %s\n",errMsg(type));

  return 1;
}

static int del(string str)
{
  object ob;
  int ret;

  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }
  if (!str) {
    notify_fail("Syntax: epdel <was>\n");
    return 0;
  }
  if (!(ob = getOb(str))) {
    write("Kann das Objekt nicht finden!\n");
    return 1;
  }
  ret = EPMASTER->RemoveEPObject(ob);
  if (ret < 0)
    printf("Fehler: %s\n", errMsg(ret));

  return 1;
}

static int info(string str)
{
  object ob;
  mixed info;

  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }

  if (!str) {
    notify_fail("Syntax: epinfo <was>\n");
    return 0;
  }

  if (!(ob = getOb(str))) {
    write("Das finde ich leider nicht...\n");
    return 1;
  }
  if (!(info = EPMASTER->QueryEPObject(ob)))
    write ("Das Objekt ist nicht eingetragen!\n");
  else
    printf("Nummer: %d\nBonus: %d\nTyp: %s\nSchluessel: %s\n",
	   info[MPOS_NUM],
	   info[MPOS_TYPE+1],
	   ({ "Detail", "Ausgang", "Kommando", "Info", "Misc", "ReadDetail",
         "Kneipe", "Geruch", "Geraeusch", "Tastdetail"})[info[MPOS_TYPE]],
	   strArr(info[MPOS_KEY] ));

  return 1;
}

static object find_playerob( string name ) {
  return find_player(name)||find_netdead(name);
}

static int epcount( string str ) {
  object pl,epm;
  
  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }
  
  if (!str || str=="") {
    notify_fail("Syntax: epcount <spielername>\n");
    return 0;
  }

  printf( "%s hat %d von %d FPs. (Durchschnitt = %d)\n",
	  capitalize(str),
	  EPMASTER->QueryExplorationPoints(str),
	  EPMASTER->QueryMaxEP(),
	  EPMASTER->QueryAverage()
	  );
  return 1;
}

static int epscan( string str ) {
  object pl;
  int erg;
  string *astr;

  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }
  
  if (!str || str=="") {
    notify_fail("Syntax: epscan <spielername> [Teil des Filenamens]\n");
    return 0;
  }

  astr=old_explode(this_player()->_unparsed_args()," ");
  
  if (sizeof(astr)<2)
    erg = EPMASTER->ShowPlayerEPs(astr[0]);
  else
    erg = EPMASTER->ShowPlayerEPs(astr[0],astr[1]);
  if (erg < 0)
    printf("Fehler: %s\n",errMsg(erg));
  return 1;
}

static eppldel( string str ) {
	string player, rest;
	int epnum;
	object tmp;

	if( !ARCH_SECURITY ) {
	  notify_fail("Du bist kein Erzmagier!\n");
	  return 0;
	}

	notify_fail( "eppldel <spieler> <anzahl_eps> <grund>\n" );
	if( !str || sscanf( str, "%s %d %s", player, epnum, rest )<2 )
		return 0;

	if( epnum <= 0 ) {
		write( "Anzahl der FPs <= 0 ?\n" );
		return 1;
	}
	if( !rest || rest=="" ) {
		write( "Bitte Grund angeben!\n" );
		return 1;
	}

	printf( "%O %O %O\n", epnum, player, secure_level() );
	epnum =	"/secure/explorationmaster"->RemoveFP( epnum, player, rest );
	printf( "Ergebnis (abgezogene EPs) = %O\n", epnum );
	return 1;

}

static eppladd( string str ) {
	string player, rest;
	int epnum;
	object pl,tmp;

	if( !ARCH_SECURITY ) {
	  notify_fail("Du bist kein Erzmagier!\n");
	  return 0;
	}

	notify_fail( "eppladd <spieler> <anzahl_eps>\n" );
	if( !str || sscanf( str, "%s %d", player, epnum )<2 )
		return 0;

	if( epnum <= 0 ) {
		write( "Anzahl der FPs <= 0 ?\n" );
		return 1;
	}

	epnum =	"/secure/explorationmaster"->AddFP( epnum, player );
	printf( "Ergebnis (hinzugefuegte FPs) = %O\n", epnum );
	return 1;

}

static epplset( string str ) {
  string player, rest;
  int epnum;
  object pl,tmp;
  
  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }
  
  notify_fail( "epplset <spieler> <ep-nummer>\n" );
  if( !str || sscanf( str, "%s %d", player, epnum )<2 )
    return 0;
  
  epnum = "/secure/explorationmaster"->SetFP( epnum, player );
  printf( "Ergebnis (gesetzter FP) = %O\n", epnum );
  return 1;
}

static epplclr( string str ) {
  string player, rest;
  int epnum;
  object pl,tmp;
  
  if( !ARCH_SECURITY ) {
    notify_fail("Du bist kein Erzmagier!\n");
    return 0;
  }
  
  notify_fail( "epplclr <spieler> <ep-nummer>\n" );
  if( !str || sscanf( str, "%s %d", player, epnum )<2 )
    return 0;
  
  epnum = "/secure/explorationmaster"->ClearFP( epnum, player );
  printf( "Ergebnis (geloeschter FP) = %O\n", epnum );
  return 1;
}

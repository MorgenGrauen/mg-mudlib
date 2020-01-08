// MorgenGrauen MUDlib
//
// player/soul.c -- Die Seele des Spielers
//
// $Id: soul.c 9527 2016-03-12 11:37:54Z Arathorn $

// Letzte Aenderung vom 08.09.95  Wargon

// Set TabStop to 2 characters

/* Version 1.41 MG, September 95
   - Bei "frage" und "antworte" Test auf P_PERM_STRING fuer Sprachflueche.
   - bugfix bei "schweige" (nahm keine Adverbien an)
   */

/* Version 1.4 MG, August 95
   - Hilfefunktion eingebaut, siehe auch soulhelp.c
   - einige kleinere Aenderungen erst jetzt durch eine neue Version gewuerdigt
   - neue Verben, wie eigentlich fast immer :>
   - typos und bugs gefixed (und neue eingebaut...)
   - Funktion zur Abfrage von Adverbien von aussen eingebaut (Hallo, Anthea :>)
   - so schlimm kann das doch nicht sein, einen TabStop von 2 zu nehmen, fuer
     dieses eine file, alles andere zerlegt meine Formatierung immer so :<
   - koenntet ihr bitte mal oben das "Letzte Aenderung" aendern, wenn ihr
     irgendwo was aendert?
   */

/* Version 1.35 MG, Dezember 94
   - Verben
   - Aenderung des Parsings der quoted adverbs
   - rknuddel ignorieren wird vom normalen ignore uebernommen
   */

/* Version 1.32 MG, Juli 94
   - einige Verben
   - ignorieren des rknuddel
   */

/* Version 1.31 MG, Mai 94
   - einige Verben
   */

/* Version 1.3 MG, Mai 94
   - quoted adverbs GEHEN jetzt
   - ParseRest / ParseRemote neu geschrieben
   */

/* Version 1.21 MG, April 94
   - quoted adverbs
   */

/* Danke an Angus fuer den Text von "liebe" */

/* Version 1.2 MG, Januar 94
   - Umstellung der Feelings von vielen kleinen Funktionen auf eine grosse,
     damit der Funktionsoverhead wegfaellt.
   - neue Ausgabe der Adverbien, mit more und nur noch eines pro Zeile
   - mal wieder neue Verben.
   - und das neue Standardadverb "jofi" :)
   */

/* Version 1.1 MG, November 93
   Aenderungen:
   - Ich habe "alle(n)" eingebaut. Die Verwaltung steht, man kann jetzt
     Verben die Moeglichkeit "alle" geben (Bsp. "wink alle").
   - inzwischen wieder einige Verben mehr, aber muss ich das noch
     erwaehnen?
   - (HASS) Rumata hat mein schoenes System fuer die Ausgabestrings
     verkompliziert. Statt &a steht da jetzt z.B. @@adverb@@, was in
     einer weiteren unnoetigen Funktion resultierte. Naja.
   Highlander ryn Tahar
   */

/*
   Ha! Ich nehme jetzt die erste ganze Versionsnummer fuer mich in Anspruch :)
   So, heute ist's geschafft (Ich auch...). Ich bin fertig - ich mach nur
   noch neue Verben rein, wenn Ideen kommen.
   Gegeben an Dienstag, dem 22. Juni 1993 im heimatlichen Horst.
   Highlander ryn Tahar.
   P.S.: Kopiere sich das Ding, wer will - unter folgenden Bedingungen:
   - Den Goettern von MorgenGrauen und
   - Highlander@TAPPMud     Bescheid sagen und ausserdem
   *seufz* nehmt Highlander hier in MorgenGrauen!! -HrT
   Ha! Ihr koennt wieder TAPPMud nehmen :>  Aber sagt's ruhig in MG. -HrT
   - entweder den ganzen Schwall hier drinlassen oder mich mit einem
   neuen Text erwaehnen.
   Das Ganze unter der Voraussetzung, dass ueberhaupt mal jemand eine deutsche
   Seele braucht und sie nicht selber schreiben will :-) (ersparts euch lieber)
   Highlander ryn Tahar.

   **Arbeit:
   Einbau von Adverbien, andere Reaktionen bei Geistern als "Wie bitte?",
   einige neue Verben, einige alte Verben rausgeschmissen.
   Weil es Probleme mit dem autoloading der playeradverbs gab, wurde
   ausserdem die soul vom Objekt zum inheritfile fuer die playershell.

   **Ideen  zum Weitermachen:
   - (verb) alle, z.B. tritt alle   -- Moeglichkeit eingebaut
   - Geisterverben mit Adverb

   Version 1.0 fuer MorgenGrauen    Highlander Mai/Juni 93
   */


/* Hier ist sie nun, die DEUTSCHE version der soul,
   viel Spass damit.
   Poietix  Mai 1992
   Vers.: 0.4 fuer JRMud
   P.S. bitte meckert nicht dran rum , sondern verbessert und
   erweitert sie.

   Olpp November 1992
   Vers.: 0.5 fuer MorgenGrauen

   He, Olpp, schreibt man nicht die neueste Aenderung UEBER die alten?
   *grins*, Highlander
   */
#pragma strong_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#define SOULHELP "/std/player/soulhelp"
#define LF "\n"
#define NOT_SELF 1
#define NOT_DEAD 1

#define QPP QueryPossPronoun
#define RETURN return _notify_fail
#define Return return 0||_notify_fail  // netter Trick, muss ich mir merken -HrT
#define GHOSTCHECK(sel,oth,vic) if (ghost()) {  write(sel); say(oth,who||ME);\
                                  if (vic) who->Message(vic); return 1;  }
#define HELPCHECK(x) if (str && (str=="-h" || str=="-?" || str=="/h" \
         || str=="/?" || str=="hilfe"))\
                       { More(SOULHELP->Help(x)); return 1; }

// "schau an" als nicht-Untersuchung. Klappt aber anscheinend nicht, weil
// ich nicht gegen den GD ankomme. Also besser auskommentiert lassen.
#ifdef SCHAU_AN
#undef SCHAU_AN
#endif

// Anpiepsen mit Text. Im Moment erlaubt, bei Missfallen auskommentieren und
// in der Hilfe auskommentieren.
#ifdef WECKE
#undef WECKE
#endif

#define WECKE

#define NEED_PROTOTYPES
#include <thing/description.h>
#include <thing/properties.h>
#include <player.h>
#include <player/comm.h>
#include <language.h>
#undef NEED_PROTOTYPES

#include <properties.h>

#include <defines.h>
#include <moving.h>
#include <wizlevels.h>
#include <class.h>

static object who, ofoo;
static int for_all, flag, ifoo;
mapping plr_adverbs;
static string out_sel, out_vic, out_oth, adverb, sfoo;

private void ParseAdverb(string *words);
private string convert_string(string str);
varargs mixed More(string str, int fflag, string returnto);
string MatchAdverb(string str);

mapping
QueryStdAdverbs()  {
  return ([
    "unve" : "unverschaemt",
    "gutg" : "gutgelaunt",
    "gutm" : "gutmuetig",
    "froh" : "froh",
    "glue" : "gluecklich",
    "wuet" : "wuetend",
    "frec" : "frech",
    "daem" : "daemonisch",
    "boes" : "boese",
    "ungl" : "ungluecklich",
    "lang" : "langsam",
    "schn" : "schnell",
    "jamm" : "jammernd",
    "freu" : "freundlich",
    "shue" : "schuechtern",
    "amue" : "amuesiert",
    "aerg" : "aergerlich",
    "aner" : "anerkennend",
    "erst" : "erstaunt",
    "bitt" : "bitter",
    "brei" : "breit",
    "vors" : "vorsichtig",
    "char" : "charmant",
    "kalt" : "kalt",
    "verf" : "verfuehrerisch",
    "zufr" : "zufrieden",
    "tief" : "tief",
    "verz" : "verzweifelt",
    "drec" : "dreckig",
    "vert" : "vertraeumt",
    "uebe" : "ueberzeugt",
    "frus" : "frustriert",
    "stra" : "strahlend",
    "hoff" : "hoffnungsvoll",
    "unge" : "ungeduldig",
    "unsi" : "unsinnigerweise",
    "unsc" : "unschuldig",
    "unwi" : "unwissend",
    "iron" : "ironisch",
    "wiss" : "wissend",
    "gema" : "gemaechlich",
    "sehn" : "sehnsuechtig",
    "laut" : "laut",
    "lieb" : "liebevoll",
    "froe" : "froehlich",
    "dank" : "dankbar",
    "natu" : "natuerlich",
    "gedu" : "geduldig",
    "perf" : "perfekt",
    "vers" : "verspielt",
    "hoef" : "hoeflich",
    "stol" : "stolz",
    "frag" : "fragend",
    "rupp" : "ruppig",
    "trau" : "traurig",
    "vera" : "veraechtlich",
    "scha" : "schamlos",
    "erns" : "ernst",
    "schu" : "schuechtern",
    "zaer" : "zaertlich",
    "sanf" : "sanft",
    "entg" : "entgeistert",
    "heim" : "heimtueckisch",
    "gela" : "gelangweilt",
    "wild" : "wild",
    "jofi" : "wie Jof, wenn er mal nicht idlet",
  ]);
}

mapping
QueryAdverbs() {
  if (extern_call())
    return deep_copy(plr_adverbs);
  return plr_adverbs;
}

string
MatchAdverb(string a)  {
  ParseAdverb(explode(a," "));
  return adverb;
}

// Verwaltungsroutinen

static void
add_soul_commands()  {
  if (!plr_adverbs)
    plr_adverbs=([]);
  add_action("SoulComm", "", 1);
}

static int
verben_liste()  {
  More(SOULHELP->Help());
  return 1;
}

#define ghost() QueryProp(P_GHOST)
#define frog() QueryProp(P_FROG)
#define capname() capitalize(name())
#define gname() (ghost()?(frog()?"Der Geist eines Frosches"\
			        :"Der Geist von "+capname())\
		        :capname())

varargs private void
ParseRest(string arg, mixed extra)  {
  string wie,*words,quotea;
  int num,bis;
  who = for_all = adverb = 0;
  if (!arg) return;
  if (extra)
    if (!pointerp(extra)) {
      if (sscanf(arg, extra+" %s", wie)==1)
      arg=wie;
    }
    else
      for (bis=sizeof(extra),num=0; num<bis; num++)
        if (sscanf(arg, extra[num]+" %s", wie)==1)
          arg=wie;

  if ((bis=strstr(arg, "/"))>=0)
    quotea=arg[bis..],arg=arg[0..bis-1];
  quotea=quotea||"",arg=arg||"";

  words=explode(implode(explode(arg, ","), " und"), " ");
  if (!sizeof(words)) return;
  <string|int> wer;
  if (sizeof(words) && (words[0]=="alle" || words[0]=="allen")) {
    for_all=1;
    wer=words[0];
    words=words[1..];
  }
  if (!for_all)  {     /* noch kein Opfer */
    wer=match_living(lower_case(words[0]));
    if (stringp(wer)) who=present(wer, environment(ME));
    if (!who) who=present(words[0], environment(ME));
    if (who && who->QueryProp(P_INVIS)) who=0;
    }
  if (who && sizeof(words))
    words=words[1..];  /* Opfer gefunden - wenn's eines gibt */
  words+=explode(quotea, " ");
  words-=({""});
  if (sizeof(words)) ParseAdverb(words);
}

private int
ParseRemote(string arg)  {
  string* words;

  adverb = 0; // Adverb vom letzten Mal keinesfalls wiederverwenden. ;-)

  if (!stringp(arg) || !sizeof(arg)) return 0;
  
  words=explode(arg," ");

  mixed liv = match_living(lower_case(words[0]));
  if (stringp(liv))
      who=find_player(liv);
  
  if (who) {
    // Ziel ist ein Spieler.
    if (!who->QueryProp(P_INVIS) || IS_WIZARD(ME))
    {
      // Spieler ist nicht Invis oder ich bin Magier.
      string nam = (query_once_interactive(ME) ? getuid() : 
	             lower_case(name(RAW)));
      if (query_verb()[0..5]=="rknudd" &&
	  who->TestIgnore(nam+".rknuddel") )
      {
        // ich oder das Kommando werde ignoriert.
        write(who->Name(WER)+" ignoriert Deinen Knuddelversuch.\n");
        return 1;
      }
    }
    else
      // Spieler ist invis und ich bin kein Magier.
      who = 0;
  }
  // kein eingeloggter und sichtbarer Spieler. Vielleicht ein NPC? (BTW: kein
  // else if, weil im if fuer Spieler oben who genullt werden kann und dann
  // nochmal nach nem NPC gesucht werden soll.)
  if (!who) {
    string|int wer = match_living(lower_case(words[0]));
    if(stringp(wer)) 
      who=present(wer,environment(ME));
    if (!who) who=present(words[0], environment(ME));
    if (who && who->QueryProp(P_INVIS)) who=0;
  }

  if (!who || sizeof(words)==1) return 0;
  words=words[1..];
  ParseAdverb(words);
  return(0);
}

/**
 Gibt den passenden Adverb-Text zu einem key zurueck
 \param s Danach wird in der Adverbien-Liste gesucht
 \param fuzzy 
 \return Der gefundene Adverbientext oder 0
 */
varargs string GetPlayerAdverb( string s, int fuzzy ) {
  int i, j; 
  string *search_pattern,
         *search_result, 
          result;

  // Erstmal gucken, ob der String direkt gefunden werden kann
  // das geht am schnellsten
  result = QueryStdAdverbs()[s] || plr_adverbs[s];

  // Wenn noch kein Ergebnis gefunden, und man unscharf suchen will
  if ( fuzzy && !result) {

    // Suchmuster fuer das Intersect erzeugen
    search_pattern=({s});

    j = sizeof(s)-1;
    for ( i=2;  i < j ;i++) {
      search_pattern += ({s[0..<i]});
    }

    // Intersect zwischen allen bekannten Abkuerzungen und Search-Pattern
    // erzeugen. Dieses wird dann gleichzeitig nach Laenge sortiert
    // genauester Treffer = String mit groesster Laenge
    search_result = sort_array(
      (m_indices(QueryStdAdverbs()) | m_indices(plr_adverbs))&search_pattern, 
         #'>);

    // Adverb zum genauesten Treffer zurueckgeben
    if (sizeof(search_result)) 
      result = QueryStdAdverbs()[search_result[0]] || 
               plr_adverbs[search_result[0]];
  }

  return result;
}

/**
  Parst die Adverbienparameter fuer Verben und speichert die
  passende Textausgabe in der globalen Variable "adverb"
  \param words Array mit den zu parsenden Adverbien-Strings
*/
private void
ParseAdverb(string *words)  {
  int num,andsign,bis;
  string qadv,*adv,cnt;

  adv=({});
  qadv=0;

  bis=sizeof(words);
  // Sonderfall Gequotetes Adverb (alles nach dem Quote) speichern und aus 
  // Words rausschneiden.
  for (num=0; num<bis; num++)
    if (words[num][0..0]=="/")  {
      words[num]=words[num][1..];
      qadv=implode(words[num..], " ");
      words=words[0..num-1];
      break;
    }

  // Es kann sein, dass vor dem Quote noch eine und steht. Das wird jetzt auch 
  // noch entfernt, muss aber spaeter wieder eingefuegt werden.
  if (sizeof(words) && words[<1]=="und")  {
    words=words[0..<2];
    andsign=1;
  }

  // Weitersuchen?
  if (bis=sizeof(words))
    for (num=0; num<bis; num+=2)
       adv+=({GetPlayerAdverb(words[num], 1)});
  cnt=CountUp(adv-({0}));

  // Ausgabe zusammenbauen
  if (andsign)
    adverb=CountUp((sizeof(adv) ? adv : ({}))+(qadv ? ({qadv}) : ({})));
  else if (sizeof(cnt) && sizeof(qadv))
    adverb = cnt + " " + qadv;
  else if (sizeof(qadv))
    adverb = qadv;
  else if (sizeof(cnt))
    adverb = cnt; 
  if (adverb=="") adverb=0;
}

private mixed MixedOut(int casus)  {
  object *envs,*vics;
  string *names,out,aufz;
  int count,msg;

  for_all=0;
  vics=({});
  names=({});
  envs=all_inventory(environment())-({this_player()});
  if (!(count=sizeof(envs)))
    RETURN("Nichts und niemand da. Schau Dich naechstes Mal besser um.\n");
  for ( ; count--; )
    if (living(envs[count]) && !envs[count]->QueryProp(P_INVIS))  {
      vics+=({envs[count]});
      names+=({envs[count]->name(casus)});
    }
  if (!sizeof(vics))
    RETURN("Keiner da. Schau Dich naechstes Mal besser um.\n");
  aufz=CountUp(names);
  for (count=sizeof(vics); count--;)
  {
    out=implode(explode(out_vic, "@@alle@@"),aufz);
        out = regreplace( out, "\\<"+vics[count]->name(casus)+"\\>",
                          capitalize(vics[count]->QueryDu(casus)), 0 );

    msg=vics[count]->ReceiveMsg(convert_string(out),MT_COMM,MA_EMOTE,
                                0,this_object());
    switch(msg)
    {
      case MSG_DELIVERED:
      case MSG_BUFFERED:
        break;
      case MSG_IGNORED:
      case MSG_VERB_IGN:
      case MSG_MUD_IGN:
        write(vics[count]->Name()+" ignoriert Dich oder diesen Befehl.\n");
        break;
      default:
        write(vics[count]->Name()+" konnte Dich gerade nicht lesen.\n");
    }
  }
  write(break_string(convert_string(implode(explode(out_sel,"@@alle@@"),aufz)
    +LF), 78));
  return 1;
}

varargs private int
CheckLife(int no_self,int no_dead, string no_self_text, string no_dead_text)  {
  if (who && living(who) && who!=this_player()) return 0;
  if (no_self && who && who==this_player())  {
    if (no_self_text)
      write(no_self_text+LF);
    else
      write("Mach das mit anderen, nicht mit Dir selber.\n");
    return 1;
  }
  if (who && !living(who) && no_dead)  {
    if (no_dead_text)
      write(no_dead_text+LF);
    else
      write("Das darfst Du nur mit Lebewesen.\n");
    return 2;
  }
  if (!who)  {
    write("Schau Dich erst mal um - das angegebene Objekt ist nicht da.\n");
    return 3;
  }
  return(0); //non-void. Fall-through, alles OK.
}

private string
convert_string(string str)  {
  /* Ich bin unschuldig, ich hatte das viel einfacher und schoener :)
     Rumata wollte das so ;)  -HrT                                     */
  str = implode( explode( str, "@@name@@" ), capname() );
  str = implode( explode( str, "@@gname@@" ), gname() );
  str = implode( explode( str, "@@wer@@" ),
    (who?capitalize(who->name(WER,2)||""):"" ));
  str = implode( explode( str, "@@ wen@@" ),
    (who?" "+who->name(WEN,2):""));
  str = implode( explode( str, "@@ wem@@" ),
    (who?" "+who->name(WEM,2):""));
  str = implode( explode( str, "@@wen@@" ),
    (who?who->name(WEN,2):""));
  str = implode( explode( str, "@@wem@@" ),
    (who?who->name(WEM,2):""));
  str = implode( explode( str, "@@wessen@@" ),
    (who?who->name(WESSEN,2):""));
  str = implode( explode( str, "@@adverb@@" ),
    (adverb?" "+adverb:"") );
  return str;
}

private int
FeelIt()  {
  int msg, flg;

  flg = MSGFLAG_SOUL;
  if (query_verb() && (query_verb()[0..3]=="frag" || query_verb()[0..3]=="antw"))
    flg |= MSGFLAG_SAY;
  if (query_verb() && (query_verb()[0..5]=="rknudd" || query_verb()=="rwink"))
    flg |= MSGFLAG_REMOTE;

  // NPC haben keine TM-Hist (comm.c). Leider erben aber div. Magier die Soul
  // (trotzdem sie in /std/player/ liegt) in ihren NPC... *fluch*
  if (query_once_interactive(ME))
    _recv(who, break_string(convert_string(out_sel),78), flg);
  else
    tell_object(ME, break_string(convert_string(out_sel),78));

  if (out_vic && who)  {
    if (query_once_interactive(who))  {
      msg=who->Message( break_string(convert_string( out_vic ),78), flg);
      if (msg==-1)
        write(who->name()+" ignoriert Dich oder diesen Befehl.\n");
    } else
      tell_object(who,break_string(convert_string( out_vic ),78));
  }
  if (out_oth)
  say( break_string(convert_string( out_oth ),78), ({who,this_player()}) );
  out_sel=out_vic=out_oth=0;
  return 1;
}

/**
  Die Funktion stellt einen Hilfetext zur Verfuegung und listet die 
  definierten Adverbien auf.
  \param mine 0 = alle Adverbien, 
              1=nur selbst definierte Adverbien 
              2=nur die vom System bereitgestellten
 */
private int zeige_adverbs(int mine)  {
  mapping adverb_list;
  string out,s;

  // Parameter auswerten
  switch (mine){
    case 1:
      adverb_list=plr_adverbs;
      out = "Du hast folgende Adverbien definiert:\n";
      break;
    case 2:
      adverb_list=QueryStdAdverbs();
      out = "Systemseitig sind folgende Adverbien definiert:\n";
      break;
    default:
      adverb_list=QueryStdAdverbs()+plr_adverbs;
      out = "Folgende Adverbien stehen Dir zur Verfuegung:\n";
  }

  out +="  Abk.    Adverb\n  ======  ======\n";

  if ( sizeof(adverb_list) == 0) 
    out += " keine.\n";
  else
    // Ueber alle Elemente der indizies der Adverbienliste gehen
   foreach ( s : sort_array(m_indices(adverb_list), #'> ) ) {
      out += break_string(adverb_list[s],78,
                          sprintf("  %-6s  ",s),BS_INDENT_ONCE);
    }

  More(out+"\nWie diese Adverbien benutzt werden ist in <hilfe adverb> "
    "beschrieben.\n");

  return 1;
}

varargs static int
SoulComm(string str, string _verb)  {
  int t_g,t_n,flag;
  string str1,str2,str3,*taenze,vb;
  out_sel=out_vic=out_oth=who=0;
//  if (this_interactive()!=ME) return 0;
  if (interactive(ME)) str=_unparsed_args(); // NPCs haben das nicht :(
  if (str=="") str=0;
  vb=_verb||query_verb();
  if (sizeof(vb)>1 && vb[<1]=='e' && vb!="noe") vb=vb[0..<2];
  sfoo = 0;
  switch (vb)  {
    /**************** Aechzen ***************/
    case "aechz":
    HELPCHECK("aechz");
    ParseRest(str);
    if (str && !adverb)
      Return("Aechze wie?\n");
    out_sel="Du aechzt@@adverb@@.";
    out_oth="@@gname@@ aechzt@@adverb@@.";
    return FeelIt();

    /**************** Anschmiegen ***************/
    case "schmieg":
    HELPCHECK("schmieg");
    ParseRest(str);
    if (!who)
      Return("An wen willst Du Dich anschmiegen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Das geht doch nicht.",
      "Nein, das macht keinen Spass. Lebt ja nicht mal."))
        return 1;
    out_sel="Du schmiegst Dich@@adverb@@ an@@ wen@@ an.";
    out_vic="@@gname@@ schmiegt sich@@adverb@@ an Dich.";
    out_oth="@@gname@@ schmiegt sich@@adverb@@ an@@ wen@@ an.";
    return FeelIt();

    /**************** Antworten ***************/
    case "antwort":
    HELPCHECK("antwort");
    if (!str)
      Return("Antworte [WEM] WAS?\n");
    ParseRest(str);
    if (!who)
      str1=capitalize(str);
    else
      if (sscanf(str,"%s %s",str1,str1)!=2)
        Return("Antworte was?\n");
      else
        str1=capitalize(str1);
    out_sel="Du antwortest@@ wem@@: "+str1;
    /* Sprachflueche beruecksichtigen -Wargon, 8. 9. 95 */
    if (QueryProp(P_PERM_STRING))
      str1 = call_other(QueryProp(P_PERM_STRING),"permutate_string",str1)||"";
    if (who) out_vic="@@gname@@ antwortet Dir: "+str1;
    out_oth="@@gname@@ antwortet@@ wem@@: "+str1;
    return FeelIt();

    /**************** Applaudieren ***************/
    case "applaudier":
    HELPCHECK("applaudier");
    GHOSTCHECK("Deine Haende fahren durcheinander durch - war wohl nix.\n",
      gname()+" will applaudieren, aber "+QPP(FEMALE,WER,PLURAL)
        +" Haende sausen\ndurcheinander durch.\n", 0);
    if (!str)  {
      out_sel="Du applaudierst von ganzem Herzen.";
      out_oth="@@name@@ gibt eine Runde Applaus.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
        out_sel="Du applaudierst @@alle@@@@adverb@@.";
        out_vic="@@name@@ applaudiert @@alle@@@@adverb@@.";
        return MixedOut(WEM);
      }
      if (!who && !adverb)
        Return("Applaudiere wem oder wie oder so aehnlich.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Nein, das gehoert sich nicht.",
        "Sachen wird hier nicht applaudiert, OK?"))
          return 1;
      out_sel="Du applaudierst@@ wem@@@@adverb@@.";
      if (who) out_vic="@@name@@ applaudiert Dir@@adverb@@.";
      out_oth="@@name@@ applaudiert@@ wem@@@@adverb@@.";
    }
    return FeelIt();

    /**************** Argln ***************/
    case "argl":
    HELPCHECK("argl");
    ParseRest(str);
    if (str && !adverb)
      Return("Argle wie?\n");
    out_sel="Du arglst"+(adverb ? "@@adverb@@." : " ein wenig vor Dich hin.");
    out_oth="@@gname@@ arglt"
      +(adverb ? "@@adverb@@." : " ein wenig vor sich hin.");
    return FeelIt();

    /**************** Aufatmen ***************/
    case "atm":
    HELPCHECK("atm");
    if (!str || sscanf(str,"%sauf",str1)!=1)
      Return("Atme wie auf?\n");
    ParseRest(str1);
    out_sel="Du atmest"+(adverb ? "@@adverb@@" : " erleichtert")+" auf.";
    out_oth="@@gname@@ atmet"+(adverb ? "@@adverb@@" : " erleichtert")+" auf.";
    return FeelIt();

    /**************** Begruessen ***************/
    case "hallo":
    case "hi":
    case "begruess":
    HELPCHECK("begruess");
    ParseRemote(str);
    if (!who)
      Return("Wen willst Du begruessen?\n");
    if (present(who, environment()))  {
      out_sel="Du heisst @@wen@@@@adverb@@ willkommen.";
      out_vic="@@gname@@ heisst Dich@@adverb@@ willkommen.";
      out_oth="@@gname@@ heisst @@wen@@@@adverb@@ willkommen.";
    }
    else  {
      out_sel="Du heisst @@wen@@@@adverb@@ aus der Ferne willkommen.";
      out_vic="@@gname@@ heisst Dich@@adverb@@ aus der Ferne willkommen.";
    }
    return FeelIt();

    /**************** Betasten ***************/
    case "betast":
    HELPCHECK("betast");
    ParseRest(str);
    if (!who)
      Return("Begrabsche wen?\n");
    out_sel="Du grabbelst@@adverb@@ an "+who->name(WEM)+" herum.";
    out_vic="@@gname@@ grabbelt@@adverb@@ an Dir herum.";
    out_oth="@@gname@@ grabbelt@@adverb@@ an "+who->name(WEM)+" herum.";
    return FeelIt();

    /**************** Bewundern ***************/
    case "bewunder":
    HELPCHECK("bewunder");
    ParseRest(str);
    if (!who)
      Return("Bewundere wen?\n");
    out_sel="Du bewunderst @@wen@@@@adverb@@.";
    out_vic="@@gname@@ bewundert Dich@@adverb@@.";
    out_oth="@@gname@@ bewundert @@wen@@@@adverb@@.";
    return FeelIt();

    /**************** Bibbern ***************/
    case "bibber":
    HELPCHECK("bibber");
    if (ghost())
      Return("Als Geist fuehlst Du keine Kaelte.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Bibbere wie?\n");
    out_sel="Du bibberst@@adverb@@ vor Kaelte.";
    out_oth="@@name@@ bibbert@@adverb@@ vor Kaelte.";
    return FeelIt();

    /**************** Bohre Nase ***************/
    case "bohr":
    HELPCHECK("bohr");
    ParseRest(str, ({"nase","in der nase","in nase"}));
    if (str && str!="nase" && str!="in nase" && str!="in der nase" && !adverb)
      Return("Bohre wie Nase?\n");
    out_sel="Du bohrst@@adverb@@ in Deiner Nase.";
    out_oth="@@gname@@ bohrt@@adverb@@ in der Nase.     Igitt! :)";
    return FeelIt();

    /**************** Brummeln ***************/
    case "brummel":
    HELPCHECK("brummel");
    ParseRest(str);
    out_sel="Du brummelst"
      +(adverb ? "@@adverb@@." : (str ? " kaum verstaendlich: "+str+"." : "."));
    out_oth="@@gname@@ brummelt"
      +(adverb ? "@@adverb@@." : (str ? " kaum verstaendlich: "+str+"." : "."));
    return FeelIt();

    /**************** cls ***************/
    case "cls":
    HELPCHECK("cls");
    write("[2J[H");
    return 1;

    /**************** Daeumchendrehen ***************/
    case "dreh":
    HELPCHECK("dreh");
    if (!str)
      Return("Drehe was?\n");
    if(strstr(str,"daeumchen")<0 && strstr(str,"daumen")<0)
      Return("Drehe was?\n");
    ParseRest(str,({"daeumchen","daumen"}));
    out_sel="Du drehst@@adverb@@ Daeumchen.";
    out_oth="@@gname@@ dreht@@adverb@@ Daeumchen.";
    return FeelIt();

    /**************** Danken ***************/
    case "dank":
    HELPCHECK("dank");
    ParseRest(str);
    if (!who)
      Return("Bei wem willst Du Dich bedanken?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
      "Leidest Du jetzt schon an Persoenlickeitsspaltung? Ne, ne...",
      "Keine Reaktion. Ist wohl befriedigender, sich bei Lebewesen zu "
        +"bedanken."))
        return 1;
    out_sel="Du bedankst Dich@@adverb@@ bei@@ wem@@.";
    out_vic="@@gname@@ bedankt sich@@adverb@@ bei Dir.";
    out_oth="@@gname@@ bedankt sich@@adverb@@ bei@@ wem@@.";
    return FeelIt();

    /**************** Denken ***************/
    case "denk":
    HELPCHECK("denk");
    if (ghost())
      Return("Womit willst Du denn denken? Du hast keine grauen Zellen...\n");
//    ParseRest(str);
    str2=old_explode(str||""," ")[0];
    if (str
    && (!adverb||((QueryStdAdverbs()[str2]||plr_adverbs[str2]))!=adverb))  {
      out_sel="Du denkst   . o O ("+str+")";
      out_oth="@@name@@ denkt   . o O ("+str+")";
      out_vic="@@name@@ denkt   . o O ("+str+")";
    }
    else  {
      out_sel="Du faengst@@adverb@@ an zu denken.\nKleine "
        +"Rauchwoelkchen steigen auf...";
      out_oth="@@name@@ faengt@@adverb@@ an zu denken.\nKleine "
        +"Rauchwoelkchen steigen auf...";
    }
    return FeelIt();

    /**************** Deuten ***************/
    case "deut":
    HELPCHECK("deut");
    ParseRest(str,"auf");
    if (for_all)  {
      out_sel="Du deutest@@adverb@@ auf @@alle@@.";
      out_vic="@@gname@@ deutet@@adverb@@ auf @@alle@@.";
      return MixedOut(WEN);
    }
    if (!who)
      Return("Auf wen oder was willst Du deuten?\n");
    out_sel="Du deutest@@adverb@@ auf"
      +(who==this_object()?" Dich." : "@@ wen@@.");
    if (who != this_object()) out_vic="@@gname@@ deutet@@adverb@@ auf Dich.";
    out_oth="@@gname@@ deutet@@adverb@@ auf"
      +(who==this_object() ? " sich selber.": "@@ wen@@.");
    return FeelIt();

    /**************** Druecken ***************/
    case "drueck":
    HELPCHECK("drueck");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du drueckst @@alle@@"+(adverb ? "@@adverb@@" : " zaertlich")
        +" an Dich.";
      out_vic="@@gname@@ drueckt @@alle@@"+
        (adverb ? "@@adverb@@" : " zaertlich")+" an sich.";
      return MixedOut(WEN);
    }
    if (!who)
      Return("Wen willst Du denn druecken?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Das macht doch keinen Spass.",
      "Ich druecke nur jemanden, nicht etwas."))
        return 1;
    GHOSTCHECK("Du willst "+who->name(WEN)+" an Dich druecken - nur hast Du "
        +"schon\nwieder nicht daran gedacht, dass so was als Geist nicht "
        +"geht.\n",
      gname()+" will "+who->name(WEN)+" an sich druecken - hat aber\n"
        +"mal wieder nicht an die Nachteile des Geisterdaseins gedacht.\n",
      gname()+" will Dich an sich druecken - hat aber mal wieder\n"
        +"nicht an die Nachteile des Geisterdaseins gedacht.\n");
    out_sel="Du drueckst @@wen@@"+(adverb ? "@@adverb@@" : " zaertlich")
      +" an Dich.";
    out_vic="@@name@@ drueckt Dich"+(adverb ? "@@adverb@@" : " zaertlich")
      +" an sich.";
    out_oth="@@name@@ drueckt @@wen@@"+(adverb ? "@@adverb@@" : " zaertlich")
      +" an sich.";
    return FeelIt();

    /**************** Entschuldige ***************/
    case "entschuldig":
    HELPCHECK("entschuldig");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Entschuldige Dich wie oder bei wem?\n");
    out_sel="Du bittest"+(who ? " @@wen@@" : "")
      +"@@adverb@@ um Entschuldigung.";
    if (who) out_vic="@@gname@@ bittet Dich@@adverb@@ um Entschuldigung.";
    out_oth="@@gname@@ bittet"+(who ? " @@wen@@" : "")
      +"@@adverb@@ um Entschuldigung.";
    return FeelIt();

    /**************** Erbleichen ***************/
    case "erbleich":
    HELPCHECK("erbleich");
    GHOSTCHECK("Ich weiss zwar nicht, wie Du das schaffst, aber Du wirst "
        +"noch bleicher.\n",
      break_string("Wie unwahrscheinlich das auch ist, aber "+gname()
        +" schafft es tatsaechlich, noch bleicher zu werden.",78), 0 );
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du erbleichen?\n");
    out_sel="Du erbleichst@@adverb@@.";
    out_oth="@@name@@ erbleicht@@adverb@@.";
    return FeelIt();

    /**************** Erroeten ***************/
    case "erroet":
    HELPCHECK("erroet");
    GHOSTCHECK("Du schaffst es nur bis zu einem blassen Rosa, aber immerhin.\n",
      "Die Wangen des Geistes von "+capname()+" werden leicht rosa.\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Erroete wie?\n");
    out_sel="Deine Wangen gluehen@@adverb@@.";
    out_oth="@@name@@ erroetet@@adverb@@.";
    return FeelIt();

    /**************** Erschrecken ***************/
    case "erschreck":
    case "erschrick":
    if (!ghost())
      Return("Du bist zu harmlos, Geist muesste man sein...\n");
    HELPCHECK("erschreck");
    ParseRest(str);
    if (!who)
      Return("Wen willst Du denn erschrecken?\n");
    out_sel="Mit einem lauten BUH! erschreckst Du @@wen@@"
      +(adverb ? "@@adverb@@." : " fuerchterlich.");
    out_vic="BUH! Du zuckst vor Schreck zusammen. @@gname@@ hat Dich"+
      (adverb ?  "@@adverb@@" : " fuerchterlich")+" erschreckt und lacht "
      "jetzt hohl ueber Dich.";
    out_oth="BUH! @@gname@@ erschreckt @@wen@@"
      +(adverb ? "@@adverb@@." : " fuerchterlich.");
    return FeelIt();

    /**************** Flippen ***************/
    case "flipp":
    HELPCHECK("flipp");
    ParseRest(str);
    if (str && !adverb)
      Return("Ausflippen wollen wir also, so so. Und wie, wenn ich "
        +"fragen darf?\n");
    out_sel="Du flippst"+(adverb ? "@@adverb@@ aus." : " total aus.");
    out_oth="@@gname@@ flippt"+(adverb ? "@@adverb@@ aus." : " total aus.");
    return FeelIt();

    /**************** Fluchen ***************/
    case "fluch":
    HELPCHECK("fluch");
    GHOSTCHECK("Du faengst mangels Resonanzkoerper leise an zu fluchen.\n",
      gname()+" faengt leise an zu fluchen. Laut kann er nicht,\n"
        +"mangels Luft und Resonanzkoerper.\n", 0);
    if (!str)  {
      out_sel="Du fluchst lautstark.";
      out_oth="@@name@@ faengt an, fuerchterlich zu fluchen.";
    }
    else  {
      ParseRest(str);
      if (!adverb)
        Return("Wie willst Du fluchen?\n");
      out_sel="Du fluchst@@adverb@@.";
      out_oth="@@name@@ flucht auf einmal@@adverb@@.";
    }
    return FeelIt();

    /**************** Fragen ***************/
    case "frag":
    HELPCHECK("frag");
    if (!str)
      Return("Frage wen was?\n");
    ParseRest(str);
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du faengst zu gruebeln an...",
      "Frage jemand, der lebt."))
        return 1;
    if (who)
      sscanf(str,"%s %s", str1,str1);
    else
      str1=str;
    if (!str1)
      Return("Frage "+who->name(WEN)+" WAS?\n");
    str1=capitalize(str1);
    if (str1[<1] != '?')
      str1 += "?";
    out_sel="Du fragst@@ wen@@: "+str1;
    /* Sprachfluch beruecksichtigen -Wargon, 8. 9. 95 */
    if (objectp(QueryProp(P_PERM_STRING)))
      str1 = call_other(QueryProp(P_PERM_STRING), "permutate_string", str1)||"";
    if (who) out_vic=(ghost() ? "Der Geist von " : /* IS_LEARNER(ME) ?
      QueryProp(P_PRESAY)||"" : */ "")+capname()+" fragt Dich: "+str1;
    out_oth=(ghost() ? "Der Geist von " : /* IS_LEARNER(ME) ?
      QueryProp(P_PRESAY)||"" : */ "")+capname()+" fragt@@ wen@@: "+str1;
    return FeelIt();

    /**************** Freuen ***************/
    case "freu":
    HELPCHECK("freu");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Freue Dich wie?\n");
    out_sel="Du "+(who ? "grinst @@wen@@ an und " : "")
      +"freust Dich@@adverb@@.";
    if (who) out_vic="@@gname@@ grinst Dich an und freut sich@@adverb@@.";
    out_oth="@@gname@@ "+(who ? "grinst @@wen@@ an und " : "")
      +"freut sich@@adverb@@.";
    return FeelIt();

    /**************** Furzen ***************/
    case "furz":
    HELPCHECK("furz");
    GHOSTCHECK("Du laesst einen fahren - aber er riecht nach gar nix.\n",
      gname()+" laesst einen fahren. Man riecht aber nix.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Furze wie?\n");
    out_sel="Du furzt"+(adverb ? "@@adverb@@." : " hemmungslos.");
    out_oth="@@name@@ laesst@@adverb@@ einen Stinkefurz fahren.";
    ofoo=clone_object("/items/furz");
    ofoo->set_furzer(this_player());
    ofoo->move(environment(this_player()));
    return FeelIt();
// DEBUG Furz testen!

    /**************** Gaehnen ***************/
    case "gaehn":
    HELPCHECK("gaehn");
    if (ghost())
      Return("Als Geist wirst Du nicht muede - also nicht gaehnen.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du gaehnen?\n");
    if (!adverb)
      out_sel="Kannst Du aber Dein(en) Mund/Maul/Schnabel weit aufreissen!";
    else
      out_sel="Du gaehnst@@adverb@@.";
    out_oth="@@gname@@ gaehnt@@adverb@@.";
    return FeelIt();

    /**************** Glucksen ***************/
    case "glucks":
    HELPCHECK("glucks");
    ParseRest(str);
    if (str && !adverb)
      Return("Gluckse wie?\n");
    out_sel="Du gluckst"+(adverb ? "@@adverb@@." : " wie ein Huhn.");
    out_oth="@@gname@@ gluckst"+(adverb ? "@@adverb@@." : " wie ein Huhn.");
    return FeelIt();

    /**************** Gratulieren ***************/
    case "gratulier":
    case "beglueckwuensch":
    HELPCHECK("gratulier");
    ParseRest(str);
    if (!who)
      Return("Wem willst Du gratulieren?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
      "Na, meinst Du nicht, dass Eigenlob stinkt?",
      "Soll ich dem Ding vielleicht zum Totsein gratulieren? Nee nee."))
        return 1;
    out_sel="Du gratulierst @@wem@@@@adverb@@.";
    out_vic="@@gname@@ gratuliert Dir@@adverb@@.";
    out_oth="@@gname@@ gratuliert @@wem@@@@adverb@@.";
    return FeelIt();

    /**************** Grinsen ***************/
    case "grins":
    HELPCHECK("grins");
    GHOSTCHECK("Als Du grinst, siehst Du regelrecht, wie die anderen eine "
        +"Gaensehaut bekommen.\n",
      "Du bekommst eine Gaensehaut, als der Geist von "+capname()
        +" zu grinsen anfaengt.\n", 0);
    ParseRest(str);
    if (for_all)  {
      out_sel="Du grinst @@alle@@@@adverb@@ an.";
      out_vic="@@name@@ grinst @@alle@@@@adverb@@ an.";
      return MixedOut(WEN);
    }
    if (!who && !adverb && str)
      Return("Grinsen - schoen und gut. Aber wen oder wie (an)grinsen?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du grinst Dir was, aber so, dass es kein anderer sieht.",
      "Nicht mal einen Spiegel darf man hier angrinsen, nur Lebewesen!"))
        return 1;
    out_sel="Du grinst@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
    if (who) out_vic="@@name@@ grinst Dich@@adverb@@ an.";
    out_oth="@@name@@ grinst@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
    return FeelIt();

    /**************** Gruebeln ***************/
    case "gruebel":
    case "gruebl":
    HELPCHECK("gruebel");
    ParseRest(str);
    if (str && !adverb)
      Return("Grueble wie?\n");
    out_sel="Du gruebelst@@adverb@@ eine Weile vor Dich hin.";
    out_oth="@@gname@@ gruebelt@@adverb@@ eine Weile vor sich hin.";
    return FeelIt();

    /**************** Grummeln ***************/
    case "grummel":
    case "grumml":
    HELPCHECK("grummel");
    ParseRest(str);
    if (str && !adverb)
      Return("Grummle wie?\n");
    out_sel="Du grummelst@@adverb@@.";
    out_oth="@@gname@@ grummelt@@adverb@@.";
    return FeelIt();

    /**************** Grunzen ***************/
    case "grunz":
    HELPCHECK("grunz");
    ParseRest(str);
    if (str && !adverb)
      Return("Grunze wie?\n");
    out_sel="Du grunzt@@adverb@@.";
    out_oth="@@gname@@ grunzt@@adverb@@.";
    return FeelIt();

    /**************** Gucken ***************/
    case "guck":
    HELPCHECK("guck");
    ParseRest(str);
    if (!adverb)
      Return("Gucke wie aus der Waesche?\n");
    out_sel="Du guckst@@adverb@@ aus der Waesche.";
    out_oth="@@gname@@ guckt@@adverb@@ aus der Waesche.";
    return FeelIt();

    /**************** Jammern ***************/
    case "jammer":
    HELPCHECK("jammer");
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du jammern?\n");
    out_sel="Du jammerst@@adverb@@.";
    out_oth="@@gname@@ jammert@@adverb@@.";
    return FeelIt();

    /**************** Haetscheln ***************/
    case "haetschel":
    case "haetschl":
    HELPCHECK("haetschel");
    GHOSTCHECK("Du ueberlegst es Dir anders - mit Deinen durchlaessigen "
        +"Haenden...",
      gname()+" will anscheinend jemand haetscheln, ueberlegt\n"
        +"es sich nach einem kurzen Blick auf seine Haende anders.\n", 0);
    ParseRest(str);
    if (for_all)  {
      out_sel="Du haetschelst @@alle@@@@adverb@@.";
      out_vic="@@name@@ haetschelt @@alle@@@@adverb@@.";
      return MixedOut(WEN);
    }
    if (!str || !who)
      Return("Wen willst Du haetscheln?\n");
    if (who && CheckLife(NOT_SELF, NOT_DEAD,
      "Das sieht viel zu albern aus - Du laesst es bleiben.",
      "Ist da ueberhaupt was zu haetscheln? Nein, da lebt doch nix."))
        return 1;
    out_sel="Du haetschelst@@ wen@@@@adverb@@.";
    out_vic="@@name@@ haetschelt Dich@@adverb@@.";
    out_oth="@@name@@ haetschelt@@ wen@@@@adverb@@.";
    return FeelIt();

    /**************** Hicksen ***************/
    case "hicks":
    HELPCHECK("hicks");
    GHOSTCHECK("Hoppla! Dieser Hickser zieht Dich ganz schoen zusammen!\n",
      gname()+" hat anscheinend Schluckauf.\n"
        +"Und was fuer einen! Fuer einen Moment zieht es "+QueryPronoun(WEN)
        +" ziemlich zusammen.\n", 0);
    if (!str)  {
      out_sel="Hicks!";
      out_oth="@@name@@ muss hicksen. Wahrscheinlich zu viel Alkohol...";
    }
    else  {
      ParseRest(str);
      if (!adverb)
        Return("Hickse wie?\n");
      out_sel="Du hickst@@adverb@@.";
      out_oth="@@name@@ hickst@@adverb@@.";
    }
    return FeelIt();

    /**************** Huepfen ***************/
    case "huepf":
    HELPCHECK("huepf");
    GHOSTCHECK("Du schwebst durch die Gegend.\n",
      gname()+" schwebt durch die Gegend.\n", 0);
    if (!str)  {
      out_sel="B O I N G !! Du huepfst in der Gegend herum.";
      out_oth="@@name@@ huepft in der Gegend herum.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
        Return("Huepfe wie oder um wen oder wie oder was oder haeh?\n");
      out_sel="Du huepfst@@adverb@@"+(who ? " um@@ wen@@" : "")+" herum.";
      if (who) out_vic="@@name@@ huepft@@adverb@@ um Dich herum.";
      out_oth="@@name@@ huepft@@adverb@@"+(who ? " um@@ wen@@" : "")+" herum.";
    }
    return FeelIt();

    /**************** Husten ***************/
    case "hust":
    HELPCHECK("hust");
    GHOSTCHECK("Du verstreust ein paar Geisterbazillen im Raum.\n",
      gname()+" macht ufff, ufff und verteilt ein paar Geister-\n"
        +"bazillen im Raum.\n", 0);
    if (!str)  {
      out_sel="Hust! Keuch! Halt dir doch wenigstens die Hand vor den Mund!";
      out_oth="@@name@@ hustet sich fast die Seele aus dem Leib.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
        Return("Wenn Du schon was hinter huste tippst, dann bitte was "
          +"vernuenftiges!\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Dir selber koennen nur andere was husten.",
        "Bitte huste nur Lebewesen was."))
          return 1;
      out_sel="Du hustest@@ wem@@@@adverb@@"+(who? " was" : "")+".";
      if (who) out_vic="@@name@@ hustet Dir@@adverb@@was.";
      out_oth="@@name@@ hustet@@ wem@@@@adverb@@"+(who? " was" : "")+".";
    }
    return FeelIt();

    /**************** Jubeln ***************/
    case "jubel":
    case "jubl":
    HELPCHECK("jubel");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Juble wie? Oder wem zu?\n");
    out_sel="Du jubelst@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    if (who) out_vic="@@gname@@ jubelt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ jubelt@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    return FeelIt();

    /**************** Keuchen ***************/
    case "keuch":
    HELPCHECK("keuch");
    if (ghost())
      Return("Als Geist strengt Dich nix an - also wird auch nicht "
        +"gekeucht.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Keuche wie?\n");
    out_sel="Du keuchst"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    out_oth="@@name@@ keucht"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    return FeelIt();

    /**************** Kichern ***************/
    case "kicher":
    HELPCHECK("kicher");
    if (!str)  {
      out_sel="Du kicherst. (Wie albern von Dir)";
      out_oth="@@gname@@ gibt ein albernes Kichern von sich.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
        Return("Das haut so nicht hin, gib vernuenftige Parameter.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "In diesem Fall nimm bitte nur kicher.",
        "Musst schon etwas Lebendes angeben, nichts Totes."))
          return 1;
      out_sel="Du kicherst@@adverb@@"+(who ? " hinter "+who->name(WESSEN)+
        " Ruecken." : ".");
      if (who)  out_vic="Jemand kichert@@adverb@@ hinter deinem Ruecken.";
      out_oth="@@gname@@ kichert@@adverb@@"+(who ? " hinter "+who->name(WESSEN)
        +" Ruecken." : ".");
    }
    return FeelIt();

    /**************** Kitzeln ***************/
    case "kitzel":
    case "kitzl":
    HELPCHECK("kitzel");
    GHOSTCHECK("Mit Deinen immateriellen Fingern schaffst Du das nicht.\n",
      gname()+" muss gerade feststellen, dass man mit\n"
        +"immateriellen Fingern nicht kitzeln kann.\n", 0);
    ParseRest(str);
    if (!who)
      Return("Wen willst Du kitzeln?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du bist doch kein Masochist! Du laesst es bleiben.",
      "Dinge sind so selten kitzlig. Lass es bleiben."))
        return 1;
    if (member(({"highlander","hobo"}), who->query_real_name())>-1)
      switch (who->query_real_name())  {
        case "highlander": str1="unter"; str2="Federn"; break;
        case "hobo"      : str1="an";    str2="Kinn"; break;
      }
    else if (who->QueryProp(P_RACE))
      switch (lower_case(who->QueryProp(P_RACE)))  {
        case "drache" : str1="unter";str2="Schuppen";
          t_g=FEMALE; t_n=PLURAL; break;
        case "greif"  : str1="unter";str2="Federn";
          t_g=FEMALE; t_n=PLURAL; break;
        default       : str1="an"; str2="Kinn"; t_g=NEUTER; t_n=SINGULAR;
      }
    else  {
      str1="an"; str2="Kinn"; t_g=NEUTER; t_n=SINGULAR;
    }
    if (getuid(who)=="trest" || getuid(who)=="woelkchen")  {
      str1="an"; str2="Fuessen"; t_g=MALE; t_n=PLURAL;
    }
    out_sel="Du kitzelst@@ wen@@@@adverb@@ "+str1+" "+who->QPP(t_g,WEM,t_n)
      +" "+str2+".\n@@wer@@ versucht, sich zu beherrschen, muss aber "
      +"doch lachen.";
    out_vic="@@name@@ kitzelt Dich@@adverb@@ "+str1+" Deine"
      +(t_n ? "n" : (t_g==FEMALE ? "r" : "m"))+" "+str2
      +".\nDu versuchst, Dich zu beherrschen, musst aber doch lachen.";
    out_oth="@@name@@ kitzelt@@ wen@@@@adverb@@ "+str1+" "
      +who->QPP(t_g,WEM,t_n)+" "+str2
      +".\n@@wer@@ versucht, sich zu beherrschen, muss aber doch lachen.";
    return FeelIt();

    /**************** Klatschen ***************/
    case "klatsch":
    HELPCHECK("klatsch");
    GHOSTCHECK("Deine Haende sausen durcheinander durch.\n",
      gname()+" will in die Haende klatschen - aber sie\n"
        +"sausen durcheinander durch.\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Klatsche wie?\n");
    out_sel="Du klatschst@@adverb@@ in die Haende.";
    out_oth="@@name@@ klatscht@@adverb@@ in die Haende.";
    return FeelIt();

    /**************** Klopfen ***************/
    case "klopf":
    HELPCHECK("klopf");
    if (!str||sscanf(str,"%s auf schulter",sfoo)!=1)
      if (!str||sscanf(str,"%s auf die schulter",sfoo)!=1)
        Return("Klopfe wie wem wieso was?\n");
    if (ghost())
      Return("Das geht leider nicht mit durchlaessigen Haenden.\n");
    str=sfoo;
    if (str=="") str=0;
    ParseRest(str);
    if (!who)
      Return("Klopfe wem auf die Schulter?\n");
    out_sel="Du klopfst @@wem@@@@adverb@@ auf die Schulter.";
    out_vic="@@name@@ klopft Dir@@adverb@@ auf die Schulter.";
    out_oth="@@name@@ klopft @@wem@@@@adverb@@ auf die Schulter.";
    return FeelIt();

    /**************** Knabbern ***************/
    case "knabber":
    HELPCHECK("knabber");
    if (ghost())
      Return("Sorry, aber dafuer fehlt Dir im Moment der noetige "
        +"\"Biss\"...\n");
    ParseRest(str);
    if (!who)
      Return("Knabbere wen an?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Du kommst nicht an dein eigenes Ohr ran...",
      "Noe, noe, das schmeckt bestimmt nicht gut."))
        return 1;
    out_sel="Du knabberst@@adverb@@ an "+who->name(WESSEN)+" Ohr.";
    out_vic="@@name@@ knabbert@@adverb@@ an Deinem Ohr.";
    out_oth="@@name@@ knabbert@@adverb@@ an "+who->name(WESSEN)+" Ohr.";
    return FeelIt();

    /**************** Knicksen ***************/
    case "knicks":
    HELPCHECK("knicks");
    GHOSTCHECK("Du knickst in der Mitte ab, kriegst Dich aber schnell wieder "
        +"zusammen.\n",
      gname()+" knick(s)t in der Mitte ab, kriegt sich aber\n"
        +"zum Glueck schnell wieder zusammen.\n", 0);
    if (!str) {
      out_sel="Du machst einen anmutigen Knicks.";
      out_oth="@@name@@ macht einen anmutigen Knicks.";
    }
    else  {
      ParseRest(str,"vor");
      if (for_all)  {
        out_sel="Du knickst@@adverb@@ vor @@alle@@.";
        out_vic="@@name@@ knickst@@adverb@@ vor @@alle@@.";
        return MixedOut(WEM);
      }
      if (!who && !adverb)
        Return("Knickse irgendwie oder vor jemandem.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Wie willst Du das denn schaffen?",
        "Vor Sachen wird hier nicht geknickst!"))
          return 1;
      out_sel="Du knickst@@adverb@@"+(who ? " vor" : "")+"@@ wem@@.";
      if (who ) out_vic="@@name@@ knickst@@adverb@@ vor Dir.";
      out_oth="@@name@@ knickst@@adverb@@"+(who ? " vor" : "")+"@@ wem@@.";
    }
    return FeelIt();

    /**************** Knirschen ***************/
    case "knirsch":
    HELPCHECK("knirsch");
    if (ghost())
      Return("Du kannst mit nichts knirschen, so als Geist. Versuche doch "
        +"dafuer\nmal, zu rasseln...\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Knirsche wie?\n");
    switch (QueryProp(P_RACE))  {
      case "greif"  : str1="dem Schnabel."; break;
      case "sandtiger" : str1="den Fangzaehnen."; break;
      case "drache" : str1="den Fangzaehnen."; break;
      default       : str1="den Zaehnen.";
    }
    out_sel="Du knirschst@@adverb@@ mit "+str1;
    out_oth="@@name@@ knirscht@@adverb@@ mit "+str1;
    return FeelIt();

    /**************** Knuddeln ***************/
    case "knuddel":
    case "knuddl":
    HELPCHECK("knuddel");
    if (ghost())
      Return("Sorry, nicht als Geist.\n");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du knuddelst @@alle@@@@adverb@@.";
      out_vic="@@name@@ knuddelt @@alle@@@@adverb@@.";
      return MixedOut(WEN);
    }
    if (!who)
      Return("Knuddle wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Das bringt doch nix, lass es halt.",
      "Du kannst soviel ich weiss ausser Lebewesen nur Teddys knuddeln."))
        return 1;
    out_sel="Du knuddelst@@ wen@@@@adverb@@.";
    out_vic="@@name@@ knuddelt Dich@@adverb@@.";
    out_oth="@@name@@ knuddelt@@ wen@@@@adverb@@.";
    return FeelIt();

    /**************** Knurren ***************/
    case "knurr":
    HELPCHECK("knurr");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du knurrst @@alle@@@@adverb@@ an.";
      out_vic="@@gname@@ knurrt @@alle@@@@adverb@@ an.";
      return MixedOut(WEN);
    }
    if (str && !who && !adverb)
      Return("Wen anknurren oder wie knurren?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du knurrst in Dich hinein.",
      "Reagiert nicht. Solltest wohl besser Lebwesen anknurren."))
        return 1;
    out_sel="Du knurrst@@ wen@@@@adverb@@"+(who ? " an." : ".");
    if (who) out_vic="@@gname@@ knurrt Dich@@adverb@@ an.";
    out_oth="@@gname@@ knurrt@@ wen@@@@adverb@@"+(who ? " an." : ".");
    return FeelIt();

    /****************  Knutschen ***************/
    case "knutsch":
    HELPCHECK("knutsch");
    if (ghost())
      Return("Das kannst Du als Geist leider nicht. Irgendwie fehlt Dir "
        +"dazu das Herz.\n");
    ParseRest(str);
    if (!who)
      Return("Knutsche wen ab?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Das geht nicht.",
      "Igitt! Lieber nicht!"))
        return 1;
    out_sel="Du gibst @@wem@@@@adverb@@ einen RICHTIGEN Kuss.";
    out_vic="@@name@@ gibt Dir@@adverb@@ einen tiefen und hingebungsvollen "
      +"Kuss.\nDu schwebst im 7. Himmel.";
    out_oth="@@name@@ gibt @@wem@@@@adverb@@ einen tiefen und "
      +"hingebungsvollen Kuss.";
    return FeelIt();

    /**************** Kotzen ***************/
    case "kotz":
    HELPCHECK("kotz");
    if (ghost())
      Return("Ne, das ist eins von den Sachen, die als Geist nicht gehen.\n");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Kotze wie? Kotze auf wen?\n");
    if (who && CheckLife(NOT_SELF,0,
      "Igitt, nein danke."))
        return 1;
    if(!str) {
      out_sel="Du kotzt ueber deine Schuhe.";
      out_oth="@@name@@ verdreht die Augen und kotzt.";
    }
    else  {
      out_sel="Du kotzt@@adverb@@"+(who ? " auf @@wen@@." : ".");
      if (who) out_vic="@@name@@ kotzt@@adverb@@ auf Dich.";
      out_oth="@@name@@ kotzt@@adverb@@"+(who ? " auf @@wen@@." : ".");
    }
    return FeelIt();

    /**************** Kratzen ***************/
    case "kratz":
    HELPCHECK("kratz");
    ParseRest(str);
    if (who && (who!=this_player()))
      Return("Das mach mal schoen nur mit Dir selber.\n");
    if (str && !adverb)
      Return("Wie willst Du Dich kratzen?\n");
    out_sel="Du kratzt dich@@adverb@@ am Kopp.";
    out_oth="@@gname@@ kratzt sich@@adverb@@ am Kopp.";
    return FeelIt();

    /**************** Krieche ***************/
    case "kriech":
    HELPCHECK("kriech");
    ParseRest(str);
    out_sel="Du kriechst"+(who ? " vor @@wem@@" : "")+"@@adverb@@ im Staub.";
    if (who) out_vic="@@gname@@ kriecht@@adverb@@ vor Dir im Staub.";
    out_oth="@@gname@@ kriecht"+(who ? " vor @@wem@@" : "")
      +"@@adverb@@ im Staub.";
    return FeelIt();

    /**************** Kuessen ***************/
    case "kuess":
    HELPCHECK("kuess");
    if (ghost())
      Return("Als Geist kannst Du leider niemanden kuessen.\n");
    ParseRest(str);
    if (!who)
      Return("Wen willst Du kuessen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Da hast Du aber Schwierigkeiten... Du gibst es schliesslich auf.",
      "Nix. Absolut nix. Kuesse lieber Lebewesen - die reagieren\n"
        +"wenigstens (und sei es, dass sie Dich fressen...)."))
        return 1;
    out_sel="Du kuesst@@ wen@@@@adverb@@.";
    out_vic="@@name@@ kuesst Dich@@adverb@@.";
    out_oth="@@name@@ kuesst@@ wen@@@@adverb@@.";
    FeelIt();
    if (who->QueryProp(P_FROG)&&QueryProp(P_LEVEL)>who->QueryProp(P_LEVEL)) {
      tell_room(environment(this_player()),"PLOPP!\n");
      write("Huch! Du wirst auf einmal so gruen und klein und kriegst auf\n"
        +"einmal furchtbar Hunger auf Fliegen und so...\n");
      who->Message("Auf einmal wird die Welt um Dich wieder so klein, wie sie\n"
        +" frueher mal war - und vor Dir sitzt ein kleiner gruener Frosch.\n");
      say(who->name(WER)+" steht auf einmal da und schaut dumm aus der "
        +"Waesche. Dafuer fehlt\njetzt seltsamerweise "+capname()
        +". Die Gesamtzahl an kleinen gruenen\nFroeschen im Raum hat sich "
        +"jedoch nicht geaendert...\n",({who,this_player()}));
      who->SetProp(P_FROG,0);
      SetProp(P_FROG,1);
    }
    return 1;

    /**************** Kuscheln ***************/
    case "kuschel":
    case "kuschl":
    HELPCHECK("kuschel");
    GHOSTCHECK("Dazu bist Du als Geist viel zu kalt und gar "
        +"schroecklich anzusehen.\n",
      gname()+" scheint Anwandlungen zu haben, sich an jemand "
        +"ankuscheln zu wollen.\nEntsetzt springen alle zurueck, weil "
        +"dazu ist er doch zu kalt und schroecklich\nanzusehen.\n", 0);
    ParseRest(str);
    if (!who)
      Return("An wen willst Du Dich ankuscheln?\n");
    out_sel="Du kuschelst Dich@@adverb@@ an @@wen@@ an.";
    out_vic="@@name@@ kuschelt sich@@adverb@@ an Dich an.";
    out_oth="@@name@@ kuschelt sich@@adverb@@ an @@wen@@ an.";
    return FeelIt();

    /**************** Lachen ***************/
    case "lach":
    HELPCHECK("lach");
    GHOSTCHECK("Du lachst mit hohler Stimme. Den Umstehenden (sind doch welche"
        +" da, oder?)\nlaeuft es kalt den Ruecken runter.\n",
      gname()+" lacht mit hohler Stimme.\nDir laeuft es eiskalt den Ruecken"
        +" runter.\n", 0);
    if (!str)  {
      out_sel="Du brichst vor Lachen zusammen.";
      out_oth="@@name@@ bricht vor Lachen zusammen.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
        out_sel="Du lachst@@adverb@@ ueber @@alle@@.";
        out_vic="@@name@@ lacht@@adverb@@ ueber @@alle@@.";
        return MixedOut(WEN);
      }
      if (!who && !adverb)
        Return("Lache wie, lache ueber wen?\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Lach Dich doch nicht selber aus - das machen schon andere...",
        "Gelacht wird nur ueber Lebewesen (die koennen sich drueber aergern)."))
          return 1;
      out_sel="Du lachst@@adverb@@"+(who?" ueber@@ wen@@":"")+".";
      if (who) out_vic="@@name@@ lacht@@adverb@@ ueber Dich.";
      out_oth="@@name@@ lacht@@adverb@@"+(who?" ueber@@ wen@@":"")+".";
    }
    return FeelIt();

    /**************** Laecheln ***************/
    case "laechel":
    case "laechl":
    HELPCHECK("laechel");
    if (ghost()) {
      write("Du laechelst innerlich.\n");
      return 1;
    }
    if(!str) {
      out_sel="Du laechelst.";
      out_oth="@@name@@ laechelt.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
        out_sel="Du laechelst @@alle@@@@adverb@@ an.";
        out_vic="@@name@@ laechelt @@alle@@@@adverb@@ an.";
        return MixedOut(WEN);
      }
      if (!who && !adverb && str)
        Return("Wie oder wen?\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Musst schon jemand anders anlaecheln.",
        "Bitte ein Lebewesen anlaecheln."))
          return 1;
      out_sel="Du laechelst@@ wen@@@@adverb@@"+(who ? " an." : ".");
      if (who) out_vic="@@name@@ laechelt Dich@@adverb@@ an.";
      out_oth="@@name@@ laechelt@@ wen@@@@adverb@@"+(who ? " an." : ".");
    }
    return FeelIt();

    /**************** Liebe ***************/
    /* These lines appear Courtesy of Angus@MorgenGrauen. So long, and thanks */
    /* for all the fish, errr, text, Angus :) */
    case "lieb":
    HELPCHECK("lieb");
    if (ghost())
      Return("Auf diese Freuden musst Du als Geist leider verzichten.\n");
    ParseRest(str);
    if (!who)
      Return("Wen hast Du lieb?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Ja, ich weiss, Du magst Dich, aber das musst Du nicht zur Schau"
        +"stellen.",
      "Du entwickelst seltsame Neigungen, finde ich."))
        return 1;
    str1=(who->QueryProp(P_GENDER)==FEMALE ? "ihr" : "ihm");
    /* old version:
      out_sel="Du fluesterst @@wem@@@@adverb@@ liebevolle Worte ins Ohr.";
      out_vic=gname()+" fluestert Dir@@adverb@@ liebevolle Worte ins Ohr.";
      out_oth=gname()+" fluestert@@adverb@@ sanfte Worte zu @@wem@@.";
    */
    out_sel="Du schliesst die Augen, schmiegst Dich eng an @@wen@@ und gibst"
      +LF+str1+" einen zaertlichen und leidenschaftlichen Kuss.\n"
      +"Um Dich herum versinkt die Welt und Du glaubst, auf Wolken zu "
      +"schweben.";
    out_vic="@@name@@ drueckt Dich zaertlich an sich und gibt Dir\n"
      +"einen zaertlichen und leidenschaftlichen Kuss. Du schliesst die\n"
      +"Augen und traeumst ein wenig......Du schwebst auf Wolken direkt\n"
      +"in den siebten Himmel.";
    out_oth="Du schaust dezent weg, als sich @@name@@ und "+who->name()
      +" verliebt in die Arme\nsinken.";
    return FeelIt();

    /**************** Loben ***************/
    case "lob":
    HELPCHECK("lob");
    if (!str)
      Return("Wen oder was willst Du loben?\n");
    ParseRest(str);
    if (who==ME)  {
      ofoo=clone_object("/items/furz");
      ofoo->set_furzer(this_player());
      ofoo->set_eigenlob();
      ofoo->move(environment(this_player()));
      //DEBUG Furz zum Eigenlob patchen :>
      out_sel="Du lobst Dich selber@@adverb@@. Die Folgen kennst Du ja...";
      out_oth="@@gname@@ lobt sich selber@@adverb@@, mit den bekannten Folgen.";
    } else if (who) {
      out_sel="Du lobst @@wen@@@@adverb@@.";
      out_vic="@@gname@@ lobt Dich@@adverb@@.";
      out_oth="@@gname@@ lobt @@wen@@@@adverb@@.";
    } else  {
      out_sel="Du lobst "+str+".";
      out_oth="@@gname@@ lobt "+str+".";
    }
    return FeelIt();

    /**************** Moppern ***************/
    case "mopper":
    HELPCHECK("mopper");
    ParseRest(str);
    if (str && !adverb)
      Return("Moppere wie?\n");
    out_sel="Du mopperst@@adverb@@.";
    out_oth="@@gname@@ moppert@@adverb@@.";
    return FeelIt();

    /**************** Mustern ***************/
    case "muster":
    HELPCHECK("muster");
    ParseRest(str);
    if (!who)
      Return("Mustere wen?\n");
    out_sel="Du musterst @@wen@@@@adverb@@.";
    out_vic="@@gname@@ mustert Dich@@adverb@@.";
    out_oth="@@gname@@ mustert @@wen@@@@adverb@@.";
    return FeelIt();

    /**************** Nicken ***************/
    case "ja":
    case "nick":
    HELPCHECK("nick");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du nickst @@alle@@@@adverb@@ zu.";
      out_vic="@@gname@@ nickt @@alle@@@@adverb@@ zu.";
      return MixedOut(WEM);
    }
    if (str && !who && !adverb)
      Return("Nicke wie oder wem zu oder wem wie zu?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du willst Dir selber zunicken? Lieber nicht, das sieht so albern aus.",
      "Hm. Nix passiert. Von Lebewesen bekommt man meistens mehr Feedback."))
        return 1;
    out_sel="Du nickst@@ wem@@@@adverb@@"
      +(who ? " zu." : (adverb ? "." : " zustimmend."));
    if (who) out_vic="@@gname@@ nickt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ nickt@@ wem@@@@adverb@@"
      +(who ? " zu." : (adverb ? "." : " zustimmend."));
    return FeelIt();

    /**************** Niesen ***************/
    case "nies":
    HELPCHECK("nies");
    if (ghost())
      Return("Du hast keine Nase mehr, in der es Dich jucken koennte...\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Niese wie?\n");
    out_sel="Haaaaaa-tschi! Gesundheit!"+(adverb ? " Du niest@@adverb@@." : "");
    out_oth="Haaaaaa-tschi! @@name@@ niest@@adverb@@.";
    return FeelIt();

    /**************** Ohrfeigen ***************/
    case "gib":
    HELPCHECK("gib");
    if (!str)
      Return("Gib wem was?\n");
    if (sscanf( str,"%s ohrfeige",str1)==0)
      return 0;
    ParseRest(str, ({"ohrfeige", "eine ohrfeige"}));
    if (for_all)  {
      out_sel="Du verpasst @@alle@@@@adverb@@ eine Ohrfeige.";
      out_vic="@@name@@ verpasst @@alle@@@@adverb@@ eine Ohrfeige.";
      return MixedOut(WEM);
    }
    if (!who)
      Return("Gib wem eine Ohrfeige?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Dazu sind Dir Deine Backen doch zu schade.",
      "Du wirst doch nichts schlagen, was sich nicht wehren kann?"))
        return 1;
    GHOSTCHECK("Deine Hand saust mit voller Wucht durch dein Opfer durch!",
      gname()+" will "+who->name(WEM)+" eine Ohrfeige geben - aber die Hand\n"
        +"saust mit voller Wucht durch das Opfer durch!", 0);
    out_sel="Du verpasst @@wem@@@@adverb@@ eine schallende Ohrfeige.";
    out_vic="@@name@@ verpasst Dir@@adverb@@ eine Watsche, dass Dir Hoeren "
      +"und Sehen vergeht.";
    out_oth="@@name@@ verpasst @@wem@@@@adverb@@ eine schallende Ohrfeige.";
    return FeelIt();

    /**************** Pfeifen ***************/
    case "pfeif":
    HELPCHECK("pfeif");
    GHOSTCHECK("Es kommt leider nur (nicht mal heisse) Luft, aber kein "
        +"Pfiff.\n",
      gname()+" spitzt den Mund und pustet angestrengt. Nichts passiert.\n", 0);
    ParseRest(str, "nach");
    if (str && !who && !adverb)
      Return("Pfeife wie? Pfeife wem nach? Haeh?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Was willst Du denn damit ausdruecken? Das gibt fuer mich keinen Sinn.",
      "Ich habe keine Lust dazu."))
        return 1;
    out_sel="Du pfeifst@@ wen@@@@adverb@@"
      +(who ? " nach." : (adverb ? "." : " anerkennend."));
    if (who) out_vic="@@name@@ pfeift Dir@@adverb@@ nach.";
    out_oth="@@name@@ pfeift@@ wen@@@@adverb@@"
      +(who ? " nach." : (adverb ? "." :" anerkennend."));
    return FeelIt();

    /**************** Philosophieren ***************/
    case "philosophier":
    HELPCHECK("philosophier");
    ParseRest(str);
    out_sel="Du philosophierst"+(adverb ? "@@adverb@@." :
      (str ? " ueber "+str+"." : "."));
    out_oth="@@gname@@ philosophiert"+(adverb ? "@@adverb@@." :
      (str ? " ueber "+str+"." : "."));
    return FeelIt();

    /**************** (Nase) Putzen ***************/
    case "putz":
    HELPCHECK("putz");
    if (ghost())
      Return("Nix da zum Putzen, so nebuloes, wie Du bist.\n");
    ParseRest(str, ({"nase", "die nase"}));
    if (str && str!="nase" && !adverb)
      Return("Putze Deine Nase wie?\n");
    out_sel="Du putzt Deine Nase@@adverb@@.";
    out_oth="@@name@@ putzt@@adverb@@ "+QPP(FEMALE,WEN)+" Nase.";
    return FeelIt();

    /**************** Quaken ***************/
    case "quak":
    HELPCHECK("quak");
    ParseRest(str);
    if (str && !adverb)
      Return("Quake wie?\n");
    sfoo="";
    flag=QueryProp(P_FROG)&&!ghost();
    for (t_g=0; t_g<=random(flag ? 4 : 2); t_g++)  {
      sfoo+=(flag ? " Qu" : " kw");
      for (t_n=0; t_n<=random(flag ? 10 : 5); t_n++)
        sfoo+="aA"[random(1)..random(1)];
      sfoo+="k";
    }
    if (!flag)
      sfoo=lower_case(sfoo);
    else
      sfoo+="!";
    out_sel="Du quakst"+(adverb ? "@@adverb@@" : (flag ? " aus voller Kehle"
      : " in etwa wie ein Frosch"))+":"+sfoo;
    out_oth="@@gname@@ quakt"+(adverb ? "@@adverb@@" : (flag ? " aus voller Kehle"
      : " in etwa wie ein Frosch"))+":"+sfoo;
    return FeelIt();

    /**************** Quietschen ***************/
    case "quietsch":
    case "quiek":
    HELPCHECK("quiek");
    ParseRest(str);
    if (str && !adverb)
      Return("Quietsche wie?\n");
    out_sel="Du quietschst"+(adverb ? "@@adverb@@." : " vergnuegt.");
    out_oth="@@gname@@ quietscht"+(adverb ? "@@adverb@@." : " vergnuegt.");
    return FeelIt();

    /**************** Raeuspern ***************/
    case "raeusper":
    HELPCHECK("raeusper");
    ParseRest(str);
    if (str && !adverb)
      Return("Hm? Wie meinen?\n");
    out_sel="Du raeusperst Dich@@adverb@@.";
    out_oth="@@gname@@ raeuspert sich@@adverb@@.";
    return FeelIt();

    /**************** Reiben ***************/
    case "reib":
    HELPCHECK("reib");
    if (ghost())
      Return("Du hast nichts zum Reiben, aber auch gar nichts.\n");
    if (str && (sscanf(str,"%s die Augen",sfoo)==1 || sscanf(str,"%s Augen",sfoo)==1))
      str=sfoo;
    else if (str && (sscanf(str,"%s die Haende",sfoo)==1
    ||sscanf(str,"%s Haende",sfoo)==1))  {
      flag=2;
      str=sfoo;
    }
    if (str=="") str=0;
    ParseRest(str);
    if (str && !adverb)
      Return("Reibe wie die "+(flag==2 ? "Haende" : "Augen")+"?\n");
    out_sel="Du reibst Dir"+(adverb ? "@@adverb@@"
      : (flag==2 ? " vergnuegt" : " muede"))+" die "
      +(flag==2 ? "Haende." : "Augen.");
    out_oth="@@name@@ reibt sich"+(adverb ? "@@adverb@@"
      : (flag==2 ? " vergnuegt" : " muede"))+" die "
      +(flag==2 ? "Haende." : "Augen.");
    return FeelIt();

    /**************** Rotfln ***************/
    case "rotfl":
    HELPCHECK("rotfl");
    ParseRest(str);
    if (str && !adverb)
      Return("Rotfl wie?\n");
    out_sel="Du rotflst@@adverb@@.";
    out_oth="@@gname@@ rotflt@@adverb@@.";
    return FeelIt();

    /**************** Ruelpsen ***************/
    case "ruelps":
    HELPCHECK("ruelps");
    GHOSTCHECK("Ein leichter Windhauch entfaehrt Deinem Mund, mehr nicht.\n",
      "Dem Mund des Geistes von "+capname()
        +" entfaehrt ein leichtes Lueftchen.\n", 0);
    if (!str)  {
      out_sel="BOOOOEEERRRRPP!  Entschuldige dich gefaelligst!";
      out_oth="@@name@@ ruelpst unanstaendig.";
    }
    else  {
      ParseRest(str);
      if (!adverb)  {
        write("Ruelpse wie (schlimm genug, dass Du Dich nicht beherrschen "
          +"kannst!)?\n");
        return 1;
      }
      out_sel="Du ruelpst@@adverb@@. Schaem Dich!";
      out_oth="@@name@@ ruelpst@@adverb@@ und wird nicht mal rot dabei.";
    }
    return FeelIt();
  }

  switch (vb)  {
    /**************** Runzeln ***************/
    case "runzel":
    case "runzl":
    HELPCHECK("runzel");
    if (ghost())
      Return("DAS geht als Geist nun wirklich nicht.\n");
    ParseRest(str,"stirn");
    if (str && !adverb)
      Return("Runzle die Stirn wie?\n");
    out_sel="Du runzelst@@adverb@@ die Stirn.";
    out_oth="@@name@@ runzelt@@adverb@@ die Stirn.";
    return FeelIt();

    /**************** Sabbere ***************/
    case "sabber":
    HELPCHECK("sabber");
    sfoo=ghost() ? "schleim" : "sabber";
    ParseRest(str);
    if (str && !adverb && !who)
      Return("Sabber wie oder wen an?\n");
    out_sel="Du "+sfoo+"st@@ wen@@@@adverb@@ "
      +(who ? "an." : "auf den Boden.");
    if (who) out_vic="@@gname@@ "+sfoo+"t Dich@@adverb@@ an.";
    out_oth="@@gname@@ "+sfoo+"t@@ wen@@@@adverb@@ "
      +(who ? "an." : "auf den Boden.");
    return FeelIt();

    /**************** Schaemen ***************/
    case "schaem":
    HELPCHECK("schaem");
    ParseRest(str);
    if (str && !adverb && lower_case(str)!="dich")
      Return("Schaeme Dich wie?\n");
    out_sel="Du schaemst Dich@@adverb@@.";
    out_oth="@@gname@@ schaemt sich@@adverb@@.";
    return FeelIt();

#ifdef SCHAU_AN
    /**************** Schau an ***************/
    case "schau":
    HELPCHECK("schau");
    if (!str || old_explode(str, " ")[sizeof(old_explode(str, " "))]!="an")
      return 0;
    ParseRest(str, "an");
    if (!who)
      Return("Schau wen an?\n");
    out_sel="Du schaust @@wen@@@@adverb@@ an.";
    out_vic="@@gname@@ schaut Dich@@adverb@@ an.";
    out_oth="@@gname@@ schaut @@wen@@@@adverb@@ an.";
    return FeelIt();
#endif

    /**************** Schluchzen ***************/
    case "schluchz":
    HELPCHECK("schluchz");
    ParseRest(str);
    if (str && !adverb)
      Return("Schluchze wie?\n");
    out_sel="Du schluchzt"+(adverb ? "@@adverb@@." : " herzzerreissend.");
    out_oth="@@gname@@ schluchzt"
      +(adverb ? "@@adverb@@." : " herzzerreissend.");
    return FeelIt();

    /**************** Schmollen ***************/
    case "schmoll":
    HELPCHECK("schmoll");
    GHOSTCHECK("Du schwebst beleidigt in die Ecke.\n",
      gname()+" schwebt beleidigt in die Ecke und schmollt.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schmolle wie?\n");
    out_sel="Du schmollst@@adverb@@.";
    out_oth="@@name@@ geht in die Ecke und schmollt@@adverb@@.";
    return FeelIt();

    /**************** Schmunzeln ***************/
    case "schmunzel":
    case "schmunzl":
    HELPCHECK("schmunzel");
    ParseRest(str);
    if (str && !adverb)
      Return("Schmunzle wie?\n");
    out_sel="Du schmunzelst@@adverb@@.";
    out_oth="@@gname@@ schmunzelt@@adverb@@.";
    return FeelIt();

    /**************** Schnalzen ***************/
    case "schnalz":
    HELPCHECK("schnalz");
    ParseRest(str, ({"zunge","mit zunge", "mit der zunge"}));
    out_sel="Du schnalzt@@adverb@@ mit der Zunge.";
    out_oth="@@gname@@ schnalzt@@adverb@@ mit der Zunge.";
    return FeelIt();

    /**************** Schnauben ***************/
    case "schnaub":
    HELPCHECK("schnaub");
    ParseRest(str);
    if (str && !adverb)
      Return("Schnaube wie?\n");
    out_sel="Du schnaubst"+(adverb ? "@@adverb@@." : " entruestet.");
    out_oth="@@gname@@ schnaubt"+(adverb ? "@@adverb@@." : " entruestet.");
    return FeelIt();

    /**************** Schnaufen ***************/
    case "schnauf":
    HELPCHECK("schnauf");
    ParseRest(str);
    if (str && !adverb)
      Return("Schnaufe wie?\n");
    out_sel="Du schnaufst"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    out_oth="@@gname@@ schnauft"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    return FeelIt();

    /**************** Schnippen ***************/
    case "schnipp":
    case "schnipps":
    HELPCHECK("schnipp");
    GHOSTCHECK("Du schaffst es nicht, weil die Finger durcheinander durch "
        +"gehen.\n",
      gname()+" versagt beim Schnippen - die Finger\ngehen durcheinander "
        +"durch.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schnippe wie?\n");
    out_sel="Du schnippst@@adverb@@ mit deinen Fingern.";
    out_oth="@@name@@ schnippt@@adverb@@ mit den Fingern.";
    return FeelIt();

    /**************** Schnarchen ***************/
    case "schnarch":
    HELPCHECK("schnarch");
    if (ghost())
      Return("Ich glaube, da fehlen Dir irgendwie die physischen "
        +"Voraussetzungen dazu.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Schnarche wie?\n");
    out_sel=(str ? "Zzzzzzzzzzz..." : "Du schnarchst@@adverb@@.");
    out_oth="@@name@@ schnarcht "+(str ? "@@adverb@@." : "laut.");
    return FeelIt();

    /**************** Schniefen ***************/
    case "snief":
    case "schnief":
    HELPCHECK("schnief");
    GHOSTCHECK("Du schniefst ganz leise.\n",
      gname()+" schnieft ganz leise.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schniefe wie?\n");
    out_sel="Du schniefst@@adverb@@.";
    out_oth="@@name@@ schnieft@@adverb@@.";
    return FeelIt();

    /**************** Schnurren ***************/
    case "schnurr":
    HELPCHECK("schnurr");
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du schnurren?\n");
    out_sel="MMMMIIIIIAAAAAAUUUUUUUU! Du schnurrst"
      +(adverb ? "@@adverb@@." : " zufrieden.");
    out_oth="@@gname@@ schnurrt"+(adverb ? "@@adverb@@." : " zufrieden.");
    return FeelIt();

    /**************** Schreien ***************/
    case "schrei":
    HELPCHECK("schrei");
    GHOSTCHECK("AAAAIIIIIIIIIIIEEEEEEEEEEEEEEEEEEEEEEEEEE! Ja, nur Geister "
        +"koennen so schreien!\n",
      gname()+" schreit - das Blut gefriert fast in deinen Ader!\n", 0);
    if (!str)  {
      out_sel="AUUUAAAHHHHHH!!!!";
      out_oth="@@name@@ schreit laut!";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
        Return("Schreien - wie denn? Oder wen denn anschreien?\n");
      out_sel="Du schreist@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
      if (who) out_vic="@@name@@ schreit Dich@@adverb@@ an.";
      out_oth="@@name@@ schreit@@ wen@@@@adverb@@"+(who? " an" : "")+".";
    }
    return FeelIt();

    /**************** Schuetteln ***************/
    case "schuettel":
    case "schuettl":
    HELPCHECK("schuettel");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du schuettelst @@alle@@@@adverb@@ die Haende.";
      out_vic="@@gname@@ schuettelt @@alle@@@@adverb@@ die Haende.";
      return MixedOut(WEM);
    }
    if (str && !who && !adverb)
      Return("Schuettle wie? Schuettle wem die Hand?\n");
    if(!who) {
      out_sel="Du schuettelst Dich@@adverb@@.";
      out_oth="@@gname@@ schuettelt sich@@adverb@@.";
    }
    else  {
      if (CheckLife(0,NOT_DEAD,
        "", "Noe, das mach ich nur mit Lebewesen."))
          return 1;
      if (who == this_player())  {
        out_sel="Du hebst"+(adverb ? "@@adverb@@" : " triumphierend")
          +" Deine Haende ueber den Kopf und schuettelst sie.";
        out_oth="@@gname@@ hebt"+(adverb ? "@@adverb@@" : " triumphierend")
          +" die Haende ueber den Kopf\nund schuettelt sie.";
      }
      else  {
        out_sel="Du schuettelst@@ wem@@@@adverb@@ die Haende.";
        if (ghost()) out_sel+="\nNaja, Du versuchst es wenigstens - "
          +"immer diese durchlaessigen Haende...";
        out_vic="@@gname@@ schuettelt Dir@@adverb@@ die Haende.";
        if (ghost()) out_vic+="\nNaja, Du versuchst es wenigstens - "
          +"immer diese durchlaessigen Haende...";
        out_oth="@@gname@@ schuettelt@@ wem@@@@adverb@@ die Haende.";
        if (ghost()) out_oth+="\nNaja, Du versuchst es wenigstens - "
          +"immer diese durchlaessigen Haende...";
      }
    }
    return FeelIt();

    /**************** Schweigen ***************/
    case "schweig":
    HELPCHECK("schweig");
    ParseRest(str);
    if (str && !adverb)
      Return("Schweige wie?\n");
    out_sel="Du schweigst@@adverb@@.";
    out_oth="@@gname@@ schweigt@@adverb@@.";
    return FeelIt();

    /**************** Seufzen ***************/
    case "seufz":
    HELPCHECK("seufz");
    GHOSTCHECK("Du seufzt geisterhaft.\n",
      gname()+" seufzt geisterhaft. Naja, wie denn sonst?\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Seufze wie?\n");
    out_sel="Du seufzst@@adverb@@.";
    out_oth="@@name@@ seufzt@@adverb@@.";
    return FeelIt();

    /**************** Singen ***************/
    case "sing":
    HELPCHECK("sing");
    if (!str) {
      out_sel="Oh sole mio!";
      out_oth="@@gname@@ singt irgendwas italienisches.";
    }
    else  {
      ParseRest(str);
      out_sel="Du singst@@adverb@@"+(adverb ? "." : " '"+capitalize(str)+"'.");
      out_oth="@@gname@@ singt@@adverb@@"+(adverb ? "." : " '"
        +capitalize(str)+"'.");
    }
    return FeelIt();

    /**************** Sniffen ***************/
    case "sniff":
    HELPCHECK("sniff");
    ParseRest(str);
    if (str && !adverb && !who)
      Return("Sniffe wie?\n");
    out_sel="Du sniffst"+(who ? " @@wen@@" : "")
      +(adverb ? "@@adverb@@" : " traurig")+(who ? " an." : ".");
    if (who) out_vic="@@gname@@ snifft Dich"
      +(adverb ? "@@adverb@@" : " traurig")+" an.";
    out_oth="@@gname@@ snifft"+(who ? " @@wen@@" : "")
      +(adverb ? "@@adverb@@" : " traurig")+(who ? " an." : ".");
    return FeelIt();

    /**************** Spucken ***************/
    case "spuck":
    HELPCHECK("spuck");
    GHOSTCHECK("Du bringst nicht genug Spucke zusammen.\n",
      gname()+" stellt gerade fest, dass man ohne Spucke nicht\n"
        +"spucken kann.\n", 0);
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Spucke wen wie an?\n");
    if (who && CheckLife(NOT_SELF,0,
      "Hast Du Dich so schlecht benommen? Lass es lieber bleiben."))
        return 1;
    out_sel="Du spuckst@@ wen@@@@adverb@@ "+(who ? "an." : "auf den Boden.");
    if (who) out_vic="@@name@@ spuckt Dich@@adverb@@ an.";
    out_oth="@@name@@ spuckt@@ wen@@@@adverb@@ "
      +(who ? "an." : "auf den Boden.");
    return FeelIt();

    /**************** Stampfen ***************/
    case "stampf":
    HELPCHECK("stampf");
    ParseRest(str, "auf");
    out_sel="Du stampfst@@adverb@@ mit dem Fuss auf.";
    out_oth="@@gname@@ stampft@@adverb@@ mit dem Fuss auf.";
    return FeelIt();

    /**************** Starren ***************/
    case "starr":
    HELPCHECK("starr");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Starre wie bzw. wen an?\n");
    if (who && CheckLife(NOT_SELF,0,
      "Wie willst Du in Deine eigenen Augen starren? "
        +"(Spiegel gelten nicht...)"))
        return 1;
    out_sel="Du starrst"+(!str ? " ins Leere." : (who ? "@@ wen@@" : "")
      +(adverb ? "@@adverb@@" : " vertraeumt")+(who ? " an." : "."));
    if (who) out_vic="@@gname@@ starrt"+(adverb ? "@@adverb@@" : " tief")
      +" in Deine Augen.";
    out_oth="@@gname@@ starrt"+(!str ? " ins Leere." : (who ? "@@ wen@@" : "")
      +(adverb ? "@@adverb@@" : " vertraeumt")+(who ? " an." : "."));
    return FeelIt();

    /**************** Staunen ***************/
    case "staun":
    HELPCHECK("staun");
    if (!str)  {
      out_sel="Du bist erstaunt.";
      out_oth="@@gname@@ ist erstaunt.";
    }
    else  {
      ParseRest(str, "ueber");
      if (!who && !adverb)
        Return("Bla bla. Wenn Du nach staune noch was tippst, sollte "
          +"das ein\nLebewesen sein.\n");
      if (who == this_player())  {
        out_sel="Du staunst@@adverb@@ ueber Dich selber.";
        out_oth="@@gname@@ staunt@@adverb@@ ueber sich selber.";
      }
      else  {
        out_sel="Du staunst@@adverb@@"+(who ? " ueber @@wen@@." : ".");
        if (who) out_vic="@@gname@@ staunt@@adverb@@ ueber Dich.";
        out_oth="@@gname@@ staunt@@adverb@@"+(who ? " ueber @@wen@@." : ".");
      }
    }
    return FeelIt();

    /**************** Stieren ***************/
    case "stier":
    HELPCHECK("stier");
    GHOSTCHECK("Du stierst mit hohlem Blick in die Gegend.\n",
      gname()+" stiert mit hohlem Blick in die Gegend.\n", 0);
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Stiere wie oder wen an?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du kannst Dich nicht selber anstieren.",
      "Bitte nur Lebewesen anstieren."))
        return 1;
    out_sel="Du stierst@@ wen@@@@adverb@@"
      +(who ? " an." : " in der Gegend herum.");
    if (who) out_vic="@@gname@@ stiert Dich@@adverb@@ an.";
    out_oth="@@gname@@ stiert@@ wen@@@@adverb@@"
      +(who ? " an." : " in der Gegend herum.");
    return FeelIt();

    /**************** Stimme zu ***************/
    case "stimm":
    HELPCHECK("stimm");
    ParseRest(str, "zu");
    if (str && !who && !adverb)
      Return("Stimme wem zu?\n");
    out_sel="Du stimmst@@ wem@@@@adverb@@ zu.";
    if (who) out_vic="@@gname@@ stimmt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ stimmt@@ wem@@@@adverb@@ zu.";
    return FeelIt();

    /**************** Stoehnen ***************/
    case "stoehn":
    HELPCHECK("stoehn");
    GHOSTCHECK("Du stoehnst schauderlich.\n",
      gname()+" stoehnt schauderlich. Zum Glueck\nhast Du ziemlich "
        +"gute Nerven.\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Wie willst Du stoehnen?\n");
    out_sel="Du stoehnst@@adverb@@.";
    out_oth="@@name@@ stoehnt@@adverb@@.";
    return FeelIt();

    /**************** Stossen ***************/
    case "stoss":
    HELPCHECK("stoss");
    ParseRest(str);
    if (!who)
      Return("Stosse wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Was soll der Unsinn? Lass das!",
      "Das gibt nur bei Lebewesen Sinn."))
        return 1;
    GHOSTCHECK("Dein Ellenbogen versinkt in "+who->name(WEM)+".\n",
      gname()+" will "+who->name(WEM)+" in die Rippen stossen, aber "
        +QPP(MALE,WER,PLURAL)+"\nEllenbogen verteilen keinen Stoss, "
        +"sondern versinken.\n",
      gname()+" will Dich in die Rippen stossen, aber "+QPP(MALE,WER,PLURAL)
        +" Ellenbogen versinken.\n");
    out_sel="Du stoesst@@ wen@@@@adverb@@ in die Rippen.";
    out_vic="@@name@@ stoesst Dir@@adverb@@ in die Rippen.";
    out_oth="@@name@@ stoesst@@ wen@@@@adverb@@ in die Rippen.";
    return FeelIt();

    /**************** Streicheln ***************/
    case "streichel":
    case "streichl":
    HELPCHECK("streichel");
    ParseRest(str);
    if (!who)
      Return("Streichle wen?\n");
    if (for_all)  {
      out_sel="Du streichelst @@alle@@@@adverb@@.";
      out_vic="@@gname@@ streichelt @@alle@@@@adverb@@.";
      return MixedOut(WEN);
    }
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Lass Dich von anderen streicheln.",
      "Ich streichle nur Lebewesen."))
        return 1;
    GHOSTCHECK("Du willst "+who->name(WEN,2)+" streicheln, aber Deine "
      +"Haende koennen\nnichts beruehren.\n",
    gname()+" will "+who->name(WEN,2)+" streicheln, aber diese\n"
      +"Geisterhaende koennen halt nix beruehren...\n",
    gname()+" will Dich streicheln, scheitert aber wie so oft an\n"
      +"diesen dummen durchlaessigen Geisterhaenden.\n");
    out_sel="Du streichelst @@wen@@@@adverb@@.";
    out_vic="@@name@@ streichelt Dich@@adverb@@.";
    out_oth="@@name@@ streichelt @@wen@@@@adverb@@.";
    return FeelIt();

    /**************** Stupsen ***************/
    case "stups":
    HELPCHECK("stups");
    if (ghost())
      Return("Das geht nicht ohne Ellenbogen,..\n");
    ParseRest(str);
    if (!who)
      Return("Stupse wen an?\n");
    out_sel="Du stupst @@wen@@@@adverb@@ an.";
    out_vic="@@name@@ stupst Dich@@adverb@@ an.";
    out_oth="@@name@@ stupst @@wen@@@@adverb@@ an.";
    return FeelIt();

    /**************** Stutzen ***************/
    case "stutz":
    HELPCHECK("stutz");
    ParseRest(str);
    if (str && !adverb)
      Return("Stutze wie?\n");
    out_sel="Du stutzt@@adverb@@.";
    out_oth="@@gname@@ stutzt@@adverb@@.";
    return FeelIt();

    /**************** Taetscheln ***************/
    case "taetschel":
    case "taetschl":
    HELPCHECK("taetschel");
    ParseRest(str);
    if (!who)
      Return("Taetschle wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Das sieht zu doof aus, das mache ich nicht.",
      "Ich taetschle nur Lebewesen."))
        return 1;
     GHOSTCHECK("Du willst "+who->name(WEN)+" taetscheln - aber Deine "
        +"Haende gehen\nglatt durch den Kopf durch.\n",
      gname()+" will "+who->name(WEN)+" den Kopf taetscheln, aber "
        +"die Geister-\nhaende gehen glatt durch den Kopf durch.\n",
      gname()+" will Deinen Kopf taetscheln, aber diese Geisterhaende "
        +"gehen\nglatt durch Deinen Kopf durch - Du hast ein seltsames "
        +"Gefuehl dabei.\n");
    out_sel="Du taetschelst @@wem@@@@adverb@@ den Kopf.";
    out_vic="@@name@@ taetschelt Dir@@adverb@@ den Kopf.";
    out_oth="@@name@@ taetschelt @@wem@@@@adverb@@ den Kopf.";
    return FeelIt();

    /**************** Tanzen ***************/
    case "tanz":
    HELPCHECK("tanz");
    GHOSTCHECK("Du tanzt den klassischen GeisterTanz (tm).\n",
      gname()+" tanzt den klassischen GeisterTanz (tm).\n", 0);
    if (!str) {
      out_sel="Kommst Du Dir nicht irgendwie bloed vor? Du tanzt "
        +"den Ententanz.";
      out_oth="@@name@@ tanzt den Ententanz.";
    }
    else  {
      taenze=({"Walzer","Polka","Rumba","Tango","Cha cha cha","Foxtrott",
        "Mambo","Salsa","Slowfox","Breakdance","Pogo","Merengue",
        "Rock'n'Roll","Ballett","Regentanz","Hexentanz"});
      ParseRest(str,"mit");
      if (!who)
        Return("Mit wem willst Du tanzen?\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Mit Dir selber kannst Du nicht tanzen.",
        "Keine Reaktion - will wahrscheinlich nicht tanzen."))
          return 1;
      ifoo=random(sizeof(taenze));
      out_sel="Du tanzt@@adverb@@ mit @@wem@@ eine Runde "+taenze[ifoo]+".";
      out_vic="@@name@@ reisst Dich an sich und tanzt@@adverb@@ eine Runde "
        +taenze[ifoo]+" mit Dir.";
      out_oth="@@name@@ schnappt sich @@wen@@ und die beiden tanzen"
        +"@@adverb@@ eine Runde "+taenze[ifoo]+".";
    }
    return FeelIt();

    /**************** Traeumen ***************/
    case "traeum":
    HELPCHECK("traeum");
    if (!str)
      Return("Traeume wovon oder von wem?\n");
    ParseRest(str);
    out_sel="Du traeumst"+(who ? "@@adverb@@ von @@wem@@."
      : (adverb ? "@@adverb@@." : " "+str+"."));
    if (who) out_vic="@@gname@@ traeumt@@adverb@@ von Dir.";
    out_oth="@@gname@@ traeumt"+(who ? "@@adverb@@ von @@wem@@."
      : (adverb ? "@@adverb@@." : " "+str+"."));
    return FeelIt();

    /**************** Treten (tritt) ***************/
    case "tritt":
    case "tret":
    HELPCHECK("tritt");
    if (!str)  {
      GHOSTCHECK("Dein Fuss faehrt durch die beruehmte langvergessene "
          +"unsichtbare Schildkroete\nhindurch.\n",
        gname()+" will die beruehmte langvergessene unsichtbare\n"
          +"Schildkroete treten, aber "+QPP(MALE,WER)
          +" Fuss faehrt durch sie hindurch.\n", 0);
      out_sel="Du trittst die beruehmte langvergessene unsichtbare "
        +"Schildkroete.";
      out_oth="@@gname@@ tritt die beruehmte langvergessene unsichtbare "
        +"Schildkroete.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
        out_sel="Du trittst @@alle@@@@adverb@@. Solltest Du nicht langsam "
          +"an Flucht denken?";
        out_vic="@@name@@ tritt @@alle@@@@adverb@@. Traut sich ganz "
          +"schoen was!";
        return MixedOut(WEN);
      }
      if (!who && !adverb)
        Return("Wenn Du schon was nach tritt tippst, dann sag mir, wen "
          +"oder wie ich das soll.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du schaffst es nicht, Dir selber in den Hintern zu treten.",
        "Tote Sachen tritt man nicht auch noch!"))
          return 1;
      if (who)  {
        out_sel="Du trittst@@ wen@@@@adverb@@.";
        if (who) out_vic="@@gname@@ tritt Dich@@adverb@@.";
        out_oth="@@gname@@ tritt@@ wen@@@@adverb@@.";
      }
      else  {
        out_sel="Du trittst die beruehmte langvergessene unsichtbare "
          +"Schildkroete@@adverb@@.";
        out_oth="@@gname@@ tritt die beruehmte langvergessene unsichtbare "
          +"Schildkroete\n@@adverb@@.";
      }
    }
    return FeelIt();

    /**************** Troesten ***************/
    case "troest":
    HELPCHECK("troest");
    ParseRest(str);
    if (!who)
      Return("Wen willst Du troesten?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Trost musst Du bei jemand anders suchen.",
      "Das Teil musst Du nicht troesten, das fuehlt nix."))
        return 1;
    out_sel="Du troestest@@ wen@@@@adverb@@.";
    out_vic="@@gname@@ troestet Dich@@adverb@@.";
    out_oth="@@gname@@ troestet@@ wen@@@@adverb@@.";
    return FeelIt();

    /**************** Umarmen ***************/
    case "umarm":
    HELPCHECK("umarm");
    ParseRest(str);
    if (!who)
      Return("Wen willst Du umarmen?\n");
    if (who && CheckLife(0,NOT_DEAD,0,"Bitte umarme nur Lebewesen."))
      return 1;
    if (ghost() && CheckLife(NOT_SELF,0,
      "Du kannst Dich als Geist nicht selber waermen."))
        return 1;
    str1=who->QueryProp(P_NAME);
    if(pointerp(str1))str1=(string)str1[0]; // Rumata
    str2=who->QueryPronoun(WEN);
    GHOSTCHECK("Du willst "+str1+" umarmen, aber Deine Arme gehen durch "
        +str2+" durch.\n",
      gname()+" will "+str1+" umarmen, aber "+QPP(MALE,WER,PLURAL)
        +" Arme gehen\ndurch "+str2+" hindurch.\n",
      gname()+" will Dich umarmen, aber "+QPP(MALE,WER,PLURAL)
        +" Arme gehen durch Dich hindurch.\n");
    if (for_all)  {
      out_sel="Du umarmst @@alle@@@@adverb@@.";
      out_vic="@@name@@ umarmt @@alle@@@@adverb@@.";
      return MixedOut(WEN);
    }
    if (who==this_player())  {
      out_sel="Du legst Deine Arme um Dich und waermst Dich "
        +"ein bisschen selber.";
      out_oth="@@name@@ legt "+QPP(MALE,WER,PLURAL)
        +" Arme um sich und waermt sich ein bisschen selber.";
    }
    else  {
      out_sel="Du umarmst@@ wen@@@@adverb@@.";
      out_vic="@@name@@ umarmt Dich@@adverb@@.";
      out_oth="@@name@@ umarmt@@ wen@@@@adverb@@.";
    }
    return FeelIt();

    /**************** Verfluchen ***************/
    case "verfluch":
    HELPCHECK("verfluch");
    if (!str)
      Return("Wen oder was willst Du denn verfluchen?\n");
    ParseRest(str);
    if (!who)  {
      out_sel="Du verfluchst "+str+".";
      out_oth="@@gname@@ verflucht "+str+".";
    }
    else  {
      if (who==this_player())
        Return("Sich selber verflucht man besser nicht...\n");
      if (!adverb)  {
        flag=sscanf(str, "%s %s", str1,str2);
        out_sel="Du verfluchst@@ wen@@"+(flag==2 ? " "+str2 : "")+".";
        out_vic="@@gname@@ verflucht Dich"+(flag==2?" "+str2 : "")+".";
        out_oth="@@gname@@ verflucht@@ wen@@"+(flag==2 ? " "+str2 : "")+".";
      }
      else  {
        out_sel="Du verfluchst@@ wen@@@@adverb@@.";
        out_vic="@@gname@@ verflucht Dich@@adverb@@.";
        out_oth="@@gname@@ verflucht@@ wen@@@@adverb@@.";
      }
    }
    return FeelIt();

    /**************** Verneigen / Verbeugen ***************/
    case "verneig":
    case "verbeug":
    HELPCHECK("verneig");
    GHOSTCHECK("Du verneigst Dich ein bisschen heftig - Dein Kopf taucht "
        +"kurz in den Boden.\n",
      gname()+" verneigt sich. Ein bisschen heftig - "+QPP(MALE,WER)
        +" Kopf\ntaucht kurz in den Boden ein.\n", 0);
    if ((!str) || (str == "dich")) {
      out_sel="Du verneigst Dich vor den Anwesenden.";
      out_oth="@@name@@ verneigt sich anmutig.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
        out_sel="Du verneigst Dich@@adverb@@ vor @@alle@@.";
        out_vic="@@name@@ verneigt sich@@adverb@@ vor @@alle@@.";
        return MixedOut(WEM);
      }
      if (!who && !adverb)
        Return("Verneige dich irgendwie oder vor jemandem.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Wie willst Du das denn schaffen?",
        "Vor Sachen wird hier nicht verneigt, klar?\n"))
          return 1;
      out_sel="Du verneigst Dich@@adverb@@"+(who ? " vor" : "")+"@@ wem@@.";
      if (who ) out_vic="@@name@@ verneigt sich@@adverb@@ vor Dir.";
      out_oth="@@name@@ verneigt sich@@adverb@@"+(who ? " vor" : "")
        +"@@ wem@@.";
    }
    return FeelIt();

    /**************** Verneinen ***************/
    case "nein":
    case "noe":
    HELPCHECK("nein");
    GHOSTCHECK("Du schuettelst Deinen Kopf so heftig, dass er kurz "
        +"davonschwebt.\n",
      gname()+" schuettelt heftig den Kopf.\nSo heftig, dass dieser "
        +"kurz davonschwebt und wieder eingefangen werden muss.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schuettle wie den Kopf?\n");
    out_sel="Du schuettelst@@adverb@@ den Kopf.";
    out_oth="@@name@@ schuettelt@@adverb@@ den Kopf.";
    return FeelIt();

    /**************** Wackeln ***************/
    case "wackel":
    case "wackl":
    HELPCHECK("wackel");
    if (ghost())
      Return("Da gibt es nichts mehr, womit Du wackeln kannst.\n");
    if (str)
      if (strstr(str, "mit ")==0)
        sscanf(str, "mit %s", sfoo);
      else if (strstr(str,"mit ")>0)  {
        sscanf(str, "%s mit %s", sfoo, sfoo);
        flag=1;
      }
    if (sfoo=="") sfoo=0;
    ParseRest(str, (sfoo ? (flag ? " mit " : "mit ")+sfoo : 0));
    if (str && !adverb && !sfoo)
      Return("Wackle wie oder womit?\n");
    out_sel="Du wackelst@@adverb@@ mit "+(sfoo ? sfoo+"." : "dem Hintern.");
    out_oth="@@name@@ wackelt@@adverb@@ mit "
      +(sfoo ? sfoo+"." : QPP(MALE,WEM)+" Hintern.");
    return FeelIt();

    /**************** Waelzen ***************/
    case "waelz":
    HELPCHECK("waelz");
    ParseRest(str);
    if (str && !adverb)
      Return("Waelze Dich wie auf dem Boden?\n");
    out_sel="Du waelzt Dich"+(adverb ? "@@adverb@@" : " vor Lachen")
      +" auf dem Boden.";
    out_oth="@@gname@@ waelzt sich"+(adverb ? "@@adverb@@" : " vor Lachen")
      +(ghost() ? " im" : " auf dem")+" Boden.";
    return FeelIt();

    /**************** Warten ***************/
    case "wart":
    HELPCHECK("wart");
    ParseRest(str);
    if (!str)  {
      out_sel="Du tippst mit dem Fuss auf den Boden.";
      out_oth="@@gname@@ tippt mit dem Fuss auf den Boden.";
    } else if (!who && adverb)  {
      out_sel="Du wartest@@adverb@@.";
      out_oth="@@gname@@ wartet@@adverb@@.";
    } else  {
      out_sel="Du wartest@@adverb@@ auf "+(who ? "@@wen@@." : str+".");
      if (who) out_vic="@@gname@@ wartet@@adverb@@ auf Dich.";
      out_oth="@@gname@@ wartet@@adverb@@ auf "+(who ? "@@wen@@." : str+".");
    }
    return FeelIt();

#ifdef WECKE
    /**************** Wecken ***************/
    case "weck":
    HELPCHECK("weck");
    if (ParseRemote(str))
      return 1;
//    ParseRest(str);
    if (!who)
      Return("Wen willst Du wecken?\n");
    if (sscanf(str, "%s %s", sfoo, sfoo)==2)
      flag=1;
    out_sel="Dein Wecker klingelt bei @@wem@@@@adverb@@"
      +(adverb ? "." : (flag ? ": "+sfoo : "."));
    out_vic=" "+name(WESSEN)+" Wecker klingelt bei Dir@@adverb@@"
      +(adverb ? "." : (flag ? ": "+sfoo : "."));
    out_oth="@@gname@@ wirft "+QPP(MALE, WEN)
      +" Wecker@@adverb@@ nach @@wem@@.";
          if (!who->QueryProp(P_VISUALBELL))
         out_vic[0]=7; // chr(7)==BEL
      else out_vic=out_vic[1..];
    return FeelIt();
#endif

    /**************** Weinen ***************/
    case "wein":
    HELPCHECK("wein");
    GHOSTCHECK("Es reicht leider nur fuer ein paar winzige Nebelwoelkchen, "
        +"nicht fuer Traenen.\n",
      gname()+" verzieht das Gesicht und ein paar winzige Nebel-\n"
        +"woelkchen entfernen sich von seinen \"Augen\".\n", 0);
    if (!str)  {
      out_sel="Waaaaah! Du weinst bitterlich.";
      out_oth="@@name@@ bricht in Traenen aus und weint bitterlich.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
        Return("Weine Dich irgendwie bei irgendwem aus, aber nicht so.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Bei sich selber kann man sich so schlecht ausweinen.",
        "Bei wem willst Du Dich ausweinen???"))
          return 1;
      if (who)  {
        out_sel="Du weinst Dich@@adverb@@ bei@@ wem@@ aus.";
        out_vic="@@name@@ weint sich@@adverb@@ bei Dir aus.";
        out_oth="@@name@@ weint sich@@adverb@@ bei@@ wem@@ aus.";
      }
      else  {
        out_sel="Du brichst in Traenen aus und weinst@@adverb@@.";
        out_oth="@@name@@ bricht in Traenen aus und weint@@adverb@@.";
      }
    }
    return FeelIt();

    /**************** Winken ***************/
    case "wink":
    HELPCHECK("wink");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du winkst @@alle@@@@adverb@@ zu.";
      out_vic="@@name@@ winkt @@alle@@@@adverb@@ zu.";
      return MixedOut(WEM);
    }
    if (!who && !adverb && str)
      Return("Vielleicht solltest Du auch sagen, wem oder wie Du "
        +"(zu)winken willst.\n");
    if (who && CheckLife(NOT_SELF, NOT_DEAD,
      "Wink Dir nicht selber zu.",
      "Du musst schon einem Lebewesen zuwinken."))
        return 1;
    out_sel="Du winkst@@ wem@@@@adverb@@"+(who ? " zu" : "")+".";
    if (who) out_vic="@@gname@@ winkt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ winkt@@ wem@@@@adverb@@"+(who ? " zu" : "")+".";
    return FeelIt();

    /**************** Wuergen ***************/
    case "wuerg":
    HELPCHECK("wuerg");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Wuerge wen oder wie?\n");
    if (!who)  {
      out_sel="Du faengst@@adverb@@ an zu wuergen.";
      out_oth="@@gname@@ faengt@@adverb@@ an zu wuergen.";
    } else if (CheckLife(NOT_SELF, NOT_DEAD,
          "Du wuergst ein bischen an Dir rum. Dir wird schnell langweilig.",
	  "Wuerg lieber ein Lebewesen.")) {
      return 1;
    } else {
      out_sel="Du springst @@wen@@ an und faengst an, "+who->QueryPronoun(WEN)
        +"@@adverb@@ zu wuergen.";
      out_vic="@@gname@@ springt Dich auf einmal an und wuergt Dich@@adverb@@.";
      out_oth="@@gname@@ springt auf einmal @@wen@@ an und wuergt "
        +who->QueryPronoun(WEN)+"@@adverb@@.";
    }
    return FeelIt();

    /**************** Wundern ***************/
    case "wunder":
    HELPCHECK("wunder");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Wie oder ueber wen willst Du Dich wundern?\n");
    out_sel="Du wunderst Dich@@adverb@@"+(who ? " ueber @@wen@@." : ".");
    if (who) out_vic="@@gname@@ wundert sich@@adverb@@ ueber Dich.";
    out_oth="@@gname@@ wundert sich@@adverb@@"+(who ? " ueber @@wen@@." : ".");
    return FeelIt();

    /**************** Wuscheln ***************/
    case "wuschel":
    case "wuschl":
    HELPCHECK("wuschel");
    ParseRest(str);
    if (!who)
      Return("Wen willst Du denn wuscheln?\n");
    if (CheckLife(0,NOT_DEAD,
      "", "Hmm, sehr tot. Ne, lieber nicht."))
        return 1;
    if (who->QueryProp(P_FROG))  {
      write("Du verwuschelst...  aeh... hm. Ein Frosch hat wohl nix "
        +"zum Wuscheln.\n");
      return 1;
    };
    GHOSTCHECK("Du willst "+who->name(WEN)+" wuscheln - aber Deine "
        +"Haende gehen\nglatt durch den Kopf durch.\n",
      gname()+" will "+who->name(WEN)+" den Kopf wuscheln, aber "
        +"die Geister-\nhaende gehen glatt durch den Kopf durch.\n",
      gname()+" will Dich wuscheln, aber diese Geisterhaende "
        +"gehen\nglatt durch Deinen Kopf durch - Du hast ein seltsames "
        +"Gefuehl dabei.\n");
    if (member(({"highlander","boing","mieze","freund"}), who->query_real_name())>-1)
      switch (who->query_real_name())  {
        case "highlander": str1="Federn"; break;
        case "freund"    :
        case "mieze"     :
        case "boing"     : str1="Fell"; break;
      }
    else if (who->is_class_member(({CL_DRAGON, CL_FISH, CL_REPTILE})))
      str1="Schuppen";
    else if (who->is_class_member(({CL_BIRD, "elster","greif"})))
      str1="Federn";
    else if (who->is_class_member(({CL_MAMMAL_LAND,CL_FELINE,"tiger",
                                    "steinbeisser","knuddeleisbaer"})))
      str1="Fell";
    else if(who->is_class_member(({CL_PLANT})))
      str1="Blaetter";
    else str1="Haare";
    out_sel="Du verwuschelst@@adverb@@ @@wessen@@ "+str1+".";
    out_vic="@@name@@ verwuschelt@@adverb@@ Dein"
      +(str1=="Fell" ? " " : "e ")+str1+".";
    out_oth="@@name@@ verwuschelt@@adverb@@ @@wessen@@ "+str1+".";
    return FeelIt();

    /**************** Zitieren ***************/
    case "zitier":
    HELPCHECK("zitier");
    ParseRest(str);
    if (!str)
      Return("Zitiere was oder wen womit?\n");
    sfoo=implode(explode(str, " ")[1..], " ");
    if (sfoo=="") sfoo=0;
    if (who)  {
      out_sel="Du zitierst @@wen@@"+(sfoo ? ": \""+sfoo+"\"" : "")+".";
      out_vic="@@gname@@ zitiert Dich"+(sfoo ? ": \""+sfoo+"\"" : "")+".";
      out_oth="@@gname@@ zitiert @@wen@@"+(sfoo ? ": \""+sfoo+"\"" : "")+".";
    }
    else  {
      sfoo=explode(str, "/")[0];
      out_sel="Du zitierst@@adverb@@"+(sfoo ? ": \""+sfoo+"\"" : "")+".";
      out_oth="@@gname@@ zitiert@@adverb@@"+(sfoo ? ": \""+sfoo+"\"" : "")+".";
    }
    return FeelIt();

    /**************** Zittern ***************/
    case "zitter":
    HELPCHECK("zitter");
    ParseRest(str);
    if (str && !adverb)
      Return("Zittere wie?\n");
    out_sel="Du zitterst"+(adverb ? "@@adverb@@." : " vor Angst.");
    out_oth="@@gname@@ zittert"+(adverb ? "@@adverb@@." : " vor Angst.");
    return FeelIt();

    /**************** Schulterzucken ***************/
    case "zuck" :
    HELPCHECK("zuck");
          if (str)
      if (sscanf(str,"%s mit den schultern",sfoo))
        str=sfoo;
    else if (sscanf(str,"%s den schultern",sfoo))
      str=sfoo;
    else
      if (sscanf(str,"%s schultern",sfoo))
        str=sfoo;
    if (str=="") str=0;
    ParseRest(str);
    if (str && !adverb)
      Return("Zucke wie mit den Schultern?\n");
    out_sel="Du zuckst@@adverb@@ mit den Schultern.";
    out_oth="@@gname@@ zuckt"+(adverb ? "@@adverb@@" : " ratlos")
            +" mit den Schultern.";
    return FeelIt();

    /**************** Zwinkern ***************/
    case "zwinker":
    HELPCHECK("zwinker");
    if (ghost())
      Return("Vergiss es - das ist als Geist viel zu unauffaellig, als dass\n"
        +"es andere Leute sehen wuerden.\n");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Zwinkere wie? Zwinkere wem zu?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
      "Du kannst Dir nicht selber zuzwinkern.",
      "Wieso reagiert das Ding da nicht auf Dein Zwinkern? Ist es etwa tot?"))
        return 1;
    out_sel="Du zwinkerst@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    if (who) out_vic="@@name@@ zwinkert Dir@@adverb@@ zu.";
    out_oth="@@name@@ zwinkert@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    return FeelIt();

    /**************** Zunge rausstrecken ***************/
    case "streck":
    HELPCHECK("streck");
    GHOSTCHECK("Sorry, Du hast keine Zunge zum Rausstrecken.\n","",0);
    if (!str)
      Return("Strecke was wie wem wo wann wieso?\n");
    str=lower_case(str);
    if (sscanf(str, "%s zunge raus", str1)!=1 &&
    sscanf(str, "%s die zunge raus", str1)!=1)
      Return("Strecke was wie wem wo wann wieso?\n");
    ParseRest(str1);
    if (for_all)  {
      out_sel="Du streckst @@alle@@@@adverb@@ die Zunge raus.";
      out_vic="@@name@@ streckt @@alle@@@@adverb@@ die Zunge raus.";
      return MixedOut(WEM);
    }
    out_sel="Du streckst@@ wem@@@@adverb@@ die Zunge raus.";
    if (who) out_vic="@@name@@ streckt Dir@@adverb@@ die Zunge raus.";
    out_oth="@@name@@ streckt@@ wem@@@@adverb@@ die Zunge raus.";
    return FeelIt();

    // Spezialsachen - Geisterverben und Magierverben

    /**************** Rasseln ***************/
    case "rassel":
    case "rassl":
    if (!ghost())
      Return("Das ist nicht Dein Fachgebiet - Du bist doch kein Geist!\n");
    HELPCHECK("rassel");
    ParseRest(str);
    if (str && !adverb)
      Return("Rassel wie?\n");
    out_sel="Du rasselst"+(adverb ? "@@adverb@@" : " fuerchterlich")
      +" mit einer rostigen Rasselkette,\n"
      +"die Du auf einmal fuer einen Moment in der Hand haeltst.";
    out_oth="@@gname@@ holt auf einmal eine rostige Rasselkette aus\n"
      +"dem Nichts und faengt an,"+(adverb ? "@@adverb@@" : " fuerchterlich")
      +" damit zu rasseln.\n"
      +"Danach ist die Kette auf einmal wieder verschwunden.";
    return FeelIt();

    /**************** Heulen ***************/
    case "heul":
    if (!ghost())
      Return("Lass das mal den Fachleuten (also den Geistern).\n");
    HELPCHECK("heul");
    ParseRest(str);
    if (str && !adverb)
      Return("Heule wie?\n");
    out_sel="Du heulst"+(adverb ? "@@adverb@@." : " schauerlich.");
    out_oth="@@gname@@ heult"+(adverb ? "@@adverb@@." : " schauerlich.");
    return FeelIt();

    /**************** Treten (tretet) ***************/
    case "kick":
    if (!IS_WIZARD(this_player()))
      return 0;
    HELPCHECK("kick");
    if (!str)  {
      GHOSTCHECK("Dein Fuss faehrt durch die beruehmte langvergessene "
          +"unsichtbare Schildkroete\nhindurch.\n",
        gname()+" will die beruehmte langvergessene unsichtbare\n"
          +"Schildkroete treten, aber "+QPP(MALE,WER)
          +" Fuss faehrt durch sie hindurch.\n", 0);
      out_sel="Du tretest die beruehmte langvergessene unsichtbare "
        +"Schildkroete.";
      out_oth="@@name@@ tretet die beruehmte langvergessene unsichtbare "
        +"Schildkroete.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
        out_sel="Du tretest @@alle@@@@adverb@@.";
        out_vic="@@name@@ tretet @@alle@@@@adverb@@.";
        return MixedOut(WEN);
      }
      if (!who && !adverb)
        Return("Wenn Du schon was nach kick tippst, dann sag mir wen "
          +"oder wie ichdas soll.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du schaffst es nicht, Dir selber in den Hintern zu treten.",
        "Tote Sachen tritt man nicht auch noch!"))
          return 1;
      if (who)  {
        out_sel="Du tretest@@ wen@@@@adverb@@.";
        if (who) out_vic="@@gname@@ tretet Dich@@adverb@@.";
        out_oth="@@gname@@ tretet@@ wen@@@@adverb@@.";
      }
      else  {
        out_sel="Du tretest die beruehmte langvergessene unsichtbare "
          +"Schildkroete@@adverb@@.";
        out_oth="@@gname@@ tretet die beruehmte langvergessene unsichtbare "
          +"Schildkroete\n@@adverb@@.";
      }
    }
    return FeelIt();

    /************* Nassspritzen ***************/
    case "splash":
    if (!IS_WIZARD(this_player()) &&
    !(IS_SEER(this_player()) && present("SEHER\nspritzpistole",this_player())))
      return 0;
    HELPCHECK("splash");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du ziehst Deine Wasserpistole und spritzt @@alle@@@@adverb@@ "
        +"patschnass.";
      out_vic="@@gname@@ zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt\n"
        +"@@alle@@@@adverb@@ patschnass.";
      return MixedOut(WEN);
    }
    if (!who)
      Return("Wen willst Du denn nassmachen?\n");
    if (who == this_player())  {
      out_sel="Sag mal, kommst Du Dir nicht ein bisschen doof vor?\n"
        +"Du ziehst Deine Wasserpistole und spritzt Dich@@adverb@@ selber patschnass.";
      out_oth="@@gname@@ zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt "
        +"sich@@adverb@@ aus unerfindlichen Gruenden selbst patschnass.";
    }
    else  {
      out_sel="Du ziehst Deine Wasserpistole und spritzt @@wen@@@@adverb@@ "
        +"patschnass.";
      out_vic="@@gname@@ zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt "
        +"Dich@@adverb@@ patschnass.";
      out_oth="@@gname@@ zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt "
        +"@@wen@@@@adverb@@ patschnass.";
    }
    return FeelIt();

    /**************** Anflammen ***************/
    case "flam":
    if (!IS_WIZARD(this_player()) &&
    !(IS_SEER(this_player()) && present("SEHER\nflammenwerfer",this_player())))
      return 0;
    HELPCHECK("flame");
    if (ghost())
      Return("Du hast leider grade Deinen Flammenwerfer nicht dabei.\n");
    ParseRest(str);
    ifoo=!random(7);
    if (for_all)  {
      out_sel="Du holst aus Deinen tiefsten Taschen (oder was weiss denn "
        +"ich woher) Deinen\nMorgenGrauen handgearbeiteten Mini-Flammenwerfer "
        +"(tm), richtest ihn aus und...\n"
        +(ifoo ? "schaust leicht frustriert auf das Streichholz, in das "
          +"er sich verwandelt hat."
        : "feuerst@@adverb@@ einen riesigen Feuerball auf @@alle@@ ab.\n"
          +"Es riecht auf einmal so verbrannt hier...");
      out_vic="@@name@@ holt auf einmal irgendwoher einen MorgenGrauen "
        +"handgearbeiteten\nMini-Flammenwerfer (tm), richtet ihn aus und...\n"
        +(ifoo ? "schaut ziemlich frustriert auf das Streichholz, in das "
          +"sich das Ding verwandelt hat."
        : "feuert@@adverb@@ einen riesigen Feuerball auf @@alle@@ ab.\n"
          +"Dir wird so warm um's Herz...");
      return MixedOut(WEN);
    }
    if (!who)
      Return("Wen willst Du denn ankokeln?\n");
    out_sel="Du holst aus Deinen tiefsten Taschen (oder was weiss denn "
      +"ich woher) Deinen\nMorgenGrauen handgearbeiteten Mini-Flammenwerfer "
      +"(tm), richtest ihn aus und...\n"
      +(ifoo ? "schaust leicht frustriert auf das Streichholz, in das er "
        +"sich verwandelt hat."
      : "feuerst@@adverb@@ einen riesigen Feuerball auf @@wen@@ ab.\n"
        +"Es riecht auf einmal so verbrannt hier...");
    out_vic="@@name@@ holt auf einmal irgendwoher einen MorgenGrauen "
      +"handgearbeiteten\nMini-Flammenwerfer (tm), richtet ihn auf Dich "
      +"aus und...\n"
      +(ifoo ? "schaut ziemlich frustriert auf das Streichholz, in das "
        +"sich das Ding\nverwandelt hat."
      : "feuert@@adverb@@ einen riesigen Feuerball auf Dich ab.\n"
        +"Dir wird so warm ums Herz...");
    out_oth="@@name@@ holt auf einmal irgendwoher einen MorgenGrauen "
      +"handgearbeiteten\nMini-Flammenwerfer (tm), richtet ihn "
      +"auf@@ wen@@ aus und...\n"
      +(ifoo ? "schaut ziemlich frustriert auf das Streichholz, in das "
        +"sich das Ding\nverwandelt hat."
      : "feuert@@adverb@@ einen riesigen Feuerball auf@@ wen@@ ab.\nEs "
        +"riecht auf einmal irgendwie verbrannt hier ...");
    return FeelIt();

    // Special 2: remote verbs

    /**************** Remote knuddeln ***************/
    case "rknuddel":
    case "rknuddl":
    HELPCHECK("rknuddel");
    if (ParseRemote(str))
      return 1;
    if (!who)
      Return("Knuddle wen?\n");
    if (CheckLife(NOT_SELF,0,
      "Das bringt doch nix, lass es halt.",
      0))
        return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm knuddel.\n");
    out_sel="Du knuddelst @@wen@@@@adverb@@ aus der Ferne.";
    out_vic="@@gname@@ knuddelt Dich@@adverb@@ aus der Ferne.";
    return FeelIt();

    /**************** Remote winken ***************/
    case "rwink":
    HELPCHECK("rwink");
    if (ParseRemote(str))
      return 1;
    if (!who)
      Return("Winke wem zu?\n");
    if (CheckLife(NOT_SELF,0,
      "Sehr witzig. Pah.", 0))
        return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm wink.\n");
    out_sel="Du winkst @@wem@@@@adverb@@ aus der Ferne zu.";
    out_vic="@@gname@@ winkt Dir@@adverb@@ aus der Ferne zu.";
    return FeelIt();

    /**************** Verbenliste ***************/
    case "verb":
    case "verben":
    HELPCHECK("verb");
    More(SOULHELP->Help());
    return 1;

    /**************** Adverbienverwaltung ***************/
    case "adverb":
    case "adverben":
    case "adverbien": {   /* Das ist die richtige Form, aber wer weiss das? */
    string f1,f2;
    HELPCHECK("adverb");
    if (!str || str=="#" || str=="$")
      return zeige_adverbs((str=="#" ? 1 : (str=="$" ? 2 : 0)));
    if (sscanf(str, "%s %s", f1,f2)==2)  {
      f1 = lower_case(f1); // kleingeschrieben speichern, spart Umwandlung
      if (f1=="")
        Return("Hm, da muss wohl ein Leerzeichen zu viel gewesen sein. Bitte "
          +"nochmal,\naber ohne zuviele Leerzeichen.\n");
      if (f1=="?")  {
        f2 = lower_case(f2);
        string match;
        if ((match=QueryStdAdverbs()[f2] || plr_adverbs[f2]))
          write("Die Abkuerzung "+f2+" gehoert zu dem Adverb:\n"+match+LF);
        else
          write("Diese Abkuerzung ist bisher nicht definiert.\n");
        return 1;
      }
      if (QueryStdAdverbs()[f1])
        Return("Die Standardabkuerzungen koennen nicht neu definiert "
          +"werden.\n");
      if (sizeof(plr_adverbs)>=100) 
      {
        write("Mehr als 100 eigene Adverbien kannst Du nicht definieren.\n");
        return 1;
      }
      if (plr_adverbs[f1])  {
        plr_adverbs[f1]=f2;
        write("OK, Adverb mit der Abkuerzung \""+f1+"\" auf \""+f2
          +"\" gesetzt.\n");
      }
      else  {
        if (sizeof(f1) > 6)
          Return("Die Abkuerzung ist zu lang, bitte nicht mehr als "
            +"6 Zeichen.\n");
        plr_adverbs[f1]=f2;
        write("OK, neues Adverb \""+f2+"\" mit der Abkuerzung \""+f1+"\".\n");
      }
    }
    else  {
      str = lower_case(str);
      if (QueryStdAdverbs()[str])
        Return("Die Standardadverben koennen nicht geloescht werden.\n");
      else if (!plr_adverbs[str])
        Return("Du hast kein Adverb mit dieser Abkuerzung.\n"
          +"Syntax: adverb, um die Adverbien anzuzeigen,\n"
          +"        adverb #, um nur Deine Adverbien anzuzeigen,\n"
          +"        adverb $, um nur die Standardadverbien anzuzeigen,\n"
          +"        adverb ? <Abkuerzung>, um nachzusehen, ob <Abkuerzung> "
            +"definiert ist,\n"
          +"        adverb <Abkuerzung> <Adverb>, um der <Abkuerzung> das "
            +"<Adverb>\n"
          +"               zuzuordnen,\n"
          +"        adverb <Abkuerzung>, um das Adverb mit der <Abkuerzung> "
            +"zu loeschen,\n");
      else  {
        write("OK, Adverb \""+plr_adverbs[str]+"\" geloescht.\n");
        plr_adverbs=m_copy_delete(plr_adverbs, str);
      }
    }
    return 1; 
    }
  }
  return(0);  //fallthrough
}


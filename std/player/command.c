// MorgenGrauen MUDlib
//
// player/commands.c -- alias, history and player command handling
//
// $Id: command.c 9576 2016-06-18 15:00:01Z Zesstra $
#pragma strong_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <player/command.h>
#include <player/comm.h>
#include <thing/properties.h>
#include <living/moving.h>
#include <player.h>
#undef NEED_PROTOTYPES

#include <properties.h>
#include <language.h>
#include <new_skills.h>
#include <config.h>
#include <defines.h>
#include <wizlevels.h>
#include <logging.h>
#include <strings.h>

#define CBLOG(x)    log_file(SHELLLOG("DISABLECOMMANDS"),x,200000)

#define HIST_SIZE 40
#define EPMASTER "/secure/explorationmaster"

private mapping aliases;
private string *commands;
private int hist_size, show_processing, histmin;
private string default_notify_fail;
private nosave string *history, *unparsed_args, unmodified;
private nosave int hist_now;
private nosave object last_command_env;
private nosave int cmds_per_time, last_chg, max_commands, *cmd_types;
// Datenstruktur: ({Setzer, Ablaufzeit, String/Closure})
private nosave mixed disablecommands;
private nosave object* syntaxdb;

nomask void __set_bb(int flag);

static varargs int __auswerten(string str, string intern);
varargs int SoulComm(string str, string _verb);
varargs mixed More(string str, int fflag, string returnto);
static int _starts_with(string str, string start);
static void reallocate_histbuf();

private void AddHistory(string str)
{
  if (!stringp(str) || str=="" || str[0]=='&' || str[0]=='^' ||
      str=="hist")
    return;
  if (!hist_size) return;
  if (!pointerp(history) || sizeof(history)!=hist_size)
    reallocate_histbuf();
  if (sizeof(str)>=histmin && history[(hist_size+hist_now-1)%hist_size]!=str)
    history[(hist_now++)%hist_size]=str;
}

static void create()
{
  last_chg=0;
  histmin=hist_now=0;
  Set(P_LOCALCMDS,({}));
  Set(P_LOCALCMDS,PROTECTED,F_MODE_AS);
  Set("_syntaxdb", SECURED|SAVE, F_MODE_AS);

  show_processing=1;
  unparsed_args=({0,0,0});
  hist_size=HIST_SIZE;
}

static int replacedisplay(string str)
{
  if (!str || str=="" || !sscanf(str,"%d",show_processing))
    printf("Unzulaessige Eingabe!\n%s 0|1|2\n",query_verb());
    printf("Ersetzungsanzeige auf Level %d.\nLevel 0: Nichts anzeigen\n"+
     "Level 1: Nur History-Ersetzungen anzeigen\n"+
     "Level 2: History- und Alias-Ersetzungen anzeigen\n",show_processing);
  if (show_processing>2&&!IS_WIZARD(ME)) show_processing=2;
  return 1;
}

static int histmin(string str)
{
  int len;

  if (!str||!sscanf(str,"%d",len)||len<0)
  {
    write("Benutzung: histmin ZAHL\nLegt die Mindestlaenge fest, die eine \
Befehlszeile haben muss, um in den\nHistory-Puffer zu gelangen. Derzeit \
eingestellt auf "+(string)histmin+" Zeichen.\n");
    return 1;
  }
  histmin=len;
  write("Mindestlaenge auf "+(string)len+" eingestellt.\n");
  return 1;
}

static void reallocate_histbuf()
{
  int i;

  history=allocate(hist_size);
  hist_now=0;
  for (i=0;i<hist_size;i++)
    if (!stringp(history[i]))
      history[i]="\n\n";
}

static int histlen(string str)
{
  int d;
  if (!str||!sscanf(str,"%d",d)||d<0||d>40)
  {
    write("Benutzung: histlen ZAHL\nZAHL muss zwischen 0 und 40 liegen.\n");
    printf("Deine History-Buffer-Laenge liegt bei %d Befehlen.\n",hist_size);
    return 1;
  }
  hist_size=d;
  printf("Deine History-Buffer-Laenge liegt jetzt bei %d Befehlen.\n",
   hist_size);
  reallocate_histbuf();
  return 1;
}

static void initialize()
{
  if (!pointerp(history)||sizeof(history)!=hist_size)
    reallocate_histbuf();
  add_action("__auswerten","",1);
    max_commands = EPMASTER->QueryCommands();
    cmd_types = EPMASTER->QueryCmdTypes() || ({});

    if ( !mappingp(aliases) )
        aliases = ([]);

    if ( !pointerp(commands) )
        commands = ({});

    if (QueryProp("_syntaxdb"))
        syntaxdb = ({find_object("/secure/syntaxdb")});
/*    else if (QueryProp(P_TESTPLAYER))
    {
      SetProp("_syntaxdb", 1);
      call_out(#'_notify, 2,
          "\nDa Du als Testspieler markiert bist, wurde bei Dir "
          "die Syntaxsammlung eingeschaltet. Du kannst dies "
          "wieder ausschalten. (hilfe syntaxsammlung) "
          "Es waere schoen, wenn Du es beim Testen von "
          "Gebieten einschaltest.", 0);
    }*/
}

static mixed _set_default_notify_fail(string s)
{
  if (stringp(s)&&s!="")
  {
    if (s[<1]!='\n') s+="\n";
    return default_notify_fail=s;
  }
  else if (!s||s=="")
    return (default_notify_fail=0);
}

static mixed _query_default_notify_fail()
{
  return default_notify_fail;
}

static int set_errormessage(string s)
{
  if (!(s=_unparsed_args()))
  {
    _set_default_notify_fail(0);
    write("Standard-Fehlermeldung auf \"Wie bitte?\" gesetzt.\n");
  } else
  {
    write(break_string(sprintf("Standard-Fehlermeldung auf %s gesetzt.\n",
             s),78));
    _set_default_notify_fail(s);
  }
  return 1;
}

void reconnect()
{
  if (!mappingp(aliases)) aliases=([]);

    if ( !pointerp(commands) )
        commands = ({});

    max_commands = EPMASTER->QueryCommands();
    cmd_types = EPMASTER->QueryCmdTypes() || ({});
}

static int show_hist()
{
  int i;
  string comm;

  tell_object( ME, "Die History-Liste enthaelt folgende Kommandos:\n" );

  for( i = 0; i < hist_size; i++ )
      if ((comm=history[(hist_now+i)% hist_size])!= "\n\n")
          tell_object( ME, " &"+(hist_now+i-hist_size)+"/-"+ (hist_size-i-1)
      +"\t= "+comm+"\n");
  return 1;
}

static string present_alias(mixed *ali)
{
  int j,k;
  string s,s2;

  for (s="",j=sizeof(ali)-1;j>=0;j--)
    if (intp(ali[j]))
      if ((k=ali[j])<0)
  s="$"+(k==-1?"":(string)-k)+"*"+s;
      else
  s="$"+(string)k+s;
    else
      {
         s2=implode(explode(ali[j],"\\"),"\\\\");
         s=implode(explode(s2,"$"),"\\$")+s;
      }
  return s;
}

#define ALIFORMAT ({" %s\t= %s", "alias %s %s"})[display_as_aliascommand]
// Ich weiss, den Variablennamen im define zu haben ist unfein, aber das
// macht es im Code dann angenehm uebersichtlich.  -HrT

static int query_aliases(int display_as_aliascommand)
{
  int i;
  string *a,*ali;

  if(i=sizeof(ali=sort_array(m_indices(aliases),#'<))) //')))
  {
    for(a=({}),i--; i>=0; i--)
        a+=({sprintf(ALIFORMAT, ali[i], present_alias( aliases[ali[i]] ) ) });
    More("Du hast folgende Aliase definiert:\n"+implode(a,"\n"));
  }
  else
    write("Du hast keine Aliase definiert.\n");
  return 1;
}

static int
_starts_with(string str, string start)
{
  return (sizeof(start)>sizeof(str) ? 0
    : str[0..sizeof(start)-1]==start);
}

static int alias(string str)
{
  string commandverb;
  string *tmp,um,*hits;
  int num, l, pos, cont;
  int display_as_aliascommand, familymode;

  // unbearbeitetes Kommando ohne Verb ermitteln (auch ohne Trim an Anfang und
  // Ende)
  if (unmodified && unmodified!="")
    um=implode(old_explode(unmodified," ")[1..]," ");

  if (um=="") um=0;
  if( !(str = um||_unparsed_args()) || str=="*")
    return query_aliases(0);

  while(sizeof(str) >= 2 && str[0] == '-')
  {
    if (str[1] == 'a')
      display_as_aliascommand = 1;
    else if (str[1] == 'f')
      familymode = 1;
    else
      break;
    // "-? " abschneiden
    str = trim(str[2..], TRIM_LEFT);
  }
  if (!sizeof(str) || str=="*")
    return query_aliases(display_as_aliascommand);

  pos=member(str,' ');
  if (pos < 0) // Nur 1 Arg, Alias abfragen
  {
    if ((tmp=aliases[str])) // genau eins angegebenen
      printf(ALIFORMAT+"\n",str,present_alias(tmp));
    else if (str[<1]=='*')  // * am Ende, alle ausgeben, die passend anfangen
    {
      str=str[0..<2];
      hits=filter(m_indices(aliases), #'_starts_with, str);
      if (!sizeof(hits))
      {
        printf("Du hast kein Alias, das mit \"%s\" anfaengt.\n", str);
        return 1;
      }
      hits=sort_array(hits, #'>);
      for (l=sizeof(hits); l--;)
        hits[l]=sprintf(ALIFORMAT, hits[l], present_alias(aliases[hits[l]]));
      More("Folgende Aliase beginnen mit \""+str+"\":\n"+implode(hits,"\n"));
    }
    else  // Nix gefunden
      printf("Du hast kein Alias \"%s\" definiert.\n",str);
    return 1;
  }

  if (!pos)
  {
    write("Fehler: Leerzeichen am Alias-Anfang\n");
    return 1;
  }
  // Kommandoverb alles bis zum ersten " ".
  commandverb=str[0..pos-1];
  if (commandverb=="unalias")
  {
    write
      ("Es nicht moeglich, den Befehl unalias zu ueberladen (waer dumm :))\n");
    return 1;
  }
  if (commandverb=="*")
  {
    write
      ("Es nicht moeglich, den Befehl \"*\" zu ueberladen.\n");
    return 1;
  }

  str=str[pos+1..],tmp=({});
  while (l=sizeof(str)) {
    pos=0,cont=1;
    while (cont) {
      if (pos<l) {
        if(str[pos]=='\\') {
          str=str[0..pos-1]+str[pos+1..];
          l--;
        } else {
          if (str[pos]=='&' || str[pos]=='$') {
            cont=0;
            if (pos>0) {
              tmp+=({str[0..pos-1]});
            }
            if (pos==l-1) {
              printf("Fehler: %c am Zeilenende\n",str[pos]);
              return 1;
            }
            if ((num=str[++pos])=='*') {
              num=1;
              pos--;
            } else {
              num-='0';
            }
            if (num<0 || num>9) {
              printf("Fehler: Nach %c muss Ziffer oder * folgen\n",
               str[pos-1]);
              return 1;
            }
            if ((str=str[pos+1..])!=""&&str[0]=='*') {
              str=str[1..];
              num=-num;
            }
            tmp+=({num});
          }
        }
        pos++;
      } else {
        cont=0;
        if (str!="") tmp+=({str});
        str="";
      }
    }
  }
  if ((!aliases[commandverb]) && (sizeof(aliases)>2000))
    printf("Du hast schon genuegend Aliase definiert!\n");
  else
  {
    aliases[commandverb]=tmp;
    printf("Neues Alias: %s\t= %s\n",commandverb, present_alias(tmp));
  }
  return 1;
}

static int unalias(string str) {
  int i, familymode;
  string *als,um;

  if (unmodified&&unmodified!="")
    um=implode(old_explode(unmodified," ")[1..]," ");
  if (um=="") um=0;
  if ( !(str=um || _unparsed_args()))
    return 0;

  while(sizeof(str) >= 2 && str[0] == '-')
  {
    if (str[1] == 'f')
      familymode = 1;
    else
      break;
    // "-f " abschneiden
    str = trim(str[2..], TRIM_LEFT);
  }

  if (str == "*.*" || str == "*") {
    write(break_string(
      "Versuchs mal mit 'unalias .*', wenn Du wirklich alle Alias entfernen "
      "willst.",78));
    return 1;
  }
  if (!member(aliases,str))
  {
    als=regexp(m_indices(aliases),("^"+str+"$"));
    if (!(i=sizeof(als)))
    {
      write("So ein Alias hast Du nicht definiert.\n");
      return 1;
    }
    for (--i;i>=0;i--)
      m_delete(aliases,als[i]);
    write(break_string(("Du entfernst folgende Aliase: "+
      implode(als," ")+".\n"),75));
    return 1;
  }
  m_delete(aliases,str);
  write("Du entfernst das Alias \""+str+"\".\n");
  return 1;
}

varargs string _unparsed_args(int level)
{
  return unparsed_args[level];
}

#define ARTIKEL ({"das","der","die","des","dem","den","ein","eine","einer",\
                  "eines"})

#define TRENNER ({"in","aus","ueber","auf","unter","mit","durch","fuer",\
                  "von","vom","im","aufs","ein","weg","zurueck"})

static string _single_spaces(string str)
{
  return regreplace(str, "  *", " ", 1);
}

static mixed _return_args(string str)
{
  string *t,*t2,verb,s2;
  int i,l,j,l2;

  t=explode(trim(str,TRIM_BOTH)," ");
  verb=t[0];
  t = t[1..];
  if (!sizeof(t))
  {
    unparsed_args[0]=unparsed_args[1]=unparsed_args[2]=0;
    return str=verb;
  }
  else
    str = unparsed_args[0] = implode(t, " ");

  str=unparsed_args[1]=lower_case(_single_spaces(str));
  t=regexplode(str,"\\<im\\>|\\<ins\\>");
  for (i=1;i<sizeof(t);i+=2) t[i]="in";
  t=regexplode(implode(t,""),"[\\,\\!\\:][\\,\\!\\:]*");
  l=sizeof(t);
  for(i=1;i<l;i+=2) t[i]="";
  t=old_explode(implode(t,"")," ")-({""});
  for (i=sizeof(t)-2;i>=0;i--)
  {
    if (member(ARTIKEL,t[i])>=0)
      t=t[0..i-1]+t[i+1..];
  }
  unparsed_args[2]=implode(t," ");
  t=regexplode((str=implode(t," ")),"[0-9][0-9]*\\.");
  if ((l=sizeof(t))>2)
  {
    i=1;
    while (i<l-1)
    {
      t[i]=" "+t[i][0..<2]+" ";
      if ((l2=sizeof(t2=old_explode(t[i+1]," ")))<2)
  t[i+1]+=t[i];
      else
      {
  for (j=1;j<l2;j++)
  {
    if (member(TRENNER,t2[j])>=0)
    {
      t2[j-1]+=t[i];
      l2=0;
    }
  }
  if (!l2)
    t[i+1]=implode(t2," ");
  else
    t[i+1]+=t[i];
      }
      t[i]="";
      i+=2;
    }
    str=_single_spaces(verb+" "+implode(t," "));
    if (str[<1]==' ') str=str[0..<2];
  } else str=verb+(str==""?"":" "+str);
  if (show_processing>2)
    printf("-> {%s}\n",str);
  return str;
}

static void decay_average()
{
  if (absolute_hb_count()-last_chg>14)
  {
    last_chg=absolute_hb_count()-last_chg;
    if (last_chg>3000)
      last_chg=absolute_hb_count(),cmds_per_time=0;
    else
    {
      while (last_chg>14)
  cmds_per_time=cmds_per_time*9/10, last_chg-=15;
      last_chg=absolute_hb_count()-last_chg;
    }
  }
}

private void DelayPreparedSpells() {
  mixed ps;

  if (pointerp(ps=QueryProp(P_PREPARED_SPELL))
      && sizeof(ps)>=1 && intp(ps[0])) {
    ps[0]++;
    SetProp(P_PREPARED_SPELL,ps);
    write("Die Ausfuehrung Deines vorbereiteten Spruches wird verzoegert.\n");
  } else if (ps) {
    SetProp(P_PREPARED_SPELL,0);
  }
}

static mixed bb;
#ifndef BBMASTER
#define BBMASTER "/secure/bbmaster"
#endif

/** Interpretiert Aliase und History-Kommandos
  Eigentlich muesste hier noch die Umwandlung der Sonderzeichen
  verschiedener Zeichensaetze mit convert_charset gemacht werden,
  aber noch gibt es keine Moeglichkeit, den vom Spieler genutzten
  Zeichensatz zu identifizieren.
  \param[in] str string - Kommando des Spielers
  \return interpretiertes Alias bzw. korrektes Kommando aus der History
*/
private string parsecommand(string str)
{
  if (str[0]=='\\')
  {
    // Kommando soll nicht interpretiert werden
    return str[1..];
  }
  else if (str[0]=='&')
  {
    // Kommando aus der History
    string cmd = str[1..];
    int cmd_size = sizeof(cmd);
    int cmd_found = 0;
    if (cmd_size)
    {
      // Test ob &<text> etwas findet
      for (int i=0;i<hist_size-1 && !cmd_found;i++)
      {
        int idx = (hist_size-i+hist_now-1)%hist_size;
        if (history[idx][0..cmd_size-1]==cmd)
        {
          str = history[idx];
          cmd_found = 1;
        }
        if (cmd_found)
        {
          if (show_processing)
            printf("[%s]\n",str);
        }
      }
    }
    if (!cmd_found)
    {
      // Test, ob &<nr> klappt
      int nummer;
      if (str=="&&")
        str = "&-0";
      if (sscanf(str,"&%d",nummer))
      {
        if (nummer<0 || (!nummer && str[1]=='-'))
        {
          if (nummer<-(hist_size-1))
            nummer=-1;
          else
            nummer=(hist_now+nummer-1+hist_size)%hist_size;
        }
        else
        {
          if (nummer>hist_now || hist_now-nummer>hist_size)
            nummer=-1;
          else
            nummer=nummer%hist_size;
        }
        if (nummer<0 
            || ( (cmd=history[nummer]) =="\n\n") )
          notify_fail("Der Befehl ist nicht in der History!\n");
        else
        {
          str = cmd;
          if (show_processing)
            printf("[%s]\n",str);
        }
      }
    }
  }
  switch (str)
  {
    case "n": return "norden";
    case "s": return "sueden";
    case "w": return "westen";
    case "o": return "osten";
    case "nw": return "nordwesten";
    case "sw": return "suedwesten";
    case "so": return "suedosten";
    case "no": return "nordosten";
    case "ob": return "oben";
    case "u": return "unten";
  }
  // Test auf Alias
  string output = "";
  string* input = explode(str," ");
  int input_size = sizeof(input);
  mixed alias = aliases[input[0]];
  if (!alias)
    return str;
  foreach (mixed a:alias)
  {
    if (!intp(a))
      output += a;
    else
    {
      if (a >= 0)
      {
        if (input_size > a)
          output += input[a];
      }
      else
      {
        a = -a;
        if (input_size > a)
          output += implode(input[a..]," ");
      }
    }
  }
  output = _single_spaces(output);
  str = trim(output,TRIM_RIGHT);
  if (show_processing>1)
    printf("[%s]\n",str);
  return str;
}

/** Behandelt alle Sonderfaelle der Eingabe des Spielers
  Alle Befehle des Spielers, die nicht durch Objekte behandelt
  werden sollen, werden hier erkannt und ausgefuehrt.
  Dazu gehoert auch die Interpretation von Aliases und History-
  befehlen.
  \param[in] str string: Kommando des Spielers
  \return auszufuehrendes Kommando
    oder 0 fuer ein nicht interpretierbares Kommando
    oder 1 fuer ein bereits durchgefuehrtes Kommando
*/
mixed modify_command(string str)
{

  if (extern_call() && previous_object() &&
      (previous_object()!=this_object() || process_call()) )
  {
    return 0;
  }

  // Leerzeichen an den Enden abschneiden.
  str = trim(str, TRIM_BOTH);

  if (bb)
    BBMASTER->BBWrite(trim(str,TRIM_RIGHT,"\n"), 0);

  decay_average();
  cmds_per_time+=10000;

  unparsed_args[0]=unparsed_args[1]=unparsed_args[2]=unmodified=""; 

  if (!sizeof(str)) return "";

  // Kommando wird geparst
  unmodified=parsecommand(str);

  // Environment schonmal merken.
  last_command_env=environment();

  if (unmodified == "")
      return "";
  // Kommando in History merken, auch wenn es im Kommandoblock abgebrochen
  // wird.
  AddHistory(unmodified);

  // pruefen, ob Kommandoblock gesetzt ist.
  // (Fuer Magier mit mschau ein wird das ignoriert.)
  // BTW: Es wird absichtlich nicht das Ergebnis der Closure zurueckgegeben,
  // sonst wuerde man beliebigen Objekten nicht nur das Abbrechen, sondern
  // auch das Aendern von Kommandos ermoeglichen.
  if (disablecommands && !IS_LEARNING(ME) )
  {
    if (disablecommands[B_TIME] >= time()
      && objectp(disablecommands[B_OBJECT]))
    {
      // disablecommands valid
      // hart-kodierte Ausnameliste pruefen
      if ( member(({"mrufe","mschau","bug","idee","typo","detail"}),
            explode(str," ")[0]) == -1)
      {
        if (closurep(disablecommands[B_VALUE]))
        {
          if (funcall(disablecommands[B_VALUE],_return_args(unmodified)))
          {
            // Non-zero Closure-Ergebnis, Abbruch. Die gerufene Funktion ist
            // fuer eine geeignete Meldung an den Spieler verantwortlich.
            return 1;
          }
        }
        // wenn Text, dann auch pruefen, ob das Kommandoverb in den Ausnahmen
        // steht. (query_verb() geht leider hier noch nicht.)
        else if (stringp(disablecommands[B_VALUE])
                 && member(disablecommands[B_EXCEPTIONS],
                           explode(str," ")[0]) == -1)
        {
          // meldung ausgeben...
          tell_object(PL, disablecommands[B_VALUE]);
          // und Ende...
          return 1;
        }
      }
    }
    else disablecommands=0;
  }

  // Verfolger direkt ins Env reinholen.
  if (remove_call_out("TakeFollowers")>=0)
    catch(TakeFollowers();publish);

  DelayPreparedSpells();

  // Historyeintrag korrigieren
  if (unmodified[0]=='^')
  {
    string *oldnew,pre,post;
    if (sizeof(oldnew=explode(unmodified,"^"))>2)
    {
      int hist_idx = (hist_now-1)%hist_size;
      sscanf(history[hist_idx],"%s"+oldnew[1]+"%s", pre, post);
      unmodified = pre+oldnew[2]+post;
      if (show_processing)
        write("["+unmodified+"]\n");
      // korrigiertes Kommando natuerlich auch in die History.
      AddHistory(unmodified);
    }
  }

  if( bb )
    BBMASTER->BBWrite(" -> " + unmodified, 1);

  if (show_processing>1)
    printf("[%s]\n",unmodified);

  mixed ret = _return_args(unmodified);

  // wenn Spieler eingewilligt hat und die SyntaxDB geladen ist, Befehl
  // dorthin melden.
  if (syntaxdb)
  {
    if (!objectp(syntaxdb[0]))
      syntaxdb[0] = find_object("/secure/syntaxdb");
    if (syntaxdb[0])
      catch(syntaxdb[0]->start_cmd(unmodified);nolog);
  }
  return ret;
}

static int do_list(string str)
{
  string *cmdlist;
  int i;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  cmdlist=old_explode(_unparsed_args()||"",";")-({ "" });
  for (i=0;i<sizeof(cmdlist);i++)
  {
    cmdlist[i]=implode(old_explode(cmdlist[i]," ")-({}), " ");
    if (show_processing)
      write("["+cmdlist[i]+"]\n");
    command(cmdlist[i]);
  }
  return 1;
}

//falls die aliasliste kaputt ist ...

int unalias_all()
{
  if (IS_ELDER(this_interactive())) aliases=([]);
  return 1;
}

object _query_last_command_env()
{
  return last_command_env;
}

int _query_show_alias_processing()
{
  return show_processing;
}

int _query_histmin()
{
  return histmin;
}

varargs void AddAction(mixed fun, mixed cmd, int flag, int lvl)
{
  int i;
  mixed *cmds;

  log_file( "ARCH/ADD_ACTION", sprintf(
	"%s:\n  TO: %O TP: %O PO: %O\n   fun: %O cmd: %O flag: %O lvl: %O",
        dtime(time()), this_object(), this_player(), previous_object(),
	fun, cmd, flag, lvl));

  if (!(cmds=Query(P_LOCALCMDS))) cmds=({});

  if (!pointerp(cmd)) cmd=({cmd});

  for (i = sizeof(cmd)-1; i>=0; i--)
    cmds += ({({ cmd[i] , fun, flag, lvl})});

  Set(P_LOCALCMDS, cmds);
}

static int auswerten(mixed cmd, string str)
{
  if (closurep(cmd))
    return funcall(cmd,str);
  if (stringp(cmd))
    return call_other(this_object(),cmd,str);
  return 0;
}

static varargs int __auswerten(string str, string intern)
{
  string verb;
  mixed *cmd, cmds;
  int i,ret,lvl,l,vl;

  if (!intern)
    verb=query_verb();
  else
    verb=intern;
  lvl=query_wiz_level(ME);
  vl=sizeof(verb);
  cmds=QueryProp(P_LOCALCMDS);

  for(i=sizeof(cmds)-1;i>=0;i--)
  {
    cmd=cmds[i],l=sizeof(cmd[0]);
    if (cmd[0]==verb[0..l-1] && cmd[3]<=lvl && (cmd[2]||vl==l) &&
  (ret=auswerten(cmd[1],str)))
      return ret;
  }
  // An dieser Stelle gibt es hier und vermutlich nirgendwo anders etwas, was
  // dieses Kommando als gueltig betrachtet. Wir informieren ggf. die
  // Syntax-DB. (Achtung: wenn jemand ne add_action() im Spielerobjekt
  // einbaut, die vor dieser eingetragen wird, ist die Annahme ggf. falsch.)
  // wenn Spieler eingewilligt hat und die SyntaxDB geladen ist, Befehl
  // dorthin melden.
  if (syntaxdb)
  {
    if (!objectp(syntaxdb[0]))
      syntaxdb[0] = find_object("/secure/syntaxdb");
    if (syntaxdb[0])
      catch(syntaxdb[0]->cmd_unsuccessful();nolog);
  }

  return 0;
}

public void syntax_log_ep(int type)
{
  // wenn Spieler eingewilligt hat und die SyntaxDB geladen ist, Befehl
  // dorthin melden.
  if (syntaxdb && syntaxdb[0])
  {
      catch(syntaxdb[0]->LogEP(type);nolog);
  }
}

static mixed _query_localcmds()
{
  mixed *l;

  l=Query(P_LOCALCMDS);
  if (!pointerp(l))
    l=({});
  return ({
    ({"ali","alias",0,0}),
    ({"alias","alias",0,0}),
    ({"unali","unalias",1,0}),
    ({"histmin","histmin",0,0}),
    ({"histlen","histlen",0,0}),
    ({"hist","show_hist",0,0}),
    ({"history","show_hist",0,0}),
    ({"do","do_list",0,LEARNER_LVL}),
    ({"ersetzungsanzeige","replacedisplay",0,0}),
    ({"syntaxsammlung","collect_cmds",0,0}),
    ({"fehlermeldung","set_errormessage",0,SEER_LVL}),
  })+l;
}

static int collect_cmds(string cmd)
{
  if (!stringp(cmd))
  {
    _notify("Mit diesem Befehl kannst Du mithelfen, Syntaxen im MG zu "
            "verbessern. Wenn Du einverstanden bist, speichern wir "
            "anonym (d.h. ohne Deinen Charnamen), welche Deiner Befehle "
            "erfolgreich und nicht erfolgreich waren. Uebliche "
            "Kommunikationsbefehle werden dabei nicht gespeichert.",
            0);
    _notify("Mit 'syntaxsammlung ja' kannst Du die Speicherung einschalten, "
            "mit 'syntaxsammlung nein' kannst Du sie ausschalten.",0);
    _notify("Deine Befehle werden zur Zeit"
            + (QueryProp("_syntaxdb") ? " " : " NICHT ")
            + "gespeichert.", 0);
  }
  else if (cmd == "ja")
  {
    SetProp("_syntaxdb", 1);
    _notify("Ab jetzt werden Deine Befehle gespeichert. Vielen Dank!", 0);
  }
  else
  {
    SetProp("_syntaxdb", 0);
    _notify("Ab jetzt werden Deine Befehle NICHT gespeichert.", 0);
  }
  return 1;
}

int _query_command_average()
{
  decay_average();
  return cmds_per_time;
}

nomask void __set_bb(int flag)  {
  if( previous_object()!=find_object(BBMASTER) || process_call() )
    return;
  bb=flag;
}


nomask public void countCmds( int type, string key )
{
    string tmp;

    if ( this_player() != this_interactive()
         || this_interactive() != this_object()
         || member( cmd_types, type ) < 0 )
        return;

    tmp = sprintf( "%d\n%s", type, key );

    commands -= ({ tmp });
    commands += ({ tmp });
    commands = commands[0..max_commands-1];
}


nomask public string *getCmds()
{
    string *tmp;

    if ( previous_object() != find_object(BBMASTER) )
        return ({});

    tmp = commands;
    commands = ({});

    return tmp;
}

/*
 * Force the monster to do a command. The force_us() function isn't
 * always good, because it checks the level of the caller, and this function
 * can be called by a room.
 */
int command_me(string cmd)
{
  if (IS_LEARNER(ME))
  {
    if (!this_interactive() || !previous_object())
      return 0;
    if( geteuid(ME)!=geteuid(this_interactive())
        || geteuid(ME)!=geteuid(previous_object()) )
    {
      if( query_wiz_level(ME)<query_wiz_level(previous_object()))
        tell_object(ME,previous_object()->name()+" zwingt Dich zu: "
                    + cmd + ".\n");
      else
      {
        tell_object(ME,previous_object()->name()
                    + " versucht, Dich zu " + cmd + " zu zwingen.\n" );
        return 0;
      }
    }
  }
  return command(cmd);
}


static mixed _query_p_lib_disablecommands() {
    // abgelaufen oder Objekt zerstoert? Weg damit.
    if (pointerp(disablecommands)
        && (disablecommands[B_TIME] < time()
          || !objectp(disablecommands[B_OBJECT])) )
        return(disablecommands = 0);

    // sonst Kopie zurueck (copy(0) geht)
    return(copy(disablecommands));
}

static mixed _set_p_lib_disablecommands(mixed data) {

  // setzendes Objekt ermitteln, da diese Funktion nur per SetProp() gerufen
  // werden sollte (!), ist das PO(1);
  object origin = previous_object(1);
  // wenn nicht existent, direkt abbruch
  if (!objectp(origin))
    return _query_p_lib_disablecommands();

  // Prop loeschen? Explizit loeschen darf jeder, allerdings nicht direkt
  // ungeprueft ueberschreiben.
  if (!data) {
      return (disablecommands = 0 );
  }
  // mal direkt buggen bei falschen Datentyp, damits auffaellt.
  if (!pointerp(data) || sizeof(data) < 2 || !intp(data[0])
      || (!stringp(data[1]) && !closurep(data[1]))
      || (sizeof(data) >= 3 && !pointerp(data[2])) )
      raise_error(sprintf(
            "Wrong data type for P_DISABLE_COMMANDS. Expected Array with "
            "2 or 3 elements (int, string|closure, [string*]), got %.25O\n",
            data));

  // Wenn abgelaufen oder gleiches Objekt wie letztes Mal: eintragen.
  if (!disablecommands || (disablecommands[B_TIME] < time()
        || !objectp(disablecommands[B_OBJECT]) 
        || disablecommands[B_OBJECT] == origin) ) {
      // Loggen nur, wenn eine Closure eingetragen wird. Reduziert den
      // Logscroll und Strings haben deutlich weniger Missbrauchspotential.
      if (closurep(data[1])) {
        CBLOG(sprintf("[%s] CB gesetzt von %O, gueltig bis %s, Daten: %O\n",
        strftime("%Y%m%d-%H:%M:%S"),origin,
        strftime("%Y%m%d-%H:%M:%S",data[0]),
        (stringp(data[1]) ? regreplace(data[1],"\n","\\n",0)
                          : data[1])));
      }
      if (sizeof(data)+1 <= B_EXCEPTIONS)
        disablecommands = ({ origin, data[0], data[1], ({}) });
      else
        disablecommands = ({ origin, data[0], data[1], data[2] });
      return(copy(disablecommands));
  }

  return(_query_p_lib_disablecommands());
}

static mixed _set__syntaxdb(mixed v)
{
  Set("_syntaxdb", v, F_VALUE);
  if (v)
      syntaxdb = ({find_object("/secure/syntaxdb")});
  else
      syntaxdb = 0;
  return QueryProp("_syntaxdb");
}


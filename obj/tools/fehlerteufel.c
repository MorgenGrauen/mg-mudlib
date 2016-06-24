/* /obj/toostels/fehlerteufel.c
   Fehlerteufel - Magiertool zur Abfrage des Fehler-Daemons der Mudlib
   Autor: Zesstra
   Changelog:
*/

#pragma strict_types
#pragma pedantic
#pragma range_check
#pragma no_shadow
#pragma no_inherit

inherit "/std/secure_thing";
inherit "/secure/errord-structs";

#include <defines.h>
#include <wizlevels.h>
#include <properties.h>
#include <moving.h>
#include <errord.h>
#include <config.h>
#include <debug_info.h>
#include <input_to.h>
#include <strings.h>

#define TI this_interactive()

#define DEBUG(x)  if (funcall(symbol_function('find_player),"zesstra"))\
        tell_object(funcall(symbol_function('find_player),"zesstra"),\
        "EDBG: "+x+"\n")

// variables
private string owner;  // UID vom Eigentuemer
private string *uids=({}); // UIDs, auf die man schreibrechte hat
private string *filteruids=({}); // wenn nicht-leer, nur diese UIDs zeigen
private string *monitoruids=({}); // diese UIDs in die Liste einschliessen
private int filterstatus=0;       // Filter an oder aus?
private mapping issuelist = ([]);
/* ([ type1: ([uid1: <issuelist>, uid2: <issuelist> ]),
      type2: ... ])
   <issuelist> ist ein < <int|string>* >* vom ErrorD */

//Welche Art von Fehler anzeigen?
private int modus=T_RTERROR | T_RTWARN | T_REPORTED_ERR;
private string *xmonitoruids = ({}); // expanded Monitor-UIDs
private int lfehler;              // letzter 'benutzter' Fehler.
private int fehlerzahl;       // fehlerzahl im letzten Reset.
private mapping feingabe;         // gerade eingegebener Fehler

// ************** private functions  **************
private varargs int get_issuelist(int lmodus);
private void get_uids();

// irgendeinen Fehler oder Warnung anzeigen.
private int show_entry(struct fullissue_s issue)
{
  if (!issue) return 0;

  string txt=ERRORD->format_error(issue, 0);

  if (!stringp(txt) || !sizeof(txt))
      return 0;

  tell_object(PL,txt+"\n");
  return 1;
}

protected int _feingabe_fertig(string arg) {

  if (arg == "j")
  {
    if (!feingabe[F_MSG])
    {
      tell_object(PL,
        "Also, eine Fehlerbeschreibung solltest Du schon eingeben! Abgebrochen.\n");
    }
    else
    {
      string hashkey = (string)ERRORD->LogReportedError(feingabe);
      tell_object(PL, sprintf(
         "Vielen Dank! Die ID des eingegebenen Fehlers lautet: %s\n",
         hashkey || "N/A"));
    }
  }
  else
  {
    tell_object(PL, "Fehlereingabe abgebrochen.\n");
  }

  feingabe = 0;

  return 1;
}

public int CmdFehlerEingabe(string arg) {
  object target;

  if (feingabe)
  {
    tell_object(PL, "Du gibts doch bereits einen Fehler ein!\n");
    return 1;
  }
  feingabe = ([]);

  if (arg)
  {
    target = present(arg);
    if (!target)
      target = find_object(arg); // vielleicht direkt angegeben?
  }
  // wenn nix gefunden, Environment des Magiers nehmen.
  if (!target)
    target = environment(environment());

  feingabe[F_OBJ] = target;

  tell_object(PL, break_string(sprintf(
          "Du beginnst einen Fehlereintrag fuer das Objekt: %O\n", target),78));

  input_to(function void (string str)
      {
        if (sizeof(str)) feingabe[F_MSG] = str;
      }, INPUT_PROMPT, "Fehlerbeschreibung eingeben:\n");

  input_to(function void (string str)
      {
        if (sizeof(str)) feingabe[F_PROG] = str;
        else feingabe[F_PROG] = "unbekannt";
      }, INPUT_PROMPT|INPUT_APPEND, 
     "Programm eingeben, in dem der Fehler auftritt (falls bekannt):\n");

  input_to(function void (string str)
      {
        if (sizeof(str)) feingabe[F_LINE] = to_int(str);
        tell_object(PL,break_string(sprintf(
          "Du hast die folgenden Daten eingegeben:\n"
          "Objekt: %O\n"
          "Fehlerbeschreibung: %s\n"
          "Programm: %s\n"
          "Zeilennr.: %d\n",
          feingabe[F_OBJ], feingabe[F_MSG] || "", feingabe[F_PROG],
          feingabe[F_LINE]), 78, "", BS_LEAVE_MY_LFS));
      }, INPUT_PROMPT|INPUT_APPEND,
      "Zeilennr. eingeben, in der der Fehler auftritt (falls bekannt):\n");

  input_to(#'_feingabe_fertig,
      INPUT_PROMPT|INPUT_APPEND, "Eingaben korrekt? (j/n)\n");

  return 1;
}

public int CmdFehlerZeigen(string arg)
{
  int issueid;

  if (stringp(arg) && sizeof(arg))
  {
      arg = trim(arg, TRIM_BOTH);
      issueid = to_int(arg);
  }
  else
  {
      issueid = lfehler;
      arg = to_string(issueid);
  }
  notify_fail("Einen Eintrag mit dieser ID gibt es nicht!\n");

  // Mit einem / am Anfang ist arg wohl ein Filename, wenn to_string(issueid)
  // == arg wird die Issueid von oben genommen.
  struct fullissue_s issue;
  struct fullissue_s *issues;
  if (arg[0] == '/')
  {
    issues=({});
    foreach(int m: ALL_ERR_TYPES)
    {
      if (!(m & modus))
        continue;
      struct fullissue_s *tmp = 
                       (struct fullissue_s *)ERRORD->QueryIssuesByFile(arg, m);
      if (tmp)
        issues+=tmp;
    }
    if (!sizeof(issues))
      issues=0;
  }
  else if (to_string(issueid) == arg)
    issue = (struct fullissue_s)ERRORD->QueryIssueByID(issueid);
  else
    issue = (struct fullissue_s)ERRORD->QueryIssueByHash(arg);

  if (structp(issue))
  {
    show_entry(issue);
    // letzten Fehler merken.
    lfehler = issueid;
    return 1;
  }
  // Wenn das nicht erfolgreich ist, ist das Argument evtl. ein Objekt-,
  // Programm- oder Ladename. In dem Fall alle von denen anzeigen, die passen
  // hierbei wird der Typ NICHT beruecksichtigt.
  else if (pointerp(issues))
  {
    foreach(issue : issues)
    {
      show_entry(issue);
    }
    return 1;
  }

  notify_fail("Keine Eintraege fuer diesen Dateinamen/diese ID gefunden.\n");
  return 0;
}

// Loescht alle Fehler und Warnungen eines Objekts (soweit per modus
// ausgewaehlt). Entscheidend ist der _Loadname_!
private int DeleteErrorsForLoadname(string loadname)
{
  int sum_deleted;
  // Bei == 0 wird sonst alles geloescht. ;-)
  if (!loadname)
    return 0;

  foreach(int m: ALL_ERR_TYPES)
  {
    if (!(m & modus))
      continue;
    < <int|string>* >* list = ERRORD->QueryIssueListByLoadname(loadname,m);
    if (pointerp(list))
    {
      foreach(<int|string>* row : list)
      {
          if ((int)ERRORD->ToggleDeleteError(row[0]) == 1)
          {
            tell_object(PL,
                row[0] + " als geloescht markiert.\n");
          }
      }
      sum_deleted+=sizeof(list);
    }
  }
  return sum_deleted;
}

public int CmdFehlerLoeschen(string arg)
{
  int issueid;
  arg = (string)this_player()->_unparsed_args(0);

  if (stringp(arg) && sizeof(arg))
      issueid = to_int(arg);
  else
      issueid = lfehler;

  notify_fail("Einen Eintrag mit dieser ID/diesem Loadname gibt es nicht!\n");

  int res = (int)ERRORD->ToggleDeleteError(issueid);
  if (res == 1)
  {
    tell_object(PL,
      "Fehler/Warnung wurde zum Loeschen markiert und wird in Kuerze "
      "geloescht.\n");
    lfehler = issueid;
    return 1;
  }
  else if (res==0)
  {
    tell_object(PL,"Loeschmarkierung wurde entfernt.\n");
    lfehler = issueid;
    return 1;
  }
  else if (res < -1)
  {
    tell_object(PL, "Irgendwas ist beim Loeschen schiefgegangen. "
                    "Keine Schreibrechte?\n");
    lfehler = issueid;
    return 1;
  }
  // res war == -1 -> Fehler nicht gefunden. Vielleicht ist es nen Loadname
  return DeleteErrorsForLoadname(arg);
}

public int CmdRefresh(string arg) {
    reset();
    tell_object(PL,"Fehlerdaten wurden neu eingelesen.\n");
    return 1;
}

private int select_modus(string arg) {
    int lmodus;
    switch(arg) {
    case "alles":
    case "alle":
      lmodus = T_RTERROR | T_RTWARN | T_CTERROR | T_CTWARN | T_REPORTED_ERR
               | T_REPORTED_IDEA | T_REPORTED_TYPO | T_REPORTED_MD;
      break;
    case "fehler":
    case "error":
    case "errors":
      lmodus=T_RTERROR | T_CTERROR | T_REPORTED_ERR;
      break;
    case "warnungen":
    case "warnung":
    case "warning":
    case "warnings":
      lmodus=T_RTWARN | T_CTWARN;
      break;
    case "laufzeitfehler":
      lmodus=T_RTERROR;
      break;
    case "ladezeitfehler":
      lmodus=T_CTERROR;
      break;
    case "fehlerhinweis":
    case "fehlerhinweise":
    case "hinweise":
      lmodus=T_REPORTED_ERR;
      break;
    case "ideen":
    case "idee":
      lmodus=T_REPORTED_IDEA;
      break;
    case "md":
      lmodus=T_REPORTED_MD;
      break;
    case "typo":
    case "typos":
      lmodus=T_REPORTED_TYPO;
      break;
    case "laufzeitwarnungen":
    case "runtimewarnings":
      lmodus=T_RTWARN;
      break;
    case "ladezeitwarnungen":
    case "compiletimewarnings":
      lmodus=T_CTWARN;
      break;
    default:
      lmodus=modus;
    }
    return lmodus;
}

private string  * errorlabel(int t)
{
  switch(t) {
    case T_RTERROR:
      return ({"Laufzeitfehler","Laufzeitfehler","Dieser Laufzeitfehler"});
    case T_REPORTED_ERR:
      return ({"Fehlerhinweis","Fehlerhinweise","Dieser Fehlerhinweis"});
    case T_REPORTED_IDEA:
      return ({"Idee","Ideen","Diese Idee"});
    case T_REPORTED_MD:
      return ({"fehlende Detail","fehlende Details","Dieses fehlende Detail"});
    case T_REPORTED_TYPO:
      return ({"Typo","Typos","Dieser Typo"});
    case T_RTWARN:
      return ({"Laufzeitwarnung","Laufzeitwarnungen","Diese Laufzeitwarnung"});
    case T_CTWARN:
      return ({"Ladezeitwarnung", "Ladezeitwarnungen","Diese Ladezeitwarnung"});
    case T_CTERROR:
      return ({"Ladezeitfehler","Ladezeitfehler","Dieser Ladezeitfehler"});
  }
  raise_error("Unkannter Fehlertyp: "+t+"\n");
  return 0;
}

public int CmdFehlerListe(string arg) {
  string txt;
  //string *luids;
  int lmodus; // modus fuer diese Liste
  mapping fehlerbackup;

  if (stringp(arg) && sizeof(arg))
  {
    lmodus=select_modus(arg);
    if (lmodus != modus)
    {
      fehlerbackup=issuelist; // Fehlerliste von 'modus' aufheben
      get_issuelist(lmodus);  // neue Fehlerliste holen
    }
  }
  else
    lmodus=modus;
/*
  if (!fehlerzahl)
  {
    txt="Fuer Deine UIDs sind keine Fehler/Warnungen bekannt. :-)\n";
    tell_object(PL,txt);
    return 1;
  }
*/
  foreach(int typ, mapping typemap : issuelist)
  {
    if (!(typ & lmodus))
      continue; // Type nicht gewaehlt.
    txt="";
    if (!sizeof(typemap)) {
      tell_object(PL,
        "Es sind keine " + errorlabel(typ)[1] + "Deiner UIDs bekannt. :-)");
      continue;
    }
    foreach(string uid, < <int|string>* >* list : typemap)
    {
      if (!sizeof(list)) continue;
      if (filterstatus && member(filteruids, uid) > -1) continue;
      //txt+=sprintf("%s:\n", uid);
      foreach(<int|string>* row : list)
      {
        txt+=sprintf("%:6d | %:40-s | %:26-s\n",
              row[0], row[1], row[2]);
      }
    }
    if (txt && sizeof(txt))
    {
      txt = sprintf("\nFuer Deine UIDs sind folgende %s bekannt (Filter: %s):\n"
                    "%:6|s | %:40|s |   %s\n",
                 errorlabel(typ)[1], (filterstatus ? "an" : "aus"),
                 "ID", "Loadname", "UID")
          + txt;
      tell_object(PL, txt);
    }
    else
    {
      tell_object(PL, sprintf(
            "\nFuer Deine UIDs sind keine %s bekannt (Filter: %s):\n",
            errorlabel(typ)[1], (filterstatus ? "an" : "aus")));
    }
  }

  if (mappingp(fehlerbackup) && modus!=lmodus)
    issuelist=fehlerbackup; // fehlerliste fuer 'modus' restaurieren
  return 1;
}

public int CmdFilter(string arg) {

  arg=(string)PL->_unparsed_args(0);  

  if (!stringp(arg) || !sizeof(arg)) {
    tell_object(PL,break_string(
          "Momentan interessieren Dich folgende UIDs nicht"
          +(filterstatus ? " (Filter an):\n" : " (Filter aus):\n")
          +CountUp(filteruids)+"\n", 78,"",BS_LEAVE_MY_LFS));
    return 1;
  }

  if (arg=="keine") {
    filteruids=({});
    filterstatus=1;
    tell_object(PL,break_string(
        "Dein Fehlerteufel wird Dir nun nur noch ausgewaehlte "
        "Fehler berichten. Momentan hast Du keine UIDs ausgeblendet. "
        "(Filter an)",78));
  }
  else if (arg=="alle") {
    filterstatus=1;
    filteruids=uids;
    tell_object(PL,break_string(
        "Dein Fehlerteufel wird Dir nun nur noch ausgewaehlte "
        "Fehler berichten. Du blendest momentan alle UIDs aus. "
        "(Filter an)",78));
  }
  else if (arg=="aus") {
    filterstatus=0;
    tell_object(PL,break_string(
        "Dein Fehlerteufel wird Dir nun wieder alle Fehler berichten. ",
        78));
  }
  else if (arg=="an" || arg=="ein") {
    filterstatus=1;
    tell_object(PL,break_string(
        "Dein Fehlerteufel wird Dir nun nur noch ausgewaehlte "
        "Fehler berichten.",78));
  }
  else {
    foreach(string uid: explode(arg," ")-({""})) {
      if (sizeof(uid)>1 && uid[0]=='+') {
        if (member(filteruids,uid[1..])==-1)
          filteruids+=({uid[1..]});
      }
      else if (sizeof(uid)>1 && uid[0]=='-') {
          filteruids-=({uid[1..]});
      }
      else {
        if (member(filteruids,uid)==-1)
          filteruids+=({uid});
        else
          filteruids-=({uid});
      }
    }
  }

  tell_object(PL,break_string(
          "Momentan interessieren Dich folgende UIDs nicht"
          +(filterstatus ? " (Filter an):\n" : " (Filter aus):\n")
          +CountUp(filteruids)+"\n", 78,"",BS_LEAVE_MY_LFS));

  return 1;
}

public int CmdMonitor(string arg) {

  arg=(string)PL->_unparsed_args(0);  

  if (!stringp(arg) || !sizeof(arg)) {
    tell_object(PL,break_string(
          "Momentan interessieren Dich folgende UIDs zusaetzlich zu Deinen: \n"
          +(sizeof(monitoruids) ? CountUp(monitoruids) : "")
          +"\n", 78,"",BS_LEAVE_MY_LFS));
    return 1;
  }

  if (arg=="keine") {
    monitoruids=({});
    xmonitoruids=({});
    tell_object(PL,break_string(
        "Dein Fehlerteufel wird Dir nun nur noch "
        "Fehler Deiner eigenen UIDs berichten.",78));
    return 1;
  }
  else {
    foreach(string uid: explode(arg," ")-({""})) {
      if (sizeof(uid)>1 && uid[0]=='+') {
        if (member(monitoruids,uid[1..])==-1)
          monitoruids+=({uid[1..]});
      }
      else if (sizeof(uid)>1 && uid[0]=='-') {
          monitoruids-=({uid[1..]});
      }
      else {
        if (member(monitoruids,uid)==-1)
          monitoruids+=({uid});
        else
          monitoruids-=({uid});
      }
    }
  }
  get_uids();
  tell_object(PL,break_string(
        "Momentan interessieren Dich folgende UIDs zusaetzlich zu Deinen: \n"
        +(sizeof(monitoruids) ? CountUp(monitoruids) : "")
        +"\n", 78,"",BS_LEAVE_MY_LFS));

  return 1;
}

public int CmdModus(string arg) {
  string txt;

  // Argument verwursten
  if (stringp(arg) && sizeof(arg)) {
    modus = select_modus(arg);
    reset();  // neue Fehlerliste holen
  }
  // aktuelle Einstellung ausgeben.
  string *modstr=({});
  if (modus & T_RTERROR)
      modstr+=({"Fehler (Laufzeit)"});
  if (modus & T_RTWARN)
      modstr+=({"Warnungen (Laufzeit)"});
  if (modus & T_CTERROR)
      modstr+=({"Fehler (Ladezeit)"});
  if (modus & T_CTWARN)
      modstr+=({"Warnungen (Ladezeit)"});
  if (modus & T_REPORTED_ERR)
      modstr+=({"Fehlerhinweise"});
  if (modus & T_REPORTED_IDEA)
      modstr+=({"Idee"});
  if (modus & T_REPORTED_MD)
      modstr+=({"fehlende Details"});
  if (modus & T_REPORTED_TYPO)
      modstr+=({"Typo"});

  tell_object(PL, break_string(
      "Dein Fehlerteufel wird Dir nun ueber aufgetretene "
      +CountUp(modstr)+" Bericht erstatten.",78));
  return(1);
}

int CmdAddNote(string str) {
  string *arr;

  notify_fail("Bitte eine ID und einen Text angeben!\n");
  if(!objectp(TI))
    return 0;

  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return 0;

  arr=explode(str," ")-({""});
  if (sizeof(arr)<2)
    return 0;
  int issueid = to_int(arr[0]);

  str=implode(arr[1..]," ");  //text wiederherstellen, aber ohne ID

  switch((int)ERRORD->AddNote(issueid,str))
  {
    case -1:
      tell_object(PL,
          sprintf("Es gibt keinen Fehler mit der ID: %d\n",issueid));
      return 1;
    case -3:
      return 0; //offenbar keine Notiz angegeben.
  }
  // letzten Fehler merken.
  lfehler = issueid;

  tell_object(PL,
      sprintf("Deine Notiz wurde zu %d hinzugefuegt.\n",
        issueid));
  return 1;
}

int CmdFix(string str)
{
  string *arr;
  int fixing, res;

  notify_fail("Bitte eine ID und optional eine Notiz angeben!\n");
  if(!objectp(TI))
    return 0;

  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return 0;

  arr=explode(str," ")-({""});
  if (!sizeof(arr))
    return 0;

  int issueid=to_int(arr[0]);
  if (sizeof(arr)>1)
    str=implode(arr[1..]," ");  //text wiederherstellen, aber ohne Key
  else str=0;

  if (query_verb()=="ffix" || query_verb()=="fehlerfix")
  {
      fixing=1;
      res = (int)ERRORD->ResolveIssue(issueid, str);
  }
  else
  {
      res = (int)ERRORD->ReOpenIssue(issueid, str);
  }

  if (res==-1)
  {
      tell_object(PL,
          sprintf("Es gibt keinen Fehler mit der ID: %d\n",issueid));
  }
  else if (res==-10)
  {
      tell_object(PL,
        "Du hast leider keinen Schreibzugriff diesen Fehler.\n"
        "Aber vielleicht moechtest Du mit fnotiz eine Notiz anhaengen?\n");
  }
  else if (res==-3)
  {
    if (fixing)
      tell_object(PL,"Dieser Fehler ist bereits gefixt.\n");
    else
      tell_object(PL,"Dieser Fehler ist noch nicht gefixt.\n");
  }
  else if (res==1)
  {
      tell_object(PL,
          sprintf("Fehler %d als gefixt markiert.\n",issueid));
  }
  else if (res==0)
  {
      tell_object(PL,
          sprintf("Fehler %d als nicht gefixt markiert.\n",issueid));
  }
  // letzten Fehler merken.
  lfehler = issueid;

  return 1;
}

int CmdLock(string str) {
  string *arr;
  int locking;
  int res;

  notify_fail("Bitte eine ID und optional eine Notiz angeben!\n");
  if(!objectp(TI))
    return 0;

  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return 0;

  arr=explode(str," ")-({""});
  if (!sizeof(arr))
    return 0;

  int issueid=to_int(arr[0]);
  if (sizeof(arr)>1)
    str=implode(arr[1..]," ");  //text wiederherstellen, aber ohne Key
  else str=0;

  if (query_verb()=="flock" || query_verb()=="fehlerlock")
  {
      locking=1;
      res=(int)ERRORD->LockIssue(issueid,str);
  }
  else
  {
      res=(int)ERRORD->UnlockIssue(issueid,str);
  }

  if (res==-1)
  {
      tell_object(PL,
          sprintf("Es gibt keinen Fehler mit der ID: %d\n",issueid));
  }
  else if (res==-10)
  {
       tell_object(PL,
           "Du hast leider keinen Schreibzugriff diesen Fehler.\n");
  }
  else if (res==-3)
  {
      if (locking)
          tell_object(PL,
              "Dieser Fehler ist bereits vor autom. Loeschen geschuetzt.\n");
      else
          tell_object(PL,
              "Dieser Fehler ist bereits zum autom. Loeschen freigegeben.\n");
  }
  else if (res==-2)
  {
       tell_object(PL,
           "Dieser Fehler ist bereits gefixt und wird bald geloescht.\n");
  }
  else if (res==1)
  {
    tell_object(PL,
        sprintf("Fehler %d vor autom. Loeschen geschuetzt.\n",issueid));
  }
  else if (res==0)
  {
    tell_object(PL,
        sprintf("Fehler %d zum autom. Loeschen freigegeben.\n",issueid));
  }
  // letzten Fehler merken.
  lfehler = issueid;

  return 1;
}

int CmdReassign(string str) {

  notify_fail("Bitte eine ID, die neue UID und ggf. eine Notiz angeben!\n");
  if(!objectp(TI))
    return 0;

  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return 0;

  string *arr=explode(str," ")-({""});
  if (sizeof(arr)<2)
    return 0;
  int issueid=to_int(arr[0]);
  string newuid=arr[1];

  //text wiederherstellen, aber ohne Key und UID
  if (sizeof(arr) > 2)
    str = implode(arr[2..]," ");
  else
    str = 0;

  switch((int)ERRORD->ReassignIssue(issueid, newuid, str))
  {
    case -1:
      tell_object(PL,
          sprintf("Es gibt keinen Fehler mit der ID: %d\n",issueid));
      return(1);
    case -10:
      tell_object(PL,
          sprintf("Du hast keine Schreibrechte auf Fehler %d\n",issueid));
      return 1;
    case -2:
      return(0); //offenbar keine neue uid angegeben. (kann nicht)
    case -3:
      tell_object(PL,break_string(
          "Alte == Neue UID ist irgendwie unsinnig...",78));
      return 1;
  }
  // letzten Fehler merken.
  lfehler = issueid;

  tell_object(PL,break_string(
      sprintf("Der Fehler der ID %d wurde an die UID %s "
        "uebertragen.\n", issueid, newuid),78));
  return 1;
}

// ************** public 'internal' functions **************
public string QueryOwner() {return owner;}
public mixed QueryIssueList() {return issuelist;}

void create() {
    if (!clonep(ME))
        return;
    ::create(); 

    SetProp(P_SHORT,"Der Fehlerteufel");
    SetProp(P_LONG,break_string(
          "Dein Fehlerteufel soll Dir helfen, Informationen "
          "ueber aufgetretene Fehler zu erhalten. Hierzu fragt er die "
          "in \"Deinen\" UIDs aufgetretenen Fehler und deren Details vom "
          "Fehlerspeicher der Mudlib ab. Folgende Kommandos kennt er:",78)
    +"fehlerabfrage <id>  - Fragt Details ueber Fehler mit der ID ab.\n"
    "fehlerloeschen <id> - Fehler zum Loeschen markieren.\n"
    "fehlerliste         - Fehlerliste der eigenen UIDs anzeigen\n"
    "fehlerrefresh       - Fehlerdaten und UIDs neu einlesen\n"
    "fehlerfilter        - UIDs fuer den Filter angeben (s. manpage!)\n"
    "fehlermodus         - Fehler oder Warnungen ausgeben? (s. manpage)\n"
    "fehlermonitor       - zus. UIDs beobachten (s. manpage)\n"
    "fnotiz <id> <note>  - eine Notiz anhaengen\n"
    "flock <id>  <note>  - Fehler vor autom. Loeschen schuetzen\n"
    "funlock <id> <note> - Fehler zum autom. Loeschen freigeben\n"
    "ffix <id> <note>    - Fehler als gefixt kennzeichnen\n"
    "funfix <id> <note>  - gefixten Fehler als nicht-gefixt markieren\n"
    "fuebertrage <id> <newuid> <note>\n"
    "                    - Fehler an die UID uebertragen\n"
    );
    SetProp(P_NAME,"Fehlerteufel");
    SetProp(P_GENDER,MALE);
    SetProp(P_WEIGHT,0);
    SetProp(P_VALUE,0);
    SetProp(P_SIZE,10);
    SetProp(P_NODROP,"Den Fehlerteufel behaelst Du lieber bei Dir.\n");
    SetProp(P_NEVERDROP,1);

    AddId( ({"fehlerteufel","teufel"}) );

    AddCmd(({"fehlerabfrage","fabfrage"}), "CmdFehlerZeigen" );
    AddCmd(({"fehlerloeschen","floeschen"}), "CmdFehlerLoeschen");
    AddCmd(({"fehlerliste","fliste", "fehleruebersicht","fuebersicht"}), 
        "CmdFehlerListe");
    AddCmd(({"fehlerrefresh","frefresh"}),"CmdRefresh");
    AddCmd(({"fehlerfilter","ffilter"}),"CmdFilter");
    AddCmd(({"fehlermodus","fmodus"}),"CmdModus");
    AddCmd(({"fehlermonitor","fmonitor"}),"CmdMonitor");
    AddCmd(({"fehlernotiz","fnotiz"}),"CmdAddNote");
    AddCmd(({"fehlerlock","flock","fehlerunlock","funlock"}),
        "CmdLock");
    AddCmd(({"fehlerfix","ffix","fehlerunfix","funfix"}),
        "CmdFix");
    AddCmd(({"fehleruebertrage","fuebertrage"}),"CmdReassign");
    AddCmd(({"fehlereingabe", "feingabe"}), "CmdFehlerEingabe");
}

void init()
{
    if (find_call_out("remove") != -1) return;

    // pruefung auf env nicht noetig, move geht nur in ein env und ohne env
    // auch kein init().
    if ( !query_once_interactive(environment()) || 
        !IS_LEARNER(environment())) {
        // in interactive, aber kein magier -> direkt weg.
        call_out("remove",0,1);
        return;
    }
    else if (!sizeof(owner))
        // Env ist Interactiv und Magier (sonst waer man oben rausgeflogen)
        owner=getuid(environment());
    else if (owner!=getuid(environment())) {
       //ok, nicht der Eigentuemer, direkt weg.
       call_out("remove",0);
       return;
   }
   SetProp(P_EXTRA_LOOK,break_string(
         "Auf "+environment()->Name(WESSEN)+" Schulter sitzt ein kleiner "
         "Fehlerteufel, der "
         +environment()->QueryPronoun(WEM)
         +" immer wieder etwas ins Ohr fluestert.",78));

   call_out("reset",1);

   ::init();
}

public mixed Configure(mixed data)
{
  if (!data)
  {
    return (["filteruids":filteruids,
             "filterstatus":filterstatus,
             "modus":modus,
             "monitoruids":monitoruids,
             "fehlerzahl": fehlerzahl]);
  }
  else if (mappingp(data))
  {
    if (member(data,"filteruids") && pointerp(data["filteruids"]))
      filteruids=data["filteruids"];
    if (member(data,"filterstatus") && intp(data["filterstatus"]))
      filterstatus=data["filterstatus"];
    if (member(data,"modus") && intp(data["modus"]))
      modus=data["modus"];
    if (member(data,"monitoruids") && pointerp(data["monitoruids"]))
      monitoruids=data["monitoruids"];
    if (member(data,"fehlerzahl") && intp(data["fehlerzahl"]))
      fehlerzahl=data["fehlerzahl"];
    return 1;
  }
  return 0;
}

mapping _query_autoloadobj()
{
  return Configure(0);
}

mapping _set_autoloadobj(mixed data)
{
  Configure(data);
  return _query_autoloadobj();
}


void reset()
{
    get_uids();
    int neuefehlerzahl=get_issuelist();

    if (fehlerzahl < neuefehlerzahl)
      tell_object(environment(ME), break_string(
        "Deine Fehlerliste ist soeben laenger geworden.",78,
        "Dein Fehlerteufel teilt Dir mit: "));
    else if (fehlerzahl > neuefehlerzahl)
      tell_object(environment(ME), break_string(
        "Deine Fehlerliste ist soeben kuerzer geworden.",78,
        "Dein Fehlerteufel teilt Dir mit: "));
    fehlerzahl = neuefehlerzahl;
}

// ******** private functions *********************
private void get_uids()
{
    uids=(string *)master()->QueryUIDsForWizard(owner);
    xmonitoruids=({});
    if (sizeof(monitoruids))
    {
      closure cl=symbol_function("QueryUIDAlias", master());
      foreach(string uid: monitoruids) {
        xmonitoruids += (string*)funcall(cl, uid);
      }
    }
}

/* Holt sich aus dem ErrorD die Liste ungeloeschter und ungefixter Fehler fuer
 * fuer die UIDs des Magier fuer alle Typen
 */
private varargs int get_issuelist(int lmodus)
{
    int count;

    if (!lmodus)
      lmodus=modus;

    issuelist=m_allocate(sizeof(ALL_ERR_TYPES),1);

    foreach(int type: ALL_ERR_TYPES)
    {
      if (type & lmodus)
      {
          //DEBUG(sprintf("Type: %d\n",type));
          foreach(string uid : uids + xmonitoruids)
          {
            //DEBUG(sprintf("Type: %d, UID: %s\n",type,uid));
            < <int|string>* >* list =
                  (< <int|string>* >*)ERRORD->QueryIssueList(type,uid);
            count += sizeof(list);

            if (!member(issuelist, type))
              issuelist += ([ type: ([ uid: list ]) ]);
            else if (!member(issuelist[type], uid))
              issuelist[type] += ([uid: list ]);
        }
      }
    }
    return count;
}


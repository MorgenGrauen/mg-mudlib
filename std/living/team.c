// MorgenGrauen MUDlib
//
// living/team.c
//
// $Id: team.c 9138 2015-02-03 21:46:56Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include <properties.h>
#include <thing/properties.h>
#include <living/combat.h>
#include <combat.h>
#include <living/team.h>
#include <wizlevels.h>
#include <hook.h>

#define ME this_object()
#define TP this_player()
#define PO previous_object()
#define ENV environment()

private nosave string team_attack_cmd;
private nosave mapping team_follow_todo;
private nosave int team_autofollow;
private nosave object teammove;

void create() {
  Set(P_TEAM_ATTACK_CMD,-1,F_SET_METHOD);
  Set(P_TEAM_ATTACK_CMD,PROTECTED,F_MODE_AS);
  Set(P_TEAM_AUTOFOLLOW,-1,F_SET_METHOD);
  Set(P_TEAM_AUTOFOLLOW,PROTECTED,F_MODE_AS);
  teammove=0;
  offerHook(H_HOOK_TEAMROWCHANGE, 1);
}

void add_team_commands() {
  add_action("teamcmd","gruppe");
  add_action("teamcmd","g");
  add_action("teamcmd","team");
}

string _query_team_attack_cmd() {
  return Set(P_TEAM_ATTACK_CMD,team_attack_cmd);
}

int _query_team_autofollow() {
  return Set(P_TEAM_AUTOFOLLOW,team_autofollow);
}

private int team_help() {
  // Syntax-Kompatiblitaet (Avalon) ist ganz nett :-)
  write("\
(Befehle des Teamleiters sind mit * gekennzeichnet\n\
\n\
* team angriff\n\
  team angriffsbefehl <befehl>\n\
* team aufnahme <name>\n\
  team autof[olge] <ein/aus>\n\
* team autoi[nfo] <ein/aus> [+[lp]] [+[kp]] [sofort]\n\
* team entlasse <name>\n\
  team farben lp_rot lp_gelb kp_rot kp_gelb\n\
  team flucht[reihe] <reihe>\n\
  team folge <name>\n\
* team formation <min[-max]> [<min[-max]> ...]\n\
  team hilfe|?\n\
  team [info] [sortiert|alphabetisch]\n\
  team [kampf]reihe <reihe>\n\
* team leiter[in] <name>\n\
  team liste\n\
* team name <gruppenname>\n\
  team orte [alle]\n\
  team ruf[e]\n\
  team uebersicht\n\
  team verlasse\n");
  return 1;
}

object IsTeamLeader() {
  object team;

  if (!objectp(team=Query(P_TEAM))
      || team!=Query(P_TEAM_LEADER)
      || team->Leader()!=ME)
    return 0;
  return team;
}

object *TeamMembers() {
  object team;

  if (!objectp(team=Query(P_TEAM)))
    return ({ME});
  return team->Members();
}

string TeamPrefix() {
  object team;

  if (!objectp(team=Query(P_TEAM)))
    return "";
  return "["+team->Name()+"] ";
}


private int team_aufnahmewunsch(string arg) {
  object pl;

  if ((!objectp(pl=find_player(arg)) && !objectp(pl=present(arg,ENV)))
      || pl->QueryProp(P_INVIS) || environment(pl)!=ENV)
    return notify_fail(capitalize(arg)+" nicht gefunden.\n"),0;
  if (!living(pl))
    return notify_fail(pl->Name(WER)+" ist etwas zu inaktiv.\n"),0;
  if (pl==ME)
    return notify_fail("Du bist eine Person zu wenig fuer ein Team.\n"),0;
  SetProp(P_TEAM_NEWMEMBER,pl);
  if (pl->IsTeamLeader()) {
    write("Du bittest "+pl->name(WEN)+" um Aufnahme ins Team.\n");
    tell_object(pl,TP->Name(WER)+" bittet Dich um Aufnahme ins Team.\n");
  } else {
    write("Du bittest "+pl->name(WEN)+" um Gruendung eines Teams.\n");
    tell_object(pl,TP->Name(WER)+" bittet Dich um Gruendung eines Teams.\n");
  }
  return 1;
}

private int team_aufnahme(string arg) {
  object pl,team;
  int res;

  if ((!objectp(pl=find_player(arg)) && !objectp(pl=present(arg,ENV)))
      || pl->QueryProp(P_INVIS) || environment(pl)!=ENV)
    return notify_fail(capitalize(arg)+" nicht gefunden.\n"),0;
  if (pl->QueryProp(P_TEAM_NEWMEMBER)!=ME)
    return notify_fail(pl->Name(WER)+" hat Dich nicht um Aufnahme gebeten.\n"),
      0;
  if (pl==ME)
    return notify_fail("Du bist eine Person zu wenig fuer ein Team.\n"),0;
  if (!objectp(team=QueryProp(P_TEAM)))
    team=clone_object(TEAM_OBJECT);
  res=team->AddMember(pl);
  if (!sizeof(team->Members()))
    team->remove();
  return res;
}

object IsTeamMove() {
  if (!objectp(teammove) || (teammove!=Query(P_TEAM)))
    teammove=0;
  return teammove;
}

static void DoTeamAttack(object env, object callbackto) {
  if (env==ENV && stringp(team_attack_cmd) && !IS_LEARNER(ME)
      && (interactive(ME) || !query_once_interactive(ME))
      && objectp(callbackto) && callbackto==Query(P_TEAM)) {
    teammove=callbackto;
    command(team_attack_cmd);
  }
  if (objectp(callbackto))
    callbackto->TeamAttackExecuted_Callback(teammove?1:0);
  teammove=0;
}

int CallTeamAttack(object env) {
  if (stringp(team_attack_cmd)
      && find_call_out("DoTeamAttack")<0
      && PO
      && PO==Query(P_TEAM))
    return call_out("DoTeamAttack",0,env,PO),1;
  return 0;
}

static int DoTeamFollow() {
  string cmd;

  if (!team_autofollow
      || (!interactive(ME) && query_once_interactive(ME))
      || IS_LEARNER(ME)
      || !mappingp(team_follow_todo))
    return 0;
  if (!stringp(cmd=team_follow_todo[ENV]))
    return team_follow_todo=0;

  do {
    m_delete(team_follow_todo,ENV);
    tell_object(ME,sprintf("Du folgst Deinem Team mit \"%s\".\n",cmd));
    command(cmd);
  } while (get_eval_cost()>900000 && random(1000)>20 && objectp(ME)
           && stringp(cmd=team_follow_todo[ENV]));

  // Ist Spieler in Umgebung gelandet, fuer die noch ein
  // Befehl auszufuehren ist?
  if (!objectp(ME) || !stringp(team_follow_todo[ENV]))
    return team_follow_todo=0;
  while  (remove_call_out("DoTeamFollow")!=-1) ;
  call_out("DoTeamFollow",0);
  return 0;
}

int CallTeamFollow(object env, string cmd) {
  if (!team_autofollow
      || PO!=Query(P_TEAM)
      || !PO
      || !objectp(env)
      || !stringp(cmd))
    return 0;
  if (!mappingp(team_follow_todo))
    team_follow_todo=([]);
  if (ENV!=env && !team_follow_todo[ENV])
    return 0;
  team_follow_todo[env]=cmd;
  if (find_call_out("DoTeamFollow")<0)
    call_out("DoTeamFollow",0);
  return 1;
}

int ClearTeamFollow() {
  if (PO!=Query(P_TEAM) || !PO)
    return 0;
  team_follow_todo=([]);
  return 1;
}

mixed *PresentTeamRows() {
  object team;
  mixed *res;
  int i;

  if (!objectp(team=Query(P_TEAM))) {
    res=EMPTY_TEAMARRAY;
    res[0]=({ME});
    return res;
  }
  res=team->PresentRows(ENV);
  for (i=0;i<MAX_TEAMROWS;i++)
    if (member(res[i],ME)>=0)
      return res;
  res[0]+=({ME});
  return res;
}

varargs mixed *PresentEnemyRows(object *here) {
  mixed *res,*rows;
  mapping added_teams;
  int i,j;
  object ob,team;

  added_teams=([Query(P_TEAM):1]); // Nicht auf eigenes Team hauen
  res=EMPTY_TEAMARRAY;
  if (!pointerp(here))
    here=PresentEnemies();
  for (i=sizeof(here)-1;i>=0;i--) {
    if (!objectp(ob=here[i]))
      continue;
    if (!objectp(team=ob->QueryProp(P_TEAM))) {
      res[0]+=({ob});
      continue;
    }
    if (added_teams[team])
      continue;
    added_teams[team]=1;
    rows=team->PresentRows(ENV);
    for (j=0;j<MAX_TEAMROWS;j++)
      res[j]+=rows[j];
  }
  return res;
}

varargs object SelectNearEnemy(object *here, int forcefrom) {
  object ob,en,team;
  mixed *rows;
  int *prob,prot,i,r,sz,upsz,sum;

  if (!pointerp(here))
    here=PresentEnemies();
  if (!objectp(ob=SelectEnemy(here)))
    return 0;
  en=ob->QueryProp(P_TEAM);          // Feindliches Team
  if (objectp(team=Query(P_TEAM))) { // Eigenes Team
    if (en==team) // Feind im eigenen Team, kein ANDERES Mitglied waehlen.
      return ob;  // Aber auch ausserhalb Reihe 1 draufhauen
    rows=team->PresentRows(ENV);
    if (member(rows[0],ME)<0) // Stehe ich in der ersten Reihe?
      return 0; // Falls nein ist auch kein Gegner nahe.
  }
  if (!objectp(en))
    return ob; // Ist nicht in einem Team, also drauf.
  rows=en->PresentRows(environment(ob));
  prob=({1,0,0,0,0});
  prot=sum=0;
  for (i=0;i<MAX_TEAMROWS;i++) {
    if (prot>0) prot--;                // Schutzkegel nimmt ab.
    if (!sz=sizeof(rows[i])) continue; // Gegner in dieser Reihe
    upsz=sz-prot;if (upsz<0) continue; // Anzahl ungeschuetzter Gegner
    prob[i]+=(upsz+sum);               // Wahrscheinlichkeit += ungeschuetzt
    sum=prob[i];                       // Summe bisheriger Wahrscheinlichkeiten
    if (sz>prot) prot=sz;              // Neuer Schutzkegel
  }
  r=random(sum);
  for (i=0;i<MAX_TEAMROWS;i++)
    if (r<prob[i])
      break;
  if (i>=MAX_TEAMROWS)
    i=0;
  if (objectp(en=SelectEnemy(forcefrom?(here&rows[i]):rows[i])))
    return en;
  if (i && objectp(en=SelectEnemy(forcefrom?(here&rows[0]):rows[0])))
    return en;
  return ob;
}

varargs object SelectFarEnemy(object *here, int min, int max, int forcefrom) {
  mixed *rows;
  int *prob,i,r,sum;
  object en;

  if (max<0 || min>=MAX_TEAMROWS || max<min)
    return 0;
  if (min<0) min=0;
  if (max>=MAX_TEAMROWS) max=MAX_TEAMROWS-1;
  if (!pointerp(here))
    here=PresentEnemies();
  rows=PresentEnemyRows(here);
  prob=({0,0,0,0,0});
  sum=0;
  for (i=min;i<=max;i++)
    sum=prob[i]=sum+sizeof(rows[i])+max-i;

  r=random(sum);
  for (i=min;i<=max;i++)
    if (r<prob[i])
      break;
  if (i>max)
    i=min;
  if (objectp(en=SelectEnemy(forcefrom?(here&rows[i]):rows[i])))
    return en;
  for (i=min;i<=max;i++)
    if (objectp(en=SelectEnemy(forcefrom?(here&rows[i]):rows[i])))
      return en;
  return 0;
}

mixed _query_friend() {
  mixed res;

  if (res=Query(P_FRIEND))
    return res;
  if (objectp(res=Query(P_TEAM_ASSOC_MEMBERS))
      && query_once_interactive(res))
    return res;
  return 0;
}

int DeAssocMember(object npc) {
  mixed obs;
  object team;

  if (extern_call() && PO!=npc &&
      member(({"gilden","spellbooks"}),
             explode(object_name(PO),"/")[1])<0)
    return 0;
  obs=QueryProp(P_TEAM_ASSOC_MEMBERS);
  if (!pointerp(obs))
    return 0;
  obs-=({npc,0});
  SetProp(P_TEAM_ASSOC_MEMBERS,obs);
  if (objectp(team=QueryProp(P_TEAM)))
    team->RemoveAssocMember(ME,npc);
  return 1;
}

int AssocMember(object npc) {
  mixed obs;
  object team;

  if (extern_call() && PO!=npc &&
      member(({"gilden","spellbooks"}),
             explode(object_name(PO),"/")[1])<0)
    return 0;
  if (!objectp(npc)
      || npc->QueryProp(P_TEAM_ASSOC_MEMBERS)
      || IsEnemy(npc)
      || npc==ME
      || query_once_interactive(npc))
    return 0;
  obs=QueryProp(P_TEAM_ASSOC_MEMBERS);
  if (objectp(obs))
    return 0;
  if (!pointerp(obs))
    obs=({});
  obs=(obs-({npc,0}))+({npc});
  SetProp(P_TEAM_ASSOC_MEMBERS,obs);
  npc->SetProp(P_TEAM_ASSOC_MEMBERS,ME);
  if (objectp(team=QueryProp(P_TEAM)))
    team->AddAssocMember(ME,npc);
  return 1;
}

varargs void InsertEnemyTeam(mixed ens, int rek) {
  object *obs,ob,eteam,team;
  int i;

  team=Query(P_TEAM);
  // Alle Teammitglieder des Gegners sind Feind:
  if (objectp(ens)) {
    if (objectp(eteam=ens->QueryProp(P_TEAM))) {
      if (eteam==team) // feindliches Team = eigenes Team?
        return;        // also nicht alle Teammitglieder gegeneinander hetzen
      ens=eteam->Members();
    } else {
      ens=({ens});
    }
  }
  if (!pointerp(ens))
    return;
  ens-=({ME});

  // Interactives sollen keine Interactives durch Team angreifen:
  if (query_once_interactive(ME)) {
    for (i=sizeof(ens)-1;i>=0;i--)
      if (objectp(ob=ens[i]) && environment(ob)==environment()
          && !query_once_interactive(ob))
        InsertSingleEnemy(ob);
  } else {
    for (i=sizeof(ens)-1;i>=0;i--)
      if (objectp(ob=ens[i]) && environment(ob)==environment())
        InsertSingleEnemy(ob);
  }

  // Alle anderen Teammitglieder Informieren:
  if (rek || !objectp(team) || !pointerp(obs=team->Members()))
    return;
  obs-=({ME});
  obs-=ens;
  for (i=sizeof(obs)-1;i>=0;i--)
    if (objectp(ob=obs[i]))
      ob->InsertEnemyTeam(ens,1);
}

int TeamFlee() {
  object team;

  if (Query(P_TEAM_WIMPY_ROW)<2 || !objectp(team=Query(P_TEAM)))
    return 0;
  if (!team->FleeToRow(ME))
     return 0;
  if (Query(P_TEAM_LEADER)==team) {
    if (team_autofollow)
      tell_object(ME,"Du versuchst zu fliehen, "+
                  "Dein Team folgt Dir nicht mehr.\n");
    team_autofollow=0;
  }
  return 1;
}

varargs mapping PresentTeamPositions(mixed pres_rows) {
  mapping res;
  int i,j;
  object *obs,ob;

  res=([]);
  if (!pointerp(pres_rows))
    pres_rows=PresentTeamRows();
  for (i=0;i<MAX_TEAMROWS;i++) {
    obs=pres_rows[i];
    for (j=sizeof(obs)-1;j>=0;j--)
      if (objectp(ob=obs[j]) && !res[ob])
        res[ob]=i+1;
  }
  return res;
}

varargs int PresentPosition(mixed pmap) {
  object team;
  int i;

  if (!objectp(team=Query(P_TEAM)))
    return 1;
  if (mappingp(pmap))
    return pmap[ME];
  if (!pointerp(pmap))
    pmap=team->PresentRows(ENV);
  for (i=1;i<MAX_TEAMROWS;i++)
    if (member(pmap[i],ME)>=0)
      return i+1;
  return 1;
}

#define FILLSTRING "                                        "
varargs private string center_string(string str, int w) {
  return (FILLSTRING[0..((w-sizeof(str))/2-1)]+str+FILLSTRING)[0..(w-1)];
}

private int ShowTeamRows() {
  int i,j,sz;
  mixed *pres_rows;
  object *obs,ob;
  string str;

  pres_rows=PresentEnemyRows();
  for (sz=MAX_TEAMROWS-1;sz>=0;sz--)
    if (sizeof(pres_rows[sz]))
      break;
  for (i=sz;i>=0;i--) {
    obs=pres_rows[i];str="";
    for (j=sizeof(obs)-1;j>=0;j--)
      if (objectp(ob=obs[j])) {
        if (str!="") str+=" / ";
        str+=ob->Name(RAW);
      }
    printf("%d. %s\n",i+1,center_string(str,75));
  }
  if (sz>=0)
    write("   ---------------------------------------------------------------------------\n");
  pres_rows=PresentTeamRows();
  for (sz=MAX_TEAMROWS-1;sz>0;sz--)
    if (sizeof(pres_rows[sz]))
      break;
  for (i=0;i<=sz;i++) {
    obs=pres_rows[i];str="";
    for (j=sizeof(obs)-1;j>=0;j--)
      if (objectp(ob=obs[j])) {
        if (str!="") str+=" / ";
        str+=ob->Name(RAW);
      }
    printf("%d. %s\n",i+1,center_string(str,75));
  }
  return 1;
}

varargs int team_list(string arg) {
  object *tobs,*obs,tob,ob,ld;
  string *nms,*tnms,str;
  int i,j;

  if (!pointerp(tobs=TEAM_MASTER->ListTeamObjects())) return 0;
  if (arg!="alle") arg=0;
  tnms=({});
  for (i=sizeof(tobs)-1;i>=0;i--) {
    if (!objectp(tob=tobs[i])
        || !objectp(ld=tob->Leader())
        || (!query_once_interactive(ld) && !arg)
        || !pointerp(obs=tob->Members()))
      continue;
    nms=({});
    for (j=sizeof(obs)-1;j>=0;j--) {
      if (!objectp(ob=obs[j])
          || (!query_once_interactive(ob) &&!arg))
        continue;
      if (!stringp(str=ob->Name(WER))) str="?";
      if (ob==ld) str+="(*)";
      nms+=({str});
      nms=sort_array(nms,#'>);
    }
    if (!stringp(str=tob->Name())) str="Team ?";
    str+=": ";
    tnms+=({break_string(implode(nms,", "),78,str)});
    tnms=sort_array(tnms,#'<);
  }
  if (sizeof(tnms))
    tell_object(ME, sprintf("%@s\n", tnms));
  else
    tell_object(ME, "Keine Teams gefunden.\n"); 
  
  return 1;
}

varargs int teamcmd(string arg) {
  string *words,narg;
  object team;

  if (!arg)
    arg="";
  if (!stringp(narg=TP->_unparsed_args()))
    narg = arg;
  if (!sizeof(words=explode(arg," ")))
    return 0;
 
  if (sizeof(words) > 1) {
      arg=implode(words[1..]," ");
      narg = implode(explode(narg, " ")[1..], " ");
  }
  else
      arg = narg = "";

  switch(words[0]) { // Befehle die keine Mitgliedschaft erfordern:
  case "aufnahme":
    return team_aufnahme(arg);
  case "folge":
    return team_aufnahmewunsch(arg);
  case "?":
  case "hilfe":
    return team_help();
  case "liste":
    return team_list(arg);  
  case "uebersicht":
    return ShowTeamRows();
  default:;
  }

  if (!objectp(team=QueryProp(P_TEAM)))
    return notify_fail("Du bist in keinem Team.\n"),0;

  switch(words[0]) {
  case "angriffsbefehl":
    if (narg=="") narg=0;
    team_attack_cmd=narg;
    if (stringp(narg))
      write("Du beginnst den Kampf mit \""+narg+"\"\n");
    else
      write("Du hast den Teamangriffsbefehl deaktiviert.\n");
    break; // NICHT return!
  case "autofolge":
  case "autof":
    if (arg=="ein" || arg=="an") {
      team_autofollow=1;
      if (IsTeamLeader())
        write("Dein Team folgt Dir.\n");
      else
        write("Du folgst jetzt dem Teamleiter.\n");
    } else {
      team_autofollow=0;
      if (IsTeamLeader())
        write("Dein Team folgt Dir nicht mehr.\n");
      else
        write("Du folgst jetzt nicht mehr dem Teamleiter.\n");
    }
    break; // NICHT return!
  default: ;
  }
  return team->TeamCmd(words[0],narg); // Befehle die Mitgliedschaft erfordern:
}

varargs void InformRowChange(int from, int to, object caster) {

  if (caster) return; // Fuer den Fall, dass Gildenobjekt==ME ist
  if (PO!=Query(P_TEAM)) return;
#if __BOOT_TIME__ < 1281904437
  mixed gilde = QueryProp(P_GUILD);
  if (!stringp(gilde)) return;
  if (!objectp(gilde=find_object("/gilden/"+gilde))) return;
  gilde->InformRowChange(from,to,ME);
#endif
  HookFlow(H_HOOK_TEAMROWCHANGE, ({from,to}) );
}

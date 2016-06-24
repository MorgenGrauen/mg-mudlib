#pragma strong_types,save_types
#pragma no_shadow
// TODO: Morgoth erbt das Teamobjekt: klaeren warum und wieso eigentlich.
//#pragma no_inherit
#pragma pedantic

#include <living/team.h>
#include <properties.h>
#include <language.h>
#include <new_skills.h>
#include <ansi.h>
#include <wizlevels.h>
#define ME this_object()
#define PO previous_object()
#define TP this_player()
#define TI this_interactive()

#define AUTOINF_HP_MINUS 0x01
#define AUTOINF_HP_PLUS  0x02
#define AUTOINF_SP_MINUS 0x04
#define AUTOINF_SP_PLUS  0x08
#define AUTOINF_INSTANT  0x10

private nosave mapping is_member;   // Teammitglieder
private nosave object leader;       // Teamleiter
private nosave string tname;        // Teamname
private nosave mapping wanted_row;  // Gewuenschte Reihe
private nosave mapping wimpy_row;   // Fluchtreihe
private nosave mapping act_row;     // Aktuelle Reihe
private nosave mapping autofollow;  // Spieler folgt Teamleiter
private nosave mapping attack_cmd;  // Spieler hat Angriffsbefehl
private nosave mapping assoc_mem;   // Zugeordnete Mitglieder (Kampf NPCs)
private nosave int *formin,*formax; // Formation
private nosave mixed *rows;         // Die Reihen
private nosave int last_reorder;    // Letzte Formationspruefung
private nosave mapping h;           // Sortier-Score: 125*G.Reihe-HP
private nosave object *att_exec;    // Mitglieder, die Attacke ausfuehren.
private nosave object *mis_attacked;// (Ex-)Mitglieder ohne Begruessungsschlag
private nosave mapping mis_init_att;// Fehlende Begruessungsschlaege
private nosave mapping hp_info;     // HP, MAX_HP, SP, MAX_SP
private nosave int autoinf_flags;
private nosave mapping autoinf_hp;
private nosave mapping autoinf_sp;
private nosave int autoinf_time;
private nosave string *hist;

private nosave object debugger;

void _set_debug() {
  if (!debugger && IS_LEARNER(TI))
    debugger=TI;
  else if (debugger==TI)
    debugger=0;
}
 
private void debug(string str) {
  if (objectp(debugger) && stringp(str))
    tell_object(debugger,"#TEAM: "+str);
}

void create() {
  autoinf_flags=0;
  autoinf_sp=([]);
  autoinf_hp=([]);
  is_member=([]);
  leader=0;
  wanted_row=([]);
  act_row=([]);
  wimpy_row=([]);
  autofollow=([]);
  attack_cmd=([]);
  assoc_mem=([]);
  formin=({1,0,0,0,0});
  formax=({5,4,3,2,1});
  rows=EMPTY_TEAMARRAY;
  h=([]);
  att_exec=({});
  mis_init_att=([]);
  mis_attacked=({});
  hp_info=([]);
  hist=({});
  if (object_name(ME)==TEAM_OBJECT)
    return;
  if (!stringp(tname=TEAM_MASTER->RegisterTeam()))
    tname="?";
}

object *Members() {
  return (m_indices(is_member)-({0}));
}

object Leader() {
  return leader;
}

varargs string name(int casus, int demon) {
  if (!stringp(tname))
    return "Team ?";
  return "Team "+capitalize(tname);
}

varargs string Name(int casus, int demon) {
  return name(casus,demon);
}

varargs int remove(int silent) {
  if (mappingp(is_member) && sizeof(Members())) // Nur leere Teams removen
    return 0;
  TEAM_MASTER->UnregisterTeam(); // Teamnamen freigeben.
  destruct(ME);
  return 1;
}

private void TryRemove() {
  if (clonep(ME)
      && (!mappingp(is_member) || !sizeof(Members()))
      && !first_inventory(ME)
      && find_call_out("remove")<0)
    call_out("remove",0);
}

int CmpFirstArrayElement(mixed *a, mixed *b) {
  return(a[0]<b[0]);
}

varargs private void gtell(string str, string who, int tohist) {
  int i;
  object *tmembers,rochus;
  string prefix,msg;

  tmembers=Members();
  prefix=sprintf("[%s:%s] ",name(),stringp(who)?who:"");
  msg=break_string(str,78,prefix);
  for (i=sizeof(tmembers)-1;i>=0;i--)
    tell_object(tmembers[i],msg);
  if (objectp(rochus=find_player("rochus"))
      && rochus->QueryProp("debug_team"))
    tell_object(rochus,msg);
  if (tohist)
    hist=(hist+({break_string(str+" <"+ctime()[11..15]+">",78,prefix)}))[-100..];
}

int IsMember(object ob) {
  return (objectp(ob) && is_member[ob]);
}

int IsInteractiveMember(object ob) {
  return (objectp(ob) && is_member[ob] && query_once_interactive(ob));
}

varargs private int *GetHpInfo(object ob, closure cl) {
  int *res;

  if (!closurep(cl)) cl=symbol_function("QueryProp",ob);
  if (!pointerp(res=hp_info[ob]) || sizeof(res)<4)
    res=({0,funcall(cl,P_MAX_HP),0,funcall(cl,P_MAX_SP)});
  res[0]=funcall(cl,P_HP);
  res[2]=funcall(cl,P_SP);
  return hp_info[ob]=res;
}

int CompareHp(object a, object b) {
  return h[a]>h[b];
}

// Aktualisiert act_row (->wer steht in welcher Reihe).
private void UpdateActRow() {
  int i,j,update_hp;
  object *new;
  mixed aso;

  act_row=([]);
  rows[0]+=Members();
  update_hp=0;
  if (!mappingp(h)) {
    h=([]);
    update_hp=1;
  }
  for (i=MAX_TEAMROWS-1;i>=0;i--) {
    new=({});
    foreach(object ob: rows[i]) {
      if (objectp(ob) && is_member[ob] && !act_row[ob]) {
        act_row[ob]=i+1;
        new+=({ob});
        if (update_hp) {
          if (!objectp(aso=assoc_mem[ob]) || environment(aso)!=environment(ob))
            aso=ob;
          h[ob]=
            1000*wanted_row[aso]
            +40*act_row[aso]
            -(query_once_interactive(aso)?8:2)*aso->QueryProp(P_HP)
            -query_once_interactive(ob);
          // NPCs bekommen fast gleichen Wert wie Caster,
          // im Zweifelsfalle steht der Caster weiter vorne...
        }
      }
    }
    rows[i]=sort_array(new,"CompareHp",ME);
  }
}

private void CheckFormation() {
  int i,mincap,maxcap,d,num;

  mincap=maxcap=0;
  if (formin[0]<1)
    formin[0]=1;
  
  // erstmal die Wuensche normalisieren/korrigieren auf sinnvolle Werte.
  for (i=0;i<MAX_TEAMROWS;i++) {
    if (formin[i]<0) formin[i]=0;
    if (formax[i]<formin[i]) formax[i]=formin[i];
    if (formax[i]>MAX_TEAM_ROWLEN) formax[i]=MAX_TEAM_ROWLEN;
    if (formin[i]>formax[i]) formin[i]=formax[i];
    mincap+=formin[i]; // Summe der min. je Reihe gewuenschten.
    maxcap+=formax[i]; // Summe der max. je Reihe gewuenschten.
  }

  num=sizeof(Members());

  // max. gewuenschte Reihenlaenge verlaengern, wenn die Summe der maximal je
  // Reihe gewuenschten kleiner als die Anzahl der Spieler ist. Von vorne
  // natuerlich.
  d=num-maxcap;
  for (i=0;i<MAX_TEAMROWS;i++) {
    if (d<=0)
      break;
    d-=(MAX_TEAM_ROWLEN-formax[i]);
    formax[i]=MAX_TEAM_ROWLEN;
    if (d<0)
      formax[i]+=d;  // doch noch was uebrig, wieder anhaengen. 
  }
  // min. gewuenschte Reihenlaenge auf 0 verkuerzen, wenn die Summe der
  // minimal je Reihe gewuenschten groesser als die Anzahl der Spieler ist.
  // Von hinten natuerlich.
  d=mincap-num; // 
  for (i=MAX_TEAMROWS-1;i>=0;i--) {
    if (d<=0)
      break;
    d-=formin[i];
    formin[i]=0;
    if (d<0)
      formin[i]-=d; // doch noch was uebrig, wieder anhaengen.
  }
}

private void MakeFormation() {
  // Verlegungsstrategie:
  //    Richtung Test Verschieben
  // 1. -----> a) MAX          <- X
  //           b) MAX             X ->
  //           c) MIN             X <- <- <- <-
  // 2. <----- a) MIN -> -> -> -> X
  //           b) MAX          <- X
  int i,j,d;

  last_reorder=time();
  UpdateActRow();
  CheckFormation();
  for (i=0;i<MAX_TEAMROWS;i++) {
    d=sizeof(rows[i]);
    if (d<formin[i] || d>formax[i])
      break;
  }
  if (i>=MAX_TEAMROWS)
    return; // Formation ist noch in Ordnung

  for (i=0;i<MAX_TEAMROWS;i++) {
    if (sizeof(rows[i])>formax[i]) {     // Reihe ist zu voll
      if (i>0) {
        d=formax[i-1]-sizeof(rows[i-1]);
        if (d>0) {                       // Reihe vorher hat d freie Plaetze
          rows[i-1]+=rows[i][0..(d-1)];  // Also d Mitglieder abgeben
          rows[i]=rows[i][d..];
        }
      }
      if (i<MAX_TEAMROWS-1 && sizeof(rows[i])>formax[i]) {// Immer noch zu voll
        rows[i+1]=rows[i][formax[i]..]+rows[i+1];         // Rest nach hinten.
        rows[i]=rows[i][0..(formax[i]-1)];
      }
      continue; // War zu voll, kann nicht zu leer sein
    }
    for (j=i+1;j<MAX_TEAMROWS;j++) {
      d=formin[i]-sizeof(rows[i]);
      if (d<=0)                   // Ausreichende Anzahl
        break;                    // kein weiteres j noetig
      rows[i]+=rows[j][0..(d-1)]; // Sonst Nachschub von hinten holen
      rows[j]=rows[j][d..];
    }
  }
  for (i=MAX_TEAMROWS-1;i>0;i--) {
    for (j=i-1;j>=0;j--) {
      d=formin[i]-sizeof(rows[i]);
      if (d<=0)                   // Ausreichende Anzahl
        break;                    // kein weiteres j noetig
      rows[i]+=rows[j][0..(d-1)]; // Sonst Nachschub von vorne holen
      rows[j]=rows[j][d..];
    }
    d=sizeof(rows[i])-formax[i];
    if (d>0) {
      rows[i-1]+=rows[i][0..(d-1)]; // Ueberschuss nach vorne schieben
      rows[i]=rows[i][d..];
    }
  }
  UpdateActRow();
}

private void RemoveFromRow(object ob, int src) {
  if (src<0 || src>=MAX_TEAMROWS)
    return;
  rows[src]-=({ob});
  if (sizeof(rows[src])>=formin[src])
    return;
  // Falls hinten noch Ueberschuss da ist, her damit.
  if (src<MAX_TEAMROWS-1 && sizeof(rows[src+1])-1>=formin[src+1]) {
    rows[src]+=rows[src+1][0..0];
    rows[src+1]=rows[src+1][1..];
    return;
  }
  // Falls vorne noch Ueberschuss da ist, her damit.
  if (src>0 && sizeof(rows[src-1])-1>=formin[src-1]) {
    rows[src]=rows[src-1][<1..]+rows[src];
    rows[src-1]=rows[src-1][0..<2];
  }
}

private void AddToRow(object ob, int dest) {
  if (dest<0 || dest>=MAX_TEAMROWS)
    return;
  rows[dest]+=({ob});
  if (sizeof(rows[dest])<=formax[dest])
    return;
  // Falls vorne noch jemand hin kann, dorthin
  if (dest>0 && sizeof(rows[dest-1])+1<=formax[dest-1]) {
    rows[dest-1]+=rows[dest][0..];
    rows[dest]=rows[dest][1..];
    return;
  }
  // Falls hinten noch jemand hin kann, dorthin
  if (dest<MAX_TEAMROWS-1 && sizeof(rows[dest+1])+1<=formax[dest+1]) {
    // Dest: ({... <3, <2, ob});
    rows[dest+1]=rows[dest][<2..<2]+rows[dest+1];
    rows[dest]=rows[dest][0..<3]+({ob});
  }
}

private void CycleRows(object ob, int src, int dest) {
  int i;

  if (src<0 || src>=MAX_TEAMROWS || dest<0 || dest>=MAX_TEAMROWS)
    return;
  rows[src]-=({ob});
  if (sizeof(rows[src])<formin[src] || sizeof(rows[dest])>=formax[dest]) {
    if (src<dest) {             // (<- -X) <- <- <- <- (+X <-)
      for (i=src+1;i<=dest;i++) {
        rows[i-1]+=rows[i][0..0];
        rows[i]=rows[i][1..];
      }
    } else if (src>dest) {           // (-> +X) -> -> -> -> (-X ->)
      for (i=src-1;i>=dest;i--) {
        rows[i+1]=rows[i][<1..]+rows[i+1];
        rows[i]=rows[i][0..<2];
      }
    }
  }
  if (src<=dest)
    rows[dest]+=({ob});
  else
    rows[dest]=({ob})+rows[dest];
}

// tauscht zufaellig aus den ersten 4 Reihen aus einer Reihe den letzten mit
// dem ersten aus der Folgereihe, wenn die der Score des vorderen Spieler mehr
// als 10 groesser als der des hinteren Spielers ist.
private void RandomChangeRow() {
  int *nums,i;
  object p1,p2;

  if (!mappingp(h))
    UpdateActRow();
  for (nums=({0,1,2,3});sizeof(nums);nums-=({i})) {
    i=nums[random(sizeof(nums))];
    if (!sizeof(rows[i]) || !sizeof(rows[i+1])) continue;
    if (!objectp(p1=rows[i][<1]) || !objectp(p2=rows[i+1][0])) continue;
    if (wanted_row[p1]<wanted_row[p2]) continue;
    if (h[p2]-h[p1]>=-10) continue;
    rows[i][<1]=p2;
    rows[i+1][0]=p1;
    return;
  }
}

varargs private string
CountUpNames(object *obs, string zsing, string zplur) {
  string res;
  object ob;
  int i,sz;

  res="";
  if (!pointerp(obs)) return res;
  if (!stringp(zsing)) zsing="";
  if (!stringp(zplur)) zplur="";
  if (sz=sizeof(obs)) {
    for (i=0;i<sz;i++) {
      if (i)
	      res+=((i<sz-1)?", ":" und ");
      if (objectp(ob=obs[i]))
        res+=ob->name(WER);
    }
    if (sz>1)
      res+=zplur;
    else
      res+=zsing;
  }
  return res;
}

static void DoChangeRow(object pl, int dest) {
  mapping old_row,ec1,ec2,ecb;
  int i;
  object *obs,*envs,env;
  string *msg,str;

  dest--;
  CheckFormation();
  h=0;  // damit HP-Liste geupdated wird.
  UpdateActRow();
  old_row=deep_copy(act_row);

  // welche Objekte bewegen?
  obs=({});
  if (objectp(pl)) {
    obs=({pl});
    if (pointerp(assoc_mem[pl]))
      obs+=assoc_mem[pl];
  } else {
    RandomChangeRow();
  }

  foreach (object ob:obs) {
    if (!objectp(ob))
      continue;
    // Alle assoziierten NPC kriegen die gleiche gewuenschte Reihe wie der
    // Spieler.
    wanted_row[ob]=wanted_row[pl];
    UpdateActRow();
    // und dann in die gew. Reihe stopfen.
    int src=act_row[ob]-1;
    if (dest<0 || dest>=MAX_TEAMROWS)
      RemoveFromRow(ob,src);
    else if (src<0 || src>=MAX_TEAMROWS)
      AddToRow(ob,dest);
    else if (src!=dest)
      CycleRows(ob,src,dest);
  }

  MakeFormation();

  obs = Members(); // alle Members beruecksichtigen beim Abgleich!
  object *changed = allocate(0);
  foreach (object ob:obs)
    if (objectp(ob) && old_row[ob]!=act_row[ob]) {
      ob->InformRowChange(old_row[ob],act_row[ob]);
      changed += ({ob});
    }

  // Ab jetzt nur noch Ausgabe.
  if (get_eval_cost()<800000) return; // War schon teuer genug, Lagvermeidung
  msg=({});ec1=([]);ec2=([]);ecb=([]);
  foreach (object ob:changed) {
    tell_object(ob,sprintf("Du bist jetzt in Reihe %d.\n",act_row[ob]));
    msg+=({sprintf("%s->%d",ob->Name(WER),act_row[ob])});
    if (query_once_interactive(ob) && !interactive(ob)) continue;
    if (!objectp(env=environment(ob))) continue;
    if (old_row[ob]<=1) {
      if (!pointerp(envs=ec1[env])) envs=({});
      ec1[env]=envs+({ob});ecb[env]|=1;
    }
    if (act_row[ob]<=1) {
      if (!pointerp(envs=ec2[env])) envs=({});
      ec2[env]=envs+({ob});ecb[env]|=2;
    }
  }
  if (sizeof(msg))
    gtell(implode(msg,", ")); // Ausgabe an alle Gruppenmitglieder.
  m_delete(ecb,find_object("/room/netztot")); // Das gaebe Mega-Lag :-)
  envs=m_indices(ecb);obs=Members();
  for (i=sizeof(envs)-1;i>=0;i--) {
    if (!objectp(env=envs[i])) continue;
    str="";
    str+=CountUpNames(ec1[env]," tritt zurueck"," treten zurueck");
    if (ecb[env]==3) str+=", ";
    str+=CountUpNames(ec2[env]," tritt vor"," treten vor");
    str+=".\n";
    tell_room(env,capitalize(break_string(str,78)),obs);
  }
}

void UpdateFormation() {
  DoChangeRow(0,0);
}

int SwapRows(object ob1, object ob2) {
  int i,r1,r2,p1,p2;

  if (!objectp(ob1) || !objectp(ob2) || ob1==ob2)
    return 0;
  r1=r2=-1;
  for (i=0;i<MAX_TEAMROWS;i++) {
    if (r1==-1 && (p1=member(rows[i],ob1))>=0)
      r1=i;
    if (r2==-1 && (p2=member(rows[i],ob2))>=0)
      r2=i;
  }
  if (r1==-1 || r2==-1)
    return 0;
  if (r1==r2)
    return 1;
  if (r1<r2) { // Nicht Monster vor Spieler stellen
    if (query_once_interactive(ob1) && !interactive(ob2))
      return 0;
  } else {
    if (query_once_interactive(ob2) && !interactive(ob1))
      return 0;
  }
  rows[r1][p1]=ob2;ob2->InformRowChange(r2,r1);
  rows[r2][p2]=ob1;ob1->InformRowChange(r1,r2);
  gtell(ob1->Name(WER)+" und "+ob2->name(WER)+" tauschen die Plaetze.\n");
  return 1;
}

private int ChangeRow(string arg) {
  int num;

  if (!arg || sscanf(arg,"%d",num)!=1)
    return notify_fail("In welche Reihe willst Du denn wechseln?\n"),0;
  if (num<1 || num>MAX_TEAMROWS)
    return notify_fail("Die Reihenangabe ist ungueltig.\n"),0;
  TP->SetProp(P_TEAM_WANTED_ROW,wanted_row[TP]=num);
  printf("Du versuchst in Reihe %d zu wechseln.\n",num);
  DoChangeRow(TP,num);
  return 1;
}

private int ChangeWimpyRow(string arg) {
  int num;

  if (!arg || sscanf(arg,"%d",num)!=1)
    return notify_fail("In welche Reihe willst Du fliehen?\n"),0;
  if (num<0 || num>MAX_TEAMROWS)
    return notify_fail("Die Reihenangabe ist ungueltig.\n"),0;
  TP->SetProp(P_TEAM_WIMPY_ROW,wimpy_row[TP]=num);
  if (num>1)
    printf("Bei der Flucht wirst Du in Reihe %d wechseln.\n",num);
  else
    write("Bei der Flucht wirst Du den Raum verlassen.\n");
  return 1;
}

mixed *PresentRows(object env) {
  int i,j,d,arbeit;
  mixed *res;
  object *nd,ob;

  if (!objectp(env))
    env=environment(TP);
  if (last_reorder!=time() || !mappingp(h))
    UpdateFormation();
  res=EMPTY_TEAMARRAY;arbeit=0;nd=({});
  for (i=0;i<MAX_TEAMROWS;i++) {
    object *new=({});
    foreach(ob: rows[i]) {
      if (objectp(ob) && is_member[ob] && environment(ob)==env) {
        if (query_once_interactive(ob) && !interactive(ob)) {
          nd+=({ob});
          arbeit=1;
        } else {
          new+=({ob});
        }
      } else {
        arbeit=1;
      }
    }
    res[i]=new;
  }
  if (!arbeit)
    return res;
  for (i=j=0;i<MAX_TEAMROWS;i++) {
    if (j<=i) j=i+1;
    for (;j<MAX_TEAMROWS;j++) {
      d=formin[i]-sizeof(res[i]);
      if (d<=0)                   // Ausreichende Anzahl
        break;                    // kein weiteres j noetig
      res[i]+=res[j][0..(d-1)];   // Sonst Nachschub von hinten holen
      res[j]=res[j][d..];
    }
  }
  res[MAX_TEAMROWS-1]+=nd; // Netztote bieten keine Deckung, nach hinten.
  return res;
}

mapping PresentPositions(mixed pres_rows) {
  mapping res=([]);
  if (objectp(pres_rows))
    pres_rows=PresentRows(pres_rows);
  if (!pointerp(pres_rows))
    return res;
  for (int i=0;i<MAX_TEAMROWS;i++) {
    foreach(object ob: pres_rows[i])
      if (ob)
        res[ob]=i+1;
  }
  return res;
}


varargs int FleeToRow(object ob) {
  int num;

  if (!objectp(ob))
    ob=TP;
  if (!IsMember(ob))
    return 0;
  h=0; // Reihen bei naechster Abfrage neu sortieren
  num=wimpy_row[ob];
  if (num<2 || num>MAX_TEAMROWS) // Flucht in 1. Reihe nicht sinnvoll.
    return 0;
  if (num==wanted_row[ob]) // Ist schonmal nach hinten geflohen.
    return 0;
  tell_object(ob,sprintf("Du versuchst in Reihe %d zu fliehen.\n",num));
  ob->SetProp(P_TEAM_WANTED_ROW,wanted_row[ob]=num);
  DoChangeRow(ob,num);
  if (PresentPositions(environment(ob))[ob]<=1) // Flucht gescheitert?
    return 0;
  return 1;
}

static int ChangeFormation(string arg) {
  string *words;
  int i,min,max;
  mapping old_row;

  if (arg=="aus")
    arg="1-6 0-6 0-6 0-6 0-6";
  i=sizeof(words=old_explode(arg," "));
  if (i>MAX_TEAMROWS)
    i=MAX_TEAMROWS;
  for (--i;i>=0;i--) {
    if (sscanf(words[i],"%d-%d",min,max)==2) {
      formin[i]=min;
      formax[i]=max;
    } else if (sscanf(words[i],"%d",min)==1) {
      formin[i]=formax[i]=min;
    }
  }

  UpdateFormation();

  words=({});
  for (i=0;i<MAX_TEAMROWS;i++)
    words+=({sprintf("%d-%d",formin[i],formax[i])});
  gtell("Die Formation ist jetzt "+implode(words," / ")+".\n");
  return 1;
}

int Shout(string str) {
  if (!str || str=="")
    return notify_fail("Was willst Du den anderen Teammitgliedern sagen?\n"),0;
  gtell(str,TP->Name(WER),1);
  return 1;
}

int Hist(string str) {
  int i,anz,maximal;

  // non-interactive oder Nicht-Mitglieder sollten die Hist nicht abfragen.
  if (!IsInteractiveMember(TP)) return -1;

  maximal=sizeof(hist);
  if (str && sscanf(str,"%d",anz)==1)
    i=maximal-anz;
  if (i<0)
    i=0;

  TP->More(sprintf("%@s",hist[i..maximal])||"");
  return 1;
}

private void DoChangeLeader(object ob) {
  if (objectp(leader) && leader->QueryProp(P_TEAM_LEADER)==ME)
    leader->SetProp(P_TEAM_LEADER,0);
  leader=ob;
  leader->SetProp(P_TEAM_LEADER,ME);
}

private int ChangeLeader(string arg) {
  object ob;

  if (stringp(arg) && arg!="") {
    if (!objectp(ob=find_player(arg))
        && !objectp(ob=present(arg,environment(TP))))
      return notify_fail(capitalize(arg)+" nicht gefunden.\n"),0;
  } else {
    ob=TP;
  }
  if (objectp(leader)
      && TP!=leader
      && (!interactive(TP) || (interactive(leader) && query_idle(leader)<180)))
    return notify_fail("Der Teamleiter ist noch aktiv.\n"),0;
  if (objectp(leader)
      && query_once_interactive(leader)
      && !query_once_interactive(leader))
    return notify_fail("Nur ein Spieler kann das Team leiten.\n"),0;
  if (!IsMember(ob))
    return notify_fail(ob->Name(WER)+" ist kein Teammitglied.\n"),0;
  DoChangeLeader(ob);
  gtell(ob->Name(WER)+" leitet jetzt das Team.\n");
  return 1;
}

varargs private int CheckSecond(object pl, object *obs) {
  mixed x,ip;
  mapping second;
  object ob;
  int i;

  if (!objectp(pl))
    pl=TP;
  if (!query_once_interactive(pl))
    return 0;
  if (!pointerp(obs))
    obs=Members();
  obs-=({pl});
  second=([]);
  for (i=sizeof(obs)-1;i>=0;i--) {
    if (!objectp(ob=obs[i]) || !query_once_interactive(ob)) continue;
    second[getuid(ob)]=1;
    if (stringp(x=ob->QueryProp(P_SECOND)))
      second[lower_case(x)]=1;
  }
  if (second[getuid(pl)] ||
      (stringp(x=pl->QueryProp(P_SECOND)) && second[lower_case(x)]))
    return 1;

  if (!stringp(ip=pl->QueryProp(P_CALLED_FROM_IP)) || ip=="")
    return 0;
  for (i=sizeof(obs)-1;i>=0;i--) {
    if (!objectp(ob=obs[i]) || !query_once_interactive(ob)) continue;
    if (ob->QueryProp(P_CALLED_FROM_IP)!=ip) continue;
    log_file("rochus/zweitieverdacht",
             sprintf("%s %s,%s\n",ctime()[4..15],getuid(pl),getuid(ob)));
    break;
  }
  return 0;
}

static int Allowed(object pl) {
  mixed x;
  string nm;

  if (!objectp(pl))
    return notify_fail("WER soll ins Team aufgenommen werden?\n"),0;
  if (pl==TP)
    nm="Du b";
  else
    nm=pl->Name(WER)+" ";
  if (objectp(x=pl->QueryProp(P_TEAM)) && x!=ME)
    return notify_fail(nm+"ist schon in einem anderen Team.\n"),0;
  if (pl->QueryGuest())
    return notify_fail(nm+"ist ein Gast.\n"),0;
  if (pl->QueryProp(P_GHOST))
    return notify_fail(nm+"ist ein Geist.\n"),0;
  if (CheckSecond(pl))
    return notify_fail(nm+"ist Zweitspieler eines Teammitglieds.\n"),0;
  if (sizeof(filter(Members(),"IsInteractiveMember",ME))
      >=MAX_TEAM_MEMBERS)
    return notify_fail("Es sind schon zuviele Spieler im Team.\n"),0;
  return 1;
}

private void DoAddMember(object ob) {
  closure cl;

  if (!IsMember(leader))
    DoChangeLeader(ob);
  ob->SetProp(P_TEAM,ME);
  if (IsMember(ob))
    return;
  is_member[ob]=1;
  cl=symbol_function("QueryProp",ob);
  attack_cmd[ob]=funcall(cl,P_TEAM_ATTACK_CMD);
  autofollow[ob]=funcall(cl,P_TEAM_AUTOFOLLOW);
  wimpy_row[ob]=funcall(cl,P_TEAM_WIMPY_ROW);
  wanted_row[ob]=funcall(cl,P_TEAM_WANTED_ROW);
  if (!wanted_row[ob]) wanted_row[ob]=1;;
  ob->SetProp(P_TEAM_NEWMEMBER,0);
  GetHpInfo(ob,cl);
  if (query_once_interactive(ob)) ob->AddHpHook(ME);
  if (!objectp(assoc_mem[ob]))
    gtell(ob->Name(WER)+" wurde ins Team aufgenommen.\n");
  DoChangeRow(ob,wanted_row[ob]);
}

int AddAssocMember(object caster, object npc) {
  object *obs;

  if (extern_call() && PO!=caster)
    return 0;
  if (!IsMember(caster)
      || !objectp(npc)
      || query_once_interactive(npc)
      || IsMember(npc)
      || objectp(assoc_mem[caster])
      || assoc_mem[npc]
      || caster==npc)
    return 0;
  assoc_mem[npc]=caster;
  DoAddMember(npc);
  if (!pointerp(obs=assoc_mem[caster]))
    obs=({});
  obs=(obs-({npc,0}))+({npc});
  assoc_mem[caster]=obs;
  return 1;
}

static void AddMemberAndAssocs(object caster) {
  object *obs,ob;
  int i;

  DoAddMember(caster);
  if (!pointerp(obs=caster->QueryProp(P_TEAM_ASSOC_MEMBERS)))
    return;
  for (i=sizeof(obs)-1;i>=0;i--)
    if (objectp(ob=obs[i]) && !caster->IsEnemy(ob))
      AddAssocMember(caster,ob);
}

int AddMember(object ob) {
 
  if (!Allowed(ob))
    return 0;
  
  // Wenn es einen TP gibt, unterliegt er einigen Einschraenkungen.
  // Dafuer wird er aber via Aufnahme ins Team auch ggf. Teamleiter, wenn
  // noetig.
  // TODO: Dieser Code geht davon aus, dass er nur aus einem Kommando
  // ausgefuehrt wird und TP der Veranlasser der Aufnahme ist. Dies scheint
  // mir unrealistisch. (Zesstra)
  if (TP && TP==previous_object()) {
    if (!Allowed(TP))
      return 0;
    if (TP!=ob && CheckSecond(ob,({TP}))) {// Zweitiereglung bei Gruendung.
      notify_fail(ob->Name(WER)+" ist Zweitspieler von Dir.\n");
      return 0;
    }
    if (!IsMember(leader))
      AddMemberAndAssocs(TP);
    if (TP!=leader) {
      notify_fail("Nur der Teamleiter kann Mitglieder aufnehmen.\n");
      return 0;
    }
  }

  AddMemberAndAssocs(ob);
  return 1;
}

private void DoRemoveMember(object ob) {
  object *tmembers,caster;
  mixed asmem;
  int i;

  ob->SetProp(P_TEAM,0);
  m_delete(is_member,ob);
  m_delete(wanted_row,ob);
  m_delete(act_row,ob);
  m_delete(wimpy_row,ob);
  m_delete(autofollow,ob);
  m_delete(attack_cmd,ob);
  if (objectp(caster=assoc_mem[ob]) && pointerp(asmem=assoc_mem[caster]))
    assoc_mem[caster]=asmem-({ob,0});
  if (query_once_interactive(ob)) ob->RemoveHpHook(ME);
  m_delete(hp_info,ob);
  m_delete(autoinf_hp,ob);
  m_delete(autoinf_sp,ob);
  DoChangeRow(ob,-1);

  if (!objectp(assoc_mem[ob])) {
    if (ob->QueryProp(P_GHOST)) {
      gtell(upper_case(ob->name(WER))+" HAT DAS TEAM VERLASSEN.","Tod");
    } else {
      tell_object(ob,"Du verlaesst das Team.\n");
      gtell(ob->Name(WER)+" hat das Team verlassen.\n");
    }
  }
  m_delete(assoc_mem,ob);

  if (IsMember(leader)) // Hat das Team noch einen Leiter?
    return;
  tmembers=Members();ob=0;
  if (i=sizeof(tmembers)) {
    ob=tmembers[0];
    for (--i;i>=0;i--) {
      if (interactive(tmembers[i])) {
        ob=tmembers[i];
        break;
      }
      if (query_once_interactive(tmembers[i]))
        ob==tmembers[i];
    }
    DoChangeLeader(ob);
    gtell(leader->Name(WER)+" hat die Teamleitung uebernommen.\n");
    return;
  }
  TryRemove();
}

int RemoveAssocMember(object caster, object npc) {
  object *obs;

  if (extern_call() && PO!=caster)
    return 0;
  if (!IsMember(caster) || assoc_mem[npc]!=caster)
    return 0;
  DoRemoveMember(npc);
  return 1;
}

static void RemoveMemberAndAssocs(object caster) {
  object *obs,ob;
  int i;

  if (pointerp(obs=caster->QueryProp(P_TEAM_ASSOC_MEMBERS))) {
    for (i=sizeof(obs)-1;i>=0;i--)
      if (objectp(ob=obs[i]))
        RemoveAssocMember(caster,ob);
  }
  DoRemoveMember(caster);
}

private void RemoveSingles() {
  object *obs;
  mixed aso;

  if (!IsMember(leader)) return;
  if (!query_once_interactive(leader)) return; // NPC Team
  obs=Members()-({leader});
  if (pointerp(aso=assoc_mem[leader]))
    obs-=aso;
  if (sizeof(obs)) return;
  gtell("Das Team loest sich auf.\n");
  RemoveMemberAndAssocs(leader);
  TryRemove();
}

int RemoveMember(mixed arg) {
  object *mems,mem,ob;
  int i;
  
  if (objectp(arg)) {
    ob=arg;
  } else if (stringp(arg) && arg!="") {
    mems=Members()-({leader});
    for (i=sizeof(mems)-1;i>=0;i--) {
      if (objectp(mem=mems[i]) && mem->id(arg))  {
        ob=mem;
        if (query_once_interactive(ob))
          break;
      }
    }
    if (!objectp(ob))
      return notify_fail(capitalize(arg)+" nicht gefunden.\n"),0;
  } else {
    return 0;
  }
  if (TP!=leader && TP!=ob && ob!=PO)
    return notify_fail("Nur der Teamleiter kann Mitglieder entlassen.\n"),0;
  if (!IsMember(ob) && ob->QueryProp(P_TEAM)!=ME)
    return notify_fail(ob->Name(WER)+" ist kein Teammitglied.\n"),0;
  if (PO!=ob && objectp(assoc_mem[ob]))
    return notify_fail(ob->Name(WER)+" gehoert zu "+
                       assoc_mem[ob]->Name(WEM)+".\n"),0;

  RemoveMemberAndAssocs(ob);
  RemoveSingles();
  return 1;
}

static int ChangeName(string str) {
  if (leader && TP!=leader)
    return notify_fail("Nur der Teamleiter kann den Namen aendern.\n"),0;
  if (!str || str=="")
    return 0;
  str=str[0..19];
  if (!stringp(str=(TEAM_MASTER->RegisterTeam(str))))
    return notify_fail("Der Name ist schon vergeben.\n"),0;
  tname=str;
  if (objectp(TP))
    gtell(TP->Name(WER)+" aendert den Teamnamen auf \""+tname+"\".\n");
  return 1;
}

void TeamInitAttack() {
  object *obs,*ens,*removed,ob,en;
  int i,j;

  debug(sprintf("mis_init_att: %O\n",mis_init_att));
  ens=m_indices(mis_init_att);removed=({});
  for (i=sizeof(ens)-1;i>=0;i--) {
    if (!objectp(en=ens[i]))
      continue;
    if (!pointerp(obs=mis_init_att[en]))
      continue;
    for (j=sizeof(obs)-1;j>=0;j--)
      if (!IsMember(ob=obs[j]) || environment(ob)!=environment(en))
        obs[j]=0;
    obs-=({0});
    ob=en->SelectNearEnemy(obs,1);
    debug(sprintf("Begruessungsschlag von %O fuer %O\n",en,ob));
    if (!objectp(ob))
      continue;
    en->Attack2(ob); // Begruessungsschlag
    removed+=({en}); // Kein Begruessungsschlag mehr von diesem Monster
  }

  for (i=sizeof(mis_attacked)-1;i>=0;i--)
    if (objectp(ob=mis_attacked[i]))
      ob->ExecuteMissingAttacks(removed);
  // Begruessungsschlaege die ausgefuehrt wurden entfernen
  // und nicht ausgefuehrte nachholen

  mis_attacked=({});
  mis_init_att=([]);
  att_exec=({});
}

int InitAttack_Callback(object enemy) {
  object *arr;

  if (!IsMember(PO) || member(att_exec,PO)<0)
    return 0;
  if (!pointerp(arr=mis_init_att[enemy]))
    arr=({});
  mis_init_att[enemy]=arr+({PO});
  if (member(mis_attacked,PO)<0)
    mis_attacked+=({PO});
  return 1;
}

void TeamAttackExecuted_Callback(int success) {
  if (!IsMember(PO))
    return;
  att_exec-=({PO,0});
  if (!sizeof(att_exec))
    TeamInitAttack();
}

private int StartAttack() {
  object *tmembers,ob,env;
  int i;

  if (TP!=leader || !objectp(env=environment(TP)))
    return notify_fail("Nur der Teamleiter kann den Angriff starten.\n"),0;
  tmembers=Members();
  TeamInitAttack(); // Falls noch Schlaege fehlen....
  for (i=sizeof(tmembers)-1;i>=0;i--)
    if (objectp(ob=tmembers[i]) && stringp(attack_cmd[ob]))
      if (ob->CallTeamAttack(env)) // Angriff wird ausgefuehrt?
        att_exec+=({ob});          // Liste der Angreifer
  gtell(TP->Name(WER)+" startet den Angriff.\n");
  return 1;
}

void StartFollow(object env) {
  object *tmembers,ob;
  int i;
  string cmd,args;

  // printf("ld:%O PO:%O TP:%O qv:%O env:%O\n",leader,PO,TP,query_verb(),env);
  if (TP!=leader
      || !stringp(cmd=query_verb())
      || !objectp(env)
      || TP!=PO
      || TP->QueryProp(P_GHOST) // Der Befehl war wohl ungesund...
      || TP->IsTeamMove()) // Angriffsbefehl nicht durch verfolge 2 mal...
    return;
  cmd="\\"+cmd;
  if (stringp(args=TP->_unparsed_args()) && args!="")
    cmd+=(" "+args);
  tmembers=Members()-({leader});
  for (i=sizeof(tmembers)-1;i>=0;i--)
    if (objectp(ob=tmembers[i]) && autofollow[ob])    
    	if(!query_once_interactive(ob)  || env==environment(ob)) // autofolge nur bei anfuehrer im gleichen raum
    	      ob->CallTeamFollow(env,cmd);
}

void ShowTeamInfo() {
  int i;
  object *tmembers,ob;
  string form;

  if (!TI || TP!=TI || !IS_LEARNER(TI))
    return;

  UpdateActRow();
  form="";
  for (i=0;i<MAX_TEAMROWS;i++)
    form+=sprintf(" %d-%d",formin[i],formax[i]);
  printf("%s [%O] L: %s F:%s\n",name(),ME,
         (objectp(leader)?leader->Name(WER):"?"),form);
  tmembers=Members();
  for (i=sizeof(tmembers)-1;i>=0;i--) {
    ob=tmembers[i];
    printf("  %d(%d) %s [%O]\n",
           act_row[ob],wanted_row[ob],ob->Name(WER),ob);
  }
}

varargs int ShowTeamHP(string arg) {
  object *tmembers,ob;
  closure qp;
  int i,longinf;
  mixed *vals,*res,cols,fr,rr,termstr;
  mapping real_row;
  string nm,inf;

  if (arg && arg[0..3]=="lang") {
    if (TP!=leader)
      return notify_fail("Nur der Teamleiter kann die lange "+
                         "Uebersicht aufrufen.\n"),0;
    longinf=1;
    arg=arg[5..];
  } else
    longinf=0;
  real_row=PresentPositions(environment(TP));
  tmembers=({});
  for (i=0;i<MAX_TEAMROWS;i++)
    tmembers+=rows[i];
  res=({});
  switch(TP->QueryProp(P_TTY)) {
  case "ansi":
    termstr=({ANSI_RED+ANSI_BOLD,ANSI_YELLOW,ANSI_GREEN,ANSI_NORMAL,
              ANSI_UNDERL});
    break;
  case "vt100":
    termstr=({ANSI_BOLD,"","",ANSI_NORMAL,ANSI_UNDERL});
    break;
  default:
    termstr=({"","","","",""});
  }
  vals=({"",0,0,termstr[3],"",0,0,termstr[3]});

  printf("  Name        Gilde           LV GLV  LP (MLP)  KP (MKP) Vors. GR AR TR FR A V\n");
  if (longinf)
    printf("  (Zeile 2)   Angriffsbefehl                             Fluchtrichtung\n");

  for (i=sizeof(tmembers)-1;i>=0;i--) {
    if (!objectp(ob=tmembers[i])) continue;
    qp=symbol_function("QueryProp",ob);
    fr=GetHpInfo(ob,qp);
    vals[1]=fr[0];vals[2]=fr[1];vals[5]=fr[2];vals[6]=fr[3];
    /*
    vals[1]=funcall(qp,P_HP);vals[2]=funcall(qp,P_MAX_HP);
    vals[5]=funcall(qp,P_SP);vals[6]=funcall(qp,P_MAX_SP);
    */
    if (!pointerp(cols=funcall(qp,P_TEAM_COLORS)) || sizeof(cols)<4)
      cols=({vals[2]/4,vals[2]/2,vals[6]/4,vals[6]/2});
    if (vals[1]<cols[0])
      vals[0]=termstr[0];
    else if (vals[1]<cols[1])
      vals[0]=termstr[1];
    else
      vals[0]=termstr[2];
    if (vals[5]<cols[2])
      vals[4]=termstr[0];
    else if (vals[5]<cols[3])
      vals[4]=termstr[1];
    else
      vals[4]=termstr[2];
    if (intp(fr=wimpy_row[ob]) && fr>1) fr=sprintf("%2d",fr); else fr="--";
    if (intp(rr=real_row[ob]) && rr>0)  rr=sprintf("%2d",rr); else rr="--";
    nm=ob->Name(RAW);
    inf=sprintf("%s %-11s%s %-14s %3d %3d %s%3d (%3d)%s %s%3d (%3d)%s %5d %2d %2d %2s %2s %1s %1s\n",
               ((ob==leader)?(termstr[4]+"*"):" "),
               nm[0..10],termstr[3],
               capitalize(funcall(qp,P_GUILD)||"")[0..13],
               funcall(qp,P_LEVEL),
               funcall(qp,P_GUILD_LEVEL),
               vals[0],vals[1],vals[2],vals[3],
               vals[4],vals[5],vals[6],vals[7],
               funcall(qp,P_WIMPY),
               wanted_row[ob],act_row[ob],rr,fr,
               (attack_cmd[ob]?"X":"-"),
               (autofollow[ob]?"X":"-"));
    if (longinf) {
      if (!stringp(fr=funcall(qp,P_WIMPY_DIRECTION))) fr="";
      if (!stringp(rr=attack_cmd[ob])) rr="";
      if (fr!="" || rr!="")
        inf+=sprintf("              %-42s %-21s\n",rr[0..41],fr[0..20]);
    }

    switch (arg) {
    case "alphabetisch":
      res+=({({(query_once_interactive(ob)?"0":"1")+nm,inf})});
      break;
    case "sortiert":
      res+=({({sprintf("%2s %2d %2d %s",rr,act_row[ob],wanted_row[ob],nm),
                 inf})});
      break;
    default:
      res+=({({i,inf})});
    }
  }
  if (arg && arg!="")
    res=sort_array(res,"CmpFirstArrayElement",ME);
  for (i=sizeof(res)-1;i>=0;i--)
    write(res[i][1]);
  return 1;
}

varargs int ShowTeamRooms(string arg) {
  object *tmembers,ob;
  string s1,s2;
  mixed *res;
  int i;

  tmembers=Members();res=({});
  for (i=sizeof(tmembers)-1;i>=0;i--) {
    if (!objectp(ob=tmembers[i])) continue;
    if (!query_once_interactive(ob) && arg!="alle") continue;
    s1=ob->Name(RAW);
    if (!objectp(ob=environment(ob))) continue;
    if (!stringp(s2=ob->QueryProp(P_INT_SHORT))) s2="";
    res+=({({s1,s2})});
  }
  res=sort_array(res,"CmpFirstArrayElement",ME);
  for (i=sizeof(res)-1;i>=0;i--)
    printf("%-11s %-66s\n",res[i][0][0..10],res[i][1][0..65]);
  return 1;
}

int ChangeColors(string arg) {
  int *col;

  notify_fail("team farben lp_rot lp_gelb [kp_rot kp_gelb]\n");
  if (!arg)
    return 0;
  col=({0,0,0,0});
  if (sscanf(arg,"%d %d %d %d",col[0],col[1],col[2],col[3])!=4) {
    if (sscanf(arg,"%d %d",col[0],col[1])!=2)
      return 0;
    col[2]=col[0];col[3]=col[1];
  }
  printf("Die Anzeige ist jetzt gelb unter %d LP, rot unter %d LP\n"+
         "                 bzw. gelb unter %d KP, rot unter %d KP.\n",
         col[1],col[0],col[3],col[2]);
  TP->SetProp(P_TEAM_COLORS,col);
  TP->Set(P_TEAM_COLORS,SAVE,F_MODE_AS);
  return 1;
}

int ChangeAutoInfo(string arg) {
  string *words,txt;
  int i,fl;

  if (TP!=leader)
    return notify_fail("Nur der Teamleiter kann automatisch "+
                       "informiert werden.\n"),0;
  words=old_explode(arg?arg:""," ");fl=0;
  for (i=sizeof(words)-1;i>=0;i--) {
    switch(words[i]) {
    case "aus":
      write("Du wirst nicht mehr automatisch informiert.\n");
      autoinf_flags=0;
      return 1;
    case "+kp":
      fl|=AUTOINF_SP_PLUS;
    case "kp":
      fl|=AUTOINF_SP_MINUS;
      break;
    case "+lp":
      fl|=AUTOINF_HP_PLUS;
    case "lp":
    case "ein":
    case "an":
      fl|=AUTOINF_HP_MINUS;
      break;
    case "sofort":
      fl|=AUTOINF_INSTANT;
      break;
    default:
      ;
    }
  }
  if (!fl)
    return notify_fail("WIE moechtest Du automatisch informiert werden?\n"),0;
  if (fl==AUTOINF_INSTANT) fl|=AUTOINF_HP_MINUS;
  autoinf_flags=fl;
  txt="Du wirst"+((fl&AUTOINF_INSTANT)?" sofort ":" ")+"informiert, wenn";
  if (fl&(AUTOINF_HP_PLUS|AUTOINF_HP_MINUS)) {
    txt+=" die Lebenspunkte eines Teammitglieds";
    if (fl&(AUTOINF_HP_PLUS)) txt+=" sich aendern"; else txt+=" fallen";
    if (fl&(AUTOINF_SP_PLUS|AUTOINF_SP_MINUS)) txt+=" oder";
  }
  if (fl&(AUTOINF_SP_PLUS|AUTOINF_SP_MINUS)) {
    txt+=" die Konzentrationspunkte";
    if (fl&(AUTOINF_SP_PLUS)) txt+=" sich aendern"; else txt+=" fallen";
  }
  write(break_string(txt+".\n",78));
  return 1;
}

private void DoNotifyHpChange() {
  object *obs,ob;
  string str;
  int i;

  autoinf_time=time();
  str="";
  obs=m_indices(autoinf_hp);
  for (i=sizeof(obs)-1;i>=0;i--) {
    if (!objectp(ob=obs[i])) continue;
    if (str!="") str+=", ";
    str+=sprintf("%s: %d LP",ob->name(WER),autoinf_hp[ob]);
    if (member(autoinf_sp,ob))
      str+=sprintf(" / %d KP",autoinf_sp[ob]);
    m_delete(autoinf_sp,ob);
  }
  obs=m_indices(autoinf_sp);
  for (i=sizeof(obs)-1;i>=0;i--) {
    if (!objectp(ob=obs[i])) continue;
    if (str!="") str+=", ";
    str+=sprintf("%s: %d KP",ob->name(WER),autoinf_sp[ob]);
  }

  if (str!="" && IsMember(leader))
    tell_object(leader,break_string(capitalize(str)+"\n",78));
  autoinf_hp=([]);
  autoinf_sp=([]);
}

void NotifyHpChange() {
  mixed act,old;
  int change;

  if (!IsMember(PO) || !pointerp(act=hp_info[PO]))
    return;
  old=act[0..];
  act=GetHpInfo(PO);change=0;
  if (!autoinf_flags) return;
  if (((autoinf_flags&AUTOINF_HP_MINUS) && act[0]<old[0]) ||
      ((autoinf_flags&AUTOINF_HP_PLUS)  && act[0]>old[0]))
    autoinf_hp[PO]=act[0],change=1;
  if (((autoinf_flags&AUTOINF_SP_MINUS) && act[2]<old[2]) ||
      ((autoinf_flags&AUTOINF_SP_PLUS)  && act[2]>old[2]))
    autoinf_sp[PO]=act[2],change=1;

  if (autoinf_time<time() || (change && (autoinf_flags&AUTOINF_INSTANT)))
    DoNotifyHpChange();
}

int TeamCmd(string cmd, string arg) {
  if (!IsMember(TP)) {
    notify_fail("Du bist kein Teammitglied.\n");
    if (TP->QueryProp(P_TEAM)==ME)
      TP->SetProp(P_TEAM,0);
    return 0;
  }
  switch(cmd) {
  case "angriff":
    return StartAttack();
  case "angriffsbefehl":
    if (stringp(arg))
      attack_cmd[TP]=arg;
    else
      m_delete(attack_cmd,arg);
    return 1;
  case "autofolge":
  case "autof":
    if (arg=="ein" || arg=="an")
      autofollow[TP]=1;
    else
      m_delete(autofollow,TP);
    return 1;
  case "autoi":
  case "autoinf":
  case "autoinfo":
  case "autoinform":
    return ChangeAutoInfo(arg);
  case "entlasse":
    return RemoveMember(arg);
  case "farben":
    return ChangeColors(arg);
  case "fluchtreihe":
  case "flucht":
    return ChangeWimpyRow(arg);
  case "formation":
    if (TP!=leader)
      return notify_fail("Nur der Teamleiter kann die Formation aendern.\n"),0;
    return ChangeFormation(arg);
  case "hist":
  case "history":
    return Hist(arg);
  case "":
  case "info":
    return ShowTeamHP(arg);
  case "leiter":
  case "leiterin":
  case "leitung":
    return ChangeLeader(arg);
  case "name":
    return ChangeName(arg);
  case "orte":
    return ShowTeamRooms(arg);
  case "kampfreihe":
  case "reihe":
    return ChangeRow(arg);
  case "ruf":
  case "rufe":
    return Shout(arg);
  case "verlasse":
    TP->SetProp(P_TEAM,0);
    return RemoveMember(TP);
  default:;
  }
  return 0;
}

void reset() {
  RemoveSingles();
  TryRemove();
}

#include <properties.h>
#include <defines.h>
#include <moving.h>
#include <language.h>

#define NEED_PROTOTYPES
#include <doorroom.h>

inherit "/std/thing";

#pragma strong_types

private mapping door_status;
private int id_zaehler;
// Format: "filename1:filename2":status

int oeffnen (string str);

mapping QueryAllDoors() {return door_status;}

int QueryDoorStatus(string dest) {
  string source,dkey;
  object env;

  if (!dest) return 0;
  if (!objectp(env=previous_object())) return 0;
  source=object_name(env);
  dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
  return door_status[dkey];
}

void SetDoorStatus(string dest, int x) {
  string source,dkey;
  object env;

  if (!dest) return;
  if (!objectp(env=previous_object())) return;
  source=object_name(env);
  dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
  door_status[dkey]=x;
}

protected void create() {
  door_status=([]);
  id_zaehler=0;
  if (IS_CLONE(this_object())) remove();
}

varargs int remove(int silent) {return 0;}

varargs int NewDoor(string|string* cmds, string dest, string|string* ids,
                    mapping|<int|string|string*>* props)
{
  object env;
  string source,dkey,sec_id;
  mixed *info2,zw;
  mapping info;
  int i;

  if (!objectp(env=previous_object())) return 0;
  if (!cmds || !dest) return 0;
  if (!ids) ids=({"tuer"});
  if (stringp(cmds)) cmds=({cmds});
  if (stringp(ids)) ids=({ids});
  if (!pointerp(cmds) || !pointerp(ids)) return 0;
  if (dest[0..0]!="/") dest="/"+dest;
  id_zaehler++;
  sec_id=sprintf("secure_id:door%d",id_zaehler);

  info=([D_DEST   : dest,
	 D_CMDS   : cmds,
	 D_IDS    : ({ sec_id }) + ids,
	 D_FLAGS  : (DOOR_CLOSED|DOOR_RESET_CL),
         D_LONG   : "Eine Tuer.\n",
	 D_SHORT  : "Eine %se Tuer. ",
	 D_NAME   : "Tuer",
	 D_GENDER : FEMALE]);
  source=object_name(env);
  dkey=(source<dest)?(source+":"+dest):(dest+":"+source);

  if (pointerp(props)) {
    for (i=0;i<sizeof(props)-1;i+=2)
      if (intp(props[i]) && props[i]>=D_MINPROPS && props[i]<=D_MAXPROPS)
        info[props[i]]=props[i+1];
  }
  else if(mappingp(props)){
    info += props;
  }

  if (!door_status[dkey]) {
    // Nur initialisieren, wenn Tuer noch nicht existiert.
    if (info[D_FLAGS] & DOOR_OPEN)
        door_status[dkey]=D_STATUS_OPEN;
    if (info[D_FLAGS] & DOOR_CLOSED)
        door_status[dkey]=
          ((info[D_FLAGS] & DOOR_NEEDKEY) ? D_STATUS_LOCKED : D_STATUS_CLOSED);
  }
  info2=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info2))
    info2=({info});
  else
    info2+=({info});
  env->SetProp(P_DOOR_INFOS,info2);

  if(mappingp(info[D_LONG]) && pointerp(zw=m_indices(info[D_LONG])) &&
     sizeof(zw) && intp(zw[0]))
    env->AddSpecialDetail(ids,"special_detail_doors");
  else if(info[D_LONG] && info[D_LONG]!="")
    env->AddDetail(ids,info[D_LONG]);

  return 1;
}

void init_doors () {
  object env;
  mixed *info;
  string source,dest,dkey;
  int i;

  if (!objectp(env=previous_object())) return;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return;

  source=object_name(env);
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
    env->set_doors(info[i][D_CMDS],(door_status[dkey]>0));
  }
}

string look_doors () {
  object env;
  mixed *info, ds;
  string source,dkey,dest,res;
  int i, st;

  if (!objectp(env=previous_object())) return "";
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return "";
  init_doors(); // Aktueller Zustand soll auch bei den Exits angezeigt werden

  source=object_name(env);res="";
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
    if (stringp(info[i][D_SHORT])) {
      res+=sprintf(info[i][D_SHORT],
		    ((door_status[dkey]>=D_STATUS_OPEN)?"geoeffnet":
			 ((door_status[dkey]!=D_STATUS_CLOSED)?"ab":"")+"geschlossen"));
	  if (sizeof(res) && res[<1]!='\n')
		res+=" ";
	}
    else if(mappingp(ds=info[i][D_SHORT])){
      st=door_status[dkey];
      if((st==D_STATUS_LOCKED) && !ds[D_STATUS_LOCKED])
        st=D_STATUS_CLOSED;
      if(stringp(ds[st])){
        res += ds[st];
        if(sizeof(ds[st]) && res[<1]!='\n')res+=" ";
      }
    }
  }
  if (res && res!="")
    return break_string(res,78,0,1);
  return res;
}

static void door_message(object room, string dname, int dgender, string msg) {
  object ob;

  // printf("%O,%O,%O,%O\n",room,dname,dgender,msg);
  if (!room || !dname || !msg) return;
  ob=this_object();
  ob->SetProp(P_NAME,dname);
  ob->SetProp(P_GENDER,dgender);
  ob->SetProp(P_ARTICLE,1);
  tell_room(room,capitalize(ob->name(WER))+msg+"\n");
}

static void door_message_other(string source, string dest, string msg) {
  mixed info;
  object env;
  int i;

  // printf("%O %O %O\n",source,dest,msg);
  if (!source || !dest || !msg) return;
  if (!objectp(env=find_object(dest))) return;
  if (!pointerp(info=env->QueryProp(P_DOOR_INFOS))) return;
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    if (info[i][D_DEST]!=source) continue; // Andere Tuer zu diesem Raum
    door_message(env,info[i][D_NAME],info[i][D_GENDER],msg);
  }
}

void reset_doors () {
  object env;
  mixed *info;
  string source,dest,dkey,msg;
  int i,j;

  if (!objectp(env=previous_object())) return;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return;

  source=object_name(env);
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);

// 	if (door_status[dkey]>-2 && door_status[dkey]<2) {
// 	  if (door_status[dkey]>0)
// 		door_status[dkey]=2;
// 	  else
// 		door_status[dkey]=-2;
// 	  continue; // nur jeder 2. Reset wird ausgefuehrt.
// 	}

    if (info[i][D_FLAGS] & DOOR_RESET_CL) {
      // Tuer muss bei Reset geschlossen werden
      if (door_status[dkey] >= D_STATUS_OPEN) {
        if(!msg=info[i][D_RESET_MSG])msg=" schliesst sich.";
        door_message(env,info[i][D_NAME],info[i][D_GENDER],msg);
        door_message_other(source,dest,msg);
      }
	  if (door_status[dkey]!=D_STATUS_LOCKED)
		door_status[dkey]=
      ((info[i][D_FLAGS] & DOOR_NEEDKEY) ? D_STATUS_LOCKED : D_STATUS_CLOSED);
    }
    if (info[i][D_FLAGS] & DOOR_RESET_OP) {
      // Tuer muss bei Reset geoeffnet werden
      if (door_status[dkey] < D_STATUS_OPEN) {
        if(!msg=info[i][D_RESET_MSG])msg=" oeffnet sich.";
        door_message(env,info[i][D_NAME],info[i][D_GENDER],msg);
        door_message_other(source,dest,msg);
      }
      door_status[dkey]=D_STATUS_OPEN;
    }
  }
  init_doors();
}

static varargs int exec_func2(string dest, mixed func) {
  if (!stringp(dest) || !stringp(func)) return 1;
  call_other(dest,func);
  return 1;
}

varargs int go_door (string str) {
  object env,pl,ob;
  mixed *info;
  string source,dest,dkey;
  int i;

  if (!str) return 0;
  if (!objectp(env=previous_object())) return 0;
  if (!objectp(pl=this_player())) return 0;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return 0;

  source=object_name(env);
  ob=this_object();
  ob->SetProp(P_ARTICLE,1);
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    if (member(info[i][D_CMDS],str)<0) continue;
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
    ob->SetProp(P_NAME,info[i][D_NAME]);
    ob->SetProp(P_GENDER,info[i][D_GENDER]);
    notify_fail(capitalize(ob->name(WER,1))+" ist geschlossen.\n");
    if((door_status[dkey]<=0) &&
       !info[i][D_OPEN_WITH_MOVE]) continue; // Tuer geschlossen
    if(door_status[dkey]<=0){
      // In diesem Fall versuchen, die Tuer zu oeffnen.
      oeffnen(info[i][D_IDS][0]);
    }
    notify_fail(capitalize(ob->name(WER,1))+" ist geschlossen.\n");
    if (door_status[dkey]<=0) continue; // Tuer immer noch zu.
    if (stringp(info[i][D_TESTFUNC]))
      if (call_other(env,info[i][D_TESTFUNC]))
		return 1; // Durchgang von der Tuer nicht erlaubt.
    if (stringp(info[i][D_FUNC]))
      call_other(env,info[i][D_FUNC]);
	if (stringp(info[i][D_MSGS])) {
	  if (pl->move(dest,M_GO,info[i][D_MSGS])>0)
		return exec_func2(dest,info[i][D_FUNC2]);
	  else
		return 1;
	}
	if (pointerp(info[i][D_MSGS]) && sizeof(info[i][D_MSGS])>=3) {
	  if (pl->move(dest,M_GO,info[i][D_MSGS][0],
				   info[i][D_MSGS][1],info[i][D_MSGS][2]))
		return exec_func2(dest,info[i][D_FUNC2]);
	  else
		return 1;
	}
    if (pl->move(dest,M_GO,"nach "+capitalize(str)))
	  return exec_func2(dest,info[i][D_FUNC2]);
    return 1;
  }
  return 0;
}

int oeffnen (string str) {
  object env,schl,ob;
  mixed *info,s2;
  string source,dkey,dest,s1;
  int i;

  notify_fail("WAS willst Du oeffnen?\n");
  if (!str || !this_player()) return 0;
  notify_fail("Das kannst Du nicht oeffnen.\n");
  str=lower_case(str);
  if (sscanf(str,"%s mit %s",s1,s2)!=2)
    {s1=str;s2=0;}
  if (s2) {
    if (!(schl=present(lower_case(s2),this_player()))) {
      notify_fail("So einen Schluessel hast Du nicht.\n");
      return 0;
    } else {
      s2=schl->QueryDoorKey();
	  if (stringp(s2)) s2=({s2});
    }
  }

  if (!objectp(env=previous_object())) return 0;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return 0;

  source=object_name(env);
  ob=this_object();
  ob->SetProp(P_ARTICLE,1);
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    if (member(info[i][D_IDS],s1)<0) continue; // Falsche Tuer
    ob->SetProp(P_NAME,info[i][D_NAME]);
    ob->SetProp(P_GENDER,info[i][D_GENDER]);
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
    notify_fail(capitalize(ob->name(WER,1))+" ist doch schon geoeffnet!\n");
    if (door_status[dkey]>0)
      continue; // Eine andere Tuer koennte gemeint sein.
    if ((info[i][D_FLAGS] & DOOR_NEEDKEY)
        && door_status[dkey] == D_STATUS_LOCKED) { // abgeschlossen
      notify_fail("Du brauchst einen Schluessel, um "+ob->name(WEN,1)+" zu oeffnen.\n");
      if (!schl) continue; // Eine andere Tuer koennte gemeint sein.
      notify_fail(capitalize(schl->name(WER))+" passt nicht!\n");
      if (!pointerp(s2)) continue;
	  if (member(s2,dkey)<0) continue; // Koennte an einer anderen passen.
    }
    door_status[dkey]=1;
    init_doors();
    write("Du oeffnest "+ob->name(WEN)+".\n");
    say(capitalize(ob->name(WER))+" wird von "+this_player()->name(WEM)+
        " geoeffnet.\n");
    door_message_other(source,dest," wird von der anderen Seite geoeffnet.");
    return 1;
  }
  return 0;
}

int schliessen (string str) {
  object env,schl,ob;
  mixed *info,s2;
  string source,dkey,dest,s1;
  int i,abg;

  notify_fail("WAS willst Du schliessen?\n");
  if (!str || !this_player()) return 0;
  notify_fail("Das kannst Du nicht schliessen.\n");
  str=lower_case(str);
  if (sscanf(str,"%s mit %s",s1,s2)!=2)
    {s1=str;s2=0;}
  if (s2) {
    if (!(schl=present(lower_case(s2),this_player()))) {
      notify_fail("So einen Schluessel hast Du nicht.\n");
      return 0;
    } else {
      s2=schl->QueryDoorKey();
	  if (stringp(s2)) s2=({s2});
    }
  }

  if (!objectp(env=previous_object())) return 0;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return 0;

  source=object_name(env);
  ob=this_object();
  ob->SetProp(P_ARTICLE,1);
  abg=0;
  for (i=sizeof(info)-1;i>=0;i--) {
    if (!mappingp(info[i])) continue;
    if (member(info[i][D_IDS],s1)<0) continue; // Falsche Tuer
    ob->SetProp(P_NAME,info[i][D_NAME]);
    ob->SetProp(P_GENDER,info[i][D_GENDER]);
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
	if (schl) {
	  notify_fail(capitalize(ob->name(WER,1))+" ist doch schon abgeschlossen!\n");
	  if (door_status[dkey]<=0
        && door_status[dkey] == D_STATUS_LOCKED)
      continue; // Eine andere Tuer koennte gemeint sein.
	  if (info[i][D_FLAGS] & DOOR_CLOSEKEY) { // Schluessel noetig?
		  notify_fail(capitalize(schl->name(WER))+" passt nicht!\n");
		  if (!pointerp(s2)) continue;
		  if (member(s2,dkey)<0) continue; // Koennte an einer anderen passen.
		  door_status[dkey]=D_STATUS_LOCKED;
		  abg=1;  // Tuer wird richtig abgeschlossen
	  }
    else {
		  // ohne Schluessel abschliessbar
		  door_status[dkey]=
        ((info[i][D_FLAGS] & DOOR_NEEDKEY) ? D_STATUS_LOCKED : D_STATUS_CLOSED);
	  }
	}
  else {
	  notify_fail(capitalize(ob->name(WER,1))+" ist doch schon geschlossen!\n");
	  if (door_status[dkey] < D_STATUS_OPEN)
		  continue; // Eine andere Tuer koennte gemeint sein.
	  if ((info[i][D_FLAGS] & DOOR_NEEDKEY) && 
        !(info[i][D_FLAGS] & DOOR_CLOSEKEY))
		  door_status[dkey]=D_STATUS_LOCKED; // Abschliessbar, aber dazu schluessel unnoetig
	  else
		  door_status[dkey]=D_STATUS_CLOSED;
	}
	init_doors();
	write("Du schliesst "+ob->name(WEN)+
		  (abg?" ab":"")+".\n");
	say(capitalize(ob->name(WER))+" wird von "+this_player()->name(WEM)+
		(abg?" ab":" ")+"geschlossen.\n");
	door_message_other(source,dest," wird von der anderen Seite "+
					   (abg?"ab":"")+"geschlossen.");
    return 1;
  }
  return 0;
}
string special_detail_doors(string key){
  object env;
  mixed *info;
  mapping dl;
  int i, st;
  string source, dest, dkey;

  if(!objectp(env=previous_object()))return 0;
  if(!pointerp(info=env->QueryProp(P_DOOR_INFOS)))return 0;
  source=object_name(env);

  for(i=sizeof(info)-1;i>=0;i--){
    if(!mappingp(info[i]))continue;
    if(member(info[i][D_IDS],key)<0)continue;
    if(stringp(info[i][D_LONG]))return info[i][D_LONG];
    if(!mappingp(info[i][D_LONG]))continue;
    dl=info[i][D_LONG];
    dest=info[i][D_DEST];
    dkey=(source<dest)?(source+":"+dest):(dest+":"+source);
    st=door_status[dkey];
    if((st==D_STATUS_LOCKED) && !dl[D_STATUS_LOCKED])
        st=D_STATUS_CLOSED; /* Falls keine eigene Beschreibung
                             * fuer abgeschlossene Tuer vorhanden */
    return dl[st];
  }
  return 0;
}

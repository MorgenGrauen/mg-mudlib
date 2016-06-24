// 18.Dez  1996 - Loco@Morgengrauen
// 8. Feb  1995 - Jof@MorgenGrauen
// 5. Juli 1992 - Jof@MorgenGrauen
// 6. Juli 1992 - Jof@MorgenGrauen
// Clear-Groups-Mechanismus eingebaut. Die 2 Konstanten TIME_TO_CLEAR und
// MIN_TOUCHED muessen def. sein. Alle TIME_TO_CLEAR Sekunden werden die
// Newsgroups, die seit dem letzten Clear seltener als MIN_TOUCHED Sekunden
// beruehrt wurden, aus dem Cache genommen

// 1. Februar 1995 - Jof@MorgenGrauen
// Rewrite (Mappings benutzen, Accessinfos im Speicher halten)


// Datenstrukturen:

// Newsgroups:  grouplist ist ein Mapping mit einem Eintrag pro Newsgroup.
//              Die Keys sind die Gruppennamen, Daten:
//              0 Zeit des letzen Artikels
//              1 Besitzer der Gruppe
//              2 Savefille-Name
//              3 Expire-Zeit
//              4 Array mit Loesch-Berechtigten
//              5 Array mit Scheib-Berechtigten
//              6 Array mit Leseberechtigten
//              7 Mindest-Level, um Artikel zu loeschen
//              8 Mindest-Level, um Artikel zu posten
//              9 Mindest-Level, um Artikel zu lesen
//             10 Max. Anzahl Nachrichten in der Gruppe

// Die Nachrichten selber stehen in einem Array.

// Eine nachricht ist ein Array mit den folgenden Elementen:
// 0         (*) string writer;
// 1         (*) string id;    Mudname+":"+time()+":"+group zur Zeit
// 2         (*) string time;
// 3             string title;
// 4             string message;

// Die mit (*) gekennzeichneten Eintraege setzt der Daemon selber

// Funktionen:
//    Returnvalues:  0 = Parameter error
//                   1 = Ok.
//                  -1 = no permission
//                  -2 = no such group/group already existing
//
//    Diese Returnvalues stehen fuer alle Funktionen

//  AddGroup(groupname, owner, savefile);
//   Funktion klar, kann nur von Erzmagiern aufgerufen werden
//    -3 = no owner, -4 = savefile already in use
//
//  RemoveGroup(groupname);
//   Ebenfalls nur fuer Erzmagier
//
//  SetGroup(groupname, dlevel, wlevel, rlevel, maxmessages, expire);
//   Erzmagier und der Groupowner koennen die nutzen. Legt Level, ab dem je-
//   mand aus der Gruppe loeschen kann (dlevel), in die Gruppe schreiben
//   kann (wlevel), die Gruppe lesen kann (rlevel) und die max. Anzahl Nach-
//   richten in der Gruppe fest.
//
//  AddAllowed(groupname, deleters, writers, readers);
//   Erzmagier/Owner koennen Arrays mit Namen von fuer die Gruppe loesch/
//   schreib/lese-Berechtigten fuer die Gruppe angeben.
//
//  RemoveAllowed(groupname, deleters, writers, readers);  analog
//
//  WriteNote(message); klar
//   -3 = Max number of msgs exceeded
//
//  RemoveNote(boardname, notenummer); notenummer>=0;
//   -3 = No such note
//
//  GetNotes(boardname); gibt einen Array mit den Notes zurueck.
//
//  AskAllowedWrite(); return wie bei WriteNote, stellt fest, ob ein Player
//  eine Note aufhaengen darf oder nicht
//
//  GetNewsTime([boardname]); gibt zurueck, wann am entsprechenden Brett zum
//  letzten Mal eine Note befestigt wurde. Falls kein boardname angegeben
//  wird, liefert GetNewsTime() den Zeitpunkt, zu dem ueberhaupt eine neue
//  Note aufgehaengt wurde.
//
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#include "/secure/wizlevels.h"
#include <defines.h>
#include <config.h>
#include <news.h>

#define WTIME 0

private int security( string name );

mixed saveload; // Diese Variable ist als einzige nicht nosave ist und dient
                 // Uebertragen von Daten in/aus savefiles

nosave mapping grouplist; // Groups und ihre save-Files, zudem LastTime
nosave int lasttime; // Um zu vermeiden, dass 2 Notes identische Uhrzeit==id
                     // haben, wird die jeweils letzte Zeit gespeichert.

nosave mapping cache = ([]); // cache fuer die Gruppeninhalte

void create() {
  seteuid(getuid(this_object()));
  if (!restore_object(NEWSPATH+"GroupList"))
    grouplist=m_allocate(0,G_MESSAGES);
  else
    grouplist=saveload;
  // ersten reset sobald wie moeglich. ;-)
  set_next_reset(1);
}

int AddGroup(string name, string owner)
{
  mixed *group;
  string savefile, *savefilea;
  int i;

  if (!name || !owner) return 0;
  
  if (!ARCH_SECURITY || process_call()) return -1; // Darf nicht

  if (member(grouplist, name)) return -2; // Gibt es schon

  if (file_size("/"+SAVEPATH+owner[0..0]+"/"+owner+".o")<0) return -3;

  savefilea = old_explode(name,".");
  savefile = implode(savefilea,"/");
  if (file_size(NEWSPATH+savefile+".o")>=0) return -4;

  // Notwendige Directories anlegen
  for (i = 0; i < sizeof(savefilea)-1; i++) {
    mkdir(NEWSPATH+implode(savefilea[0..i],"/"));
  }

  group=({});
  grouplist+=([name:0;owner;savefile;-1;({});({});({});20;0;0;80]);
  save_group_list();
  save_group(name,group);
  return 1;
}

int RemoveGroup(string name)
{
  int num;

  if (!name) return 0;

  if (!security(name) || process_call()) return -1; // Darf nicht

  if (!mappingp(grouplist) || !member(grouplist,name))
    return -2; // -2 no such group

  catch(rm(NEWSPATH+grouplist[name,G_SAVEFILE]+".o");publish);
  m_delete(grouplist,name);

  save_group_list();

  return 1;
}

int SetGroup(string name,int dlevel,int        wlevel,int rlevel,int maxmessages,int expire)
{
  mixed *group;
  
  if (!member(grouplist,name)) return -2;
  if (grouplist[name,G_OWNER]!=user_euid() &&
      (!security(name) || process_call())) return -1;
  
  grouplist[name,G_DLEVEL]=dlevel;
  grouplist[name,G_WLEVEL]=wlevel;
  grouplist[name,G_RLEVEL]=rlevel;
  grouplist[name,G_MAX_MSG]=maxmessages;
  grouplist[name,G_EXPIRE]=expire;
  
  save_group_list();
  return 1;
}

int AddAllowed(string name,mixed deleters,mixed writers,mixed readers)
{
  mixed *group;

  if (!member(grouplist,name)) return -2;

  if ( grouplist[name,G_OWNER]!=user_euid() &&
       (!security(name) || process_call()) && user_euid() != ROOTID )
      return -1;

  if (stringp(deleters)) deleters=({deleters});
  if (stringp(writers)) writers=({writers});
  if (stringp(readers)) readers=({readers});

  if (!deleters) deleters=({});
  if (!writers) writers=({});
  if (!readers) readers=({});

  grouplist[name,G_DELETERS]+=deleters;
  grouplist[name,G_WRITERS]+=writers;
  grouplist[name,G_READERS]+=readers;
        
  save_group_list();
  return 1;
}

int RemoveAllowed(string name,mixed deleters,mixed writers,mixed readers)
{
  mixed *group;

  if (!member(grouplist,name)) return -2;

  if (grouplist[name,G_OWNER]!=user_euid() &&
      (!security(name) || process_call()) && user_euid() != ROOTID )
      return -1;

  if (stringp(deleters)) deleters=({deleters});
  if (stringp(writers)) writers=({writers});
  if (stringp(readers)) readers=({readers});

  if (!deleters) deleters=({});
  if (!writers) writers=({});
  if (!readers) readers=({});

  grouplist[name,G_DELETERS]-=deleters;
  grouplist[name,G_WRITERS]-=writers;
  grouplist[name,G_READERS]-=readers;

  save_group_list();
  return 1;
}

static string user_euid()
{
  if (previous_object()) {
     if (geteuid(previous_object())==ROOTID)       return ROOTID;
     if (geteuid(previous_object())=="p.daemon")   return "p.daemon";
     if (load_name(previous_object())=="/obj/mpa") return geteuid(RPL);
  }
  return secure_euid();
}

#define F_DELETE    0
#define F_READ      1
#define F_WRITE     2
#define F_ADMIN     3
#define F_KEEPNAME  4

private int security( string name )
{
    if ( grouplist[name,G_DLEVEL] >= ARCH_LVL
         || grouplist[name,G_WLEVEL] >= ARCH_LVL
         || grouplist[name,G_RLEVEL] >= ARCH_LVL )
        return ARCH_SECURITY;
    else
        return ELDER_SECURITY;
}

static int allowed(string name, int mode)
{
  string euid;
  mixed g_level, g_mode;
  
  if (process_call()) return 0;

  euid=user_euid();

  if (euid==ROOTID) return 1;

  switch(mode) {
    case F_KEEPNAME: return (euid=="p.daemon");
    case F_WRITE:    if (euid=="p.daemon") return 1;
                     g_level=G_WLEVEL; g_mode=G_WRITERS;  break;
    case F_ADMIN:    if (!(security(name)||grouplist[name,G_OWNER]==euid)) 
                             return 0;
                     g_level=G_DLEVEL; g_mode=G_DELETERS; break;
    case F_DELETE:   if (euid=="p.daemon") return 1;
                     g_level=G_DLEVEL; g_mode=G_DELETERS; break;
    case F_READ:     g_level=G_RLEVEL; g_mode=G_READERS;  break;
    default:         return 0;
  }

  if (grouplist[name,G_OWNER] != euid && !ARCH_SECURITY &&
      grouplist[name,g_level] > query_wiz_level(euid) &&
      member(grouplist[name, g_mode], euid)==-1)
    return 0; // No such group for the requestor :)
  return 1;
}

int WriteNote(mixed message,mixed keepname)
{
  mixed *group;
  int uidok,tmp;
  string name;

  if (!pointerp(message) || sizeof(message)!=6) return 0;

  if (!pointerp(group=load_group(name=message[M_BOARD]))) return -2;

  if (!allowed(name, F_WRITE)) return -1;

  if (sizeof(group)>=grouplist[name,G_MAX_MSG]) return -3;

  if (!keepname || !allowed(name, F_KEEPNAME))
     message[M_WRITER]=capitalize(geteuid(this_interactive()||previous_object()));

  if (lasttime>=time()) lasttime++;
    else lasttime=time();
  message[M_TIME]=lasttime;
  message[M_ID]=MUDNAME+":"+lasttime;
  group+=({message});
  grouplist[name,WTIME]=lasttime;
  save_group(name,group);
  save_group_list();
  return 1;
}

int RemoveNote(string name, int note)
{
  int num;
  mixed *group;

  if ((note<0) && (name=="dwnews"))
  {
    group=({});
    grouplist[name,WTIME]=0;
    save_group(name,group);
    save_group_list();
    return 1;
  }

  if (note<0) return 0;

  if (!pointerp(group=load_group(name))) return -2;

  int count=sizeof(group);
  if (count<=note)
    return -3;

  if (!allowed(name, F_DELETE) &&
      lower_case(group[note][M_WRITER])!=user_euid()) return -1;

  if (count==1)
    group=({});
  else if (!note)
    group = group[1..];
  else if (note == count-1)
    group = group[0..<2];
  else
    group=group[0..note-1]+group[note+1..];
  
  if (sizeof(group))
    grouplist[name,WTIME]=group[<1][M_TIME];
  else
    grouplist[name,WTIME]=0;
  save_group(name,group);
  save_group_list();
  return 1;
}

mixed GetNotes(string name)
{
  mixed *group;
  
  if (!pointerp(group=load_group(name))) return -2;
  if (!allowed(name, F_READ)) return -2;
  return(deep_copy(group)); // COPY it
}

static void dump_file(string filename,mixed news)
{
  int i;
  
  for (i=0;i<sizeof(news);i++)
    write_file(filename,news[i][M_TITLE]+" ("+news[i][M_WRITER]+", "+
               dtime(news[i][M_TIME])[5..26]+"):\n"+
               news[i][M_MESSAGE]+"\n-----------------------------------------------------------------------------\n\n\n\n");
}

protected varargs void expire(string grp,int etime)
// etime ist anfangs in Tagen und bezeichnet das max. Alter, was Artikel in
// der Gruppe haben duerfen.
{
  mixed *group;

  if (!pointerp(group=load_group(grp))) return;
  if (etime)
  {
    if (etime>0)
      etime=etime*60*60*24;
  }
  else
    etime=grouplist[grp,G_EXPIRE]; 
  if (etime<=0)
    return;

  int to_expire=time()-etime;
  int size=sizeof(group);
  if (!size) return;

  int first_to_keep = size;  // ja, ist noch eins zu hoch
  // solange runterlaufen bis man ein element findet, welches geloescht werden
  // soll. first_to_keep bleibt dann eins hoeher als das.
  while ( first_to_keep && group[first_to_keep-1][M_TIME]>to_expire)
    --first_to_keep;
  // first_to_keep kann jetzt auf eins hinter dem letzten Element zeigen (==
  // size). Das wird unten beruecksichtigt.

  if (!first_to_keep) // alle behalten?
    return;
  // Zu loeschende Artikel wegschreiben.
  dump_file("news/OLD."+grp,group[0..first_to_keep-1]);
  // dann loeschen
  if (first_to_keep == size) // alle wegwerfen?
    group=({});
  else
    group=group[first_to_keep..size-1];
  
  save_group(grp,group);
}

void dump_group(string grp)
{
  int to_expire,size,last;
  mixed *group;

  if (!ARCH_SECURITY || process_call()) return;
  if (!pointerp(group=load_group(grp))) return;
  size=sizeof(group);
  last=size;
  if (!last) return;
  dump_file("news/DUMP."+grp,group[0..last-1]);
}

protected void expire_all(string *keys) {
  // neuen call_out fuer den Rest setzen
  if (sizeof(keys) > 1)
    call_out(#'expire_all,15,keys[1..]);
  // und erste Gruppe expiren
  expire(keys[0]);
}

void reset() {
  // naechstes Expire und damit Reset in einem tag
  set_next_reset(86400);
  // alte call_outs ggf. entfernen.
  while(remove_call_out(#'expire_all)>=0);
  // gruppenliste holen und callout auf expire_all starten
  if (sizeof(grouplist)) {
    call_out(#'expire_all,10,m_indices(grouplist));
  }
}

static void save_group(string grp,mixed group)
{
  saveload=group; // Do NOT save the accessed-Info
  cache[grp] = group;
  save_object(NEWSPATH+grouplist[grp,G_SAVEFILE]);
  saveload=0;
}

static void save_group_list()
{
  saveload=grouplist;
  save_object(NEWSPATH+"GroupList");
  saveload=0;
}

static mixed load_group(string name)
{
  int num;
  mixed *ret;

  if(!member(grouplist,name)) return -1;

  if (member(cache, name)) {
    ret = cache[name];
  }
  else {
    restore_object(NEWSPATH+grouplist[name,G_SAVEFILE]);
    if (!pointerp(saveload))
      saveload=({});
    ret=saveload;
    cache[name] = saveload;
    saveload=0;
  }
  return ret;
}

mixed GetGroups()
{
  mixed *returnlist;
  int i,group,slevel;
  string seuid;

  returnlist=sort_array(m_indices(grouplist),#'>); //');
  if (ARCH_SECURITY && !process_call())
    return returnlist;

  seuid = user_euid();
  slevel = secure_level();

  for (i=sizeof(returnlist)-1;i>=0;i--)
    if (!(grouplist[returnlist[i],G_RLEVEL]<=slevel ||
          grouplist[returnlist[i],G_OWNER]==seuid ||
          member(grouplist[returnlist[i],G_READERS], seuid)!=-1))
      returnlist=returnlist[0..i-1]+returnlist[i+1..];
  return returnlist;
}

int AskAllowedWrite(string n)
{
  mixed *group;

  if (!member(grouplist,n)) return -2;
  if (!pointerp(group=load_group(n))) return -2;

  if (grouplist[n,G_OWNER] != user_euid() &&
      !ARCH_SECURITY &&
      grouplist[n,G_WLEVEL]>secure_level() &&
      member(grouplist[n,G_WRITERS],user_euid())==-1)
    return -1;

  if (sizeof(group)>=grouplist[n,G_MAX_MSG]) return -3;
  return 1;
}

// Wichtig ...

int query_prevent_shadow()
{
  return 1;
}

mixed GetNewsTime(string boardname)

{
  int i, ltime, j;
  mixed *keys;

  if (!boardname)
  {
    ltime=-1;
    for (i=sizeof(keys=m_indices(grouplist))-1;i>=0;i--)
      if (ltime<(j=grouplist[keys[i],WTIME])) ltime=j;
    return ltime;
  }
  if (!member(grouplist,boardname)) return -1;
  return grouplist[boardname,WTIME];
}

mixed* GetGroup(string name)
{
  if (process_call()) return 0;
  if (extern_call() && !allowed(name, F_ADMIN)) return 0;
#define gl(x) grouplist[name,x]
  return ({name,gl(1),gl(2),gl(3),gl(4),gl(5),gl(6),gl(7),gl(8),gl(9),gl(10),load_group(name)});
}

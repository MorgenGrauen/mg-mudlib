#ifndef __ZESSTRA_MERKZETTEL_H__
#define __ZESSTRA_MERKZETTEL_H__

#define SAVEFILE(x) ("/players/"+x+"/.merkzettel")
#define ARCHIVE(x) ("/players/"+x+"/merkzettel.archiv")

#define NOTE_TEXT 0
#define NOTE_PRIO 1
#define NOTE_STARTTIME 2
#define NOTE_DEPS 3
#define NOTE_HELPER 4
#define NOTE_STATUS 5
#define NOTE_CLOSETIME 6

//Status, >0 sind aktiv, <0 inaktiv
#define NOTE_ACTIVE 1
#define NOTE_PENDING 2
#define NOTE_INACTIVE -1
#define NOTE_FINISHED -2
#define NOTE_STATES ({NOTE_ACTIVE,NOTE_PENDING,NOTE_INACTIVE,NOTE_FINISHED})

//Sortierreihenfolge
#define SORT_INVERSE 1

//wieviel wird max.abgespeichert
#define MAX_NOTE_LENGTH 1000
//wieviel wird max. in der Liste angezeigt
#define MAX_NOTE_LIST_LENGTH 50

#define BS(x) break_string(x,78)
#define BSI(x,y) break_string(x,78,y)
#define BSL(x) break_string(x,78,"",BS_LEAVE_MY_LFS)
#define BSIL(x,y) break_string(x,78,y,BS_LEAVE_MY_LFS)

#define TI this_interactive()

#ifdef NEED_PROTOTYPES
//AddCmd
int AddNotiz(string str);
int ErsetzeText(string str);
int ErgaenzeText(string str);
int RemoveNotiz(string str);
int FinishNotiz(string str);
int ListNotizen(string str);
int LiesNotiz(string str);
int ChangeDep(string str);
int ChangeStatus(string str);
int ChangePrio(string str);
int ChangeHelper(string str);
int ZeigeZettel(string str);
int WedelZettel(string str);
int LiesDeps(string str);
int Expire(string str);

//von den AddCmd-Funs gerufen
static string _LiesNotiz(int id);

//sonst. Funktionen, z.B. fuer Debugzwecke
mapping QueryNotizen();
string QueryOwner();
varargs int remove(int silent);

//int. Funktionen
static status sort_prio(int key1, int key2);
static nomask status check_allowed();
protected int restore_me();
varargs protected void save_me(int delay);
static string query_autoloadobj();
static string set_autoloadobj(mixed arg);
varargs static int checkStatus(int nocache);
static string update_deps(string str,int id);
static int note_filter(int id,int filterstate);
static int aelter_als(int id,int zeit);
static int *getUnresolvedDeps(int id);
static int *getDeps(int id,int rec);

#endif // NEED_PROTOTYPES

#endif // __ZESSTRA_MERKZETTEL_H__

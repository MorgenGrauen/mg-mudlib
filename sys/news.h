// MorgenGrauen MUDlib
//
// news.h -- Definitionen fuer die Zeitung
//
// $Id: news.h 8809 2014-05-08 19:52:48Z Zesstra $

#define G_NAME 0
#define G_OWNER 1
#define G_SAVEFILE 2
#define G_EXPIRE 3
#define G_DELETERS 4 
#define G_WRITERS 5
#define G_READERS 6
#define G_DLEVEL 7
#define G_WLEVEL 8
#define G_RLEVEL 9
#define G_MAX_MSG 10
#ifdef TESTING
#define G_MAX_NUM 11
#define G_MESSAGES 12
#else
#define G_MESSAGES 11
#endif

#define M_BOARD 0
#define M_WRITER 1
#define M_ID 2
#define M_TIME 3
#define M_TITLE 4
#define M_MESSAGE 5

static string user_euid();
static void save_group_list();
static void save_group(string grp,mixed group);
static mixed load_group(string name);
               
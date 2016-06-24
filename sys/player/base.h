// MorgenGrauen MUDlib
//
// player/base.h -- player base header
//
// $Id: base.h 8487 2013-05-21 19:15:52Z Zesstra $

#ifndef __PLAYER_BASE_H__
#define __PLAYER_BASE_H__

// da dieses File vom Master inkludiert wird und dann die Standard-Include-
// verzeichnisse noch nicht gesetzt sind, darf dieses File auch nur die abs.
// Namen verwenden. *seufz*
#include "/sys/player/can.h"
#include "/sys/player/telnetneg.h"

#define HAUSVERWALTER   "/d/seher/haeuser/hausverwalter"

// properties

#define P_KILLS              "playerkills"

#define P_ZAP_MSG            "zap_msg"
#define P_AWAY               "away"
#define P_NO_TOPLIST         "no_toplist"
#define P_CALLED_FROM_IP     "called_from_ip"
#define P_MARRIED            "married"
#define P_SIBLINGS           "siblings"
#define P_RACE_DESCRIPTION   "racedescr"
#define P_RACESTRING         "racestring"

#define P_WAITFOR            "waitfor"
#define P_WAITFOR_REASON     "waitfor_reason"
#define P_WAITFOR_FLAGS      "waitfor_flags"
#define P_VISUALBELL         "visualbell"
#define P_LOCALCMDS          "localcmds"
#define P_CLOCKMSG           "clockmsg"
#define P_TIMEZONE           "timezone"
#define P_SHOWEMAIL          "showemail"

#define P_LAST_LOGIN         "last_login"
#define P_LAST_LOGOUT        "last_logout"
#define P_DAILY_PLAYTIME     "daily_playtime"
#define P_IGNORE             "ignore"
#define P_SHOW_EXITS         "show_exits"
#define P_WANTS_TO_LEARN     "wants_to_learn"
#define P_AUTOLOADOBJ        "autoloadobj"
#define P_AUTOLOAD           "autoload"
#define P_MAILADDR           "mailaddr"
#define P_HOMEPAGE           "homepage"
#define P_ICQ                "icq"
#define P_MESSENGER          "messenger"
#define P_LOCATION           "location"

#define P_FOLLOW_SILENT      "follow_silent"

#ifndef P_INVIS
#define P_INVIS              "invis"
#endif

#define P_SECOND             "second"
#define P_SECOND_MARK        "second_mark"
#define P_SECOND_LIST        "second_list"
#define P_MUD_NEWBIE         "_lib_mud_newbie"  // not played a mud before?

#define P_TESTPLAYER         "testplayer"
#define P_ALLOWED_SHADOW     "allowed_shadow"

#define P_START_HOME         "start_home"
#define P_PRAY_ROOM          "_lib_p_prayroom"

#define P_SHELL_VERSION      "shell_version"

#define P_CMSG               "clonemsg"
#define P_DMSG               "destmsg"
#define P_CLONE_MSG          "clone_msg"
#define P_DESTRUCT_MSG       "destruct_msg"

#define P_CARRIED_VALUE      "carried"

#define P_PROMPT             "prompt"

#define P_SCREENSIZE         "screensize"
#define P_MORE_FLAGS         "more_flags"

#define P_NO_ASCII_ART       "no_ascii_art"

#define P_LAST_QUIT          "last_quit"

#define P_READ_NEWS          "read_news"

#define P_NEEDED_QP          "needed_qp"

//TODO: Remove - Property is not needed any more.
#define P_TELNET_KEEP_ALIVE  "send_telnet_keep_alive"

#endif // __PLAYER_BASE_H__

#ifdef NEED_PROTOTYPES

#ifndef __PLAYER_BASE_H_PROTO__
#define __PLAYER_BASE_H_PROTO__

// prototypes

void smart_log(string myname, string str);
int QueryGuest();
int invis(string inform);
int vis(string inform);

varargs static void stop_heart_beats(mixed obs);
static void restart_heart_beats();

nomask int query_hc_play();
varargs nomask void set_hc_play(string str, int val);
string SetDefaultHome(string str);
protected string SetDefaultPrayRoom(string str);

void save_me(mixed value_items);
nomask mixed query_real_name();
protected void call_notify_player_change(int rein);

#endif // __PLAYER_BASE_H_PROTO__

#endif// NEED_PROTOYPES

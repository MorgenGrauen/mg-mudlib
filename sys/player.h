// MorgenGrauen MUDlib
//
// player.h -- player object defines
//
// $Id: player.h 7386 2010-01-22 14:59:05Z Zesstra $

#ifndef _PLAYER_H_
#define _PLAYER_H_

#ifdef NEED_PROTOTYPES
#undef NEED_PROTOTYPES                                  // we need only the properties here!!
#define PLAYER_TMP_NEED_PROTOTYPES
#endif

#include "/sys/player/base.h"
#include "/sys/player/description.h"
#include "/sys/player/moving.h"
#include "/sys/player/filesys.h"
#include "/sys/player/potion.h"
#include "/sys/player/quest.h"
#include "/sys/player/skills.h"
#include "/sys/player/viewcmd.h"
#include "/sys/player/comm.h"
#include "/sys/player/fao.h"
#include <player/user_filter.h>
#include <daemon.h>

#ifdef PLAYER_TMP_NEED_PROTOTYPES
#undef PLAYER_TMP_NEED_PROTOTYPES
#define NEED_PROTOTYPES
#endif

#define P_LAST_COMMAND_ENV      "last_command_env"
#define P_HISTMIN               "histmin"
#define P_SHOW_ALIAS_PROCESSING "show_alias_processing"
#define P_DEFAULT_NOTIFY_FAIL   "default_notify_fail"
#define P_NETDEAD_INFO          "netdead_info"
#define P_NETDEAD_ENV           "netdead_env"
#define P_IP_NAME               "ip_name"
#define P_LAST_KILLER           "last_killer"
#define P_ACTUAL_NOTIFY_FAIL    "actual_notify_fail"
#define P_LEP                   "lep"
#define P_NEWBIE_GUIDE          "newbie_guide"

#define DeclareAutoload(name,param) \
  mlfp_create(({P_AUTOLOAD,name}),param)

#define UndeclareAutoload(name) \
  mlfp_remove(({P_AUTOLOAD}),name)

#define QueryAutoload(name) \
  mlfp_query(({P_AUTOLOAD,name}))

#ifdef NEED_PROTOTYPES
varargs string _unparsed_args(int level);
#endif

#endif

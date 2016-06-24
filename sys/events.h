// MorgenGrauen MUDlib
//
// /sys/events.h -- Standard-Event-ID und Prototypen der Mudlib
//
// $Id$

#ifndef __EVENTS_H__
#define __EVENTS_H__

#define EVENTD "/p/daemon/eventd"

// Standardlib
#define EVT_LIB_LOGIN               "evt_lib_login"
#define EVT_LIB_LOGOUT              "evt_lib_logout"
#define EVT_LIB_PLAYER_DEATH        "evt_lib_player_death"
#define EVT_LIB_PLAYER_DELETION     "evt_lib_player_deletion"
#define EVT_LIB_PLAYER_CREATION	    "evt_lib_player_creation"
#define EVT_LIB_NPC_DEATH(x)        ("evt_lib_npc_death_"+x)
#define EVT_LIB_ADVANCE             "evt_lib_advance"
#define EVT_LIB_QUEST_SOLVED        "evt_lib_quest_solved"
#define EVT_LIB_MINIQUEST_SOLVED    "evt_lib_miniquest_solved"
#define EVT_LIB_PLAYER_ATTR_CHANGE  "evt_lib_player_attr_change"
#define EVT_LIB_CLOCK               "evt_lib_clock"
#define EVT_LIB_DATECHANGE          "evt_lib_datechange"
#define EVT_LIB_NEW_ERROR           "evt_lib_eventd_new_error"

// Gilden
#define EVT_GUILD_CHANGE        "evt_guild_change"
#define EVT_GUILD_ADVANCE       "evt_guild_advance"


// Konstanten fuer Event-Daten. s. Manpages der einzelnen Events, die
// Bedeutungen dieser Keys koennen sich jeweils leicht unterscheiden!
#define E_TIME            "evt_time"
#define E_OBJECT          "evt_object"
#define E_OBNAME          "evt_object_name"
#define E_ENVIRONMENT     "evt_environment"
#define E_PLNAME          "evt_player_name"
#define E_EXTERNAL_DEATH  "evt_external_death"
#define E_POISON_DEATH    "evt_poison_death"
#define E_CORPSE          "evt_corpse"
#define E_GUILDNAME       "evt_guild_name"
#define E_LAST_GUILDNAME  "evt_last_guild_name"
#define E_QUESTNAME       "evt_quest_name"
#define E_QP_GRANTED      "evt_qp_granted"
#define E_MINIQUESTNAME   "evt_miniquest_name"
#define E_MQP_GRANTED     "evt_mqp_granted"

#endif // __EVENTS_H__

#ifdef NEED_PROTOTYPES

#ifndef __EVENTS_PROTOTYPES_H_
#define __EVENTS_PROTOTYPES_H_
        int CheckEventID(string eid);
        int RegisterEvent(string eid, string fun, object ob);
        int UnregisterEvent(string eid, object ob);
varargs int TriggerEvent(string eid, mixed args);

#endif // __EVENTS_PROTOTYPES_H_


#endif // NEED_PROTOTYPES


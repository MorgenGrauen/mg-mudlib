// MorgenGrauen MUDlib
//
// npc.h -- Definitonen fuer NPCs
//
// $Id: npc.h 6709 2008-02-02 22:14:08Z Zesstra $

#ifndef _NPC_H_
#define _NPC_H_

#define DEFAULT_INFO "\ndefault info"
#define DEFAULT_NOINFO "\ndefault noinfo"
#define P_PRE_INFO           "npc_pre_info"
#define P_NPC_INFO           "npc_info"

#define P_NPC                "is_npc"

#define P_HB                 "hb"
#define P_INFO               "info"

#define P_AGGRESSIVE         "aggressive"

#define P_SPELLS             "spells"
#define P_SPELLRATE          "spellrate"

#define P_CHATS              "chats"
#define P_CHAT_CHANCE        "chat_chance"
#define P_ACHATS             "achats"
#define P_ACHAT_CHANCE       "achat_chance"

#define P_REJECT             "reject"

#define P_DEFAULT_INFO       "default_info" // Standardantwort fuer Fragen
#define P_LOG_INFO           "log_info"

#define CLONE_NEW   0x10000000  /* VERALTET! */
#define CLONE_WEAR  0x20000000  /* anziehen */
#define CLONE_WIELD 0x40000000  /* zuecken */
#define CLONE_NO_CHECK 0x08000000  /* zuecken oder anziehen ohne checks */

#define NPC_NEEDS_ITEM_INIT "npc:need item init"

#endif

#ifdef NEED_PROTOTYPES
#ifndef _NPC_H_PROTOTYPES_
#define _NPC_H_PROTOTYPES_
// aus npc/info.c
public varargs void    AddInfo(mixed key, mixed info, string indent, 
                               mixed silent, mixed casebased);
public varargs void    AddSpecialInfo(mixed keys, string functionname, 
                          string indent, mixed silent, mixed casebased );
public int             frage(string str);
public int             do_frage(string text);
public void            RemoveInfo( string key );
public varargs mixed   GetInfo(string str);
// aus npc/items.c
public varargs object  AddItem(mixed filename, int refresh, mixed props);
#endif // _NPC_H_PROTOTYPES_
#endif // NEED_PROTOTYPES


// MorgenGrauen MUDlib
//
// npc/chat.c -- Labernde NPCs
//
// $Id: chat.c 6801 2008-03-21 23:34:46Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone

#include <properties.h>
#define NEED_PROTOTYPES
#include <living/combat.h>
#include <thing/properties.h>
#undef NEED_PROTOTYPES

#define ME this_object()

/*
 * Some simple chat variables
 */

/*
 * heart_beat is called so the monster may chat.
 */
void SetChats(int chance, mixed strs) {
  if (!pointerp(strs))
    return;
  SetProp(P_CHAT_CHANCE,chance);
  SetProp(P_CHATS,strs);
}

void SetAttackChats(int chance, mixed strs) {
  if (!pointerp(strs))
    return;
  SetProp(P_ACHAT_CHANCE,chance);
  SetProp(P_ACHATS,strs);
}

#define SR_CHAT_MSG 0
#define SR_CHAT_TYP 1

protected void process_chat(mixed strs) {
  if (!pointerp(strs) || !sizeof(strs))
    return;

  int msg_typ;
  mixed entry = strs[random(sizeof(strs))];

  if(pointerp(entry) && sizeof(entry)>=2 && intp(entry[SR_CHAT_TYP])) {
    msg_typ = entry[SR_CHAT_TYP];
    entry = entry[SR_CHAT_MSG];
  }

  if(closurep(entry))
    entry = funcall(entry, &msg_typ);

  // Falls wir in der Chat-Closure zerstoert wurden, nichts weiter tun.
  if (!objectp(ME))
    return;

  if (msg_typ)
    msg_typ |= MSG_DONT_STORE|MSG_DONT_BUFFER;

  entry = process_string(entry);

  // Falls wir durch process_string() zerstoert wurden oder keine Meldung
  // (mehr) existiert, nichts weiter tun.
  if (!objectp(ME) || !stringp(entry) || !sizeof(entry))
    return;

  send_room(environment(),
            entry,
            msg_typ||(MT_LOOK|MT_LISTEN|MT_FEEL|MT_SMELL|
                      MSG_DONT_STORE|MSG_DONT_BUFFER|MSG_DONT_WRAP));
}

void DoAttackChat() {
  if (!ME || !environment(ME))
    return;
  if (QueryProp(P_DISABLE_ATTACK)>0)return ;
  if (random(100) < QueryProp(P_ACHAT_CHANCE))
    process_chat(QueryProp(P_ACHATS));
}

void DoChat() {
  if (!ME || !environment(ME))
    return;
  if (random(100) < QueryProp(P_CHAT_CHANCE))
    process_chat(QueryProp(P_CHATS));
}

protected void heart_beat()
{
  if( InFight() ) DoAttackChat();
  else            DoChat();
}

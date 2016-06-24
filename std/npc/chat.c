// MorgenGrauen MUDlib
//
// npc/chat.c -- Labernde NPCs
//
// $Id: chat.c 6801 2008-03-21 23:34:46Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

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

void DoAttackChat() {
  string* c;
  if (!ME || !environment(ME))
    return;
  if (QueryProp(P_DISABLE_ATTACK)>0)return ;
  if (random(100) < QueryProp(P_ACHAT_CHANCE))
    if ((c = QueryProp(P_ACHATS)) && sizeof(c)) 
      tell_room(environment(ME),
		process_string(c[random(sizeof(c))]));
}

void DoChat() {
  string *c;
  if (!ME || !environment(ME))
    return;
  if (random(100) < QueryProp(P_CHAT_CHANCE))
    if ((c = QueryProp(P_CHATS)) && sizeof(c)) 
      tell_room(environment(ME),
		process_string(c[random(sizeof(c))]));
}

protected void heart_beat()
{
  if( InFight() ) DoAttackChat();
  else            DoChat();
}

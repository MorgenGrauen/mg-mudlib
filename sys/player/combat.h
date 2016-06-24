// MorgenGrauen MUDlib
//
// player/combat.h -- prototypes for player specific issues of combat
//
// $Id: comm.h,v 3.3 2000/02/09 15:56:25 Padreic Exp $

#ifndef __PLAYER_COMBAT_H__
#define __PLAYER_COMBAT_H__

public int QueryPlAttacked(object pl);

public int RegisterHelperNPC(object npc, int flags);
public int UnregisterHelperNPC(object npc);

#endif // __PLAYER_COMBAT_H__


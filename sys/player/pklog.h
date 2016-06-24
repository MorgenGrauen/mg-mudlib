// MorgenGrauen MUDlib
//
// player/pklog.h -- module for logging player vs player attacks
//
// $Id: comm.h,v 3.3 2000/02/09 15:56:25 Padreic Exp $

#ifndef __PLAYER_PKLOG_H__
#define __PLAYER_PKLOG_H__

nomask int CheckArenaFight(object victim);
nomask protected int CheckPlayerAttack(object attacker, object victim, 
                                       string angriffsmsg);

#endif // __PLAYER_PKLOG_H__


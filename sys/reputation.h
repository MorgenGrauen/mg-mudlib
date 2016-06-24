#ifndef _REPUTATION_H_
#define _REPUTATION_H_

#define REPMASTER        "/secure/repmaster"
#define REP_SAVEFILE     "/secure/ARCH/repmaster"

// Grenzwerte
#define REP_MAXIMUM      10000
#define REP_MINIMUM     -10000

// Zwischenstufen
#define REP_TRUSTED      7500
#define REP_LIKED        5000
#define REP_FAMILIAR     2500
#define REP_NEUTRAL      0
#define REP_UNFAMILIAR  -2500
#define REP_DISLIKED    -5000
#define REP_DISTRUSTED  -7500

// Returncodes
#define REP_RET_WRONGARGS  -1 // Falsche Argumente fuer ChangeRep()
#define REP_RET_INVALIDUID -2 // Unzulaessige UID
#define REP_RET_ALREADYMAX -3 // Reputation bereits Max / Min
#define REP_RET_INACTIVE   -4 // Reputation auf inaktiv gesetzt
#define REP_RET_INVALIDREP -5 // Reputation nicht vorhanden
#define REP_RET_SUCCESS     1 // Reputation wurde veraendert
#define REP_RET_SUCCESSCUT  2 // Reputation wurde auf Min / Max veraendert

// Flags fuer die Eintraege im Master
#define REP_FLAG_ACTIVE     1

#endif

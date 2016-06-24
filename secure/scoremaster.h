// MorgenGrauen MUDlib
//
// scoremaster.h -- Definitionen fuer den ScoreMaster
//
// $Id: scoremaster.h 6633 2007-12-06 21:49:02Z Zesstra $

#ifndef _SCOREMASTER_H_
#define _SCOREMASTER_H_ 1

#define SCOREMASTER "/secure/scoremaster"

#define SCORESAVEFILE    "/secure/ARCH/npcmaster"
#define SCOREDUMPFILE    "/secure/ARCH/SCORES.dump"
#define SCORELOGFILE     "/secure/ARCH/SCORES.LOG"
#define SCOREAUTOLOG     "/secure/ARCH/SCORES_AUTO.LOG"
#define SCORECHECKFILE   "/secure/ARCH/SCORES.kaputt"
#define EKCLEANLOG       "/secure/ARCH/EKCLEANER.LOG"
#define WERKILLTWEN      "/secure/ARCH/WERKILLTWEN.LOG"

// wer ist fuer die EKs aktuell zustaendig?
#define SCOREMAINTAINERS ({"arathorn", "zesstra"})

// Elemente des von QueryScore() zurueckgegebenen Arrays
#define SCORE_NUMBER	0   // (Bit-)Nummer des Objekts
#define SCORE_SCORE	1   // Zahl der Stufenpunkte
#define SCORE_KEY	2   // Filename des Obkekts

// Indizes fuer das Mapping npcs
#define NPC_NUMBER      0   // (Bit-)Nummer des EKs
#define NPC_SCORE       1   // Zahl der Stufenpunkte
#define NPC_COUNT       2   // wie oft der EK vergeben wurde
// Indizes fuer das Mapping by_num
#define BYNUM_KEY       0   // Filename des Objekts
#define BYNUM_SCORE     1   // Zahl der Stufenpunkte

#define SCORE_INVALID_ARG   -1
#define SCORE_NO_PERMISSION -2

#define SCORE_LOW_MARK	200000
#define SCORE_HIGH_MARK 600000

#define P_NO_SCORE "no_score"

// moegliche Werte fuer 'flag' bei TestScore()
#define SCORE_KILL  0x01
#define SCORE_QUEST 0x02

#define EK_GIVENLOG(x) log_file("ARCH/EK_GIVEN", \
        dtime(time())+" "+x+"\n",100000)

// ein paar defines fuer ektips
#define EKTIPS_MAX_RETRY		10
// one tip per level in list, above top entry one tip every level
#define EKTIPS_LEVEL_LIMITS 	({60,65,70,74,78,82,85,88,91,93,95,97})
#define EKTIPGIVER "/d/ebene/muadib/dragon/npc/brumni"
#define EKTIPLIST  "/d/ebene/muadib/dragon/obj/eklist"

#endif

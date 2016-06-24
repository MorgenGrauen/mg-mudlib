#ifndef _WIZLEVELS_
#define _WIZLEVELS_
/*
 * wizlevels.h
 *
 * This file defines some useful macros to determine the level of
 * a wizard in terms of ability to do certain wizard things.
 * See also doc/concepts/levels. The grouping below is taken from there.
 *
 */
#include "/secure/config.h"

#define	SEER_LVL 1
#define	LEARNER_LVL 10
#define	WIZARD_LVL 20
#define DOMAINMEMBER_LVL 25
#define SPECIAL_LVL 30
#define	LORD_LVL 40
#define ELDER_LVL 50
#define	ARCH_LVL 60
#define GOD_LVL 100

#define SEER_GRP 1
#define LEARNER_GRP 2
#define WIZARD_GRP 3
#define DOMAINMEMBER_GRP 4
#define SPECIAL_GRP 5
#define LORD_GRP 6
#define ELDER_GRP 7
#define ARCH_GRP 8

/* user is an object which is to be checked */
/* for example, use IS_WIZARD(this_player()) to check */
/* if a player is a wizard. */
#define IS_SEER(user) (query_wiz_level(user) >= SEER_LVL)
#define IS_LEARNER(user) (query_wiz_level(user) >= LEARNER_LVL)
#define IS_SPECIAL(user) (query_wiz_level(user) >= SPECIAL_LVL)
#define IS_WIZARD(user) (query_wiz_level(user) >= WIZARD_LVL)
#define IS_DOMAINMEMBER(user) (query_wiz_level(user) >= DOMAINMEMBER_LVL)
#define IS_DEPUTY(user) (master()->IsDeputy(user))
#define IS_LORD(user) (query_wiz_level(user) >= LORD_LVL)
#define IS_ELDER(user) (query_wiz_level(user) >= ELDER_LVL)
#define IS_ARCH(user) (query_wiz_level(user) >= ARCH_LVL)
#define IS_GOD(user) (query_wiz_level(user) >= GOD_LVL)

#define IS_LEARNING(user) (query_wiz_level(user) >= LEARNER_LVL &&\
                           user->QueryProp(P_WANTS_TO_LEARN))
#define IS_LORD_DOMAIN(user,domain) (master()->domain_master(user,domain))

/*
 * Interface for enhanced security functions
 */
#define SEER_SECURITY (secure_level() >= SEER_LVL)
#define LEARNER_SECURITY (secure_level() >= LEARNER_LVL)
#define WIZARD_SECURITY (secure_level() >= WIZARD_LVL)
#define DOMAINMEMBER_SECURITY (secure_level() >= DOMAINMEMBER_LVL)
#define SPECIAL_SECURITY (secure_level() >= SPECIAL_LVL)
#define LORD_SECURITY (secure_level() >= LORD_LVL)
#define ELDER_SECURITY (secure_level() >= ELDER_LVL)
#define ARCH_SECURITY (secure_level() >= ARCH_LVL)
#define GOD_SECURITY (secure_level() >= GOD_LVL)

#endif

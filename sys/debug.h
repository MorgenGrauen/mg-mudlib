// MorgenGrauen MUDlib
//
// debug.h -- Debug-Optionen fuer die MUDlib
//
// $Id: debug.h,v 3.1 1997/02/12 13:04:59 Wargon Exp $

#ifndef _DEBUG_
#define _DEBUG_


/*
 * LPC debug options
 */


#define NODEBUG			/* all debugging modes disabled */

#ifndef NODEBUG

#define DEBUG_STD_CREATE        /* debug object creation in /std objects */
/* #define DEBUG_PROPERTIES	/* some property debug functions */
/* #define DEBUG_ID		/* echo the object id operation */
#define DEBUG_PICK		/* object pickup */
#define DEBUG_CMDS		/* command debugging for init */
#define DEBUG_DOMAIN		/* debug domain files */
#define DEBUG_DOOR		/* debug doors */
#define WARN_UNDEFPROP		/* warnings if undef properties are set */
#define DEBUG_EVENTS		/* debug event handler call */
#define DEBUG_MOVE		/* debug in object moving */

#define DEBUG_RES		/* debug restriction handling */
#define DEBUG_RES_MAX		/* debug restriction |res_max| */
#define DEBUG_RES_LIMIT		/* debug restriction |res_limit| */
#define DEBUG_RES_PLAYER	/* restriction of number of something */
/* #define DEBUG_FUNCTION	/* debug message functions */
/* #define DEBUG_ARRAY		/* debug array and member comparison */
#define DEBUG_PROPAGATION	/* debug background propagation mechanism */
#define DEBUG_RES_GROUND	/* debug ground type restrictions */
#define DEBUG_FEATURES	        /* debug feature mechanism */
#define DEBUG_RES_FEATURES	/* debug feature mechanism */
#define DEBUG_VIEW		/* debug players vision */
#define DEBUG_PROPAGATION	/* debug background propagation mechanism */
#define DEBUG_MSG		/* message debugging */
#define DEBUG_SELECT		/* debug selection mechanism */
#define DEBUG_RESOURCES		/* debug living resources */
#define DEBUG_ISME		/* debug the is_me operation */
#define DEBUG_FOLLOW		/* debug follow mode for walkers */
#define DEBUG_RESOURCES2	/* debug living resources */
#define ACCESS_DEBUG 1		/* debug of access control */
#define DEBUG_SAVE		/* debug loading and saving of objects */
#define DEBUG_BOARD		/* debug board system */
#define DEBUG_ASEE		/* debug "to see" ability */
#define DEBUG_SPELLS		/* debug spells */
#define DEBUG_BATTLE		/* debug battle system */

#endif



#endif


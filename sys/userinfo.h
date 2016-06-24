// MorgenGrauen MUDlib
//
// userinfo.h -- Daten im Spieler-Cache (siehe /secure/master/userinfo.c)
//
// $Id: userinfo.h 6207 2007-02-18 23:32:35Z Zesstra $

#ifndef _USERINFO_
#define _USERINFO_

/*
 * userinfo: what each field of /secure/PASSWD is
 */

/* password (encrypted) */
#define USER_PASSWORD 0

/* wizard level (or programming level) (0 for simple users) */
#define USER_LEVEL 1

/* domain (if any) master == *domainname */
#define USER_DOMAIN 2

/* player object to be loaded by /secure/login */
#define USER_OBJECT 3

/* time of first login */
#define USER_CREATION_DATE 4

/* Time userinfo has been touched last */
#define USER_TOUCH 5

/* exploration points of player */
#define USER_EP 6

/* npcs killed by player */
#define USER_EK 7

/* miniquests solved by player */
#define USER_MQ 8

/* guilds (if any) master == *guildname */
#define USER_GUILD 9 

/* tips for eks given to player */
#define USER_EKTIPS 10

/* tips for fps given to player */
#define USER_FPTIPS 11

/* UIDs fuer den ein Magier u.U. explizit verantwortlich ist */
#define USER_UIDS_TO_TAKE_CARE 12

#endif

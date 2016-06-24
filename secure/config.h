#ifndef _CONFIG_
#define _CONFIG_

/*
 * config.h
 *
 * general configuration is done here, the selection of gamedriver mode
 * and the language to use.
 */

#define MUDHOST "mg"
#define FTPD_IP "87.79.24.60"
#define UDPSERV "87.79.24.60"

// Guess a MUDNAME if none is set.
#ifndef MUDNAME
// set mudname to MorgenGrauen if Mud started on the MG server
#  if MUDHOST == __HOST_NAME__ && !defined(__TESTMUD__)
#    define MUDNAME "MorgenGrauen"
#  else
#    define MUDNAME "MG-Homemud"
#  endif
#endif

#if MUDNAME == "MorgenGrauen"
#  define SSLPORT 4712
#else
#  define SSLPORT 4714
#endif

// undef to disable MSSP support.
#define MSSP_SUPPORT "MSSP-Plaintext"

// undef to disable support for pure-ftpd virtual users
#define _PUREFTPD_

#define _MUDLIB_NAME_ "MorgenGrauen"
#define _MUDLIB_VERSION_ "3.3.5"

/* define general pathnames */
#define MASTER          "secure/master"
#define MAILPATH        "mail/"
#define SAVEPATH        "save/"
#define NEWSPATH        "news/"
#define NEWSSERVER      "secure/news"
#define SECURESAVEPATH  "secure/save/"
#define COMBAT_MASTER   "secure/combat"

#define WIZARDDIR       "players"
#define DOMAINDIR       "d"
#define PROJECTDIR      "p"
#define DOCDIR          "doc"
#define GUILDDIR        "gilden"
#define SPELLBOOKDIR    "spellbooks"
#define MAILDIR         "mail"
#define LIBSAVEDIR      "save"
#define FTPDIR          "open"
#define TMPDIR          "tmp"
#define STDDIR          "std"
#define SYSDIR          "sys"
#define LIBOBJDIR       "obj"
#define LIBROOMDIR      "room"
#define ETCDIR          "etc"
#define LIBLOGDIR       "log"
#define NEWSDIR         "news"
#define SECUREDIR       "secure"
#define LIBDATADIR      "data"
#define LIBITEMDIR      "items"

#define NETDEAD_ROOM "/room/netztot"
#define NETDEAD_CHECK_TIME 5

/* define special userids */ 
#define BACKBONEID " S T D "
#define ROOTID     " R O O T "			/* uppercase !! */
#define MAILID     " M A I L "
#define NEWSID     " N E W S "
#define NOBODY     "NOBODY"
#define ROOMID     "room"
#define POLIZEIID  "polizei"
#define DOCID      "DOC"
#define GUILDID    "GUILD"
#define ITEMID     "ITEMS"

// "Besondere" Magierlevel
#define WIZLVLS ([ ROOTID: 100,\
                   ROOMID: 21,\
                   POLIZEIID: 21,\
                   "alle": 25,\
                   NOBODY: 0,\
                   DOCID: 0,\
                   GUILDID: 30,\
                   ITEMID: 0,\
                 ])

#define MAX_LOG_SIZE 50000

#ifndef TESTMUD
#  define CALL_OUT_HARD 1200
#  define CALL_OUT_SOFT 1000
#else // !TESTMUD
#  ifdef MIN_CALL_OUT
#    define CALL_OUT_HARD 120
#    define CALL_OUT_SOFT 100
#  else
#    define CALL_OUT_HARD 120000
#    define CALL_OUT_SOFT 100000
#  endif
#endif // TESTMUD

#define BACKBONE_WIZINFO_SIZE 8
#define LIVING_NAME 3
#define NAME_LIVING 4
#define MEMORY_BUFF 5
#define NETDEAD_MAP 6
#define IP_NAMES    7


#ifndef SIMUL_EFUN_FILE
#define SIMUL_EFUN_FILE       "secure/simul_efun/simul_efun"
#endif
#ifndef SPARE_SIMUL_EFUN_FILE
#define SPARE_SIMUL_EFUN_FILE "secure/simul_efun/spare/simul_efun"
#endif

#define MAX_MAILS_PER_HOUR 200

//max. groesse von Mappings und Arrays sollten vom Driver oder der
//Kommandozeile vordefiniert sein. Wenn nicht:
#ifndef __MAX_MAPPING_KEYS__
#define __MAX_MAPPING_KEYS__ 30000
#endif
#ifndef __MAX_MAPPING_SIZE__
#define __MAX_MAPPING_SIZE__ 60000
#endif
#ifndef __MAX_ARRAY_SIZE__
#define __MAX_ARRAY_SIZE__ 10000
#endif

// Haben wir einen Fehlerdaemonen zu Speicher der Daten? Wenn ja, welchen?
// Der normale braucht Support fuer sqlite. Wenn der nicht existiert, lassen
// wir das mit dem Errord sein.
#ifdef __SQLITE__
#define ERRORD "/secure/errord.c"
#endif

// Savefile-Version
#define __LIB__SAVE_FORMAT_VERSION__ 1

#endif // _CONFIG_


// MorgenGrauen MUDlib
//
// questmaster.h -- header file for questmaster object
//
// $Id: questmaster.h 8261 2012-12-04 22:56:29Z Zesstra $

#ifndef __QUESTMASTER_H__
#define __QUESTMASTER_H__

//XP threshold  

#define XP_NEEDED_FOR_WIZ 1000000

// the questmaster
#define QM         "/secure/questmaster"

// the quests file
#define QUESTS        "/secure/ARCH/questmaster"

// Dumpfile for MiniQuests
#define MQ_DUMP_FILE  "/secure/ARCH/MINIQUESTS.dump"

// percentage of max_QP which is needed to become wizard
#define QP_PERCENT 80


// minimum of QP to become Seer or Wizzard
#define QP_MIN 1196


// log to file

#define QMLOG(x) write_file("/log/QUESTMASTER",dtime(time())+": "+\
        geteuid(this_interactive())+" -> "+ x +"\n")

#define MQMLOG(x) write_file("/log/ARCH/MQUESTMASTER", \
        dtime(time())+": "+(this_interactive()?geteuid(this_interactive()):"UNKNOWN")+\
	" -> "+ x +"\n")

#define MQSOLVEDLOG(x) write_file("/log/ARCH/MQ_SOLVED", \
        dtime(time())+" "+x+"\n")

// quest groups

#define QGROUP_1 8
#define QGROUP_2 18
#define QGROUP_3 29
#define QGROUP_S -1

#define QGROUPS ({0,QGROUP_1,QGROUP_2, QGROUP_3})


// Welcher Eintrag im Array steht fuer was?
#define Q_QP      0
#define Q_XP      1
#define Q_ALLOWED 2
#define Q_HINT    3
#define Q_DIFF    4
#define Q_CLASS   5
#define Q_ACTIVE  6
#define Q_WIZ     7
#define Q_SCNDWIZ 8
#define Q_ATTR    9
#define Q_AVERAGE 10

// Sternchen fuer die Klasse

#define QCLASS_STARS(n) funcall(lambda(  ({ 'x, 's }),  \
  ({#'while, ({#'>, 'x, 0}), \
             's, ({#', ,({#'=, 'x, ({#'-, 'x, 1}) }),  \
                        ({#'=, 's, ({#'+, 's, "*" }) }) \
     }) }) ), n, "") /* ' */ 


// Attribute der Quests
#define QATTR_FLEISSIG  1
#define QATTR_HEROISCH  2
#define QATTR_EPISCH    3
#define QATTR_LEGENDAER 4

#define QATTR_STRINGS ([0: "", \
                        QATTR_FLEISSIG: "fleissig", \
                        QATTR_HEROISCH:  "heroisch", \
                        QATTR_EPISCH: "episch", \
                        QATTR_LEGENDAER: "legendaer"])



// Rueckgabewerte Quests

#define OK              1

#define GQ_ALREADY_SET -1
#define GQ_KEY_INVALID -2
#define GQ_ILLEGAL_OBJ -3
#define GQ_IS_INACTIVE -4

#define DQ_NOT_SET     -1
#define DQ_KEY_INVALID -2
#define DQ_ILLEGAL_OBJ -3

#define QQ_GUEST       2
#define QQ_KEY_INVALID 0

#define ERRNO_2_STRING(x,y) (["GQ1" : "Ok.",\
			      "GQ-1": "Die Quest ist bereits geloest worden.",\
			      "GQ-2": "Ungueltiger Questname.",\
			      "GQ-3": "Unbefugter Zugriff.",\
			      "GQ-4": "Die Quest ist derzeit inaktiv.",\
			      "DQ1" : "Ok.",\
			      "DQ-1": "Die Quest war nicht gesetzt.",\
			      "DQ-2": "Ungueltiger Questname.",\
			      "DQ-3": "Unbefugter Zugriff.",\
			      "QQ1" : "Ok.",\
                              "QQ2" : "Gaeste koennen keine Quest loesen.",\
			      "QQ0" : "Ungueltiger Questname.",\
			      ])[x+y]


// Miniquests

#define MQ_ALREADY_SET   -1
#define MQ_KEY_INVALID   -2
#define MQ_ILLEGAL_OBJ   -3
#define MQ_IS_INACTIVE   -4
#define MQ_GUEST         -5


#endif /* __QUESTMASTER_H__ */




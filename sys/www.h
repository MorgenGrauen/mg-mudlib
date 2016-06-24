// MorgenGrauen MUDlib
//
// www.h -- World Wide Web Services
//
// $Id: www.h 7724 2011-01-05 22:32:26Z Zesstra $

// how do you spell your mud name
#ifndef MUDNAME
# define MUDNAME        "MorgenGrauen"
#endif

// Pathnames to different services
// INETD -- the inet daemon
#ifndef INETD
# define INETD          "/secure/inetd"
#endif

#define FINGER(n)               "/p/daemon/finger"->finger_single(n)
#define WHO             "/obj/werliste"->QueryWhoListe(0,0,1)

// UDPPATH -- wherethe udp programs are
#define UDPPATH         "/secure/udp"

// the WWW request scheduler
#define WWW             (UDPPATH+"/www")
#define TIMEOUT         5

// wher to log all WWW UDP Requests
#define WWWLOG "/WWW"                   // used in log_file()

// define MUDHOST, WWW SERVER address and the ICON URL
#ifndef MUDHOST
# define MUDHOST                "MG.Mud.DE"
#else
# undef MUDHOST
# define MUDHOST           "MG.Mud.DE"
#endif
#define MUDUSER         "mud"
#define SERVER          "mg.mud.de"
#define ICONS           "/icons"
#define MUDWWW          "/cgi-bin/mudwww"

// define the address of the MUD Host
#define HOMEPAGE        "http://"+SERVER+"/"
#define MUDMAIL         "mailto:"+MUDUSER+"@"+MUDHOST

// Request Syntax tokens
#define REQ     "REQ"           // TYPE of REQuest

// The WWW Newsreader
# define R_NEWS     "news"              // - we would like to read news
#  define GROUP       "GROUP"   //   * which newsgroup
#  define ARTICLE     "ARTICLE" //   * which article

// The WWW MUD Walker
# define R_WALK     "walk"              // - or maybe walk around a bit
#  define ROOM        "ROOM"    //   * ROOM to enter
#  define DETAIL      "DETAIL"  //   * DETAIL to examine

// The WWW Finger Request
# define R_FINGER   "finger"    // - finger someone
#  define USER        "USER"    //   * USER to retrieve info from

// The WWW Wholist Request
# define R_WHO      "who"               // - get the wholist

// The WWW Intermud Gateway
# define R_INTERMUD "intermud"  // - intermud communication request
#  define IMUD        "IMUD"    //   * The MUD to request info from
#  define TYPE        "TYPE"    //   * The TYPE of Request
#  define IDATA       "DATA"    //   * extra DATA to aid request
#  define PAGE        "PAGE"    //   * Alphabetical pages of muds

#define BACK    "BACK"          // PAGE to go BACK to

#ifndef MUDNAME
# define MUDNAME "TestMUD"
#endif /* MUDNAME */

// Property specific defines
#define P_WWWINFO      "wwwinfo"

// The error message to view in any case
#define ERROR(str)      "<H1>Ein Fehler ist aufgetreten!</H1><HR>"+str

// debugging (change name to your own!)
#define DNAME   "hate"
#define DEBUG(str) if(find_player(DNAME)) \
                     tell_object(find_player(DNAME), sprintf("%O\n", str))
//#undef DEBUG

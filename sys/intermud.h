#define INETD "/secure/inetd/inetd"

// ****** Defines for field names ****************
#define RECIPIENT       "RCPNT"
#define REQUEST         "REQ"
#define SENDER          "SND"
/* The DATA field should be used to store the main body of any packet. */
#define DATA            "DATA"
/* These headers are reserved for system use only. */
#define HOST            "HST"
#define ID              "ID"
#define NAME            "NAME"
#define PACKET          "PKT"
#define UDP_PORT        "UDP"
#define SYSTEM          "SYS"
#define VERSION         "V"
#define FLAGS           "F"
#define SIGNATURE       "S"

/* Reserved headers for diagnostics. */
#define PACKET_LOSS     "PKT_LOSS"
#define RESPONSE_TIME   "TIME"

/* --- Standard REQUESTs --- */
#define PING            "ping"
#define QUERY           "query"
#define REPLY           "reply"
#define HELO            "helo"

// packet properties
#define MAX_UDP_LENGTH 65536
// packet flags
#define FL_VALID_SIGNATURE 0x1

/* --- Index macros for host arrays in old-style host lists --- */
#define HOSTLIST_NAME       0
#define HOSTLIST_IP         1
#define HOSTLIST_UDP_PORT   2
#define HOSTLIST_LOCAL_COMMANDS  3
#define HOSTLIST_COMMANDS   4
#define HOSTLIST_STATUS     5
#define HOSTFILE_DEL        ":"
#define HOSTFILE_DEL2        ","

/* Mud / Admin email address. */
#define EMAIL        "mud@mg.mud.de"

// Public commands which are usually accessible.
#define DEFAULT_COMMANDS \
({ PING, QUERY, REPLY, "channel", "finger", "tell", "who", "mail",\
    "www", "htmlwho", "locate" })


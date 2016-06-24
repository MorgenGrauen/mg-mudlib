// MorgenGrauen MUDlib
//
// query.c
//
// $Id: query.c 7397 2010-01-26 21:48:11Z Zesstra $

#include <udp.h>
#include <udp_language.h>
#include <strings.h>

#ifdef ZEBEDEE
#include <system.h>
#elif !defined(INETD)
#define INETD        "/secure/inetd"
#endif

/* Mud / Admin email address. */
#define EMAIL        "mud@mg.mud.de"

void udp_query(mapping data) {
    mapping ret;

    switch(data[DATA]) {
        case "commands":
            ret = INETD->query("hosts");
            if (ret[lower_case(data[NAME])])
                ret = ([
                DATA: implode(ret[lower_case(data[NAME])][LOCAL_COMMANDS], ":")
                ]);
            else
                ret = ([ DATA: implode(INETD->query("commands"), ":") ]);
            break;
        case "email":
            ret = ([ DATA: EMAIL ]);
            break;
        case "hosts":
        {
            string tmp = "";
      foreach(string mudname, mixed fields: INETD->query("hosts")) {
        tmp += fields[HOST_NAME] + ":" +
             fields[HOST_IP] + ":" +
             fields[HOST_UDP_PORT] + ":" +
             implode(fields[LOCAL_COMMANDS], ",") + ":" +
             implode(fields[HOST_COMMANDS], ",") + "\n";
      }
            ret = ([ DATA: trim(tmp,TRIM_RIGHT, "\n") ]);
            break;
        }
        case "inetd":
            ret = ([ DATA: INETD_VERSION ]);
            break;
        case "list":
            /* List of thingsthat can be queried. */
            ret = ([ DATA: "commands:email:hosts:inetd:mud_port:time:version" ]);
            break;
        case "mud_port":
            ret = ([ DATA: query_mud_port() ]);
            break;
        case "time":
            ret = ([ DATA: time() ]);
            break;
        case "version":
            ret = ([ DATA: version() ]);
            break;
        default:
            /* Just ignore it for the time being. */
            return;
    }
    INETD->_send_udp(data[NAME], ret + ([
        REQUEST: REPLY,
        RECIPIENT: data[SENDER],
        ID: data[ID],
        "QUERY": data[DATA]
    ]) );
}

string send_request(string mudname, string prop)
{
  if (!stringp(mudname) || !stringp(prop))
    return 0;
  
  return (INETD->_send_udp(mudname,
                             ([REQUEST: "query",
                               DATA: prop,
                               SENDER: getuid(previous_object())]),1)||
          sprintf(INETD_QUERY_REQUESTED, prop, mudname));
}


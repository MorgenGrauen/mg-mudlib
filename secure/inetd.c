/*
 * UDP port handling code. Version 0.7a
 * Written by Nostradamus for Zebedee.
 * Developed from an original concept by Alvin@Sushi.
 */

#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
//#pragma pedantic
#pragma no_range_check
#pragma no_warn_deprecated

/*
#include <living/comm.h>
#define ZDEBUG(x)        if (find_player("zesstra"))\
            find_player("zesstra")->ReceiveMsg(x, \
                MT_DEBUG|MSG_DONT_STORE,0,"IM: ",this_object())
*/
#define ZDEBUG(x)

#include <udp.h>

/* --- Configurable definitions. --- */

/* CD muds will probably need these include file. */
/* #include <std.h> */
/* #include "/config/sys/local.h" */

/* Public commands which will be accessible to any unlisted muds.
 * PING, QUERY and REPLY are included by default. */
#define COMMANDS \
({ "channel", "finger", "tell", "who", "mail", "www", "htmlwho", "locate" })

//({ "channel", "finger", "ftp", "locate", "man", "tell", "who" })

/* Define this to the object that receives incoming packets and passes
 * them to the inetd. Undefine for no _receive_udp() security.
 * NOTE: The string must be of the format that object_name() returns. */
#define UDP_MASTER                __MASTER_OBJECT__

/* How to set the euid for this object if using native mode.
 * Ensure that it can read the INETD_HOSTS file. */
#define SET_EUID                seteuid(getuid())

/* Define this if you are running another intermud package concurrently. */
/* #define RECEIVE_UDP_COMPAT(sender, packet) \
                UDP_MANAGER->incoming_udp(sender, packet)        // CD */

/* Define this if you are running another intermud package concurrently and
 * have a compatability module for it. */
/* #define SEND_UDP_COMPAT(mudname, data, expect_reply) \
                "/secure/udp_compat"->_send_udp(mudname, data, expect_reply) */

/* The maximum number of characters we can send in one packet.
 * You may need to reduce this, but 512 should be safe. */
#define MAX_PACKET_LEN        1024

/* You shouldn't need to change anything below. */

#define USE_OLD_DELIMITER
//#define DELIMITER_COMPAT
#define USE_OLD_DATA_FORMAT

#ifdef ZEBEDEE
#include <defs.h>
#endif

#ifndef DATE
#define DATE                ctime(time())[4..15]
#endif

/* --- End of Config. Do not alter anything below. --- */

#define UNKNOWN                0
#define UP                time()
#define DOWN                (-time())

#define NEW_DELIMITER        "\n"
#ifdef USE_OLD_DELIMITER
#define DELIMITER        "|"
#else
#define DELIMITER        "\n"
#endif
#define OLD_DELIMITER        "|"
#define HOSTFILE_DEL        ":"
#define HOSTFILE_DEL2        ","
#define RETRY                "_RETRY"

private nosave mapping hosts, pending_data, incoming_packets;
private nosave mapping static_host_list;
private nosave string *received_ids;
private nosave int packet_id;

void set_host_list();
varargs string _send_udp(string mudname, mapping data, int expect_reply);

#define DEBUG(msg)

void my_create() {
#ifndef COMPAT_FLAG
    SET_EUID;
#endif
    packet_id = 0;
    pending_data = ([ ]);
    incoming_packets = ([ ]);
    hosts = ([ ]);
    received_ids = ({ });
    set_host_list();
    if (!this_player())
        call_out("startup", 1, 0);
}

#ifdef CREATE_FUN
CREATE_FUN() {
#elif !defined(COMPAT_FLAG) || defined(ZEBEDEE)
void create() {
#else
void reset(arg) {
    if (arg)
        return;
#endif
    my_create();
}

status check_system_field(mapping data, string field) {
    return data[SYSTEM] && member(data[SYSTEM], field) != -1;
}

mapping add_system_field(mapping data, string field) {
    if (data[SYSTEM]) {
        if (!check_system_field(data, field))
            data[SYSTEM] += ({ field });
    }
    else
        data[SYSTEM] = ({ field });
    return data;
}

private mapping read_host_list(string file) {

  mixed data = (read_file(file));
  if (!data) {
    log_file(INETD_LOG_FILE, "*Error in reading host file: "
        + file +"\n\n");
    return 0;
  }

  data = old_explode(data, "\n");
  mapping new_hosts = m_allocate(sizeof(data),1);

  foreach(string line: data) {
    if (line == "" || line[0] == '#')
      continue;
    mixed fields = old_explode(line, HOSTFILE_DEL);
    if (sizeof(fields) < 5) {
      log_file(INETD_LOG_FILE, sprintf(
            "*Parse error in hosts file: %s, Line: %s\n", file, line));
      continue;
    }

    string name = lower_case(fields[HOST_NAME]);
    if (member(new_hosts, name))
      continue; // mud already in list

    string *local_cmds = old_explode(fields[LOCAL_COMMANDS],HOSTFILE_DEL2);
    if (member(local_cmds,"*") != -1)
        local_cmds = local_cmds - ({ "*" }) + COMMANDS;
    
    new_hosts[name] = ({ capitalize(fields[HOST_NAME]),
                     fields[HOST_IP],
                     to_int(fields[HOST_UDP_PORT]),
                     local_cmds,
                     old_explode(fields[HOST_COMMANDS], HOSTFILE_DEL2),
                     UNKNOWN
                   });
    /*
     * Get existing host status from current active host lost as long as the
     * IP and UDP ports are the same.
     */
    if (hosts[name] &&
        hosts[name][HOST_IP] == new_hosts[name][HOST_IP] &&
        hosts[name][HOST_UDP_PORT] == new_hosts[name][HOST_UDP_PORT])
        new_hosts[name][HOST_STATUS] = hosts[name][HOST_STATUS];
  }
  return new_hosts;
}

/*
 * Read the INETD_HOSTS file and set the "hosts" mapping from it.
 * Retain existing HOST_STATUS fields.
 * Reads INETD_HOSTSS and INETD_HOSTS.dump
 * Hosts from INETD_HOSTS are included in the static host list.
 */
void set_host_list() {

  // read the static host file and the last host file dump.
  mapping static_hosts = read_host_list(HOST_FILE);
  if (!static_hosts)
      return; // retain the old list(s)

  // remember the static hosts
  static_host_list = m_reallocate(static_hosts,0);
  
  // read the last host file dump and add the static hosts. Then the static
  // hosts have precedence over the ones from the dump.
  hosts = (read_host_list(HOST_FILE".dump") || ([])) + static_hosts;

}

mixed get_hosts() {return copy(hosts);}

int  dump_hosts_list() {
  
  write_file(HOST_FILE+".dump", sprintf(
    "#Hostlist dump from "MUDNAME", created %s\n", strftime()), 1);

  foreach(string mudname, mixed tmp : hosts) {
    // skip muds we did not hear from for 2 days.
//    if (tmp[HOST_STATUS] + 172800 < time())
//      continue;
    write_file(HOST_FILE+".dump",
        sprintf("%s:%s:%d:%s:%s\n",tmp[0],tmp[1],tmp[2],implode(tmp[3],","),
          implode(tmp[4],",")));
  }
  return 1;
}

/*
 * Make a PING request to all muds in the "hosts" mapping to set
 * HOST_STATUS information.
 * But don't ping all muds at once, because that may overflow the callout
 * table during mud startup, when hundreds of objects make callouts.
 */
void startup(string *muds) {

    if (!pointerp(muds))
      muds=m_indices(hosts);
    if (!sizeof(muds))
      return;
    string *part;
    if (sizeof(muds) > 9)
      part=muds[0..9];
    else
      part=muds;
    foreach(string mud: part) 
      _send_udp(mud, ([ REQUEST: PING ]), 1);
    muds -= part;
    if (sizeof(muds))
      call_out(#'startup, 4, muds);
}

/*
 * Remove a buffered packet from the "incoming_packets" mapping.
 */
void remove_incoming(string id) {
    m_delete(incoming_packets, id);
}

/*
 * Decode a string from a UDP packet.
 * Returns:   The actual value of the argument (either int or string)
 */
mixed decode(string arg) {

    if (sizeof(arg) && arg[0] == '$')
        return arg[1..];
#ifdef RESTRICTED_CASTS
    if (to_string(to_int(arg)) == arg)
        return to_int(arg);
#else
    if ((string)((int)arg) == arg)
        return (int)arg;
#endif
    return arg;
}

/*
 * Decode a UDP packet.
 * Arguments: UDP packet as a string.
 * Returns:   The decoded information as a mapping, 1 for succes but no
 *            output (buffered packet), or 0 on error.
 */
mixed decode_packet(string packet, string delimiter) {
    string *data;
    mapping ret;
    string info, tmp;
    mixed class;
    int i, id, n;

    /* If this packet has been split, handle buffering. */
    if (packet[0..sizeof(PACKET)] == PACKET + ":") {
        if (sscanf(packet, PACKET + ":%s:%d:%d/%d" + delimiter + "%s",
        class, id, i, n, tmp) != 5)
            return 0;
        class = lower_case(class) + ":" + id;
        if (pointerp(incoming_packets[class])) {
            incoming_packets[class][i-1] = tmp;
            if (member(incoming_packets[class], 0) == -1) {
                ret =
                decode_packet(implode(incoming_packets[class], ""), delimiter);
                remove_incoming(class);
                return ret;
            }
        } else {
            incoming_packets[class] = allocate(n);
            incoming_packets[class][i-1] = tmp;
/* Is it possible to already have a timeout running here ?!? */
            /* If no timeout is running then start one. */
            if (!pending_data[class]) {
                call_out("remove_incoming",
                REPLY_TIME_OUT + REPLY_TIME_OUT * RETRIES, class);
            } else {
                DEBUG("\nINETD: *** Buffered packet Timeout already running! ***\n");
            }
        }
        return 1;
    }
    ret = ([ ]);
    for(i = 0, n = sizeof(data = old_explode(packet, delimiter)); i < n; i++) {
        /* DATA fields can be denoted by a preceeding blank field. */
        if (data[i] == "") {
            tmp = DATA;
            /* Test for illegal packet length (no DATA) */
            if (++i >= n)
                return 0;
            info = data[i];
        }
        else if (sscanf(data[i], "%s:%s", tmp, info) != 2)
            return 0;
        switch((string)(class = decode(tmp))) {
            case DATA:
#ifdef USE_EXTRACT
                return ret + ([ DATA: decode(implode(
                    ({ info }) + extract(data, i+1), delimiter)) ]);
#else
                return ret + ([ DATA: decode(implode(
                    ({ info }) + data[i+1..], delimiter)) ]);
#endif
            case SYSTEM:
                ret[class] = old_explode(info, ":");
                continue;
            default:
                ret[class] = decode(info);
                continue;
        }
    }
    return ret;
}

/* Check wether a UDP packet was valid.
 * Logs are made and "host" information is updated as appropriate.
 * Arguments: Decoded UDP packet (mapping)
 * Returns:   0 for valid packets, an error string otherwise.
 */
string valid_request(mapping data) {
    mixed host_data;
    string *muds;
    string req;
    int i;
    
    if (!data[NAME] || !data[UDP_PORT])
        return DATE + ": Illegal packet.\n";
    if (host_data = hosts[lower_case(data[NAME])]) {
        if (data[HOST] != host_data[HOST_IP]) {
            if (data[NAME] == LOCAL_NAME)
                return DATE + ": *** FAKE MUD ***\n";
            log_file(INETD_LOG_FILE, DATE + ": Host change:\n" +
            host_data[HOST_NAME] + ": " + host_data[HOST_IP] + " -> " +
            data[HOST] + "\n\n");
            host_data[HOST_IP] = data[HOST];
        }
        if (data[UDP_PORT] != host_data[HOST_UDP_PORT]) {
            if (data[NAME] == LOCAL_NAME)
                return DATE + ": *** FAKE MUD ***\n";
            log_file(INETD_LOG_FILE, DATE + ": Port change:\n" +
                host_data[HOST_NAME] + " (" + host_data[HOST_IP] + "): " +
                host_data[HOST_UDP_PORT] + " -> " + data[UDP_PORT] + "\n\n");
            host_data[HOST_UDP_PORT] = data[UDP_PORT];
        }
    } else {
        if (lower_case(data[NAME]) == lower_case(LOCAL_NAME))
            return DATE + ": *** FAKE MUD ***\n";
        for(i = sizeof(muds = m_indices(hosts)); i--; ) {
            host_data = hosts[muds[i]];
            if (data[HOST] == host_data[HOST_IP] &&
            data[UDP_PORT] == host_data[HOST_UDP_PORT]) {
                log_file(INETD_LOG_FILE, DATE + ": Name change:\n" +
                    host_data[HOST_NAME] + " (" + host_data[HOST_IP] +
                    ") -> " + data[NAME] + "\n\n");
                host_data[HOST_NAME] = data[NAME];
                hosts[lower_case(data[NAME])] = host_data;
                m_delete(hosts, muds[i]);
                i = -2;
                break;
            }
        }
        if (i != -2) {
            host_data = hosts[lower_case(data[NAME])] = ({
                data[NAME],
                data[HOST],
                data[UDP_PORT],
                COMMANDS,
                ({ "*" }),
                UP
            });
            log_file(INETD_LOG_FILE, DATE + ": New mud.\n" + data[NAME] + ":" +
            data[HOST] + ":" + data[UDP_PORT] + "\n\n");
        }
    }
    if (!(req = data[REQUEST]))
        return DATE + ": System message.\n";
    if (req != PING &&
        req != QUERY &&
        req != REPLY &&
        member(host_data[LOCAL_COMMANDS], req) == -1)
    {
        /* This should probably send a system message too. */
        _send_udp(host_data[HOST_NAME], ([
            REQUEST: REPLY,
            RECIPIENT: data[SENDER],
            ID: data[ID],
            DATA: "Invalid request @" + LOCAL_NAME + ": " +
                capitalize(data[REQUEST]) + "\n"
        ]) );
        return DATE + ": Invalid request.\n";
    }
    return 0;
}

/*
 * Incoming UDP packets are sent to this function to be interpretted.
 * The packet is decoded, checked for validity, HOST_STATUS is updated
 * and the appropriate udp module called.
 * Arguments: Senders IP address (string)
 *            UDP packet (string)
 */
void _receive_udp(string sender, string packet) {
    mapping data;
    string req, err, id;

#ifdef UDP_MASTER
    if (!previous_object() ||
    object_name(previous_object()) != UDP_MASTER) {
        log_file(INETD_LOG_FILE, DATE + ": Illegal call of _receive_udp() by " +
        object_name(previous_object()) + "\n\n");
        return;
    }
#endif

    ZDEBUG(sprintf("%O -> %.500O\n",sender, packet));
    if (
#ifdef DELIMITER_COMPAT
#ifdef USE_EXTRACT
        (!(data = decode_packet(packet, NEW_DELIMITER))
#else
        (!(data = decode_packet(packet, NEW_DELIMITER))
#endif
        || (data[HOST] = sender) && (err = valid_request(data))) &&
        (!(data = decode_packet(packet, OLD_DELIMITER)) ||
        (data[HOST] = sender) && (err = valid_request(data)))
#else
#ifdef USE_EXTRACT
        !(data = decode_packet(packet, DELIMITER))
#else
        !(data = decode_packet(packet, DELIMITER))
#endif
#endif
    ) {
        if (!data)
#ifdef RECEIVE_UDP_COMPAT
            RECEIVE_UDP_COMPAT(sender, packet);
#else
            log_file(INETD_LOG_FILE, DATE + ": Received invalid packet.\nSender: " +
            sender + "\nPacket:\n" + packet + "\n\n");
#endif
        return;
    }
#ifdef DELIMITER_COMPAT
    if (!mappingp(data))
        return;
    if (err)
#else
          if (!mappingp(data))
                  return;
    data[HOST] = sender;
    if (err = valid_request(data))
#endif
    {
        log_file(INETD_LOG_FILE, err + "Sender: " + sender + "\nPacket:\n" +
        packet + "\n\n");
        return;
    }
    hosts[lower_case(data[NAME])][HOST_STATUS] = UP;
    if ((req = data[REQUEST]) == REPLY) {
            mapping pending;

        /* If we can't find the reply in the pending list then bin it. */
        if (!(pending = pending_data[lower_case(data[NAME]) + ":" + data[ID]]))
            return;
        /* Set data[REQUEST] correctly, but still send to (req = REPLY) */
        data[REQUEST] = pending[REQUEST];
#ifdef INETD_DIAGNOSTICS
        data[PACKET_LOSS] = pending[PACKET_LOSS];
        data[RESPONSE_TIME] = time() - pending[RESPONSE_TIME] + 1;
#endif
#if 0
/* channel replies may not include a recipient, and shouldn't have one set */
        /* Restore the RECIPIENT in replies if none given and it is known. */
        if (!data[RECIPIENT] && pending[SENDER])
            data[RECIPIENT] = pending[SENDER];
#endif
        m_delete(pending_data, lower_case(data[NAME]) + ":" + data[ID]);
    }
    else if (data[ID]) {
      id = lower_case(data[NAME]) + ":" + data[ID];
      if (member(received_ids, id) == -1)
      {
        received_ids += ({ id });
        call_out("remove_received_id",
              REPLY_TIME_OUT + REPLY_TIME_OUT * (RETRIES + 1), id);
      }
      else
            add_system_field(data, REPEAT);
    }
    if (err = catch(
        call_other(UDP_CMD_DIR + req, "udp_" + req, deep_copy(data));publish))
    {
        _send_udp(data[NAME], ([
            REQUEST: REPLY,
            RECIPIENT: data[SENDER],
            ID: data[ID],
            DATA: capitalize(req)+ " request failed @" + LOCAL_NAME + ".\n"
        ]) );
        log_file(INETD_LOG_FILE, DATE + ": " + data[REQUEST] + " from " +
        data[NAME] + " failed.\n" + err + packet + "\n\n");
    }
}

int do_match(string mudname, string match_str) {
    return mudname[0..sizeof(match_str)-1] == match_str;
}

#ifdef NO_CLOSURES

status greater(mixed a, mixed b) {
    return a > b;
}

string *expand_mud_name(string name) {
    return sort_array(
        filter(m_indices(hosts), "do_match", this_object(), name),
        "greater",
        this_object()
    );
}

#else

string *expand_mud_name(string name) {
    return sort_array(
        filter(m_indices(hosts), #'do_match, name),
        #'>
    );
}

#endif

string encode(mixed arg) {
    if (objectp(arg))
        return object_name(arg);
    if (stringp(arg) && sizeof(arg) &&
        (arg[0] == '$' ||
#ifdef RESTRICTED_CASTS
    to_string(to_int(arg)) == (string)arg))
#else
    (string)to_int(arg) == (string)arg))
#endif
        return "$" + arg;
    return to_string(arg);
}

string encode_packet(mapping data) {
    int i;
    mixed indices;
    string header, body, t1, t2;
    string *ret;
    status data_flag;

    for(ret = ({ }), i = sizeof(indices = m_indices(data)); i--; ) {
        if (indices[i] == DATA) {
            data_flag = 1;
            continue;
        }
        header = encode(indices[i]);
        body = encode(data[indices[i]]);
        if (sscanf(header, "%s:%s", t1, t2) ||
            sscanf(header + body, "%s" + DELIMITER + "%s", t1, t2)
        )
            return 0;
        
        ret += ({ header + ":" + body });
    }
    if (data_flag)
#ifdef USE_OLD_DATA_FORMAT
        ret += ({ DATA + ":" + encode(data[DATA]) });
#else
        ret += ({ "", encode(data[DATA]) });
#endif
    return implode(ret, DELIMITER);
}

// Funktion explode_
// Die Funktion zerlegt den String packet in gleich lange Teilstrings
// der Laenge len und gibt die Teilstrings als Array zurueck. Der letzte
// Teilstring kann kuerzer sein als die anderen.
string *explode_packet(string packet, int len) {

  int ptr, m_ptr,size;
  string *result;

  // Variablen initialisieren
  m_ptr=ptr=0;
  size=sizeof(packet);
  result=({});

  // Um Arrayadditionen zu vermeiden wird vorher allokiert. Die Division 
  // durch 0 ist nicht abgefangen, da bei len=0 was im Aufruf falch ist.
  result=allocate((size/len+1));

  while (ptr<size) {
    result[m_ptr] = // Aktuellen Teilstring speichern
#ifdef USE_EXTRACT
            extract(packet,ptr,ptr+len-1);
#else
            packet[ptr..ptr+len-1]; 
#endif
    ptr+=len;// Neuen Pointer im String berechnen
    m_ptr++; // Neuen Pointer im Mapping berechnen. Hier nutze ich eine
             // Variable mehr als noetig, weil das billiger ist, als jedes
             // mal ptr=m_ptr*len auszurechnen. Lieber zwei Additionen als 
             // eine Multiplikation und eine Addtion.
  }

  // Wenn size/len aufgeht, ist das Result-Feld zu gross. Dann bleibt 
  // ein Element leer, das wird hier gestrippt. Das ist billiger als
  // jedesmal auszurechnen, ob size/len aufgeht.
  return result-({0});
 
}

varargs string _send_udp(string mudname, mapping data, int expect_reply) {
    mixed host_data;
    string *packet_arr;
    string packet;
    int i;

    mudname = lower_case(mudname);
    if (!(host_data = hosts[mudname])) {
        string *names;

        if (sizeof(names = expand_mud_name(mudname)) == 1)
            host_data = hosts[mudname = names[0]];
        else
#ifdef SEND_UDP_COMPAT
            return (string)SEND_UDP_COMPAT(mudname, data, expect_reply);
#else
        if (!sizeof(names))
            return "Unbekannter Mudname: " + capitalize(mudname) + "\n";
        else
            return break_string("Mudname ("+capitalize(mudname)+
            ") nicht eindeutig, es passen: "+implode(map(names,#'capitalize),", ")+
            ".\n",78);
#endif
    }
    
    if (data[REQUEST] != PING &&
        data[REQUEST] != QUERY &&
        data[REQUEST] != REPLY &&
        member(host_data[HOST_COMMANDS], "*") == -1 &&
        member(host_data[HOST_COMMANDS], data[REQUEST]) == -1)
        return capitalize(data[REQUEST]) + ": Command unavailable @" +
                   host_data[HOST_NAME] + "\n";
    
    data[NAME] = LOCAL_NAME;
    data[UDP_PORT] = LOCAL_UDP_PORT;
    
    if (expect_reply) {
        /* Don't use zero. */
        data[ID] = ++packet_id;
        /* Don't need deep_copy() as we are changing the mapping size. */
        pending_data[mudname + ":" + packet_id] =
#ifdef INETD_DIAGNOSTICS
        data + ([ NAME: host_data[HOST_NAME], RESPONSE_TIME: time() ]);
#else
        data + ([ NAME: host_data[HOST_NAME] ]);
#endif
    }
    if (!(packet = encode_packet(data))) {
        if (expect_reply)
            pending_data = m_copy_delete(pending_data, mudname + ":" + packet_id);
        log_file(INETD_LOG_FILE, DATE + ": Illegal packet sent by " +
        object_name(previous_object()) + "\n\n");
        return "inetd: Illegal packet.\n";
    }
    if (expect_reply)
        call_out("reply_time_out", REPLY_TIME_OUT, mudname + ":" + packet_id);

    if (sizeof(packet) <= MAX_PACKET_LEN)
        packet_arr = ({ packet });
    else {
        string header;
        int max;

        /* Be careful with the ID.  data[ID] could have been set up by RETRY */
        header =
            PACKET + ":" + lower_case(LOCAL_NAME) + ":" +
            ((expect_reply || data[REQUEST] != REPLY)&& data[ID] ?
            data[ID] : ++packet_id) + ":";
        
        /* Allow 8 extra chars: 3 digits + "/" + 3 digits + DELIMITER */
        packet_arr = explode_packet(packet,
            MAX_PACKET_LEN - (sizeof(header) + 8));

        for(i = max = sizeof(packet_arr); i--; )
            packet_arr[i] =
            header + (i+1) + "/" + max + DELIMITER + packet_arr[i];
    }
    
    for(i = sizeof(packet_arr); i--; )
    {
      ZDEBUG(sprintf("%O <- %.500O\n",host_data[HOST_IP], packet_arr[i]));
      if (!send_udp(
        host_data[HOST_IP], host_data[HOST_UDP_PORT], packet_arr[i]))
            return "inetd: Error in sending packet.\n";
    }
    return 0;
}

void reply_time_out(string id) {
    mapping data;

    if (data = pending_data[id]) {
        object ob;

#ifdef INETD_DIAGNOSTICS
        data[PACKET_LOSS]++;
#endif
        if (data[RETRY] < RETRIES) {
            mapping send;

            data[RETRY]++;
            /* We must use a copy so the NAME field in pending_data[id]
             * isn't corrupted by _send_udp(). */
            send = deep_copy(data);
            send = m_copy_delete(send, RETRY);
#ifdef INETD_DIAGNOSTICS
            send = m_copy_delete(send, PACKET_LOSS);
            send = m_copy_delete(send, RESPONSE_TIME);
#endif
            call_out("reply_time_out", REPLY_TIME_OUT, id);
            _send_udp(data[NAME], send);
            return;
        }
        data = m_copy_delete(data, RETRY);
#ifdef INETD_DIAGNOSTICS
        data = m_copy_delete(data, RESPONSE_TIME);
#endif
        catch(call_other(UDP_CMD_DIR + REPLY, "udp_" + REPLY,
        add_system_field(data, TIME_OUT));publish);
        /* It's just possible this was removed from the host list. */
        if (hosts[lower_case(data[NAME])])
            hosts[lower_case(data[NAME])][HOST_STATUS] = DOWN;
        remove_incoming(lower_case(data[NAME]) + ":" + id);
    }
    pending_data = m_copy_delete(pending_data, id);
}

void remove_received_id(string id) {
    received_ids -= ({ id });
}

varargs mixed query(string what, mixed extra1, mixed extra2) {
    mixed data;

    switch(what) {
        case "commands":
            return COMMANDS;
        case "hosts":
            return deep_copy(hosts);
        case "pending":
            return deep_copy(pending_data);
        case "incoming":
            return deep_copy(incoming_packets);
        case "received":
            return ({ }) + received_ids;
        /* args: "valid_request", request, mudname */
        case "valid_request":
            if (data = hosts[extra2])
                return member(data[HOST_COMMANDS], "*") != -1 ||
                        member(data[HOST_COMMANDS], extra1) != -1;
            return 0;
    }
    return(0);
}


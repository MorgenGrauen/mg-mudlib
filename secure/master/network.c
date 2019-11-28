// MorgenGrauen MUDlib
//
// master/network.c - UDP-Handling
//
// $Id: network.c 8934 2014-09-10 21:57:12Z Zesstra $

#pragma strict_types

#include "/secure/master.h"
#define BBMASTER "/secure/bbmaster"

/*
#undef DEBUG
#define DEBUG(x) if (call_sefun("find_player","zesstra")) \
                 tell_object(call_sefun("find_player","zesstra"),x);
*/

//ich will hieraus momentan kein Debug, ist zuviel. Zesstra

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(x)

nosave int mails_last_hour;

static string mail_normalize( string str )
{
    str = regreplace( str, "[^<]*<(.*)>[^>]*", "\\1", 0);
    return regreplace( str, " *([^ ][^ ]*).*", "\\1", 0);
}


static string *mk_rec_list( string str )
{
    return map( explode( lower_case(str), "," ) - ({""}),
                      "mail_normalize", this_object() );
}


static int CheckPasswd( string name, string passwd ) {
    mixed *uinf;

    if (!stringp(passwd) || !sizeof(passwd))
 return 0;
    if ( sizeof(uinf = get_full_userinfo(name)) < 2 )
        return 0;

    string pwhash = uinf[USER_PASSWORD+1];
    if (sizeof(pwhash) > 13) {
 // MD5-Hash
 passwd = md5_crypt(passwd, pwhash);
    }
    else if (sizeof(pwhash) > 2) {
 // Crypt-Hash
 passwd = crypt(passwd, pwhash[0..1]);
    }
    else return 0;

    return (passwd == pwhash);
}

static void udp_query( string query, string host, int port )
{
#if __EFUN_DEFINED__(send_udp)
    string *mess;
    mixed *data;
    int i, j;

    mess = explode( query, " " );

    switch ( mess[1] ){
    case "wholist":
    case "who":
        data = ({string*})"/obj/werliste"->QueryWhoListe();
        break;

    case "uptime":
        data = ({ call_sefun("uptime") });
        break;

    case "finger":
        if ( sizeof(mess) < 3 )
            data = ({ "Error: Wen soll ich fingern ?" });
        else
            data = explode( "/p/daemon/finger"->
                            finger_single( lower_case(mess[2]), 0 ), "\n" );
        break;

    default:
        data = ({ "Error: unknown request " + mess[1] + "\n" });
    }


    send_udp( host, port,
        to_bytes(sprintf( "%s 0 %d", mess[0], sizeof(data)),
                 "ASCII//TRANSLIT"));

    for ( i = 0, j = sizeof(data); i < j; i++ )
        send_udp( host, port,
            to_bytes(sprintf( "%s %d %s", mess[0], i+1, data[i]),
                     "ASCII//TRANSLIT") );
#endif
}

#define UDP_DEBUG(x) 
//#define UDP_DEBUG(x) (write_file("/log/ARCH/udp.log",(x)))

void receive_udp(string host, bytes message, int port)
{
  // Traditionell sind einkommende Nachrichten via UDP immer ASCII-pur
  // gewesen. Davon geht auch aller hier gerufener Code aus. Daher wird jetzt
  // hier zentral die Bytesequenz als ASCII interpretiert und in string
  // gewandelt, der dann wie bislang auch verarbeitet wird.
  string msg_text;
  if (catch(msg_text = to_text(message, "ASCII");publish))
  {
    msg_text = to_text(message, "ISO8859-1");
    log_file("UDP_INVALID",sprintf(
      "Nachricht von %s mit nicht-ASCII-Zeichen: %s\n",
      host, msg_text));
  }

  UDP_DEBUG(sprintf("%s %s:%d: %s\n",strftime(),host,port,msg_text));

  // Nicht mehr benutzt, zur Sicherheit abfangen vor dem inetd.
  if (strstr(msg_text, "EXTREQ:") == 0
      || strstr(msg_text, "IPNAME") == 0
      || strstr(msg_text, "AUTH") == 0)
  {
    return;
  }

  // Die folgenden Services sind keine Intermud-Dienste
  if(strstr(msg_text, "IPLOOKUP\n") == 0)
  {
    "/p/daemon/iplookup"->update( msg_text );
    return;
  }

  if( strstr(msg_text, "DNSLOOKUP\n") == 0)
  {
    "/p/daemon/dnslookup"->update( msg_text );
    return;
  }

  if (strstr(msg_text, "udp_query:") == 0)
  {
    return udp_query(msg_text[10..],host,port);
  }
  // Rest an inetd fuer Bearbeitung als Intermud-Nachricht.
  "secure/inetd"->_receive_udp(host, msg_text);
}



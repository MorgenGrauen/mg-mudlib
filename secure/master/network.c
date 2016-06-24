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


static void FtpAccess( string host, string message, int port )
{
    string *comp, reply, head;
#if __EFUN_DEFINED__(send_udp)
    comp = efun::explode( message, "\t" );
#define FTP_ID   0
#define FTP_SEQ  1
#define FTP_TAG  2
#define FTP_CMD  3
#define FTP_ARG1 4
#define FTP_ARG2 5
#define FTP_ARG3 6

  if ( sizeof(comp) <= FTP_CMD || lower_case(comp[FTP_TAG]) != "req" ){
      log_file( "IMP_MSGS", "Host: " + host + ":" + port + " - '" +
                message + "'\n" );
      return;
  }

  reply = "INVALID";

  head = sprintf( "%s\t%s\tRPLY\t%s\t",
                  comp[FTP_ID], comp[FTP_SEQ], comp[FTP_CMD] );

  switch ( lower_case(comp[FTP_CMD]) ){
  case "user":
      if ( sizeof(comp) <= FTP_ARG1 )
          break;

      if ( IS_LEARNER(lower_case(comp[FTP_ARG1])) )
          reply = "/players/" + lower_case(comp[FTP_ARG1]);
      else
          reply = "NONE";
      break;

  case "pass":
      if ( sizeof(comp) <= FTP_ARG2 )
          break;

      comp[FTP_ARG1] = lower_case(comp[FTP_ARG1]);

      if ( IS_LEARNER(comp[FTP_ARG1]) &&
           !"/secure/master"->QueryTBanished(comp[FTP_ARG1]) ){
          if ( CheckPasswd( comp[FTP_ARG1], comp[FTP_ARG2] ) )
              reply = "OK";
          else {
              if ( get_wiz_level( comp[FTP_ARG1] ) < ARCH_LVL )
                  log_file( "LOGINFAIL",
                            sprintf( "PASSWORD:      (FTP)     %s %s\n",
                                     comp[FTP_ARG1],
                                     ctime(time()) ) );
              else
                  log_file( "ARCH/LOGINFAIL",
                            sprintf( "PASSWORD:      (FTP)     %s %s\n",
                                     comp[FTP_ARG1],
                                     ctime(time()) ) );
          }
      }
      else
          reply = "FAIL";
      break;

  case "read":
DEBUG("-FtpAccess----\nHost:"+host+"Message:\n"+message+"\n--------------\n");
      if ( sizeof(comp) <= FTP_ARG2 )
          break;

      if ( comp[FTP_ARG2][0] == '/' &&
           valid_read( comp[FTP_ARG2], lower_case(comp[FTP_ARG1]),
                       "read_file", 0 ) ){

          BBMASTER->ftpbb( lower_case(comp[FTP_ARG1]),
                           "read " + comp[FTP_ARG2] + "\n" );
          reply = "OK";
          }
      else
          reply = "FAIL";
      break;

  case "writ":
DEBUG("-FtpAccess----\nHost:"+host+"Message:\n"+message+"\n--------------\n");
      if ( sizeof(comp) <= FTP_ARG2 )
          break;

      if ( comp[FTP_ARG2][0] == '/' &&
           valid_write( comp[FTP_ARG2], lower_case(comp[FTP_ARG1]),
                       "write_file", 0 ) ){

          BBMASTER->ftpbb( lower_case(comp[FTP_ARG1]),
                           "write " + comp[FTP_ARG2] + "\n" );
          reply = "OK";
          }
       else
          reply = "FAIL";
      break;

  case "list":
DEBUG("-FtpAccess----\nHost:"+host+"Message:\n"+message+"\n--------------\n");
      if ( sizeof(comp) <= FTP_ARG2 )
          break;

      if ( comp[FTP_ARG2][0] == '/' &&
           valid_read( comp[FTP_ARG2], lower_case(comp[FTP_ARG1]),
                       "read_file", 0 ) )
          reply = "OK";
      else
          reply = "FAIL";
      break;

  default:
DEBUG("-FtpAccess----\nHost:"+host+"Message:\n"+message+"\n--------------\n");
      log_file( "IMP_MSGS", "Host: " + host + ":" + port + " - '" +
                message + "'\n" );
      break;
  }

  send_udp( host, port, head+reply );
#endif
}


static int doReadMail( string file )
{
    string str, *lines, *parts, *tmp;
    mixed *message;
    int i, j;

    if ( (i = file_size(file)) > 50000 || i < 5 ){
        rm(file);
        DEBUG( "Mail size invalid\n" );
        return -1;
    }

    if ( !(str = read_bytes( file, 0, 50000 )) )
        return -1;

    if ( !(j = sizeof(lines = explode( str, "\n" ))) )
        return -2;

    i = 0;

    while ( i < j && lines[i] != "" )
        i++;

    if ( i == j )
        return -2;

 DEBUG( sprintf( "Have %d headerlines:\n", i ) );

    message= allocate(9);
    message[MSG_CC] = ({});
    message[MSG_BCC] = ({});
    message[MSG_BODY] = implode( lines[i..], "\n" );

    for ( j = 0; j < i; j++ ){
        parts = explode( lines[j], ":" );

        if ( sizeof(parts) > 1 ){
            str = lower_case(parts[0]);
            parts[0] = implode( parts[1..], ":" );

            switch (str){
            case "subject":
                message[MSG_SUBJECT] = parts[0];
                break;

            case "from":
                DEBUG( "Found from\n" );
                DEBUG( sprintf( "PARTS[0]=%s\n", parts[0] ) );
                message[MSG_FROM] = mail_normalize(parts[0]);
                message[MSG_SENDER] = parts[0];
                DEBUG( sprintf( "FROM: %s\nSENDER: %s\n",
                                message[MSG_FROM],
                                message[MSG_SENDER] ) );
                break;

            case "cc":
                DEBUG( sprintf("FOUND CC: %O\n", parts[0]) );
                message[MSG_CC] += mk_rec_list(parts[0]);
                break;

            case "bcc":
                DEBUG( sprintf("FOUND BCC: %O\n", parts[0]) );
                message[MSG_BCC] += mk_rec_list(parts[0]);
                break;

            case "to":
                DEBUG( sprintf("FOUND TO: %O\n", parts[0]) );
                tmp = mk_rec_list(parts[0]);

                if ( !message[MSG_RECIPIENT] )
                    message[MSG_RECIPIENT] = tmp[0];

                message[MSG_CC] += tmp;
                break;

            // Das MUD-TO: wird vom Perlskript als erste Headerzeile eingefuegt
            case "mud-to":
                DEBUG( sprintf("FOUND MUD-TO: %O\n", parts[0]) );
                message[MSG_RECIPIENT] = mail_normalize(lower_case(parts[0]));
                break;
            }
        }
    }

    // Eigentlichen Empfaenger aus CC: loeschen
    message[MSG_CC] -= ({ message[MSG_RECIPIENT],
                          message[MSG_RECIPIENT]+"@mg.mud.de",
                          message[MSG_RECIPIENT]+"@morgengrauen.mud.de" });


 DEBUG( sprintf( "TO: %O\n", message[MSG_RECIPIENT] ) );
    DEBUG( sprintf( "CC: %O\n", message[MSG_CC] ) );
    DEBUG( sprintf( "BCC: %O\n", message[MSG_BCC] ) );

    if ( !stringp(message[MSG_FROM]) )
        return -2;

    if ( !stringp(message[MSG_RECIPIENT]) ){
        str = explode( file, "/" )[<1];
        i = 0;
        j = sizeof(str);

        while ( i < j && str[i] <= '9' && str[i] >= '0' )
            i++;

        if ( i >= j )
            return -2;
        
        message[MSG_RECIPIENT] = str[i..];
  DEBUG( sprintf( "EMERGENCY RECIPIENT=%s\n", str[i..] ) );
    }

    rm(file);

    // Da vom Master besser nichts von ausserhalb /secure #include't wird,
    // direkt die '5'. Normalerweise hiesse der Aufruf:
    // DeliverMail( message, NO_USER_ALIASES|NO_CARBON_COPIES );
    "/secure/mailer"->DeliverMail( message, 5 );
    return 1;
}


public void mailread()
{
    string *files;
    int i;

    DEBUG( "mailread called\n" );

    if ( mails_last_hour >= MAX_MAILS_PER_HOUR )
        return;

    files = (get_dir( "/mail/inbound/*" )||({})) - ({ "..", "." });
    i = sizeof(files);

    while ( i-- && mails_last_hour < MAX_MAILS_PER_HOUR ){
        DEBUG( "FOUND FILE \"" + files[i] + "\" ...\n" );
        mails_last_hour++;

        if ( doReadMail( "/mail/inbound/" + files[i]) < -1 ){
            mixed message;

            message = allocate(9);
            mails_last_hour++;
            rename( "/mail/inbound/" + files[i],
                    "/secure/ARCH/MAIL/" + files[i] );
            message[MSG_SENDER] = geteuid();
            message[MSG_FROM] = getuid();
            message[MSG_SUBJECT] = "Fehlerhafte Mail: /secure/ARCH/MAIL/" +
                files[i];
            message[MSG_RECIPIENT] = "mud@mg.mud.de";
            message[MSG_BODY] = "Bitte Ueberpruefen!\n";
            // NO_SYSTEM_ALIASES|NO_USER_ALIASES == 3
            "/secure/mailer"->DeliverMail( message, 3 );
        }
    }
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
        data = (string *)"/obj/werliste"->QueryWhoListe();
        break;

    case "uptime":
        data = ({ call_sefun("uptime") });
        break;

    case "finger":
        if ( sizeof(mess) < 3 )
            data = ({ "Error: Wen soll ich fingern ?" });
        else
            data = explode( (string)"p/daemon/finger"->
                            finger_single( lower_case(mess[2]), 0 ), "\n" );
        break;

    case "mailread":
        data = ({ "Okay" });
        mailread();
        break;

    default:
        data = ({ "Error: unknown request " + mess[1] + "\n" });
    }


    send_udp( host, port, sprintf( "%s 0 %d", mess[0], sizeof(data) ) );

    for ( i = 0, j = sizeof(data); i < j; i++ )
        send_udp( host, port, sprintf( "%s %d %s", mess[0], i+1, data[i] ) );
#endif
}

#define UDP_DEBUG(x) 
//#define UDP_DEBUG(x) (write_file("/log/ARCH/udp.log",(x)))

void receive_udp(string host, string message, int port)
{
  mixed *tmp;
  UDP_DEBUG(sprintf("%s %s:%d: %s\n",strftime(),host,port,message));

  if (message[0..6]=="EXTREQ:"
  	|| message[0..5]=="IPNAME"
  	|| message[0..3]=="AUTH"
  ) {	
    return;
  }

  if( message[0..8]=="IPLOOKUP\n" ) {
    "/p/daemon/iplookup"->update( message );
    return;
  }

  if( message[0..9]=="DNSLOOKUP\n" ) {
    "/p/daemon/dnslookup"->update( message );
    return;
  }

  if (message[0..4]=="NFTPD") {
#if __HOST_NAME__==MUDHOST
    if (host!=FTPD_IP) {
      DEBUG(sprintf("INVALID HOST: %s\n",host));
      return;
    }
#endif
    FtpAccess(host,message,port);
    return;
  }

  if (message[0..9]=="udp_query:") {
    return udp_query(message[10..],host,port);
  }

  "secure/inetd"->_receive_udp(host, message);
}



// MorgenGrauen MUDlib
//
// mailer.c
//
// $Id: mailer.c 9547 2016-04-17 19:27:47Z Zesstra $

/*
 *------------------------------------------------------------
 * The mail demon. Receives mail from users and delivers it into
 * the mail directory.
 *
 * Deepthought, Nightfall, 25-May-92
 * Remove-Functions : Jof, 29-June-92
 * Caching, DeleteUnreadFolder, small changes: Loco, 08-Feb-97
 * General clean-up and speed-up: Tiamak, 18-Jan-2000
 *   DON'T USE restore_object any more, use GetFolders instead!
 *------------------------------------------------------------
 *
 *     Save file format (sort of formal notation):
 *
 *     mixed *folders = ({
 *        ({ string name1; string name2; ... string nameN; })
 *        ({ mixed *msgs1; mixed *msgs2; ... mixed *msgsN; })
 *     })
 *
 *     Each msgs field is an array of messages:
 *
 *     mixed *msgs = ({ mixed *message1; ... mixed *messageM })
 *
 *     A message is represented as an array with the following fields:
 *
 *     mixed *message = ({
 *        string from;
 *        string sender;
 *        string recipient;
 *        string *cc;
 *        string *bcc;
 *        string subject;
 *        string date;
 *        string id;
 *        string body;
 *     })
 *
 *     The mailer demon (/secure/mailer) provides the following functions:
 *
 *     string *DeliverMail(mixed *message)
 *       Hand a mail message over to the mailer demon. The mailer
 *       demon extracts recipients from the recipient, cc and bcc
 *       fields and removes the bcc information. It then deposits
 *       the message to the mail files of all recipients. A valid
 *       message is shown above. Returns a list of successfully
 *       delivered recipients.
 *
 *     int FingerMail(string user)
 *       Gives the number of unread messages a user has.
 *------------------------------------------------------------
 */
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma pedantic
#pragma warn_deprecated

#include <config.h>
#include <mail.h>
#include <files.h>
#include <wizlevels.h>

// debugging
#define DEBUG(msg) if ( find_player("zesstra") ) \
                      tell_object( find_player("zesstra"), "MAILER: "+msg )
#undef DEBUG
#define DEBUG(x)

// write out a message to the recipient
#define NOTIFY_RECIPIENT
// who gets undeliverable mail?
#define BOUNCE_ADDR   "mud@mg.mud.de"
#define SECURITY(x)   (geteuid(x) == ROOTID || geteuid(x) == MAILID)
// flag for _DeliverMail
#define MAIL_DELAYED   4096

// prototypes
protected void create();
static int GetFolders( string user );
static string _unify( string str );
static string *unify( string *str );
static string *expand( string *addr, int expa );
static string _filter_addr( string addr );
public string *DeliverMail( mixed msg, int expa );
public int FingerMail( string user );
static void save_msg( mixed msg, string user );
public int RemoveMsg( int msg, int folder, string user );
public int MoveMsg( int msg, int folder, string newfolder, string user );
public int DeleteUnreadFolder( string user );
public int RemoveFolder( string folder, string user );
public int MakeFolder( string folder, string user );
public int query_recipient_ok( string name );
public void deliver_mail( string recipient, string from, string subject,
                          string mail_body );


mixed *folders;                /* used for save and restore of mail files */
static mapping alias;
static string cachedname; /* whose folder is still in memory? */


protected void create()
{
    mixed tmp;
    int i;
    string s1, s2;
  
    seteuid(ROOTID);
    alias=([]);
    
    if ( tmp = read_file("/mail/system.mailrc") ){
        tmp = explode( tmp, "\n" );
        
        for ( i = sizeof(tmp); i--; )
            if ( sscanf( tmp[i], "%s %s", s1, s2 ) == 2 )
                alias[s1] = s2;
    }
    
    // Ggf. Ordner erstellen.
    if(file_size(MAILPATH)==FSIZE_NOFILE)
    {
        // LIBDATADIR wird vom Master erstellt, ist also schon vorhanden.
        mkdir(MAILPATH);
    }
}


// GetFolders laedt einen folder, wenn er nicht im cache ist, und gibt
// 0 zurueck, wenn der folder nicht vorhanden oder evtl auch leer ist.
// Sinn: Vor allem bei Listenargumenten im mailer kann es leicht vorkommen,
// dass dasselbe mailfile einige Male hintereinander gebraucht wird.

static int GetFolders( string user )
{
    if ( user == cachedname ){
        DEBUG( "Using cached folder for " + user + "\n" );
        return sizeof(folders[1]);
    }
    
    cachedname = user;
    
    if ( !restore_object( MAILPATH + "/" + user[0..0] + "/" + user ) ){
        DEBUG( "Loading folder: " + user + " (empty)\n" );
        folders = ({ ({}), ({}) });
        return 0;
    }
    
    DEBUG( "Loading folder:" + user + "\n" );
    return 1;
}


static string _unify( string str )
{
    return str[0] == '\\' ? str[1..] : str;
}


static string *unify( string *str )
{
    if ( !pointerp(str) )
        return ({});
    
    str = map( filter( str, #'stringp/*'*/ ), #'lower_case/*'*/ );
    str = map( str, "_unify", this_object() );

    return m_indices( mkmapping(str) );
}


#define MG_NAMES ({ MUDNAME, "mg", "morgengrauen", "mud", "mg.mud.de" })

string expandSystemRecursive(string addr,int maxrec)
{
  if(maxrec>8 || !addr)
  {
    return addr;
  }
  ++maxrec;
  
  string *retlist = ({});
  int alias_found;
  foreach(string add : explode(addr,","))
  {
    if (add == "")
      continue;
    string tmp = alias[add];
    if (stringp(tmp))
    {
      retlist += explode(tmp, ",");
      ++alias_found;
    }
    else
      retlist += ({add});
  }
  string ret = implode(retlist-({""}), ",");
  // Wenn Aliase aufgeloest wurden: noch einmal versuchen...
  if (alias_found)
    ret = expandSystemRecursive(ret, maxrec);

  return ret;
}

// expa: also do alias and forward-expansion? (for inbound external mail)
// expa == 0 means full expansion, known flags are NO_SYSTEM_ALIASES
// and NO_USER_ALIASES
static string *expand( string *addr, int expa )
{
    string tmp, *new, *ret;
    int i;
    closure qf;

    ret = ({});
    addr -= ({""});
    qf = symbol_function( "QueryForward", FWSERV );

    for ( i = sizeof(addr); i--; ){
        addr[i] = lower_case( addr[i] );
        // @morgengrauen-namen werden lokal zugestellt.
        if ( sizeof(new = explode( addr[i], "@" )) == 2  &&
             member( MG_NAMES, new[1] ) != -1 )
            addr[i] = new[0];

        if ( !(expa & NO_SYSTEM_ALIASES) && tmp = expandSystemRecursive(addr[i],0) ){
            ret += explode( tmp, "," );
        }
        else
            ret += ({ addr[i] });
    }

    for ( i = sizeof(ret); i--; ){
        if ( ret[i][0] == '\\' )
            ret[i] = ret[i][1..];
        else if ( !(expa & NO_USER_ALIASES) )
            ret = ret - ({ ret[i] }) +
                explode( funcall( qf, ret[i] ), "," );
    }
    
    return ret;
}


static string _filter_addr( string addr )
{
    addr = regreplace( addr, "[^<]*<(.*)>[^>]*", "\\1", 0);
    return regreplace( addr, " *([^ ][^ ]*).*", "\\1", 0);
}

#ifdef INTERNET_MAIL_ENABLED
#define FOOTER \
    "\n*****************************************************************\n" \
    "* MorgenGrauen MailRelay v1.0 - Processed %s, %s *\n" \
    "* MorgenGrauen - mg.mud.de 23 -                  89.58.11.82 23 *\n" \
    "*****************************************************************"
#endif

public string *DeliverMail( mixed msg, int expa )
{
    string sender, *recipients, *recok, *tmp;
    <string|string*>* newmsg;
    int i;
#ifdef INTERNET_MAIL_ENABLED
    int ext;
#endif

    if ( !pointerp(msg) || sizeof(msg) != 9 )
        return 0;

    DEBUG( sprintf( "DeliverMail: %O %O\n", msg[0..4] +({0})+ msg[6..7], expa ) );

    // Ohne Empfaenger wird abgebrochen
    if (!stringp(msg[MSG_RECIPIENT]))
        return 0;

    if ( !(expa & MAIL_DELAYED) ){
        /* determine the real sender */
        if ( extern_call() && object_name(previous_object())[0..7] != "/secure/" )
            sender = getuid( this_interactive() || previous_object() );
        else
            sender = msg[MSG_SENDER];        

        /* make a list of all recipients */
        recipients = ({ msg[MSG_RECIPIENT] });
        
        if ( !(expa & NO_CARBON_COPIES) ){
            if ( pointerp(msg[MSG_CC]) )
                recipients += msg[MSG_CC];
            
            if ( pointerp(msg[MSG_BCC]) )
                recipients += msg[MSG_BCC];
        }

        // Mail-Aliase ersetzen
        recipients = expand( recipients, expa );
    
        // doppelte Adressen loeschen (nebenbei: auf Kleinschreibung wandeln)
        recipients = unify( recipients );
    
        // Realnamen und Kommentare entfernen
        recipients = map( recipients, "_filter_addr", this_object() );
    
        // auf ungueltige Zeichen checken
        if ( sizeof(tmp = regexp( recipients, "^[-_.@a-z0-9]*$" ))
             != sizeof(recipients) ){
            tmp = recipients - tmp;
            
            for ( i = sizeof(tmp); i--; )
                log_file( "MAIL_INVALID", sprintf( "%s: Mail von %O (%O) an "
                                                   "'%O'.\n", dtime(time()),
                                                   msg[MSG_FROM],
                                                   sender, tmp[i] ) );
            
            recipients -= tmp;
        }

     // check for valid Subject and Body
     if (!stringp(msg[MSG_SUBJECT]))
         msg[MSG_SUBJECT] = "(no subject given)";
     if (!stringp(msg[MSG_BODY]))
         msg[MSG_BODY] =
           "\n\nSorry - This mail was delivered without a mail body\n\n";

        DEBUG( sprintf( "NEED TO DELIVER TO %O\n", recipients ) );

        /* build the new message */
        newmsg = ({ msg[MSG_FROM], sender, msg[MSG_RECIPIENT],
                        msg[MSG_CC], "", msg[MSG_SUBJECT],
                        dtime(time()), MUDNAME + ":" + time(),
                        msg[MSG_BODY] });
    }
    
    /* Send it off ... */
    recok = ({ });

    // ACHTUNG: durch expand() geaenderte Adressen werden zugestellt,
    // aber durch /mail/mailer.c zusaetzlich als unzustellbar genannt!
    
    for ( i = sizeof(recipients); i-- /*&& get_eval_cost() > 500000*/; ){
        DEBUG( sprintf( "Begin delivering to %s. Evalcosts left: %d.\n",
                        recipients[i], get_eval_cost() ) );
        if ( member( recipients[i], '@' ) > 0 &&
             strstr( recipients[i], "daemon" ) < 0 ) {
            string rec, mud;
            
            tmp = explode( recipients[i], "@" );
            mud = tmp[1];
            rec = tmp[0];
            sender = regreplace( sender, "@", "%", 1 );
            // Zustellung via Intermud-Mail an andere Muds.
            if ( member( mud, '.' ) == -1 ) {
                ({void})"/secure/udp_mail"->deliver_mail( rec, mud, sender,
                                                  msg[MSG_SUBJECT],
                                                  msg[MSG_BODY] );
                recok += ({ recipients[i] });
            }
#ifdef INTERNET_MAIL_ENABLED
            // Zustellung in den Rest des Internets.
            else {
                string t = ctime(time());
                ext = 1;
                sender = explode( sender, "%" )[0];
                rec = explode( regreplace( rec, "@", "%", 1 ), "%" )[0];
                mud = explode( regreplace( mud, "@", "%", 1 ), "%" )[0];

                write_file( sprintf( "/mail/outbound/%s.%d-%d-%d",
                                     sender, time(), i, random(123456) ),
                            sprintf( "%s\n%s@%s\n"
                            "Subject: %s\n"
                         "X-MUD-From: %s\n"
                         "X-MUD-To: %s\n"
                         "X-MUD-Cc: %s\n"
                         "X-MU-Subject: %s\n\n",
                         sender, rec, mud,
                                     msg[MSG_SUBJECT],
                                     sender, recipients[0],
                                     pointerp(msg[MSG_CC]) ?
                                     implode( msg[MSG_CC], "," ) : "",
                                     msg[MSG_SUBJECT] ) + msg[MSG_BODY] +
                            sprintf( FOOTER, t[4..10] + t[20..], t[11..18] )
                            + "\n" );
                recok += ({ recipients[i] });
            }
#endif // INTERNET_MAIL_ENABLED

        }
        else
            if (({int})master()->find_userinfo(recipients[i]) )
            {
                save_msg( newmsg, recipients[i] );
                recok += ({ recipients[i] });
            }
            else {
                <string|string*>* tmpmsg = copy(newmsg);
                tmpmsg[MSG_BODY] = "--- Text der Mail geloescht. ---\n";
                write_file( sprintf( "/data/mail/outbound/postmaster.%d-%d",
                                     time(), random(time()) ),
                            sprintf( "postmaster\n" + BOUNCE_ADDR + 
                                     "\nSubject: Undeliverable Mail\n%O\n",
                                     tmpmsg) );
            }
        DEBUG( sprintf( "End delivering to %s. Evalcosts left: %d.\n",
                        recipients[i], get_eval_cost() ) );
    }
#ifdef INTERNET_MAIL_ENABLED
    if ( ext )
        send_udp( UDPSERV, 4123, "DELIVERMAIL" );
#endif
    return recok;
}


public int FingerMail( string user )
{
    int newfolder;

    //Zugriff beschraenken, Zahl der gelesenen Mails ist Privatsphaere
    if (!objectp(this_interactive()) || !stringp(user) || !sizeof(user)) 
     return(-1);
    if ((getuid(this_interactive())!=user) &&
     (process_call() || !ARCH_SECURITY)) return(-1);

    if ( !GetFolders(user) )
        return 0;

    if ( (newfolder = member(folders[0],"unread")) != -1 )
        return sizeof(folders[1][newfolder]);

    return 0;
}


static void save_msg( mixed msg, string user )
{
    int newfolder;
    object p;
  
    GetFolders( user );

    /* if folder 'unread' doesn't exist, create it */
    newfolder = member( folders[0], "unread");
    
    if ( newfolder == -1 ){
        folders[0] += ({ "unread" });
        folders[1] += ({ ({ }) });
        newfolder = member( folders[0], "unread");
    }
    
    folders[1][newfolder] += ({ msg });
    save_object( MAILPATH + user[0..0] + "/" + user );
#ifdef NOTIFY_RECIPIENT
    if ( p = find_player(user) )
        tell_object( p, "Ein Postreiter ruft Dir aus einiger Entfernung zu, "
                     "dass Du neue Post hast!\n" );
#endif
}


/* Remove a message from a folder */
public int RemoveMsg( int msg, int folder, string user )
{
    if ( !SECURITY(previous_object()) )
        return -2;

    if ( !GetFolders(user) ) 
        return -1; /* No such folder */

    if ( !pointerp(folders) || !pointerp(folders[0]) || 
         folder >= sizeof(folders[0]) )
        return -1;

    if ( msg < 0 || sizeof(folders[1][folder]) <= msg )
        return 0; /* No such msg */

    folders[1][folder][msg..msg] = ({});
    
    save_object( MAILPATH + user[0..0] + "/" + user );
    return 1; /* Success */
}


/* Move message into another folder */
public int MoveMsg( int msg, int folder, string newfolder, string user )
{
    int target;

    if ( !SECURITY(previous_object()) )
        return -2;

    if ( !GetFolders(user) )
        return -1; /* Source folder not found */

    if ( !pointerp(folders) || !pointerp(folders[0]) || 
         folder >= sizeof(folders[0]) )
        return -1;
  
    if ( msg < 0 || sizeof(folders[1][folder]) <= msg )
        return 0; /* No such msg */

    if ( (target = member(folders[0], newfolder)) == -1 )
        return -3;

    if ( target == folder )
        return 1;

    if ( !pointerp(folders[1][target]) ) 
        folders[1][target] = ({ folders[1][folder][msg] });
    else 
        folders[1][target] += ({ folders[1][folder][msg] });

    return RemoveMsg( msg, folder, user );
}


public int DeleteUnreadFolder( string user )
{
    int unread, newmail;

    if ( !SECURITY(previous_object()) )
        return -2;

    if ( !GetFolders(user) )
        return -1; /* Source folder not found */

    if ( (unread = member( folders[0], "unread")) == -1 )
        return 0;
    
    if ( (newmail = member( folders[0], "newmail")) == -1 ){
        folders[0] += ({ "newmail" });
        folders[1] += ({({})}); 
        newmail = sizeof(folders[1]) - 1;
    }
    
    if ( !pointerp(folders[1][newmail]) )
        folders[1][newmail] = ({});
    
    if ( pointerp(folders[1][unread]) )
        folders[1][newmail] += folders[1][unread];
    
    folders[0][unread..unread] = ({});
    folders[1][unread..unread] = ({});

    save_object( MAILPATH + user[0..0] + "/" + user );
    return 0;
}


public int RemoveFolder( string folder, string user )
{
    int i;

    if ( !SECURITY(previous_object()) )
        return -2;

    if ( !GetFolders(user) )
        return -1; /* No such folder */

    if ( (i = member( folders[0], folder)) == -1 )
        return -1; /* No such folder */

    if ( sizeof(folders[1][i]) > 0 )
        return 0; /* Folder not empty */

    folders[0][i..i] = ({});
    folders[1][i..i] = ({});

    save_object( MAILPATH + user[0..0] + "/" + user );
    return 1;
}


public int MakeFolder( string folder, string user )
{
    if ( !SECURITY(previous_object()) )
        return -2;
    
    if ( !folder || !stringp(folder) )
        return -1; /* Huh ? */

    if ( folder == "unread" )
        return 0; /* Folder exists virtually :) */

    GetFolders( user );

    if ( member( folders[0], folder) != -1 )
        return 0; /* Folder exists */

    folders[0] = folders[0] + ({ folder });
    folders[1] = folders[1] + ({ ({}) });
    
    save_object( MAILPATH + user[0..0] + "/" + user );
    return 1;
}


public int query_recipient_ok( string name )
{
#if INTERNET_MAIL_ENABLED
    return  (({int})master()->find_userinfo(name)
             || member( name, '%' ) > 0 || member( name, '@' ) > 0 );
#else
    // es darf zwar ein @ in der Adresse vorkommen, dahinter aber kein . mehr
    // (dann ist es ne Mail via Intermud-Mail, nicht ins Internet).
    string *tmp;
    return  (({int})master()->find_userinfo(name)
             || member( name, '%' ) > 0
             || (sizeof(tmp=explode(name,"@")) == 2 && strstr(tmp[1],".") == -1));
#endif
}


public void deliver_mail(
                         string recipient, /* the local players real name*/
                         string from,      /* A string depicting the sender */
                         string subject,   /* The mail subject/header */
                         string mail_body  /* The actual mail message */ )
{
    DEBUG( sprintf("DELIVER %O\n",
                   ({ from, from, recipient, ({}), ({}), subject, time() })) );

    // Geloggt wird, wenn ein aufrufendes Objekt nicht sicher ist.
    if (object_name(previous_object())[0..7]!="/secure/")
      write_file("/secure/ARCH/DELIVER_MAIL",
     sprintf("%s : Aufruf von /secure/mailer->deliver_mail()\n"
          "  Sender: %O Empfaenger: %O\n  PO: %O TI: %O TP:%O\n\n",
          ctime(time()),from, recipient,
          previous_object(), this_interactive(), this_player()));
    
    DeliverMail( ({ from, from, recipient, ({}), ({}), subject, time(),
                    "EXTERNAL", mail_body }), 0 );
}

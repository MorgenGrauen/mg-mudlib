// MorgenGrauen MUDlib
//
// admin.c
//
// $Id: admin.c 8755 2014-04-26 13:13:40Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <udp.h>
#include <wizlevels.h>
#include <input_to.h>

#define NEED_PROTOTYPES
#include <magier.h>
#include <player.h>

inherit "/std/util/cidr";

mixed _query_localcmds()
{
  return ({({"udpq","_udpq",0,LEARNER_LVL}),
           ({"shutdown","shut_down_game",0,ARCH_LVL}),
           ({"addmaster","_addmaster",0,GOD_LVL}),
           ({"removemaster","_removemaster",0,GOD_LVL}),
           ({"addguildmaster", "_addguildmaster", 0, GOD_LVL}),
           ({"removeguildmaster", "_removeguildmaster", 0, GOD_LVL}),
           ({"suender","sinners",0,WIZARD_LVL}),
           ({"banish","banish", 0, WIZARD_LVL}),
           ({"mbanish","mbanish", 0, WIZARD_LVL}),
           ({"tbanish","tbanish", 0, WIZARD_LVL}),
           ({"sbanish","sbanish", 0, WIZARD_LVL})});
}

static int _udpq(string str)
{
  string ret, mud, type;

  if (!(str=_unparsed_args()) || str=="" || sscanf(str,"%s %s",mud,type)<2)
  {
    write("Syntax: udpq mud type\n");
    return 1;
  }
  if (member(({"commands","email","hosts","inetd","list","mud_port","time",
         "version"}),type)==-1)
    write("TYPEs: commands, email, hosts, inetd, list, mud_port, time, version\n");
  if (ret=(string)INETD->_send_udp(mud,([SENDER:getuid(), REQUEST:QUERY, DATA:type]),1))
    write(ret);
  else
    write("Anfrage abgeschickt.\n");
  return 1;
}

static int shut_down_game(string str)
{
  if (!IS_ARCH(this_object()) || this_object()!=this_interactive())
    return 0;
  _notify_fail("Du musst einen Grund dafuer angeben.\n");
  if (!str) return 0;
  write( "Direkter shutdown mit Grund \""+str+"\"?\n" );
  input_to("shut_down_game_2",INPUT_PROMPT, "(ja/nein) :", str);
  return 1;
}

static int shut_down_game_2(string arg,string str)
{
  if (!IS_ARCH(this_object()) || this_object()!=this_interactive())
    return 0;
  if( arg!="ja" ) {
    write( "Shutdown abgebrochen.\n" );
  } else {
    shutdown(str);
  }
  return 1;
}


static int _addmaster(string str)
{
  string master, domain;

  if (!GOD_SECURITY)
  {
    write("Das darfst Du nicht!\n");
    return 1;
  }
  _notify_fail("Syntax: addmaster <user> <domain>\n");
  if (!str) return 0;
  if (sscanf(str,"%s %s",master,domain)!=2) return 0;
  if (!master || !domain) return 0;
  if (!"/secure/master"->add_domain_master(master,domain))
    write("Hat nicht funktioniert.\n");
  else
    write("Ok.\n");
  return 1;
}

static int _removemaster(string str)
{
  string master, domain;

  if (!GOD_SECURITY)
  {
    write("Das darfst Du nicht!\n");
    return 1;
  }
  _notify_fail("Syntax: removemaster <user> <domain>\n");
  if (!str) return 0;
  if (sscanf(str,"%s %s",master,domain)!=2) return 0;
  if (!master || !domain) return 0;
  if (!"/secure/master"->remove_domain_master(master,domain))
    write("Hat nicht funktioniert.\n");
  else
    write("Ok.\n");
  return 1;
}

static int _addguildmaster(string str)
{
  string master, guild;
  
  if (!GOD_SECURITY)
  {
    write ("Das darfst Du nicht!\n");
    return 1;
  }

  _notify_fail("Synatx: addguildmaster <user> <guild>\n");
  if (!str) return 0;
  if (sscanf(str, "%s %s", master, guild)!=2) return 0;
  if (!master || !guild) return 0;
  if (!"/secure/master"->add_guild_master(master,guild))
    write("Hat nicht funktioniert.\n");
  else
    write ("Ok.\n");
  return 1;
}

static int _removeguildmaster(string str)
{
  string master, guild;
  
  if (!GOD_SECURITY)
  {
    write ("Das darfst Du nicht!\n");
    return 1;
  }
  _notify_fail("Syntax: removeguildmaster <user> <guild>\n");
  if (!str) return 0;
  if (sscanf(str, "%s %s", master, guild)!=2) return 0;
  if (!master || !guild) return 0;
  if (!"/secure/master"->remove_guild_master(master,guild))
    write("Hat nicht funktioniert.\n");
  else
    write("Ok.\n");
  return 1;
}

static int sinners(string arg)
{   string *parts;
    int    i;

    if ( !IS_DEPUTY(this_object()) )
      return 0;

    arg=_unparsed_args()||arg;

    notify_fail(
        "Syntax: suender ?              => Liste aller Eingetragenen\n"+
        "        suender <name>         => Eintraege lesen\n"+
        "        suender +<name> <text> => Eintrag hinzufuegen\n"+
        "        suender -<name> <nr>   => Eintrag loeschen\n"+
        "        suender !              => Alle Eintraege dumpen\n"+
        "        suender *              => Alle Eintraege anzeigen\n");

    if ( !stringp(arg) || (sizeof(arg)<1) )
      return 0;

    if ( arg=="?" )
    {
        write(call_other("/secure/sinmaster","ListSinners"));
        return 1;
    }
    if ( arg=="!" )
    {
        write(call_other("/secure/sinmaster","Dump"));
        return 1;
    }
    if ( arg=="*" )
    {
        More((string)call_other("/secure/sinmaster","Dump",1));
        return 1;
    }

    if ( (i=sizeof(parts=explode(arg," ")))<1 )
      return 0;

    if ( parts[0][0..0]=="-" )
    {
      if ( i<2 )
        return 0;
      write(call_other("/secure/sinmaster","RemoveSin",
            lowerstring(parts[0][1..]),
            to_int(parts[1])));
    }
    else if ( parts[0][0..0]=="+" )
    {
      if ( i<2 )
        return 0;
      write(call_other("/secure/sinmaster","AddSin",
            lowerstring(parts[0][1..]),
            implode(parts[1..]," ")));
    }
    else
    {
      if ( i>1 )
        return 0;
      write(call_other("/secure/sinmaster","ListSins",
            lowerstring(parts[0])));
    }
    return 1;
}

static int banish(string str)
{
  string grund, name;
  int force;

  if ( !LORD_SECURITY && !IS_DEPUTY(secure_euid()) )
      return 0;

  if ( !str || !stringp(str) || !sizeof(str) ) {
      write("Syntax: banish [-f] <name> [<grund>]\n");
      return 1;
  }

  str = _unparsed_args();

  if ( explode(str, " ")[0] == "-f" ){
      str = implode( explode(str, " ")[1..], " " );
      force = 1;
  }

  if ( sscanf( str, "%s %s", name, grund ) != 2 )
      name=str;

  if ( !name || !sizeof(name) ){
    write("Syntax: banish [-f] <name> [<grund>]\n");
    return 1;
  }

  name=lower_case(name);
  "/secure/master"->BanishName( name, grund, force );
  return 1;
}

static int mbanish(string str)
{
    string grund, name, *namen, txt, *dummy;
    mapping list;
    int i;

    if ( !IS_DEPUTY(secure_euid()) )
        return 0;

    _notify_fail( "Syntax: mbanish <name> [<grund>]\n" );

    if ( !str || !stringp(str) || !sizeof(str) ){
        if ( !mappingp(list = (mapping)"/secure/merlin"->MBanishList()) ||
             !(i = sizeof(list)) ){
            write( "Momentan ist kein Spieler auf der mbanish-Liste.\n" );
            return 1;
        }

        txt = "      Name     | gebanisht von |                    Grund\n" +
            "=============================================================" +
            "==================\n";

        namen = sort_array( m_indices(list), #'</*'*/ );

        for ( ; i--; ){
            dummy = explode( break_string( list[namen[i],0] ||
                                                 "-- keine Begruendung --", 45 ),
                                   "\n" ) - ({""});

            txt += sprintf( "  %-11s  |  %-11s  |  %s\n",
                            capitalize( namen[i] ),
                            capitalize( list[namen[i],1] || "" ),
                            capitalize( implode( dummy, "\n               |"
                                                 "               |  " ) ) );
        }

        More(txt);

        return 1;
    }

    if ( sscanf( str, "%s %s", name, grund ) !=2 )
        name = str;

    if ( !name || !sizeof(name) )
        return 0;

    name = lower_case(name);

    if ( !grund || !stringp(grund) || lower_case(grund) != "loeschen" ){
        "/secure/merlin"->MBanishInsert( name, grund, this_interactive() );
        write( "Du setzt "+capitalize(name)+" auf die MBanish-Liste.\n" );
    }
    else{
        if ( !ARCH_SECURITY ){
            write( "Das duerfen nur Erzmagier.\n" );
            return 1;
        }
        "/secure/merlin"->MBanishDelete( name );
        write( "Du loescht "+capitalize(name)+" von der MBanish-Liste.\n" );
    }

    return 1;
}


static int tbanish( string str )
{
  string name;
  int days;

  if ( !IS_DEPUTY(secure_euid()) )
      return 0;

  _notify_fail("Syntax: tbanish <name> <tage>\n");

  if ( !str || !stringp(str) || !sizeof(str) )
    return 0;

  if ( sscanf(str,"%s %d",name,days) != 2 )
      return 0;

  if ( !name || !sizeof(name) )
    return 0;

  name = lower_case(name);

  if ( !"/secure/master"->TBanishName( name, days ) )
      return 1;

  if ( !days )
      write( "Okay, keine Spielpause fuer "+capitalize(name)+" mehr.\n" );
  else
      write( "Du verpasst "+capitalize(name)+" eine Spielpause fuer "+
             (days>0 ? days+" Tage" : "laaaange Zeit")+".\n" );
  return 1;
}

static int sbanish( string str )
{
    string ip;
    int days;
    mapping sites;

    // Mindestens L26 fuer diesen Befehl
    if ( secure_level() <= DOMAINMEMBER_LVL )
        return 0;

    if ( !str || !stringp(str) || !sizeof(str) ){
        if ( !sizeof(sites = (mapping)MASTER->SiteBanish( 0, 0 )) ){
            write( "Es sind zur Zeit keine Adressen gesperrt!\n" );
            return 1;
        }

        ip = "      Adresse      |  gesperrt bis                 |  gesperrt "
            + "durch\n========================================================"
            + "==============\n";

        int *keys = sort_array( m_indices(sites), #'</*'*/ );

        foreach(int key : keys) {
            ip += sprintf( "  %:15-s  |  %:27-s  |  %-s\n",
                           IPv4_int2addr(key),
                           sites[key] > 0 ? dtime(sites[key]) :
                           "St. Nimmerleinstag", 
                           capitalize(sites[key, 1]) );
        }
        write( ip + "\n" );
        return 1;
    }

    _notify_fail("Syntax: sbanish <numerische ip> <tage>\n");

    if ( sscanf( this_player()->_unparsed_args(), "%s %d", ip, days ) != 2 )
        return 0;

    if ( !ip || !sizeof(ip) )
        return 0;

//    _notify_fail( "Ungueltiges Adress-Format!\n" );

    if ( !days ){
        int res=(int)MASTER->SiteBanish(ip, 0);
        if ( res == 1 )
             printf( "Die Adresse '%s' ist jetzt nicht mehr gesperrt.\n",
                     ip );
        else if ( res == 0 )
             printf( "Die Adresse '%s' war gar nicht gesperrt!\n",
                     ip );
        else
            printf( "Du darfst nur eigene Sperrungen wieder aufheben!\n" );
    }
    else {
        int res;
        if ( days != 1 && !IS_DEPUTY(secure_euid()) )
            write( "Du darfst Adressen nur fuer einen Tag sperren!\n" );
        else if ( (res = (int)MASTER->SiteBanish(ip, days)) == 1 )
             printf( "Die Adresse '%s' ist jetzt fuer %s gesperrt.\n",
                     ip, (days > 1 ? sprintf( "%d Tage", days ) :
                      (days > 0 ? "einen Tag" : "immer")) );
        else if ( res == -1 )
            write( "Du darfst " + (LORD_SECURITY ? "255 IP-Adressen"
                                   : "nur einzelne IP-Adressen") + " sperren!\n" );
        else if ( res == -2 )
            write( "Du hast schon genug Adressen gesperrt!\n" );
    }

    return 1;
}


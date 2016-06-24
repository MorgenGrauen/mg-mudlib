#include <config.h>
#include <thing/description.h>
#include <strings.h>

// hosts who asked us in the past and timestamp of last connection which got
// the full datatset.
private nosave mapping peers = ([:1]);

/* For a list of official and extended fields:
   http://www.mudbytes.net/index.php?a=articles&s=MSSP_Fields
   For a protocal description:
   http://tintin.sourceforge.net/mssp/
   */

#define MSSP_VAR 1
#define MSSP_VAL 2

public string convert_tn(mapping ldata);

protected string list_ports() {
  int ports = query_mud_port(-1) - 1;
  string res="23";
  for(; ports>=0 ; --ports)
    res += "\t" + to_string(query_mud_port(ports));

  return res;
}

#define DESCRIPTION \
    MUDNAME" is a native German LPmud. It was founded in 1992 and has "\
    "been prospering since. The world features an original fantasy setting "\
    "with many facets. The 13 domains form a big world with 18000 rooms to "\
    "explore and several thousand NPCs. You can choose between 7 races and "\
    "10 classes (guilds). Advancing is done through a combination of "\
    "exploration points, experience points, class skills, finished "\
    "adventures and some more. The levels are not limited, current maximum "\
    "is about 200. After becoming a high level player it's possible to "\
    "become a wizard and add your own imagination to the game. We are "\
    "especially proud of attracting a number of visually impaired players "\
    "who very much enjoy playing a text based online RPG. Thats why we have "\
    "an option to turn ascii-graphics off. Multiplaying is a bit restricted "\
    "and scripting is discouraged."

private nosave mapping mindata = ([
    "NAME"             : MUDNAME,
    "UPTIME"           : to_string(__BOOT_TIME__),
    ]);

private nosave mapping data = mindata + ([
    "PORT"             : list_ports(),
    "CODEBASE"         : _MUDLIB_NAME_+"-"+_MUDLIB_VERSION_,
    "HOSTNAME"         : __HOST_NAME__ + "." + __DOMAIN_NAME__,
#if MUDHOST == __HOST_NAME__
// diese Angaben sollen nur gesendet werden, wenn das Mud wirklich auf dem
// MG-Rechner laeuft und kein Homemud ist.
    "CRAWL DELAY"      : "1",
    "DESCRIPTION"      : DESCRIPTION,
    "CREATED"          : "1992",
    "ICON"             : "http://mg.mud.de/newweb/img/icon.gif",
    "IP"               : "87.79.24.60",
    "CONTACT"          : "mud@mg.mud.de",
    "WEBSITE"          : "http://mg.mud.de/",
    "AREAS"            : "13",
    "MOBILES"          : "2600",
    "OBJECTS"          : "4000",
    "ROOMS"            : "18000",
    "CLASSES"          : "10",
    "LEVELS"           : "200",
    "RACES"            : "7",
    "SKILLS"           : "140",
    "MULTICLASSING"    : "0",
    "PLAYERKILLING"    : "Restricted",
    "NEWBIE FRIENDLY"  : "1",
    "ROLEPLAYING"      : "Accepted",
    "WORLD ORIGINALITY": "Mostly Original",
    "MINIMUM AGE"      : "6",
    "SSL"              : "4712",
    "STATUS"           : "Live",
    "STATUS-NOTES"     : "live and running",
    "PORT-NOTES"       : "player ports are 23 and 4711, SSL port is 4712",
    "QUEST-NOTES"      : "We have about 200 Quests and MiniQuests and "
                         "they play an important role in gaining "
                         "levels and skills.",
    "MULTIPLAYING-NOTES"     : "for combat and quests only 2 characters may be active",
#else
    "STATUS"           : "Alpha",
    "STATUS-NOTES"     : "Homemud for testing puposes",
#endif
    "LANGUAGE"         : "German",
    "LOCATION"         : "Germany",
    "FAMILY"           : "LPMud",
    "GENRE"            : "Fantasy",
    "SUBGENRE"         : "None",
    "GAMEPLAY"         : "Adventure",
    "GAMESYSTEM"       : "Custom",
    "INTERMUD"         : "IM2-Zebedee",
    "HELPFILES"        : "N/A",
    "MULTIPLAYING"     : "Restricted",
    "EQUIPMENT SYSTEM" : "Both",
    "TRAINING SYSTEM"  : "Both",
    "QUEST SYSTEM"     : "Integrated",
    "ZMP"              : "0",
    "ANSI"             : "1",
    "GMCP"             : "1",
    "MCCP"             : "0",
    "MCP"              : "0",
    "MSP"              : "0",
    "MXP"              : "0",
    "PUEBLO"           : "0",
    "VT100"            : "1",
    "XTERM 256 COLORS" : "0",
    "PAY TO PLAY"      : "0",
    "PAY FOR PERKS"    : "0",
    "HIRING BUILDERS"  : "1",
    "HIRING CODERS"    : "1",
    "CODEBASE-NOTES"         : "download daily snapshot of our public base "
                               "mudlib at ftp://mg.mud.de/Software/MudLib/, "
                               "get our driver at http://www.ldmud.eu/",
    "FAMILY-NOTES"           : "Descendant of Nightfall, base for several "
                               "german MUDs, uses LDMud-3.5.x", 
    "HELPFILES-NOTES"        : "each basic command, and hundreds of other docs",
    "MOBILES-NOTES"          : "npc's can be cloned, so there can be thousands",
    "OBJECTS-NOTES"          : "objects can be cloned, so there can be thousands",
    "RESETS-NOTES"           : "doesn't apply, LPMud",
    "MUDPROGS-NOTES"         : "doesn't apply, LPMud",
    "MUDTRIGS-NOTES"         : "doesn't apply, LPMud",
    "RACES-NOTES"            : "some additional for non player characters",
    "SKILLS-NOTES"           : "each class/guild and race has its own set of skills",
    "PLAYERKILLING-NOTES"    : "playerkilling is limited to specific rooms",
/*
    "ADULT MATERIAL"   : "0",
    "PLAYER CITIES"    : "0",
    "PLAYER CLANS"     : "0",
    "PLAYER CRAFTING"  : "0",
    "PLAYER GUILDS"    : "0",
    "ROOMS-NOTES"            : "areas have generated rooms, could be millions",
 */
    ]);


// cache fuer die telnetneg-basierte variante
private nosave string tn_result_min = convert_tn(mindata);
private nosave string tn_result = convert_tn(data);


// converts into the array to be sent via telnet suboption negotiation.
public string convert_tn(mapping ldata) {
  string res="";
  foreach(string key, string value: ldata) {
    res += sprintf("%c%s%c%s", MSSP_VAR, key, MSSP_VAL, value);
  }
  return res;
}


public void print_mssp_response() {
 string ip = query_ip_number(previous_object());
 mapping ldata;
 if (stringp(ip)) {
     // Vollen Datensatz alle ("CRAWL DELAY" / 2) h, daher * 1800.
   if (peers[ip] > (time() - (to_int(data["CRAWL DELAY"]) || 1) * 1800)) {
       // this peers asks to often and gets only the minimal dataset
       ldata = mindata;
   }
   else {
       ldata = data;
       peers[ip] = time(); // record timestamp
   }
 }
 else
     ldata = data;

 /* data["WHO"] = implode(map(filter(users(), 
                      function status (object o)
                      { return !o->QueryProp(P_INVIS); } ),
                    function string (object o)
                    { return capitalize(o->query_real_name()); } ),
                  "\t" );
  */
  ldata["PLAYERS"] = to_string(sizeof(users())-1);

  string reply = "\r\nMSSP-REPLY-START\r\n";

  foreach(string key, string value: ldata) {
    reply += key + "\t" + value + "\r\n";
  }

  reply += "MSSP-REPLY-END\r\n";
  write(reply);
}

public string get_telnegs_str() {
  string ip = query_ip_number(previous_object());
  string res;

  if (stringp(ip)) {
    // Vollen Datensatz alle ("CRAWL DELAY" / 2) h, daher * 1800.
    if (peers[ip] > (time() - (to_int(data["CRAWL DELAY"]) || 1) * 1800)) {
       // this peers asks to often and gets only the minimal dataset
       res = tn_result_min;
    }
    else {
        res = tn_result;
        peers[ip] = time(); // record timestamp
    }
  }
  else
    res = tn_result;

  res += convert_tn( (["PLAYERS": to_string(sizeof(users())-1) ]) );
   
  return res;
}


// MorgenGrauen MUDlib
//
// players.c
//
// $Id: players.c 9551 2016-04-20 22:54:58Z Arathorn $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone

#include <wizlevels.h>
#include <ansi.h>
#include <logging.h>
#define NEED_PROTOTYPES
#include <magier.h>
#include <thing/properties.h>
#include <thing/description.h>
#include <living/comm.h>
#include <player.h>
#undef NEED_PROTOTYPES
#include <properties.h>
#include <moving.h>

static mixed _query_localcmds()
{
  return ({({"zap","_zap",0,WIZARD_LVL}),
           ({"verfolge","_verfolge",0,LEARNER_LVL}),
           ({"trans","_trans",0,LEARNER_LVL}),
           ({"peace","_frieden",0,LEARNER_LVL}),
           ({"frieden","_frieden",0,LEARNER_LVL}),
           ({"pwho","_pwho",0,WIZARD_LVL}),
           ({"zwinge","_zwinge",0,WIZARD_LVL}),
           ({"heal","_heile",0,WIZARD_LVL}),
           ({"heil","_heile",1,WIZARD_LVL}),
           ({"people","_people",0,LEARNER_LVL}),
           ({"spieler","_spieler",0,WIZARD_LVL})});
}

//                               #######
//################################ ZAP ##################################
//                               #######

private string _zap_message(string str, object obj)
{
  str=regreplace(str,"@@wer@@",({string})obj->name(WER,2),1);
  str=regreplace(str,"@@wessen@@",({string})obj->name(WESSEN,2),1);
  str=regreplace(str,"@@wem@@",({string})obj->name(WEM,2),1);
  str=regreplace(str,"@@wen@@",({string})obj->name(WEN,2),1);
  str=regreplace(str,"@@ich@@",name(WER,2),1);
  return capitalize(str);
}

static int _zap(string str)
{
    object opfer;
    string *message, dummy;
    int spieler;

    if (!str) return USAGE("zap <name>");
    if (sscanf( str, "spieler %s", dummy ))
    {
      str = dummy;
      spieler = 1;
    }
    if (opfer=present(str,environment()))
    {
      if ( !living(opfer) )
      {
        printf("%s ist doch gar kein Lebewesen!\n",capitalize(str) );
        return 1;
      }
      if (query_once_interactive(opfer)&&!spieler )
      {
        printf( "Spieler kannst Du nur mit der Syntax 'zap spieler <name>' "
                "toeten!\n" );
        return 1;
      }
      else
        if ( !query_once_interactive(opfer) && spieler )
        {
          printf( "Aber %s ist doch gar kein Spieler!\n",capitalize(str));
          return 1;
        }

      message = QueryProp(P_ZAP_MSG);

      if ( !pointerp(message) || sizeof(message) != 3 ){
          tell_room(environment(),sprintf("%s beschwoert einen Blitz "
                "vom Himmel.\n",capitalize(getuid())),({ this_object() }));
          printf("Du toetest %s.\n",opfer->name( WEN,2));
      }
      else
      {
        printf(_zap_message(message[0],opfer));
        tell_room(environment(),_zap_message(message[1],opfer),
             ({this_player(),opfer}));
        tell_object(opfer,_zap_message(message[2],opfer));
      }

      opfer->die();
      return 1;
  }
  else{
      printf("Sowas siehst Du hier nicht.\n");
      return 1;
  }
}


//                             ############
//############################## VERFOLGE ################################
//                             ############

static int _verfolge(string str)
{
  // Wenn nichts eingegeben wurde, wird ver Verfolgungsmodus beendet, sofern
  // er zuvor eingeschaltet war. Ansonsten wird eine Fehlermeldung 
  // ausgegeben.
  if (!sizeof(str))
  {
    mixed *pur = Query(P_PURSUERS);
    if ( pointerp(pur) && sizeof(pur) && objectp(pur[0]) )
    {
      pur[0]->RemovePursuer(this_object());
      ReceiveMsg("Verfolgungsmodus abgeschaltet.", MT_NOTIFICATION);
    }
    else
    {
      ReceiveMsg("Du verfolgst doch ueberhaupt niemanden.", MT_NOTIFICATION);
    }
    return 1;
  }
  str=lower_case(str);
  
  // match_living() erlaubt die Pruefung, ob die Angabe eindeutig war.
  int|string lv = match_living(str);
  if ( intp(lv) )
  {
    if ( lv == -2 )
      ReceiveMsg("Kein solches Wesen gefunden.", MT_NOTIFICATION);
    else
      ReceiveMsg(sprintf("verfolge: '%s' ist nicht eindeutig.\n", str),
        MT_NOTIFICATION);
    return 1;
  }
  
  // Spieler zuerst auswaehlen, danach im Raum anwesende Lebewesen.
  object ziel = find_player(lv) || present(lv, environment(this_player()));
  
  // Wurde kein Lebewesen gefunden, wird das erste Element aus der Liste der
  // Lebewesen dieses Namens gewaehlt, falls vorhanden. Nur Lebewesen mit
  // Environment kommen in Frage, denn sonst gibt es keinen Raum, in den der
  // neue Verfolger bewegt werden koennte.
  if ( !objectp(ziel) ) {
    object* eligible_livings = filter(find_livings(lv), #'environment);
    if (sizeof(eligible_livings))
      ziel = eligible_livings[0];
  }
  
  // Endlich etwas gefunden? Dann Verfolger eintragen und zum Ziel bewegen.
  if ( objectp(ziel) )
  {
    if ( ziel == this_player() ) 
    {
      ReceiveMsg("Du kannst Dich nicht selbst verfolgen.", MT_NOTIFICATION);
    }
    else
    {
      ReceiveMsg(sprintf(
        "Du verfolgst jetzt %s. [%s]", ziel->name(WEN), object_name(ziel)),
        MT_NOTIFICATION, MA_MOVE);
      ziel->AddPursuer(this_object());
      ziel->TakeFollowers();
    }
  }
  else
  {
    ReceiveMsg("Kein Wesen mit dem Namen '"+lv+"' gefunden, oder nur "
      "solche ohne Environment.", MT_NOTIFICATION, 0, "verfolge: ");
  }
  return 1;
}


//                              #########
//############################### TRANS #################################
//                              #########

static int _trans(string str)
{
  object living;

  if (!sizeof(str))
    return _notify_fail("Syntax: trans <spielername>\n"),0;
  string|int livname=match_living(str,0);
  if (intp(livname))
    switch (livname)
    {
      case -1: write("Das war nicht eindeutig.\n"); return 1;
      case -2: write("So ein Wesen gibt es nicht.\n"); return 1;
    }
  if(living=find_living(livname))
  {
    if (living->move(object_name(environment()),
                     M_TPORT|M_NOCHECK)<=0)
    {
      printf("Teleportieren von %s fehlgeschlagen.\n",living->Name(WEM));
      if (IS_LEARNER(living))
        tell_object(living,sprintf("%s wollte Dich teleportieren, "
             "hat aber versagt!\n",capitalize(getuid())));
      return 1;
    }
    tell_object(living,sprintf(
    "Ein seltsames Gefuehl ueberkommt Dich ...\n"
    "Du verlierst die Orientierung ...\n"
    +(QueryProp(P_INVIS)?"":"%s holt Dich zu sich.\n"),
    capitalize(getuid())));
    printf("%s wurde herbeizitiert.\n",living->Name(WER));
    return 1;
  }
  printf("Das Lebewesen '%s' konnte nicht gefunden werden.\n",
         capitalize(str));
  return 1;
}

//                             ###########
//############################## FRIEDEN #################################
//                             ###########

static int _frieden(string sname)
{
  if (!sname)
  {
    object *enemies=all_inventory(environment());
    map_objects(enemies,"StopHuntingMode");
    tell_room(environment(),sprintf("%s stiftet Frieden.\n",
              capitalize(getuid())), ({ this_object()}));
    printf("Du stiftest Frieden.\n");
  }
  else
  {
    object obj = find_living(sname);
    if (!obj)
    {
      printf("Kein solches Lebewesen im Spiel.\n");
      return 1;
    }
    string him=({string})obj->name(WEM);
    mapping enemies = ({mapping})obj->StopHuntingMode(1);
    foreach(object en : enemies)
    {
      en->StopHuntFor(obj, 1);
      tell_object(obj,sprintf("%s beendet Deinen Kampf mit %s.\n",
                              capitalize(getuid()),en->Name(WEM)));
      tell_object(en,sprintf("%s beendet Deinen Kampf mit %s.\n",
                                  capitalize(getuid()),him));
    }
    printf("%s und alle Gegner wurden befriedet.\n",obj->Name(WER));
  }
  return 1;
}

//                              ########
//############################### PWHO ##################################
//                              ########

#if __VERSION__ < "3.2.9"
private int _pwho_learner_test(object ob)
{
  return !IS_LEARNER(ob);
}
#endif

static int _pwho()
{
  mixed* spieler, res, *hands;
  int i;
#if __VERSION__ < "3.2.9"
  spieler = filter(users(),#'_pwho_learner_test);
#else
  spieler = filter(users(),(: return !IS_LEARNER($1); :));
#endif
  spieler = sort_array(spieler, function int (object a, object b)
      { return a->QueryProp(P_LEVEL) > b->QueryProp(P_LEVEL); } );
  
  res = "Lvl Name         Erfahrung   QP  Int Str Dex Con WC   "
    "AC   HANDS HP  (max)\n"
    "--------------------------------------------------------------"
    "-----------------\n";
  for (i=sizeof(spieler)-1; i>=0; i--)
    res += sprintf("%3d %-12s %9d %5d %3d %3d %3d %3d %4d %4d  %5d "
                   "%4d (%4d)\n",
     spieler[i]->QueryProp(P_LEVEL),
     capitalize(getuid(spieler[i])),
     spieler[i]->QueryProp(P_XP),
     spieler[i]->QueryProp(P_QP),
     spieler[i]->QueryAttribute(A_INT),
     spieler[i]->QueryAttribute(A_STR),
     spieler[i]->QueryAttribute(A_DEX),
     spieler[i]->QueryAttribute(A_CON),
     spieler[i]->QueryProp(P_TOTAL_WC),
     spieler[i]->QueryProp(P_TOTAL_AC),
     (sizeof(hands=(({int *})spieler[i]->QueryProp(P_HANDS)))?hands[1]:0),
     spieler[i]->QueryProp(P_HP),
     spieler[i]->QueryProp(P_MAX_HP));
  More(res);
  return 1;
}

//                             ##########
//############################## ZWINGE #################################
//                             ##########

static int _zwinge(string str)
{
  object living;
  string what, rest;
  string living_name;

  str = _unparsed_args();
  if(!str|| sscanf( str, "%s %s", living_name, what ) != 2 )
    return _notify_fail("Zwinge WEN zu WAS?\n"),0;
  if( sscanf( what, "zu %s", rest ) == 1 ) what = rest;
  if (!(living = find_living(living_name)))
    return printf ("Ein Lebewesen namens '%s' konnte nicht gefunden werden!\n",
                   capitalize(living_name)),1;
  if (living->command_me(what))
  {
    printf("Du zwingst %s zu \"%s\".\n",capitalize(living_name),what);
    if (!IS_ARCH(this_object()) &&
        getuid()!=({string|int})living->QueryProp(P_TESTPLAYER))
      log_file(SHELLLOG("ZWINGE"),
               sprintf("%s zwingt %s (%s) zu %s [%s]\n",
                       capitalize(getuid()),living->Name(),capitalize(getuid(living)),
                       what,dtime(time())));
  }
  else
    write("Hat leider nicht geklappt!\n");
  return 1;
}

//                              #########
//############################### HEILE #################################
//                              #########

static int _heile(string name)
{
  object ob;
  int lpv, mpv;

  if (!name) return USAGE("heile <name>");
  name = lower_case(name);
  if ((!(ob = present(name,environment())))
      ||!living(ob))
    ob = find_living(name);
  if (!ob)
  {
    printf("'%s' ist momentan nicht da.\n",capitalize(name));
    return 1;
  }
  lpv = ({int})ob->QueryProp(P_HP);
  mpv = ({int})ob->QueryProp(P_SP);
  ob->heal_self(1000000);
  string|int testie = ({string|int})ob->QueryProp(P_TESTPLAYER);
  if (!IS_LEARNER(ob)
      && (!testie || (to_string(testie)[<5..<1] == "Gilde")))
  {
    log_file(SHELLLOG("HEAL"),
       sprintf("%s heilt %s (%s) %s (LP: %d -> %d, MP: %d -> %d)\n",
         capitalize(geteuid(this_player())),
         call_other(ob,"name"), capitalize(geteuid(ob)),
         dtime(time()), lpv, ({int})ob->QueryProp(P_HP),
               mpv,({int})ob->QueryProp(P_SP)));
  }
  tell_object(ob, QueryProp(P_NAME) + " heilt Dich.\n");
  printf("Du heilst %s.\n",capitalize(name));
  return 1;
}

//                             ##########
//############################## PEOPLE #################################
//                             ##########

private string _people_filename(object obj)
{
  string str;
  str=object_name(environment(obj));
  if (!str) return 0;
  if (str[0..2] == "/d/") return sprintf("+%s",str[3..<1]);
  if (str[0..8] == "/players/") return sprintf("~%s",str[9..<1]);
  return str;
}

static int _people()
{
  mixed *list, res;
  int i,j, a;
  string a_age,a_ipnum,a_name,a_level,a_idle,a_room,a_end, a_title;

  switch(QueryProp("tty"))
  {
    case "vt100":
      a_ipnum = ""; a_name = ANSI_BOLD;
      a_level = ANSI_NORMAL; a_idle = ANSI_BLINK;
      a_room = ANSI_NORMAL; a_end = ANSI_NORMAL;
      a_title = ANSI_INVERS; a_age = ANSI_NORMAL;
      break;
    case "ansi":
      a_ipnum = ANSI_BLUE; a_name = ANSI_BOLD;
      a_level = ANSI_RED; a_idle = ANSI_BLACK+ANSI_BOLD;
      a_room = ANSI_BOLD+ANSI_BLUE; a_end = ANSI_NORMAL;
      a_title = ANSI_INVERS; a_age = ANSI_PURPLE;
      break;
    default:
      a_title = a_ipnum = a_name = a_level = a_idle = a_room = a_end = "";
      a_age = "";
  }
  list = sort_array(users(), function int (object a, object b) {
      return query_ip_number(a)>query_ip_number(b);} ); 

  j=sizeof(list);
  a=0;res="";
  for(i=0; i<sizeof(list); i++) {
    string name_; 
    name_ = capitalize(list[i]->query_real_name()||"<logon>");
    res += sprintf( "%s%-15s%s %s%-13s%s %s%3d%s %s %s %s%s%s%s %s%s\n",
                    a_ipnum, query_ip_number(list[i]),a_end,a_name,
                    (list[i]->QueryProp(P_INVIS)?"("+name_+")":name_),
                    a_end,a_level, MASTER->get_wiz_level(getuid(list[i])),
                    a_end,a_age,
                    time2string("%4x %0X",(({int})list[i]->QueryProp(P_AGE))*2),
                    query_idle(list[i])>=300?(a++,(a_idle+"I")):" ",
                    a_end,
                    query_editing(list[i])?a_idle+"E"+a_end:" ",
                    query_input_pending(list[i])?a_idle+"M"+a_end:" ",
                    environment(list[i])?a_room+_people_filename(list[i]):"",
                    a_end);
  }
  if (a)
    res = sprintf("%s%d Spieler anwesend (%d aktiv). %s%s\n",a_title,j,
                  (j-a),query_load_average(),a_end)+res;
  else
    res = sprintf("%s%d Spieler anwesend. %s%s\n",a_title,j,
                  query_load_average(),a_end)+res;
  More(res);

  return 1;
}


//                             ###########
//############################## SPIELER #################################
//                             ###########

private string _spieler_time2string(int time)
{
  string ret;

  ret="";
  if (time>=86400)
  {
    ret+=time/86400+"d ";
    time%=86400;
  }
  if(time<36000) ret+="0";
  ret+=time/3600+":";
  time%=3600;
  if(time<600) ret+="0";
  ret+=time/60+":";
  time%=60;
  if(time<10) ret+="0";
  ret+=time+"";
  return ret;
}


static int _spieler(string arg)
{
  string dummy,ip;

  arg=_unparsed_args();
  if(!sizeof(arg) || sscanf(arg,"aus ip %s",dummy)!=1)
    return USAGE("spieler aus ip [von <spieler>|<ip>]");
  arg=dummy;
  if (sscanf(arg,"von %s",dummy)==1)
  {
    dummy=lower_case(dummy);
    object pl = find_player(dummy);
    if (!pl) {
      notify_fail(sprintf("Spieler '%s' konnte nicht gefunden "
                                 "werden.\n",capitalize(dummy)));
      return 0;
    }
    ip=query_ip_number(pl);
  }
  else
    ip=arg;

  ip=implode((explode(ip,".")-({""})+({"*","*","*","*"}))[0..3],".");
  object *spieler;
  if (catch(spieler=filter(users(),
            function int (object u, string re) {
              return sizeof(regexp(({query_ip_number(u)}),re));
            } )))
  {
    printf("In der IP duerfen nur Zahlen(0-255), Punkte (.) und "
                "Sterne (*) vorkommen.\n");
    return 1;
  }

  if (!sizeof(spieler))
    return printf("Es konnte kein Spieler mit der IP '%s' gefunden "
                  "werden.\n",ip),1;

  arg=sprintf("\nFolgende Spieler haben die IP %s:\n"
       "================================================================"
              "===========\n"
       "Name:       Zweitie von:      Eingeloggt:                  "
              "Idle seit:\n"
       "----------------------------------------------------------------"
              "-----------\n",ip);

  foreach(object u: spieler)
  {
    string second=u->QueryProp(P_SECOND);
    if (stringp(second) && sizeof(second))
    {
      if (!master()->find_userinfo(second))
        second = "*ungueltig*";
      else
        second = capitalize(second);
    }
    else
      second = "";

    arg+=sprintf("%-11s %-17s %26s  %-15s\n",
                 capitalize(getuid(u)), second,
                 dtime(u->QueryProp(P_LAST_LOGIN)),
                 _spieler_time2string(query_idle(u)));
  }
  arg+="==============================================================="
    "============\n\n";
  More(arg);
  return 1;
}

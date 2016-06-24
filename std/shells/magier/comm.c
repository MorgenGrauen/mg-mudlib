// MorgenGrauen MUDlib
//
// comm.c
//
// $Id: comm.c 8755 2014-04-26 13:13:40Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <wizlevels.h>
#include <magier.h>
#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing/description.h>
#include <player.h>
#include <properties.h>


//                           ###########
//############################ ECHOALL ################################
//                           ###########

static int _echoall(string str)
{
  if (!(str=_unparsed_args()))
    return USAGE("echoall <text>\n");
  str=break_string(str);
  shout(str);
  printf(str);
  return 1;
}

//                           ##########
//############################ ECHOTO #################################
//                           ##########

static int _echoto(string str)
{
  object ob;
  string who;
  string msg;

  if (!(str=_unparsed_args())||sscanf(str, "%s %s", who, msg) != 2)
    return USAGE("echoto <spieler> <text>\n");
  ob = find_player(lower_case(who));
  if (!ob)
  {
    printf("echoto: Es ist kein Spieler '%s' eingeloggt.\n",who);
    return 1;
  }
  msg=break_string(msg,78);
  tell_object(ob,msg);
  printf("%s->%s",ob->Name(WEN),msg);
  return 1;
}

//                           #########
//############################ MECHO ##################################
//                           #########

static int _mecho(string str)  {
  object *who;
  int i;

  if (!sizeof(str=_unparsed_args()))
    return USAGE("mecho <text>\n");
  who=users();
  i=sizeof(who);
  while(i--)if (IS_LEARNER(who[i]))
      tell_object(who[i], break_string(str,78));
  return 1;
}

//                            ########
//############################# PING ##################################
//                            ########

static int _ping(string str)
{
  object pl;

  if (!sizeof(str))
    return USAGE("ping <spielername>\n");
  if (!(pl=find_player(lower_case(str))))
    return
      _notify_fail(sprintf("ping: Spieler %s nicht gefunden.\n",
                           capitalize(str))),0;
  tell_object(pl,sprintf("%s pingt Dich an.\a\n",capitalize(getuid())));
  return printf("PINGGGGGGG! DAS sollte %s auf Dich aufmerksam "
                "gemacht haben.\n",capitalize(getuid(pl))),1;
}

//                           ##########
//############################ OROPAX #################################
//                           ##########

int _oropax(string cmdline)
{
  int level,old,new;

  cmdline=_unparsed_args();
  old=QueryProp(P_EARMUFFS);
  if (!sizeof(cmdline)||cmdline=="0")
  {
    if (old)
    {
      printf("Du nimmst das Oropax aus Deinen Ohren und hoerst "
             "wieder allen anderen zu.\n");
      SetProp(P_EARMUFFS, 0);
    }
    else
      printf("Du hast doch gar kein Oropax in den Ohren.\n");
    return 1;
  }
  if (sscanf(cmdline, "%d", level) == 0 || level < 1)
    return USAGE("oropax [<magierlevel>]\n");
  if ((new=SetProp(P_EARMUFFS, level))==level)
  {
    if (new==old)
      return printf("Du hattest Deine Oropaxmenge schon auf Magierlevel "
                    "%d angepasst.\n",level),1;
    if (new>old)
      return printf("Du stopfst Dir soviel Oropax in die Ohren, dass "
                    "Du nur noch %s ab\nLevel %d hoerst.\n",
                    level>=LEARNER_LVL?"Magier":"Seher",level),1;
    return printf("Du nimmst soviel Oropax aus den Ohren, dass Du ab "
                  "sofort wieder %s ab\nLevel %d verstehst.\n",
                  level>=LEARNER_LVL?"Magier":"Seher",level),1;
  }
  return printf("Du stopfst und stopfst, bis Dir das Oropax wieder zur "
                "Nase herauskommt.\nLeider musst Du damit Magier ab Level "
                "%d weiterhin hoeren.\n",new),1;
}

//                         ###################
//########################## INITIALISIERUNG #############################
//                         ###################

static mixed _query_localcmds()
{
  return
    ({({"oropax","_oropax",0,LEARNER_LVL}),
      ({"echoto","_echoto",0,LEARNER_LVL}),
      ({"echoall","_echoall",0,LEARNER_LVL}),
      ({"mecho","_mecho",0,LEARNER_LVL}),
      ({"ping","_ping",0,LEARNER_LVL})});
}

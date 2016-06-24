// $Id: objects.c 8848 2014-06-11 22:05:04Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <wizlevels.h>
#include <moving.h>
#define NEED_PROTOTYPES
#include <thing/language.h>
#include <thing/properties.h>
#include <thing/description.h>
#include <living/put_and_get.h>
#include <player.h>
#include <magier.h>

//                        ###################
//######################### INITIALISIERUNG #############################
//                        ###################


static mixed _query_localcmds()
{
  return ({({"clone","_clone",0,WIZARD_LVL}),
           ({"setcmsg","_setcmsg",0,WIZARD_LVL}),
           ({"setdmsg","_setdmsg",0,WIZARD_LVL}),
           ({"destruct","_destruct",0,WIZARD_LVL}),
          });
}

//                              #########
//############################### CLONE ##############################
//                              #########

static int _clone(string cmdline)
{
  mixed *tmp;
  int flags;
  object ob;
  string *args,*args2,err;
  
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,CLONE_OPTS,1);
  if (flags==-1||sizeof(args)!=1)
    return USAGE("clone [-" CLONE_OPTS "] <objektname>");
  if (flags&CLONE_F)
    cmdline=args[0];
  else
  {
    // etwas umstaendlich, aber so kann man auch Dateien clonen,
    // wenn man keine Leserechte hat. Man muss aber im Verzeichnis
    // lesen koennen
    args2=explode(args[0],"/");
    if (args2[<1][<1]=='.') args2[<1]+="c";
    else if (args2[<1][<2..<1]!=".c") args2[<1]+=".c";
    tmp=file_list(({implode(args2[0..<2],"/")+"/*"}),MODE_CLONE,0,"",
                  args2[<1]);
    if (!sizeof(tmp)||tmp[0][FILESIZE]<0)
      return printf("clone: %s: Datei existiert nicht.\n",args[0]),1;
    cmdline=tmp[0][FULLNAME];
  }

  if (err=catch(ob=clone_object(cmdline))||!ob)
    return printf("clone: %s: Objekt konnte nicht erzeugt werden.\n"
                  "Grund: %O",
                  args[0],err||"unbekannt"),1;
  if (!objectp(ob))
    return printf("clone: %s: Objekt beim Erzeugen zerstoert.\n",
                  args[0]),1;
  if ((ob->move(this_object(),M_GET)>0) || 
      (!objectp(ob)||ob->move(environment(),M_NOCHECK)>0)||!objectp(ob))
  {
    if (!objectp(ob))
      return printf("clone: %s: Objekt beim Erzeugen zerstoert.\n",
                  args[0]),1;
    printf("Clone: %s erzeugt.\n",object_name(ob));
    tell_room(environment(this_object()),
              sprintf("%s %s.\n",Name(WER,1),QueryProp(P_CLONE_MSG)),
                      ({ this_object()}));
    return 1;
  }
  tell_room(environment(this_object()),
            sprintf("%s malt wilde Zeichen in die Luft und "
                    "murmelt vor sich hin, aber nichts "
                    "passiert...\n",Name(WER,1)),
            ({ this_object()}));
  destruct(ob);
  printf("Clone: %s: Objekt konnte nicht bewegt werden.",args[0]);
  return 1;
}


//                            ############
//############################# DESTRUCT ##############################
//                            ############

//
// _destruct: Objekte zerstoeren
//

static int _destruct(string cmdline)
{
  int flags;
  mixed *args;
  object ob;

  if (!sizeof(cmdline=_unparsed_args()))
    return USAGE(query_verb()+" <objektname>");
  args=find_obs(lower_case(cmdline),PUT_GET_NONE);
  if (!args||!sizeof(args))
  {
    if (!(ob=find_object(cmdline)))
    {
      notify_fail(query_verb()+": Objekt \"" +cmdline+
                  "\" nicht gefunden.\n");
      return 0;
    }
  }
  else
    ob=args[0];
  cmdline=capitalize(to_string(ob->name(WER)));
  flags=(int)ob->QueryProp(P_PLURAL); // Missbrauch von flags :o)
  if (query_verb()=="destruct")
  {
    if (!ob->remove())
    {
      notify_fail(cmdline+" will nicht zerstoert werden!\n");
      return 0;
    }
  }
  else destruct(ob);
  if (!ob)
  {
    if (!QueryProp(P_INVIS))
    {
      tell_room(environment(this_object()),
                sprintf("%s %s.\n",cmdline,QueryProp(P_DESTRUCT_MSG)),
                ({ this_object() }));
    }
    printf("%s w%s von Dir zerstaeubt.\n",cmdline,(flags?"erden":"ird"));
  }
  else
    printf("%s kann nicht zerstoert werden.\n",cmdline);
  return 1;
}

//                        ####################
//######################### SetCMsg, SetDMsg ############################
//                        ####################

static int _setcmsg(string str)
{
  printf("Beim Clonen von Objekten sehen die Anderen nun:\n"
         "<Dein Name> %s.\n",
         (SetProp(P_CLONE_MSG, _unparsed_args()||"zaubert etwas aus "
          + QueryPossPronoun(MALE,WEM) + " Aermel hervor")));
  return 1;
}

static int _setdmsg(string str)
{
  printf("Beim Zerstoeren von Objekten sehen die Anderen nun:\n"
         "<Objekt> %s.\n",
         SetProp(P_DESTRUCT_MSG, _unparsed_args()||"wird von " + name(WER,1)
          + " zerstaeubt"));
  return 1;
}

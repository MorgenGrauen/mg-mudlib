// MorgenGrauen MUDlib
//
// tell.c
//
// $Id: tell.c 8755 2014-04-26 13:13:40Z Zesstra $

#include <config.h>
#include <udp.h>
#include <properties.h>

#ifdef ZEBEDEE
#include <system.h>
inherit "/sys/format";
#elif !defined(INETD)
#define INETD "/secure/inetd"
#endif


void udp_tell(mapping data) {
  object ob;
  string message_string, message_prefix, away;
  int i,re;
  string *message_array;

  if (data[RECIPIENT] &&
       (ob = find_player(lower_case(data[RECIPIENT]))) &&
       interactive(ob)) {
    
    if (!stringp(data[SENDER]) || !sizeof(data[SENDER]))
	data[SENDER]="<Unknown>";
    if (!stringp(data[DATA]) || !sizeof(data[DATA]))
	data[DATA]="<Nichts>";
   
    data[SENDER]=
      implode(filter(explode(data[SENDER], ""),
                           #'>=,/*'*/ " "), "");
    data[DATA]=
      implode(filter(explode(data[DATA], ""),
                           #'>=,/*'*/ " "), "");
    message_prefix=capitalize(data[SENDER])+"@"+data[NAME]+
                   " teilt Dir mit: ";
    message_string=break_string(data[DATA],78,message_prefix,0);

    /* Die Anzahl der Leerzeilen am Ende eines tm's scheint nicht genormt */
    while(message_string[<1]=='\n')message_string=message_string[0..<2];
    message_string += "\n";

    re = ob->Message("\n"+message_string, MSGFLAG_RTELL);

    if (!ob->QueryProp(P_INVIS)){
      /* Erst testen, ob die Meldung ueberhaupt angekommen ist! */
      if(re==MESSAGE_DEAF)
        message_string=sprintf("%s@"MUDNAME" ist momentan leider taub.\n",
                               capitalize(getuid(ob)));
      else if(re==MESSAGE_IGNORE_YOU)
        message_string=sprintf("%s@"MUDNAME" ignoriert Dich.\n",
                               capitalize(getuid(ob)));
      else if(re==MESSAGE_IGNORE_MUD)
        message_string=sprintf("%s@"MUDNAME" ignoriert das Mud '%s'.\n",
                               capitalize(getuid(ob)),
                               data[NAME]);
      else {
        /* Erst dann die Erfolgs-Rueckmeldung abschicken */
        message_prefix="Du teilst "+capitalize(data[RECIPIENT]) + "@" 
                       LOCAL_NAME + " mit: ";
        message_string=break_string(data[DATA],78,message_prefix,0);
        if(ob->QueryProp(P_AWAY))
          message_string=sprintf("%s%s@"MUDNAME" ist gerade nicht "
                                 "da: %s\n",
                                 message_string,
                                 ob->name(WER),
                                 ob->QueryProp(P_AWAY));
        else if ((i=query_idle(ob))>=600){ // 10 Mins
          if (i<3600) away=time2string("%m %M",i);
          else away=time2string("%h %H und %m %M",i);
          message_string=
             sprintf("%s%s@"MUDNAME" ist seit %s voellig untaetig.\n",
                     message_string,
                     ob->Name(WER),
                     away);
        }
        switch(re) {
        case MESSAGE_CACHE:
          message_string +=
            sprintf("%s@"MUDNAME" moechte gerade nicht gestoert werden.\n"+
                    "Die Mittelung wurde von einem kleinen Kobold in Empfang"+
                    "genommen.\nEr wird sie spaeter weiterleiten.\n",
                    capitalize(getuid(ob)));
          break;
        case MESSAGE_CACHE_FULL:
          message_string +=
            sprintf("%s@"MUDNAME" moechte gerade nicht gestoert werden.\n"+
                    "Die Mitteilung ging verloren, denn der Kobold kann sich "+
                    "nichts mehr merken.\n",
                    capitalize(getuid(ob)));
          break;
        }
      }
    }
    else message_string="\nRoot@"MUDNAME": Spieler "+
                        capitalize(data[RECIPIENT])+
                        " finde ich in "MUDNAME" nicht!\n";
    INETD->_send_udp(data[NAME],
                    ([ REQUEST: REPLY,
                       RECIPIENT: data[SENDER],
                       ID: data[ID],
                       DATA: "\n"+message_string ]) );
  }
  else
    INETD->_send_udp(data[NAME],
                    ([ REQUEST: REPLY,
                       RECIPIENT: data[SENDER],
                       ID: data[ID],
                       DATA: sprintf("Root@"MUDNAME": Spieler %s finde "+
                                     "ich in "MUDNAME" nicht!\n",
                                     capitalize(data[RECIPIENT]))
                    ]) );
}

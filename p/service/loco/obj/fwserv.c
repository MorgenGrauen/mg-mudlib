/***************************************************************************

 Forward-Server
 Teil des MG-Mailsystems
 (C) 1996 Loco@Morgengrauen

 Es gilt dasselbe Copyright wie fuer mailer.c, bitte dort nachlesen.

***************************************************************************/
/* 24.03.2008, Arathorn
   - Auch Erzmagier duerfen jetzt Eintraege aendern (Aenderung an Security())
 */

#include <properties.h>
#include "/secure/config.h"
#include "/mail/post.h"
#include <wizlevels.h>

#define LOG "/p/service/loco/save/FWLOG"

static int Security(string name);

static mixed forwarddata;
mixed properties;


create() {
  seteuid(getuid());
  forwarddata=([]);
}


LoadFile(s) {
  if (!forwarddata[s])
    forwarddata[s]=FWSAVESERV->LoadData(s);
  if (!forwarddata[s])
    forwarddata[s]=([]);
}


string QueryForward(string adr) {
/* Setze eine Adresse um. Rueckgabe: Forwardadresse oder Name, wenn keine
   gesetzt ist. Externe Adressen werden nicht umgesetzt. */
  string s, res;
  object player;
  int i;

  if (member(adr, '@')>=0) return adr; /* Externe Adresse */

  for (i=sizeof(adr)-1;i>=1;i--) {
    if (adr[i]>'z'||adr[i]<'a')
      return "*UNGUELTIGER NAME: "+adr+" *";
  }
  if (adr[0]=='\\')
    return adr;
  
  if (adr[0]>'z'||adr[0]<'a') 
    return "*UNGUELTIGER NAME: "+adr+" *";


  s=adr[0..0];
  LoadFile(s);
  
  if (!member(forwarddata[s],adr)) return adr;

  /* Achtung, ab hier wechselt Variable s z.T. ihre Bedeutung */

  if (player=find_player(adr)) 
    s=player->QueryProp(P_MAILADDR);
  else {
    /* Gibts den Spieler? */
    if (!master()->find_userinfo(adr))
        return 0;
    /* hol die Daten */
    restore_object(SAVEPATH+s+"/"+adr);
    s=properties[P_MAILADDR];
    properties=0;
  }    

  s=lower_case(s);
  if (!s || !sizeof(s) ||
      s=="none")
    return adr;  /* keine sinnvolle Zieladresse: nicht umsetzen */

  // jetzt noch pruefen, ob s eine fuer den MAILDEMON gueltige adresse ist.
  if (MAILDEMON->query_recipient_ok(s))
    return s;

  return adr; // offenbar keine gueltige Adresse.
}


string AddForward(string name) {
  string s;

  name=lower_case(name);
  s=name[0..0];

  if (!Security(name)) 
    return "Nachsendeauftrag abgelehnt, bitte EM oder Loco benachrichtigen.";

  LoadFile(s);
  
  if (member(forwarddata[s],name))
    return "Nachsendeauftrag war bereits gestellt worden.";

  forwarddata[s]+=([name]);
  FWSAVESERV->SaveData(s,forwarddata[s]);

  write_file(LOG,ctime()+": ADD "+name+"\n");
  
  return "Nachsendeauftrag angenommen. Die Adresse lautet:\n  "+
    QueryForward(name)+
      "\nFalls diese falsch ist, bitte augenblicklich die email aendern!\n";

}


string DeleteForward(string name) {
  string s;

  name=lower_case(name);
  s=name[0..0];

  if (!Security(name)) 
    return "Loeschen des Nachsendeauftrags ist nicht erlaubt,\n  bitte EM oder Loco benachrichtigen.";

  LoadFile(s);
  
  if (!member(forwarddata[s],name))
    return "Es war kein Nachsendeauftrag gestellt worden.";

  forwarddata[s]-=([name]);
  FWSAVESERV->SaveData(s,forwarddata[s]);
  
  write_file(LOG,ctime()+": DEL "+name+"\n");

  return "Nachsendeauftrag geloescht, Post wird wieder direkt ausgeliefert.\n";
}



static int Security(string name) {
  if (geteuid(this_interactive())==name) return 1;
  if (geteuid()==geteuid(previous_object())) return 2;
  if (member( ({ROOTID,MAILID}), geteuid(previous_object())) >=0 ) return 3;
  if ( this_interactive() && IS_ARCH(this_interactive()) ) return 4;
  return 0;
}


QueryForwards() { if (Security(" N I X ")) return forwarddata; }

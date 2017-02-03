/* Neue std/post
   (C) 1993 by Loco
*/

inherit "/std/room";

#include "/mail/post.h"
#include <properties.h>
#include <rooms.h>

void create() {
  int i;
  (::create());
  SetProp(P_INT_SHORT,"Postamt");
  SetProp(P_INT_LONG,"\
Dies ist ein Testpostamt.\n\
Von hier aus kannst Du Briefe an Deine Mitspieler schicken und Briefe von\n\
ihnen lesen. Wenn Du das willst, tippe 'post' oder 'mail',\n\
bzw. 'mail <spieler>'.\n");
  SetProp( P_LIGHT, 1 );
  AddCmd("post","do_mail");
  AddCmd("mail","do_mail");
  SetProp(P_INDOORS,1);
  //  if (!argh) {
  //   for (i=0;i<NRCABINS;++i) AddItem(MAILCABIN,REFRESH_REMOVE);
  //  AddItem(COUNTER,REFRESH_REMOVE);
  //  SetProp(P_POST,PP_ALLOWED);
    //  }
}

int do_mail(string str) {
  object mailer;
  if (this_interactive()!=this_player()) return 0;
  mailer=clone_object(MAILER);
  mailer->SetOfficeName(short());
  mailer->do_mail(str);
  return 1;
}



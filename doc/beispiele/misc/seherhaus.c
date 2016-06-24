/*
Kurze Uebersicht der Termini:

P_HAUS_ERLAUBT     Propertie, die das Bauen eines Hauses im Raum er-
                   laubt oder verbietet. Default auf 0
                   Syntax: xcall $h->SetProp(P_HAUS_ERLAUBT,x)
                   
Save()             Muss nach Aenderungen am Haus aufgerufen werden.
                   Syntax: xcall /d/seher/haeuser/tillyhaus->Save()

Okay, also ein Seher moechte, dass sein Haus 1) unsichtbar ist, 2) mit
einem anderen Kommando betreten werden soll und 3) nicht auf die id 
<haus> hoert sondern auf eine andere:

Zu 1): Ganz einfach, man tippe 

       'xcall /d/seher/haeuser/tillyhaus->SetProp(P_SHORT,0)' und
       'xcall /d/seher/haeuser/tillyhaus->Save()'

Zu 2) und 3): Das ist nicht so einfach, sondern muss ueber den Raum, in 
              dem das Haus steht, gemacht werden. Deswegen folgend nun 
              ein Beispiel zu Funktionen in so einem Raum.


Folgende Dinge stehen natuerlich schon im Raum, sonst liesse er sich
nicht laden :-)              
*/

#pragma strong_types, save_types, rtt_checks

inherit "/std/room";

#include <properties.h>
#include <language.h>
#include <moving.h>

/*
   Fuer properties und sonstige Definitionen zum Seherhaus.
*/
#include "/d/seher/haeuser/haus.h"

/*
   Aus Gruenden der Uebersichtlichkeit ziehen viele Magier vor, den Hauskrams
   in eine separate Funktion zusammenzufassen.
*/
private void haus_create();

/*
Es folgt nun das normale create() eines Raumes
*/
void create(){
  ::create();
  /* ... div. Konfiguration des Raum ... */

  /*
  Das extra-create() muss natuerlich aufgerufen werden
  */  
  haus_create();
}      

/*
Hier kommen wir nun zu der Fkt, die alles am Seherhaus verwaltet
*/
private void haus_create()
{
  object ob = find_object(HAUSNAME("tilly")); // findet Haus von Tilly

  /*
  Wenn das Haus im Raum steht. Hat den Vorteil, dass bei einem Verlegen
  des Hauses, Cmd's usw nicht mehr verfuegbar sind -> Keine evtl Bugs :)
  */
  if(objectp(ob))
  {
    /*
    Der Spieler wollte es ja nicht anders
    */
    ob->RemoveCmd(({"betret","betritt","betrete"}));
    ob->RemoveId("haus");
    ob->RemoveId("haus von tilly");
    ob->AddId("steinhaufen");
    
    AddDetail(({"haufen","steinhaufen"}),"Da ist doch ein kleiner Spalt? "
     +"Vielleicht kannst Du auf den Steinhaufen klettern?\n");

    AddCmd(({"kletter","klettere"}),"kletter_cmd");
  }
}

public int kletter_cmd(string str)
{
  object ob = find_object(HAUSNAME("tilly"));

  _notify_fail("Wo moechtest Du hinklettern?\n");

  if(!str) return 0;

  if(str=="auf haufen" || str=="auf steinhaufen")
  {
    if(objectp(ob))
    {
      /*
      Das ist die Meldung, die ein Spieler bekommt, wenn das Seherhaus
      abgeschlossen ist und er versucht, es zu 'betreten'.
      */
      notify_fail("Du kletterst auf den Haufen, doch der Spalt ist zu "
       "schmal fuer Dich.\n");
       
      if(!(ob->QueryProp(H_DOORSTAT) & D_CLOSED))
      {
        tell_object(this_player(),
            "Du kletterst auf den Steinhaufen, rutschst den Spalt runter "
             "und findest Dich urploetzlich in einer schummrigen Hoehle.\n");
        tell_room(this_object(),this_player()->name()+" klettert auf einen "
          "Steinhaufen und ist ploetzlich verschwunden.",({this_player()}));
        tell_room(RAUMNAME("tilly",0),this_player()->name()+
          " faellt fluchend herein.\n");
        this_player()->move(RAUMNAME("tilly",0),M_GO|M_SILENT,0);
        return 1;
      }
    }
  }
  return 0;
}
  
/*
Natuerlich gibt es noch viel mehr Moeglichkeiten rund um Seherhaeuser, 
doch sollte dies erstmal reichen. Zu beachten ist bei solcher Vorgehens-
weise, dass ein 'schliesse|oeffne haus' evtl zu Fehlermeldungen fuehrt.
Dem Spieler sei nahegelegt, es doch mit 'oeffne haustuer' zu versuchen.

HAUSNAME und RAUMNAME (u.a.) sind Defines aus dem haus.h. Man sollte sich
dieses File anschauen und die dortigen Defines uebernehmen. Dann bugt auch 
nichts, falls mal etwas an den Haeusern umgestellt wird.

Tilly, 20. Mai 2001, 00:10:24h
*/

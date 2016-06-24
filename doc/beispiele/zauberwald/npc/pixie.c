// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <moving.h>
#include <combat.h>

inherit NPC("stdnpc");

void create()
{
   ::create();
   SetProp(P_SHORT, "Ein kleiner Pixie");
   SetProp(P_SIZE, 55+random(11));
   SetProp(P_LONG, BS(
     "Der Pixie ist etwa "+QueryProp(P_SIZE)+"cm gross und sieht vermutlich einem "
    +"kleinen pumeligen Menschenkind am aehnlichsten. Wie alle Pixies scheint er "
    +"sehr verspielt zu sein und nichts als Unfug im Kopf zu haben, Du solltest "
    +"Dich also vor ihm ihn acht nehmen. Auch wenn er aussieht wie ein Kind, sich "
    +"benimmt wie ein Kind, so kann er Dir mit seiner Magie sicher uebel zu spieln."));
   SetProp(P_INFO, /* kleine Warnung fuer die Kaempfer :) */
     "Man sollte sich immer vor der Magie der Pixies in acht nehmen, sie ist nicht\n"
    +"unbedingt gefaehrlich, aber es koennen die unerwartesten Dinge geschehn. So\n"
    +"ist durchaus von Leuten bekannt die einen Kampf als kleine Ratte beendeten,\n"
    +"oder deren Schwert im Kampf ploetzlich zu Scheisse zerfloss...\n");
   SetProp(P_NAME_ADJ, "klein");
   SetProp(P_NAME, "Pixie");
   SetProp(P_GENDER, MALE);
   SetProp(P_RACE, "pixie");
   SetProp(P_ATTRIBUTES, (["int":30,"con":20,"str":15,"dex":30]) );
   SetProp(P_LEVEL, 20);
   SetProp(P_MAX_HP, 600);
   SetProp(P_HP, 600);
   SetProp(P_MAX_SP, 800);
   SetProp(P_SP, 800);
   SetProp(P_ALIGN, 250);
   SetProp(P_HANDS, ({" mit seinen kleinen Haenden", 150, DT_BLUDGEON}) );
   SetProp(P_XP, 600*150*5);
   AddAdjective("klein");
   AddId("pixie");
}

// teleporter Ziele im Gebiet. Der Pixie will ja einfach nur ein bisschen
// Aergern und nicht helfen (ein Pixiekampf ist kein Ausgang :)
#define DEST ({"lichtungno", "lichtungso", "lichtungn", "lichtungs", \
               "lichtungnw", "lichtungsw", "lichtungo", "lichtungw", \
               "weg2", "stein"})

void Attack(object enemy)
{
   object ob, weapon;
   
   ob=SelectEnemy();
   if (ob) switch(random(7*8)) { // jede 8te Runde ein Zauberspruch...
      case 0: // in Frosch verwandeln...
        if (!ob->QueryProp(P_FROG)) {
          tell_object(ob, "Der Pixie dreht Dir eine lange Nase und verwandelt Dich in einen Frosch.\n");
          say(BS("Der Pixie dreht "+ob->name(WEM)+" eine lange Nase und verwandelt "
                +ob->QueryPronoun(WEN)+" in einen Frosch,"), ob);
          ob->SetProp(P_FROG, 1);
          return;
        }
        break;
      case 8..10: // einfach nur nerviger Teleport innerhalb des Gebiets :)
        tell_object(ob,
          "Der Pixie schliesst kurz seine Augen und eh Du Dich versiehst, verschwimmt\n"
         +"alles um Dich herum...\n");
        say(BS("Der Pixie schliesst kurz seine Augen und ploetzlich loest sich "
              +ob->name(WER)+" in Luft auf."), ob);
        ob->move(ROOM(DEST[random(sizeof(DEST))]), M_TPORT|M_NOCHECK|M_SILENT);
        return;
      case 16: // Geschlecht aendern... :)
        tell_object(ob,
          "Der Pixie grinst breit bis ueber beide Ohren und schaut Dich an, Du weisst\n"
         +"gar nicht wie Dir geschieht, aber irgendetwas aendert sich an Deinem Koerper.\n");
        say(BS("Der Pixie grinst "+ob->name(WEN)+" breit an. Ploetzlich "
         +(ob->QueryProp(P_GENDER)==FEMALE
         ? "verschwinden ihre Brueste und es waechst ihr ploetzlich ein Bart."
         : "verschwindet sein Bart und ihm wachsen zwei neue Brueste.")), ob);
        if (ob->QueryProp(P_GENDER)==FEMALE)
          ob->SetProp(P_GENDER, MALE);
        else ob->SetProp(P_GENDER, FEMALE);
        return;
      case 24:
        if (objectp(weapon=ob->QueryProp(P_WEAPON))) {
           weapon->DoUnwield(1);
           // verfluchte Waffen nicht betreffen :))
           if (!objectp(weapon->QueryProp(P_WIELDED))) {
              string str1, str2;
              str1=str2=(weapon)->name(WER);
              if (str1[0..2]=="ein") {
                str1="D"+str1;
                str2="s"+str2;
              }
              tell_object(ob, BS(
               "Der Pixie starrt Dir in die Augen und schnippst einmal kurz mit seinen "
              +"Fingern. Ploetzlich zerrint "+str1+" in Deinen Haenden zu einem Haufen "
              +"Scheisse."));
              say(BS("Der Pixie starrt "+weapon->name(WEM)+" in die Augen und "
                    +"schnippst einmal kurz mit den Fingern. Ploetzlich "
                    +"zerrint "+weapon->name(WEM)+" "+str2
                    +" in einen Haufen Scheisse."), ob);
              weapon->remove();
              if (weapon) destruct(weapon);
              return;
           }
        }
        break;
      case 32:
        if (!ob->QueryProp(P_BLIND)) {
          tell_object(ob, BS(
             "Der Pixie haelt sich die Hand vor die Augen und grinst breit. Ploetzlich "
            +"merkst Du, wie alles ganz dunkel um Dich rum wird..."));
          say(BS("Der Pixie haelt sich die Hand vor die Augen und grinst breit. "
                +ob->name(WER)
                +" schaut daraufhin ziemlich irritiert in die Gegend."), ob);
          ob->SetProp(P_BLIND, 1);
          return;
        }
        break;
      case 40:
        if (ob->QueryProp(P_POISON)<3 && ob->QueryProp(P_MAX_POISON)>=3) {
          tell_object(ob, BS(
             "Der Pixie streicht sich mit der Hand zufrieden ueber seinen Bauch und schaut "
            +"Dich dabei an. Ploetzlich wird Dir richtig uebel..."));
          say(BS("Der Pixie streicht sich mit seiner Hand zufrieden ueber seinen Bauch und\n"
                +"schaut "+ob->name(WEM)+" dabei in die Augen. Ploetzlich wird "+ob->name(WEM)
                +" ganz uebel."), ob);
          ob->SetProp(P_POISON, 3);
          return;
        }
        break;
      case 48:
        if (!ob->QueryProp(P_DEAF)) {
          tell_object(ob, "Der Pixie haelt sich die Ohren zu und grinst breit.\n");
          say(BS("Der Pixie haelt sich die Ohren zu und grinst dabei "
                +ob->name(WEN)+" an."), ob);
          ob->SetProp(P_DEAF, 1);
          return;
        }
        break;
      default:
   }
   if (enemy && present(enemy, environment())) ::Attack(enemy);
}

void NotifyPlayerDeath(object who, object killer, object lost_exp)
{
  if (!who || killer!=ME) return; // uninteressant
  log_file("padreic/kill", ctime(time())+" "+capitalize(getuid(who))+" getoetet von /zauberwald/pixie\n");
}

/* Hinweis zu diesem Beispielobjekt: Lebensmittel lassen sich besser von
 * /std/food ableiten, das bringt viele Funktionalitaeten mit, die man sonst
 * muehsam selber basteln muss.
 */

inherit "/std/thing";

#include <moving.h>
#include <properties.h>
#include <wizlevels.h>

#define SAETTIGUNG 2
#define HEILUNG 20
#define EAT_DELAY 1200 // 20 Minuten

#define BS78(x) break_string(x,78)

protected void create()
{
  ::create();

  // Gemeinsamer Teil des create
  AddId("obst");
  SetProp( P_VALUE, 60+random(10) );

  // AddCmd( ({"iss","esse"}), "act_essen");  
  // new style AddCmd
  AddCmd( "iss|esse&@ID","act_essen","Was willst Du denn essen?");

  // Nach ca. 10 Minuten resetet das Obst, im Reset wird es destructet, 
  // so spare ich call_outs
  set_next_reset(600);

  // Name, ID, Gender, Desc und Gewicht sind Abhaengig von der Obstart
  switch (random(11)){
  default:
    SetProp( P_GENDER, MALE );
    SetProp( P_NAME, "Apfel" );
    AddId( "apfel" );
    SetProp( P_SHORT, "Ein Apfel" );
    SetProp( P_LONG, BS78(
      "Ein saftiger Apfel. Dir laeuft das Wasser im Munde zusammen."
    ));
    SetProp( P_WEIGHT, 80 );
    break;
  case 1:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Birne" );
    AddId( "birne" );
    SetProp( P_SHORT, "Eine Birne" );
    SetProp( P_LONG, BS78(
      "Eine koestliche, reife Birne. Die ist sicher lecker."
    ));
    SetProp( P_WEIGHT, 90 );
    break;
  case 2:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Banane" );
    AddId( "banane" );
    SetProp( P_SHORT, "Eine Banane" );
    SetProp( P_LONG, BS78(
      "Die Banane sieht wirklich lecker aus, am besten gleich essen."
    ));
    SetProp( P_WEIGHT, 110 );
    break;  
  case 3:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Kiwi" );
    AddId( "kiwi" );
    SetProp( P_SHORT, "Eine Kiwi" );
    SetProp( P_LONG, BS78(
      "Klein und gruen, aber mit leckeren Innenleben. Yam yam."
    ));
    SetProp( P_WEIGHT, 50 );
    break;  
  case 4:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Erdbeere" );
    AddId( "erdbeere" );
    SetProp( P_SHORT, "Eine Erdbeere" );
    SetProp( P_LONG, BS78(
      "Eine ueberdurchschnittlich grosse Erdbeere. Sie "
      "ist sicher suess und saftig."
    ));
    SetProp( P_WEIGHT, 20 );
    break;  
  case 5:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Kirsche" );
    AddId( "kirsche" );
    SetProp( P_SHORT, "Eine Kirsche" );
    SetProp( P_LONG, BS78(
      "Zwar nur klein ist diese Kirsche, aber so dunkel wie sie "
      "aussieht, ist sie sicher total suess und lecker."
    ));
    SetProp( P_WEIGHT, 15 );
    break;  
  case 6:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Orange" );
    AddId( "orange" );
    SetProp( P_SHORT, "Eine Orange" );
    SetProp( P_LONG, BS78(
      "Eine grosse, gelbe Orange. Eine von diesen suessen, die sich so "
      "leicht schaelen lassen und keine Zicken machen, so wie Saftorangen."
    ));
    SetProp( P_WEIGHT, 90 );
    break;  
  case 7:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Mandarine" );
    AddId( "mandarine" );
    SetProp( P_SHORT, "Eine Mandarine" );
    SetProp( P_LONG, BS78(
      "Suess und saftig und gesund. So muss das Essen im Paradies "
      "gewesen sein."
    ));
    SetProp( P_WEIGHT, 60 );
    break;  
  case 8:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Zitrone" );
    AddId( "zitrone" );
    SetProp( P_SHORT, "Eine Zitrone" );
    SetProp( P_LONG, BS78(
      "Sauer, sauer und nochmals sauer. Aber ultralecker. Na, sabberst Du "
      "schon auf die Tastatur?"
    ));
    SetProp( P_WEIGHT, 60 );
    break;  
  case 9:
    SetProp( P_GENDER, MALE );
    SetProp( P_NAME, "Granatapfel" );
    AddId( "granatapfel" );
    SetProp( P_SHORT, "Ein Granatapfel" );
    SetProp( P_LONG, BS78(
      "Er ist zwar etwas schwieriger, den zu essen, aber der Aufwand "
      "lohnt sich."
    ));
    SetProp( P_WEIGHT, 90 );
    break;  
  case 10:
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Pflaume" );
    AddId( "pflaume" );
    SetProp( P_SHORT, "Eine Pflaume" );
    SetProp( P_LONG, BS78(
      "Eine grosse, blaue Pflaume. Lass sie Dir doch schmecken."
    ));
    SetProp( P_WEIGHT, 20 );
    break;
  }
}

void reset()
{
  object env = environment(this_object());

  if (env) {
    if (query_once_interactive(env)){
      // Mein Env is ein User
      tell_object(env, BS78(
        capitalize(this_object()->name(WER))+" verschwindet mit einem "+
        "\"PLOPP\" aus deinem Inventar. Da scheint Magie im Spiel zu sein."
      ));
    } else if (sizeof(all_inventory(env)&users())){
      // In meinem Environment sind User (Es ist ein Raum)
      tell_room(env, BS78(
        capitalize(this_object()->name(WER))+" verschwindet mit einem "+
        "\"PLOPP\". Da scheint Magie im Spiel zu sein."
      ));
    }
  }

  // Objekt zerstoeren
  remove(1);
}

static int act_essen(string args)
{
    // Nur ein Demo-Objekt
    if (!IS_LEARNING(this_player()))
    {
      write("Schwups, nun ist @@name@@ verschwunden. War wohl eine Illusion.");
      remove(1);
    }

    // Der Syntaxtest ist unnoetig, der ist schon im new style
    // AddCmd() eingebaut.

    // Wenn das Objekt ein Environment hat (hoff ich doch)
    // und das Environment nicht this_player() ist, 
    // hat der Spieler das Obst nich im Inv - das geht doch nicht ...
    if (environment(this_object()) && (environment(this_object()) != this_player()) )
    {
      write(BS78(
        "Du solltest "+ this_object()->name(WER,1) + " erstmal nehmen."
      ));
      return 1;
    }

    // DEBUG ("Satt?");
    if (this_player()->eat_food(SAETTIGUNG))
    {
      // DEBUG("Noe!");
      if (this_player()->check_and_update_timed_key(EAT_DELAY,"vanion_zach_obst")==-1)
      {
        write(BS78(
          "Du isst genuesslich "+this_object()->name(WEN,0)+
          ". Du fuehlst Dich gestaerkt."
        ));
        say(BS78(
          this_player()->name()+" isst mit vollem Genuss "+this_object()->name(WEN,0)+". "
          "Dir laeuft das Wasser im Munde zusammen."
        ));
        // Heilen und aufessen :)
        this_player()->heal_self(HEILUNG);
      }
      else { // if (check_and_update_timed_key())
        // Wenn es noch zu frueh ist, wieder welches zu essen
        // Keine Heilung, aber Saettigung
        write(BS78(
          "Du solltest "+this_object()->name(WEN,1)+" lieber jemandem geben, "
          "den Du magst, statt alles selbst zu futtern, findest Du nicht? "
          "Deine Gier hat Dir jetzt nur einen vollen Bauch und ein schlechtes "
          "Gewissen bescheert."
        ));
      } // end if (check_and_update_timed_key())
      if (!remove()) destruct(this_object());
    } // end of eat_food
    return 1;
}

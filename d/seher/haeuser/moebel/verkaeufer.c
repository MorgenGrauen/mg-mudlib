//--------------------------------------------------------------------------------
// Name des Objects:    Verkaeufer (August vom SCHRAENKER)
// Letzte Aenderung:    24.08.2006
// Magier:              Seleven
//--------------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include <properties.h>
#include <language.h>
#include <moving.h>
#include <wizlevels.h>
#include <bank.h>

#include "schrankladen.h"
inherit "/std/npc";

// Wenn im Laden bzw. im Environment von August die Prop "test" auf 1 gesetzt
// wird, kostet der Kauf nichts. Jeder der ein Seherhaus besitzt, kann kaufen.

// Fuer Prop im Spieler = individuellere Begruessung
#define LASTVISIT "schrankladen_lastvisit"
// 10% Rabatt fuer Leute, die intensiv testeten, Ideen fuer den Laden beisteuerten,
// oder halfen, groessere Bugs zu beheben!
#define RABATT ({"geordi","foobar","blood","lurchi", "eldaron"})

// kaufe... andere im raum sollten vor der antwort sehen wer august nach was fragt

protected void create()
{
  if(!clonep(TO)) return;
  ::create();
  SetProp(P_LOG_FILE,"seleven/schraenker"); // Seleven 06.04.2006
  SP(P_SHORT, "August");
  SP(P_LONG, BS(
     "Das ist August, ein kleiner Zwerg und seines Zeichens Seher-Berater. "
    +"Er versorgt Seher mit Eigenheim mit dem Mobiliar, das man braucht, "
    +"will man sein Haus uebersichtlich halten und etwas Ordnung in die "
    +"Sammlungen bringen, die sich in so einem Seherhaus zwangslaeufig "
    +"anhaeufen. Im Gegensatz zu den meisten Zwergen sieht August nicht nur "
    +"aus wie frisch geduscht, er steckt auch in einem feinen Anzug. So "
    +"einen gepflegten Bart wie den von August hast Du vorher noch nie "
    +"gesehen. Und dann duftet er auch noch nach irgendeinem exotischen "
    +"Waesserchen. Waere nicht der gierige Blick in seinen Augen, koenntest "
    +"Du wirklich zweifeln, dass es sich hier wirklich um einen Zwerg "
    +"handelt.",78,0,1));
  SP(P_NAME, "August");
  SP(P_GENDER, MALE);
  SP(P_ARTICLE, 0);
  SP(P_RACE, "Zwerg");
  // Arathorn, 2015-Okt-13, Zeile vor P_XP gezogen, damit auch sicher kein
  // EK angezeigt wird.
  create_default_npc(20);
  SP(P_XP, 0);
  AddId(({"august", "inhaber", "ladeninhaber", "besitzer",
          "firmenbesitzer", "verkaeufer", "zwerg", "seher-berater"}));

  SP(P_SIZE, 130);
  SP(P_NO_ATTACK, BS("Die beruhigende Melodie scheint nicht nur zum "
    +"Vergnuegen der Kunden zu erklingen. Du bringst es einfach "
    +"nicht fertig, Deine Hand gegen August zu erheben!"));

  AD(({"anzug"}), BS(Name(WER)+" steckt in einem feinen rotbraunen Anzug, "
    +"in dem vermutlich auch ein Bettler koeniglich auf Dich wirken wuerde. "
    +"Offensichtlich ist er nicht der Aermsten einer."));
  AD(({"bettler"}), BS("Wie ein Bettler wirkt August nicht auf Dich."));
  AD(({"waesserchen"}), BS("Es riecht koestlich. Schnupper doch mal!"));
  AD(({"bart"}), BS("Der vermutlich weisse Bart wurde von Kuenstlerhand "
    +"roetlich eingefaerbt und passt farblich vorzueglich zum Anzug."));
  AD(({"kuenstlerhand"}), BS("Bestimmt hat August das nicht selbst gemacht."));
  AD(({"mobiliar"}), BS("Das verkauft er hier."));
  AD(({"augen"}), BS("Obwohl "+Name(WER)+" perfekte Manieren an den Tag "
    +"legt, seine Augen verraten ihn. Die Dollarzeichen in seinem Blick "
    +"sind nicht zu uebersehen."));
  AD(({"dollarzeichen"}), BS("Eine Redensart. Keine Ahnung was das sein soll."));
  AD(({"redensart"}), BS("Irgend ein Spruch, den Du irgendwo aufgeschnappt hast."));
  AD(({"spruch"}), BS("Im Spruecheklopfen bist Du gut, oder?"));
  AD(({"spruecheklopfen"}), BS("OK, ich geb auf. :)"));
  AD(({"manieren"}), "Und das bei einem Zwerg!!!\n");

  AddInfo(({"laden", "schraenker"}), "Der Schraenker ist mein Laden, "
    +"und ich denke es ist ein guter Laden. Das Geschaeft laeuft, seit "
    +"jeder Ordnung in seinem Seherhaus haben moechte. Und Seherhaeuser "
    +"gibt es ja mehr als genug! :)", "sagt: ");
  AddInfo(({"haeuser", "seherhaeuser"}), "Bei mir koennen Seher "
    +"Mobiliar fuer ihr Seherhaus kaufen. Unsere Qualitaet ist weithin "
    +"beruehmt. Ausserdem sind wir die einzigen auf dem Markt.", "grinst: ");
  AddInfo(({"melodie"}), "lauscht vertraeumt.\n");
  AddInfo(({"geld"}), "Sie haben doch welches?\n", "fragt besorgt: ");
  // Seleven 01.04.2006: frage nach (schluessel)brett eingefuegt
  AddInfo(({"mobiliar", "schrank", "schraenke", "truhe", "truhen", "kommode",
     "kuehlschrank", "waffenschrank", "waffentruhe", "spind", "kleiderschrank","brett","schluesselbrett",
     "muellcontainer", "zauberkiste", "tresor", "holztruhe","vitrine","pult","lesepult"}),
     "Lesen Sie doch bitte die Preisliste an der Wand.", "sagt: ");
  AddInfo(({"liefern", "lieferung"}), "Wir liefern sofort.", "sagt: ");
  AddInfo(({"bart"}), "Es ist ein schoener Bart, nicht? :)", "fragt: ");
  AddInfo(({"augen"}), "Was haben Sie an denen auszusetzen?", "fragt: ");
  AddInfo(({"dusche"}), "Jeden Morgen. Das ist leider Pflicht. Und 364 mal "
    +"zu oft :(", "sagt: ");
  AddInfo(({"pflicht", "pflichten"}), "Als Ladeninhaber hat man deren "
    +"viele!", "sagt: ");
  AddInfo(({"anzug"}), "Ein echter Armgani!", "verkuendet stolz: ");
  AddInfo(({"armgani"}), "Du kennst Armgani nicht?", "fragt naseruempfend: ");
  AddInfo(({"geordi"}), "Von ihm stammt die urspruengliche Geschaeftsidee "
    +"fuer den SCHRAENKER. Dafuer bekommt er bei uns auch Rabatt!", "erzaehlt: ");
  AddInfo(({"august"}), "Wollen Sie sich ueber mich lustig machen?", "fragt drohend: ");
  AddInfo(({"rabatt"}), "Nur Stammkunden, die uns schon brauchbare Ideen "
    +"geliefert haben, oder die uns beim Aufbau des Geschaefts geholfen haben, "
    +"gewaehren wir Rabatt.", "sagt: ");
  AddInfo(({"ideen"}), "Es muessen brauchbare Ideen sein.", "sagt: ");
  AddInfo(({"stammkunde", "stammkunden"}), "Einige kommen oefter vorbei.", "sagt: ");
  AddInfo(({"aufbau"}), "Es kommt mir noch so vor, als waers gestern gewesen.", "sagt: ");
  AddInfo(({"kunde", "kunden"}), "Ich hoffe mal, dass SIE der Kunde sind!?", "sagt: ");

  AddCmd(({"kauf", "kaufe", "bestell", "bestelle"}), "container_kaufen");
  AddCmd(({"schnupper", "schnuppere"}), "schnuppern");
}

int schnuppern()
{
  write(BS("Ein betoerender Duft umgibt August, und verdeckt jeglichen Geruch, "
    +"den Du von sonstigen Zwergen gewohnt bist."));
  return 1;
}

static string anrede()
{
  switch( TP->QueryProp(P_GENDER) )
  {
    case MALE   : return "mein Herr"; break;
    case FEMALE : return "meine Dame"; break;
  }
  return "mein H... D... Wasauchimmer";
}

void begruessung(object pl, string txt)
{
  if( objectp(pl) && present(pl) )
    tell_room(ETO, "\n"+txt);
}

public varargs void init(object origin)
{
  ::init();
  if (!interactive(TP) || TP->QueryProp(P_INVIS))
      return 0; // Keine Meldungen wenn invis oder kein Spieler!
  if(TP->QueryProp(LASTVISIT) < time()-3600)  // laenger als 1 Std. nicht da gewesen
  {
    if( member(RABATT, geteuid(TP)) != -1 )
      call_out("begruessung", 0, TP, BS("Herzlichst Willkommen in meinem "
        +"bescheidenen Laden, "+TP->name(WER)+". Es ist jedesmal eine Freude, "
        +"Sie zu sehen!",78, Name(WER)+" sagt: "));
    else
      call_out("begruessung", 0, TP, BS("Willkommen in meinem bescheidenen Laden, "
        +anrede()+".",78, Name(WER)+" sagt: "));
  }
  else  // schonmal innerhalb 1 Std. da gewesen?
  {
    if( member(RABATT, geteuid(TP)) != -1 )
      call_out("begruessung", 0, TP, BS("Hallo, "+TP->name(WER)+". "
        +"Sie konnten es ja nicht lange ohne mich aushalten, aber Stammkunden "
        +"sind mir die liebsten Kunden. Womit kann ich Ihnen diesmal "
        +"dienen?",78, Name(WER)+" sagt: "));
    else
      call_out("begruessung", 0, TP, BS("Guten Tag, "+anrede()+". "
        +"Ich freue mich, dass Sie meinen Laden so bald wieder besuchen kommen! "
        +"Womit kann ich Ihnen diesmal dienen?",78, Name(WER)+" sagt: "));
  }

  TP->SetProp(LASTVISIT, time() );
}

int container_kaufen(string str)
{
  string cnt_file;
  int preis;
  object zielraum,cnt,ob;
  tell_room(ETO, TP->Name(WER)+" wendet sich wegen eines Kaufs an "
    +Name(WEN)+".\n", ({TP}) );
  if( (query_wiz_level(TP) >= SEER_LVL && query_wiz_level(TP) < LEARNER_LVL) ||
      ETO->QueryProp("test") ) // TP ist Seher, oder Testmodus.
  {
    catch(call_other("/d/seher/haeuser/"+geteuid(TP)+"raum0", "???", 0));
    if( !zielraum=find_object("/d/seher/haeuser/"+geteuid(TP)+"raum0") )
    {
      tell_room(ETO, BS("Tut mir leid, "+anrede()+". Voraussetzung fuer eine "
        +"Lieferung unserer Artikel ist ein begehbares "
        +"Seherhaus.",78, Name(WER)+" sagt: "));
      return 1;
    }
    notify_fail( BS("Tut mir leid, "+anrede()+". So einen Artikel fuehren wir nicht. "
      +"Bitte lesen Sie die Preisliste an der Wand. Dort koennen Sie sich genau "
      +"darueber informieren, ueber welche Artikel wir zur Zeit "
      +"verfuegen.",78, Name(WER)+" sagt: "));
    if(!str) return 0;
    switch(str)
    {
      case "1" :
      case "kommode" :       cnt_file="kommode";
                             preis=3799;
                             break;
      case "2" :
      case "kleiderschrank": cnt_file="kleiderschrank";
                             preis=5899;
                             break;
      case "3" :
      case "spind" :         cnt_file="spind";
                             preis=5299;
                             break;
      case "4" :
      case "kuehlschrank" :  cnt_file="kuehlschrank";
                             preis=3499;
                             break;
      case "5" :
      case "waffenschrank" : cnt_file="waffenschrank";
                             preis=5899;
                             break;
      case "6" :
      case "waffentruhe" :   cnt_file="waffentruhe";
                             preis=5399;
                             break;
      case "7" :
      case "tresor" :        cnt_file="tresor";
                             preis=4499;
                             break;
      case "8" :
      case "zauberkiste" :   cnt_file="zauberkiste";
                             preis=7999;
                             break;
      case "9" :
      case "muellcontainer": cnt_file="muellcontainer";
                             preis=999;
                             break;
      // Seleven 01.04.2006
      case "10" :
      case "schluesselbrett": cnt_file="schluesselbrett";
                             preis=499;
                             break;
      case "11" :
      case "duschkabine": cnt_file="wasch";
                             preis=3499;
                             break;
      case "12":
      case "holztruhe":
                             cnt_file="autoloadertruhe";
                             preis=74999;
                             break;
      case "13":
      case "vitrine":
                             cnt_file="vitrine";
                             preis=49999;
                             break;
      case "14":
      case "lesepult":
                            cnt_file="lesepult";
                            preis=60000;
                             break;
      case "15":
      case "aquarium": 
                             cnt_file="aquarium";
                             preis=3000;
                             break;
      default:               return 0; break;
    }
    if( member(RABATT, geteuid(TP)) != -1 )  // 10% Rabatt fuer bestimmte Leute,
      preis=preis/100*90;                    // die Ideen fuer den Laden beisteuerten.
    if( !ETO->QueryProp("test") )
    {
      if( !TP->QueryMoney() )
      {
        tell_room(ETO, BS("Ohne Geld bekommen Sie bei uns nichts, "+anrede()
          +". Wir sind kein Wohlfahrtsinstitut!",78, Name(WER)+" sagt: "));
        return 1;
      }
      if( TP->QueryMoney() < preis )
      {
        tell_room(ETO, BS( CAP(anrede())+", Sie haben zuwenig Geld dabei. "
          +"Der Preis fuer dieses Objekt ist "+preis
          +" Muenzen.",78, Name(WER)+" sagt: "));
        return 1;
      }
      TP->AddMoney(-preis);      // Spieler Geld abziehen
      ZENTRALBANK->PayIn(preis); // Geld auf die Bank.
    }
    cnt=clone_object( LADEN(cnt_file) );
    cnt->move(zielraum, M_PUT|M_SILENT);
    tell_room(ETO, TP->Name(WER)+" kauft "+cnt->name(WEN)+".\n", ({TP}) );
    tell_room(ETO, BS("Es ist ein Vergnuegen, mit Ihnen Geschaefte zu machen, "
      +anrede()+"! Wenn Sie in Ihr Haus zurueckkehren, werden Sie die Ware bereits "
      +"vorfinden, so schnell arbeiten wir.",78, Name(WER)+" sagt: "));
    tell_room(zielraum, BS("Mit Knall und Schwefelduft erscheint ein kleines, "
      +"verhutzeltes Maennchen. Es traegt einen gruenen Anzug und eine gruene "
      +"Kappe, auf der in gelbem Schriftzug \"SCHRAENKER\" steht. Das Maennchen "
      +"stellt "+cnt->name(WEN)+" ab und verschwindet auf dem gleichen Wege, wie "
      +"es hereingekommen ist."));
    if( present("Interactive", SCHRANKMASTER) )
      tell_room(SCHRANKMASTER, TP->Name(WER)+" kauft "+name(WEN)+".\n");
    SCHRANKMASTER->RegisterCnt(cnt, cnt->QueryProp("cnt_version_std")
      +cnt->QueryProp("cnt_version_obj"), geteuid(TP),
       "/d/seher/haeuser/"+geteuid(TP)+"raum0");
    return 1;
  }
  else if(query_wiz_level(TP) >= LEARNER_LVL )  // TP ist Magier.
  {
    tell_room(ETO, BS("Tut mir leid, "+anrede()+". An Magier liefern wir nichts. "
      +"Das ist ein Prinzip des Hauses, und Prinzipien sind dazu da, dass "
      +"sie eingehalten werden. ",78, Name(WER)+" sagt: "));
    return 1;
  }
  else  // TP ist Spieler, kein Seher oder Magier.
  {
    tell_room(ETO, BS("Tut mir leid, "+anrede()+". Wir liefern nur an Seher. "
      +"Aber da ich Sie als strebsame und erfolgreiche Person ansehe, "
      +"bin ich sicher, dass wir uns bald wiedersehen!",78, Name(WER)+" sagt: "));
    return 1;
  }
}

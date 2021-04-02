// Einiger Code wurde aus dem Questtagebuch des Tempels uebernommen.
// Da man das Buch, sobald es im aufgestellten Lesepult des Spielers
// abgelegt wurde, auch lesen koennen muss, sind die Funktionen und
// Details auch weiterhin sinnvoll. Kaufen kann es jeder Seher, aber
// nutzen kann man es nur mit dem von Hels Avatar ueberreichten Buch.

#include <properties.h>
#include <moving.h>
#include <language.h>
#include <wizlevels.h>
#include "/d/seher/haeuser/moebel/moebel.h"
#include "/d/seher/haeuser/haus.h"
#include "/d/anfaenger/ark/knochentempel/files.h"
#pragma strong_types,rtt_checks

inherit LADEN("swift_std_container");

static string owner;
string le(string key);
string in(string key);
int cmd_eintragen(string str);
int cmd_lesen(string str);
int take_page(string str);
int drop_book(string str);
int cmd_blaettern(string str);
int cmd_oeffnen(string str);
int cmd_schliessen(string str);
void AddTheOldDetails();

protected void create()
{
 if(!clonep(this_object()))
 {
  set_next_reset(-1);
  return;
 }

 ::create();

owner="Anonymus";

AddDetail("finger",BS(
"Deine Finger sollten schon vorsichtig sein, wenn sie das Zeder"+
"nholz beruehren. Einerseits, damit Du Dich nicht verletzt, and"+
"ererseits, damit Du nichts beschmutzt.",78));
AddDetail(({"holz","zedernholz","zeder"}),BS(
"Aus einem einzigen starken Stamm einer Zeder bearbeitet, ist d"+
"ieses edle Lesepult schon etwas sehr Feines und Dekoratives.",78));
AddDetail(({"stamm","zedernstamm","holzstamm"}),BS(
"Der Zedernstamm ist wahrlich kunstvoll veredelt worden, sein k"+
"raeftiges Braun wurde hervorragend zur Geltung gebracht.",78));
AddDetail(({"braun","dunkelbraun"}),BS(
"Der dunkelbraune Stamm glaenzt wunderschoen, wenn Licht auf ih"+
"n faellt. Ein sehr angenehmes Holz, das fuer das Lesepult verw"+
"endet wurde.",78));
AddDetail("geltung",BS(
"Ja, dieses Holz gefaellt Dir sehr, das ganze Pult ist ein beme"+
"rkenswertes Kunstwerk.",78));
AddDetail(({"kunst","werk","kunstwerk"}),BS(
"Immer wieder faellt Dein bewundernder Blick auf das Lesepult.",78));
AddDetail("blick",BS(
"Wachsam und flink wie immer, hat er jedoch ein wenig pausiert,"+
" indem er muessig auf dem Lesepult ruht.",78));
AddDetail(({"feines","dekoratives"}),BS(
"Das teilweise eckige und dann wieder sanft geschwungene, runde"+
" Lesepult sieht schon ziemlich repraesentativ aus.",78));
AddDetail("zeder",BS(
"Von ihr ist nurmehr das Pult geblieben, allerdings findest Du,"+
" dass es nicht der schlechteste Teil ist.",78));
AddDetail("teil",BS(
"Du siehst nur den bearbeiteten Teil der Zeder vor Dir.",78));
AddDetail("exemplar",BS(
"Es ist ganz klar zu sehen, dass es sich nur um eine Zeder hand"+
"elt, aus der dieses Pult entstand.",78));
AddDetail(({"handwerk","kunst"}),BS(
"Tatsaechlich ist dieses Lesepult ein sehr schoenes Beispiel ho"+
"echster handwerklicher Kunst.",78));
AddDetail("beispiel",BS(
"Ein weiteres steht hier dummerweise nicht herum.",78));

 SetProp("cnt_version_obj","1");
 SetProp(P_NAME,"Lesepult");
 SetProp(P_GENDER,NEUTER);
 SetProp(P_NEVERDROP,1);
 SetProp(P_VALUE,10);
 SetProp(P_WEIGHT,500000);
 SetProp(P_NOGET,"Das Lesepult ist Dir viel zu schwer.\n");
 SetProp(P_NOSELL,"Du behaeltst das Lesepult lieber.\n");
 SetProp(P_MATERIAL,([MAT_CEDAR:100]));
 SetProp(H_FURNITURE,1);
 SetProp(P_INFO,BS(
 "Auf dieses Lesepult kann man ein ganz bestimmtes schwarzes Bu"+
 "ch legen. Dort liegt es perfekt, um darin zu blaettern und zu"+
 " lesen. Ausserdem birgt es sicher noch Geheimnisse.",78));

 AddId(({"pult","lesepult","\nlesepult_ark"}));

 AddCmd("eintragen&quest|aufgabe|loesung|erlebnis|abenteuer",
 #'cmd_eintragen,"Was willst Du hier denn eintragen ?\n");
 AddCmd(({"lies","lese","les"}),#'cmd_lesen);
 AddCmd("nimm",#'take_page);
 AddCmd(({"lege","leg"}),#'drop_book);
 AddCmd("blaetter|blaettre|blaettere&in&buch|tagebuch|"
 "tempeltagebuch",#'cmd_blaettern,"Worin willst Du blaettern ?|"
 "Willst Du im Tagebuch blaettern ?");
 AddCmd("oeffne&@ID|buch|tagebuch|tempeltagebuch",#'cmd_oeffnen,
 "Was willst Du oeffnen?\n");
 AddCmd("schliesse|schliess&@ID|buch|tagebuch|tempeltagebuch",
 #'cmd_schliessen,"Was willst Du schliessen?\n");

 if (MQTBMASTER->QueryTagebuchAufPult(this_player()))
 AddTheOldDetails();
}

void SetOwner(string str)
{
 owner=str;
}

string QueryOwner()
{
 return owner;
}

string _query_short()
{
 if ((owner=="Anonymus")||
      !this_player()||
      !(MQTBMASTER->QueryTagebuchAufPult(this_player())))
 return "Ein wunderschoenes Lesepult aus dunkelbraunem Zedernholz";
 if (getuid(this_player())==owner)
 return
 "Ein wunderschoenes Lesepult, auf dem Dein Tagebuch des Tempels ruht";
 else return
 "Ein wunderschoenes Lesepult, das ein gueldenes Buch traegt";
}

string _query_long()
{
 if ((owner=="Anonymus")||
      !this_player()||
      !(MQTBMASTER->QueryTagebuchAufPult(this_player())))
 return BS(
 "Vorsichtig faehrst Du mit den Fingern ueber das dunkelbraune Holz."+
 " Was Du spuerst, fuehlt sich kuehl und angenehm an, die Zeder, die"+
 " fuer dieses bestaunenswerte Lesepult herhalten musste, wurde vors"+
 "ichtig und aeusserst kunstfertig bearbeitet. Ein wirklich schoenes"+
 " Exemplar holzhandwerklicher Kunst.",78);
 if (getuid(this_player())==owner)
 {
  if (MQTBMASTER->QueryTagebuchAufPult(this_player()))
  return BS(
  "Vorsichtig faehrst Du mit den Fingern ueber das dunkelbraune Holz"+
  ". Was Du spuerst, fuehlt sich kuehl und angenehm an, die Zeder, d"+
  "ie fuer dieses bestaunenswerte Lesepult herhalten musste, wurde v"+
  "orsichtig und aeusserst kunstfertig bearbeitet. Ein wirklich scho"+
  "enes Exemplar holzhandwerklicher Kunst, Du bist froh, dieses Lese"+
  "pult erstanden zu haben. Auf dem Lesepult siehst Du Dein schwarze"+
  "s Tagebuch des Knochentempels, in dem Du nun nur noch lesen kanns"+
  "t, denn Deine Abenteuer dort sind vollstaendig zu Papier gebracht"+
  ". Allerdings kannst Du auch noch ein wenig weiter drin herumblaet"+
  "tern, wenn Du magst.",78);
  return BS(
  "Vorsichtig faehrst Du mit den Fingern ueber das dunkelbraune Holz"+
  ". Was Du spuerst, fuehlt sich kuehl und angenehm an, die Zeder, d"+
  "ie fuer dieses bestaunenswerte Lesepult herhalten musste, wurde v"+
  "orsichtig und aeusserst kunstfertig bearbeitet. Ein wirklich scho"+
  "enes Exemplar holzhandwerklicher Kunst, Du bist froh, dieses Lese"+
  "pult erstanden zu haben. Ein wenig gruebeln laesst Dich die Tatsa"+
  "che, dass das Lesepult leer ist. Was man dort wohl platzieren kan"+
  "n ?",78);
 }
 return BS(
 "Vorsichtig faehrst Du mit den Fingern ueber das dunkelbraune Holz."+
 ". Was Du spuerst, fuehlt sich kuehl und angenehm an, die Zeder, di"+
 "e fuer dieses bestaunenswerte Lesepult herhalten musste, wurde vor"+
 "sichtig und aeusserst kunstfertig bearbeitet. Ein wirklich schoene"+
 "s Exemplar holzhandwerklicher Kunst, Du bist ein wenig neidisch."+
 ""+(MQTBMASTER->QueryTagebuchAufPult(this_player())?" Auf dem Lesep"+
 "ult wurde ein schwarzes Buch abgelegt, das sehr wertvoll aussieht.":
 " Das Lesepult sieht ohne dazugehoeriges Buch ein wenig einsam aus.")+".",78);
}

void AddTheOldDetails()
{
 AddDetail(({"buch","buechlein","tagebuch"}),BS(
 "Dies ist ein kleines Buechlein aus Gold, auf dem ein Name in "+
 "schwarzen Lettern steht. Ansonsten ist der Einband schlicht, "+
 "das Papier schon ausfuehrlich beschrieben und vom vielen Blae"+
 "ttern hier und da auch schon ein wenig geknickt.",78));
 AddDetail("blaettern",BS(
 "Ja, das schoene Buechlein ist von haeufigem Blaettern nicht s"+
 "choener geworden, aber irgendetwas Magisches hat es schon an "+
 "sich. Ab und an magst Du immer wieder einmal herumblaettern.",78));
 AddDetail("magisches",BS(
 "Es ist nicht so einfach zu ergruenden, indem man einmal genau"+
 "er hinsieht.",78));
 AddDetail("leder",BS(
 "Das Leder ist glatt und vollkommen guelden. Nur die schwarzen"+
 " Lettern geben einen angenehmen Kontrast ab. Irgendwie seltsa"+
 "m, denn Du findest, dass Schwarz und Gold eigentlich umgekehr"+
 "t mehr Sinn machen.",78));
 AddDetail("sinn",BS(
 "Vielleicht ist aber auch alles bestens, und nur Dein Verstand"+
 " will das nicht nachvollziehen.",78));
 AddDetail("verstand",BS(
 "Ja, auch Du kannst ihm nicht immer folgen, also wundere Dich "+
 "nicht, warum dieses Leder hier golden verfaerbt ist und die B"+
 "uchstaben darauf schwarz.",78));
 AddDetail(({"lettern","buchstaben"}),BS(
 "Du kannst sie lesen.",78));
 AddDetail(({"name","namen"}),BS(
 "Man kann ihn genauso lesen wie die Buchstaben.",78));
 AddDetail("kontrast",BS(
 "Die Buchstaben heben sich wirklich deutlich vom gueldenen Led"+
 "er des Einbands ab.",78));
 AddDetail("einband",BS(
 "Es ist kein aufwaendiger Einband, fuerwahr. Dennoch faszinier"+
 "t die Pracht, die mit den schwarzen Lettern kontrastiert.",78));
 AddDetail("pracht",BS(
 "So muss man den goldenen Einband wohl nennen.",78));
 AddDetail(({"papier","seiten","buchseiten","seite","buchseite"}),BS(
 "Das Papier ist von guter Qualitaet, die Buchseiten lassen sic"+
 "h sicher einfach beschreiben.",78));
 AddDetail(({"schwarze buchseiten","schwarze buchseite",
 "schwarze buchseite"}),BS(
 "Tatsaechlich, beim Blaettern siehst Du einige schwarze Buchse"+
 "iten, die unbeschrieben sind. Was hat dies zu bedeuten ?",78));
 AddDetail("qualitaet",BS(
 "Du siehst, dass das Papier fest und bluetenweiss ist.",78));
 AddReadDetail(({"buch","buechlein","tagebuch"}),BS(
 "Das Buch hat ein Inhaltsverzeichnis. Wenn dort etwas steht, k"+
 "annst Du das oder die Kapitel sicher lesen.",78));
 AddDetail("kapitel",BS(
 "Lies nur im Inhaltsverzeichnis nach, vielleicht hat das Buch "+
 "bereits ein oder mehrere Kapitel.",78));
 AddReadDetail("kapitel",BS(
 "Welches Kapitel willst Du denn lesen ? Sie sind sicher durchn"+
 "ummeriert, wenn man schon etwas im Buch niedergeschrieben hat.",78));
 AddReadDetail(({"name","namen","buchstaben","lettern"}),#'le);
 AddReadDetail(({"inhalt","inhaltsverzeichnis"}),#'in);
 SetProp(P_MATERIAL,([MAT_LEATHER:10,MAT_PAPER:15,MAT_CEDAR:75]));
}

int drop_book(string str)
{
 if (!stringp(str))
 return 0;
 if (strstr(lower_case(str),"pult",0)==-1)
 return 0;
 notify_fail(BS(
 "Willst Du etwas auf das Pult legen ? Ein Buch waere passend, "+
 "aber es sollte schon etwas Besonderes sein.",78));
 if (strstr(lower_case(str),"buch",0)==-1&&
     strstr(lower_case(str),"buechlein",0)==-1)
 return 0;
 notify_fail(BS(
 "Das ist nicht Dein Lesepult, Du laesst es besser in Ruhe.",78));
 if (getuid(this_player())!=owner)
 return 0;
 notify_fail(BS(
 "Auf dem Pult liegt schon ein goldenes Buch.",78));
 if (MQTBMASTER->QueryTagebuchAufPult(this_player()))
 return 0;
 notify_fail(BS(
 "Entweder hast Du Dich noch nicht als wuerdig erwiesen, indem "+
 "Du alle Aufgaben im Knochentempel erfuellt hast oder Du hast "+
 "dem Avatar des Waechters der Nacht, Mhun'dae Hel, noch nicht "+
 "Deine Aufwartung gemacht und ihm gezeigt, dass Du wuerdig bis"+
 "t, ein geeignetes Buch zu empfangen. Auf alle Faelle kannst D"+
 "u hier noch nichts auf das Pult legen.",78));
 if (!present_clone(OBJ("buechlein_invers"),this_player()))
 return 0;
 MQTBMASTER->AddBuchAufPult(this_player());
 this_player()->ReceiveMsg(BS(
 "Vorsichtig schlaegst Du Dein gueldenes Tagebuch auf, das Dir "+
 "Mhun'dae Hel, das Avatar des Waechters der Nacht, ueberreicht"+
 " hat. Du legst es sachte auf das Pult und merkst, dass es sof"+
 "ort eine magische Verbindung mit dem Lesepult eingeht. Es ist"+
 " nun untrennbar mit diesem verbunden.",78),MT_MAGIC,MA_USE);
 tell_room(environment(),BS(
 this_player()->Name(WER)+" legt vorsichtig ein gueldenes Buch "+
 "mit schwarzen Lettern aufgeschlagen auf dem Lesepult ab und b"+
 "etrachtet dieses dann ehrfuerchtig und zutiefst zufrieden.",78),
 ({this_player()}));
 return 1;
}

int cmd_blaettern(string str)
{
 notify_fail(BS(
 "Auf dem Lesepult liegt doch gar kein Tagebuch.",78));
 if (!(MQTBMASTER->QueryTagebuchAufPult(find_player(owner))))
 return 0;
 notify_fail(BS(
 "Das ist nicht Dein Tagebuch, Du laesst es lieber in Frieden.",78));
 if (getuid(this_player())!=owner)
 return 0;
 this_player()->ReceiveMsg(BS(
 "Du blaetterst ein wenig durch Dein Tagebuch und stoesst auf e"+
 "inige raetselhafte schwarze Seiten. Was mag es damit auf sich"+
 " haben ? Ob Du sie herausnehmen kannst ?",78),MT_LOOK,MA_LOOK);
 return 1;
}

int take_page(string str)
{
 if (!stringp(str))
 return 0;
 if (present(str,environment()))
 return 0;
 notify_fail(BS(
 "Wo siehst Du denn hier ein Buch ?",78));
 if (!(MQTBMASTER->QueryTagebuchAufPult(this_player()))&&
     strstr(lower_case(str),"buch")!=-1)
 return 0;
 if ((owner!=getuid(this_player()))&&
     strstr(lower_case(str),"buch")!=-1)
 notify_fail(BS(
 "Dies ist nicht Dein Lesepult oder Dein Buch darauf, deshalb l"+
 "aesst Du es vorsichtshalber liegen.",78));
 return 0;
 if ((owner!=getuid(this_player()))&&
     strstr(lower_case(str),"seite")!=-1)
 notify_fail(BS(
 "Die Eigentuemerin oder der Eigentuemer des Lesepults haette s"+
 "icher etwas dagegen, wenn Du das Buch zerstoerst, also laesst"+
 " Du es lieber.",78));
 return 0;
 // Heilung 1x alle 6 Stunden.
 if (this_player()->check_and_update_timed_key(21600,"Ark_Hels_Schwarzes_Buch")==-1)
 {
  object o;
  this_player()->ReceiveMsg(BS(
  "Du reisst erst vorsichtig eine schwarze Seite und dann eine"+
  " weitere aus dem goldenen Buch. Dann ueberfaellt Dich eine "+
  "kleine Schwaeche, die Dich vor weiterem Zerstoeren innehalt"+
  "en laesst. Du solltest Hels Grosszuegigkeit nicht ueberscha"+
  "etzen.",78),MT_FEEL,MA_TAKE);
  tell_room(environment(),BS(
  this_player()->Name(WER)+" reisst zwei Seiten aus dem Buch, "+
  "das auf dem Pult liegt. Dann haelt "+this_player()->QueryPronoun(WER)+""+
  " inne, wahrscheinlich folgt "+this_player()->QueryPronoun(WER)+""+
  " einer klugen Eingebung.",78));
  o=clone_object(OBJ("reg_seite"));
  if (o->move(this_player())!=MOVE_OK)
  {
   o->remove(1);
   this_player()->ReceiveMsg(BS(
   "Du kannst die erste Seite nicht mehr tragen, sie loest sic"+
   "h einfach auf.",78),MT_MAGIC,MA_REMOVE);
  }
  // Die erste Seite behandelt, hier, falls man auch die zweite
  // Seite nicht tragen kann oder mit der ersten am Limit ist.
  o=clone_object(OBJ("reg_seite"));
  if (o->move(this_player())!=MOVE_OK)
  {
   o->remove(1);
   this_player()->ReceiveMsg(BS(
   "Du kannst die zweite Seite nicht mehr tragen, sie loest si"+
   "ch einfach auf.",78),MT_MAGIC,MA_REMOVE);
  }
  return 1;
 }
 this_player()->ReceiveMsg(BS(
 "Vergeblich reisst Du mehr oder weniger vorsichtig an einigen"+
 " schwarzen Seiten im Buch herum, doch sie bleiben stoerrisch"+
 ". Es wird wohl ein wenig dauern, bis Du das wieder versuchen"+
 " kannst.",78),MT_MAGIC,MA_USE);
 tell_room(environment(),BS(
 this_player()->Name(WER)+" reisst im Buch auf dem Lesepult he"+
 "rum.",78),({this_player()}));
 return 1;
}

int cmd_lesen(string str)
{
int k;
 if(!str) return 0;
 if (owner=="Anonymus")
 {
  write (BS(
  "Dieses Lesepult scheint niemandem zu gehoeren ... es loest s"+
  "ich daher einfach auf, als Du darauf etwas lesen willst.",78));
  remove(1);
  return 1;
 }
 if (getuid(this_player())!=owner)
 {
  notify_fail(BS(
  "Dieses Tagebuch gehoert "+capitalize(owner)+", nicht Dir, Du"+
  " bist eigentlich nicht so recht an dem interessiert, was man"+
  " in dem Buch niedergeschrieben hat. Sicher ist es uebertrieb"+
  "en langatmig formuliert und stimmt nicht genau mit dem ueber"+
  "ein, was wirklich geschah. Man kennt das ja.",78));
  return 0;
 }
 if (strstr(lower_case(str),"kapitel",0)!=-1&&
     sscanf(lower_case(str),"kapitel %d",k)!=1)
 {
  write (BS(
  "Willst Du ein Kapitel aus dem Buch lesen ? Dann gib bitte ei"+
  "ne Ziffer an.",78));
  return 1;
 }
 if (k>0)
 {
  MQTBMASTER->LiesQuest(k,this_player());
  return 1;
 }
 return 0;
}

int cmd_eintragen(string str)
{
 if (!(MQTBMASTER->QueryTagebuchAufPult(this_player())))
 {
  notify_fail(BS(
  "Du findest hier nichts, worin Du etwas eintragen koenntest.",78));
  return 0;
 }
 if (getuid(this_player())!=owner)
 {
  notify_fail(BS(
  "Dieses Tagebuch gehoert "+capitalize(owner)+", nicht Dir, Du"+
  " kannst darin nichts eintragen.",78));
  return 0;
 }
 notify_fail(BS(
 "Deine Missionen im Knochentempel ... besser gesagt, dem Wyrmt"+
 "empel, sie sind allesamt tapfer von Dir erfuellt worden. Du k"+
 "annst hier nichts weiter mehr eintragen, sicher aber von der "+
 "arkanen Macht des Buches profitieren, wenn Du es einmal durch"+
 "blaetterst.",78));
 return 0;
}

string calc_name()
{
string *nam;
string res;
int i,j;
 res="";
 if (owner=="")
 return "";
 nam=explode(upperstring(owner),"");
 if (j=sizeof(nam))
 {
  switch(j)
  {
   case 3..4:
    res="          ";
    break;
   case 5:
    res="        ";
    break;
   case 7:
    res="      ";
    break;
   case 9:
    res="    ";
    break;
   default:
    res="   ";
  }
  for (i=0;i<j;i++)
  {
   res+=nam[i]+" ";
  }
 }
 return res;
}

string le(string key)
{
 return "\n"+
 "    De Templum Os Hel\n\n"+
 calc_name()+"\n\n";
}

string in(string key)
{
 if (owner!=getuid(this_player()))
 return "Dort steht allerdings nichts. Seltsam.\n";
 return MQTBMASTER->LiesInhalt(find_player(owner));
}

// Das hier bezeichnet den Urheber des Moebelstuecks.
// Nicht meine Idee!
string GetOwner() { return "Ark"; }

int cmd_oeffnen(string str)
{
 if (strstr(lower_case(str),"pult")!=-1)
 {
  this_player()->ReceiveMsg(
  "Du kannst das Pult nicht oeffnen, nur etwas zum Lesen darauflegen.",
  MT_LOOK,MA_FEEL);
  return 1;
 }
 if (strstr(lower_case(str),"buch")!=-1)
 {
  if (!(MQTBMASTER->QueryTagebuchAufPult(this_player())))
  return 0;
  this_player()->ReceiveMsg(
  "Das schwarze Buch ist doch schon geoeffnet.",MT_LOOK,MA_FEEL);
  return 1;
 }
 // Alles andere per Default.
 return 0;
}

int cmd_schliessen(string str)
{
 if (strstr(lower_case(str),"pult")!=-1)
 {
  this_player()->ReceiveMsg(
  "Du kannst das Pult nicht schliessen.",MT_LOOK,MA_FEEL);
  return 1;
 }
 if (strstr(lower_case(str),"buch")!=-1)
 {
  if (!(MQTBMASTER->QueryTagebuchAufPult(this_player())))
  return 0;
  this_player()->ReceiveMsg(
  "Das Buch bleibt trotz Deiner Bemuehungen einfach offen.",MT_LOOK,MA_FEEL);
  return 1;
 }
 else return 0;
}

mixed _query_noget()
{
 if (getuid(this_player())!=owner)
 return BS(
 "Dies ist offensichtlich nicht Dein Pult, deshalb laesst "+
 "Du es auch besser liegen.",78);
 return 0;
}

// Beim Kauf setzen wir erstmal nur den Eigentuemer.
// Ist aus Swifts Container, der diese Funktion im
// NotifyMove() aufruft.
protected void SetBesitzer(string uid, string uuid)
{
 SetOwner(uid);
 MQTBMASTER->AddLesepult(find_player(uid)||find_netdead(uid));
}

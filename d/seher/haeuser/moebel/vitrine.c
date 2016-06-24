/************************************************************************************
Vitrine fuer die Pokale
Autor: Miril
letzte Aenderung:
05.09.2011
************************************************************************************/

#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include "/d/ebene/miril/fraternitas/fraternitas.h"
#include "/d/ebene/miril/fraternitas/trophy.h"
#pragma strong_types,rtt_checks
#include "/d/seher/haeuser/moebel/moebel.h"
#include "/d/seher/haeuser/haus.h"

inherit LADEN("swift_std_container");

string besitzer;
int staub;
string *trophynamen;

static string my_long();
static string staub_descr();
void GetTrophyDetails();
string *GetAllTrophyNames();
void NeueDetails();

protected void create()
{
  if (!clonep(this_object())){
    set_next_reset(-1);
    return;
  }
  set_next_reset(86400);
  ::create();
  besitzer="";
  staub=0;
  SetProp(P_NAME,"Vitrine");
  SetProp(P_SHORT,"Eine Vitrine");
  Set(P_LONG,#'my_long,F_QUERY_METHOD);
  SetProp(P_GENDER,FEMALE);
  SetProp(P_MATERIAL,([MAT_GLASS:50,MAT_EBONY:50]));
  SetProp(P_VALUE,10000);
  SetProp(P_WEIGHT,500000);
  SetProp(P_MAX_OBJECTS,0);
  SetProp(P_MAX_WEIGHT,0);
  SetProp(H_FURNITURE,1);
  SetProp(P_NOGET,"Die Vitrine ist viel zu schwer.\n");
  SetProp("cnt_version_obj",2);
  GetTrophyDetails();
  AddId(({"\nmiril_pokal_vitrine","vitrine"}));

  NeueDetails();

  AddCmd(({"saeubere","saeuber","reinige","reinig","mach","mache"}),
           "saeuber_cmd");

  AddCmd(({"stelle","stell"}),"stell_cmd");
  RemoveCmd(({"serlaube"}));

}

void reset(){
  ++staub;
  set_next_reset(86400);
  ::reset();
  return;
}

void SetBesitzer(string uid,string uuid){
  besitzer=uuid;
  GetTrophyDetails();
  return;
}

string QueryBesitzer(){
  return besitzer;
}

void NeueDetails(){
  AddDetail(({"holz","ebenholz"}),BS("Die Vitrine besteht teilweise "
    "aus Ebenholz. Tueren, Deckel und Einlegeboeden sind aus Glas."));
  AddDetail(({"tueren","tuer","glastueren","glastuer","deckel"}),BS(
            "Durch das Glas kannst Du ins Innere der Vitrine gucken."));
  AddDetail(({"inneres","gegenstaende","gegenstand"}),
            BS("Dort kann man Pokale hineinstellen, aber nicht wieder "
            "herausnehmen."));
  AddDetail(({"glas","scheibe","scheiben"}),
     BS("Glas ist praktisch, solange es sauber ist: "
        "Man kann dadurch ins Innere schauen."));
  AddDetail(({"reihe","platz","einlegeboeden","einlegeboden","boeden",
              "boden"}),
           BS("Die vielen Boeden bieten Platz fuer viele Pokale!"));
  return;
}


static string my_long(){
  if(staub<21)
  return BS(
  "Die schlichte Vitrine aus dunklem Holz eignet sich hervorragend, um "
  "wertvolle Gegenstaende darin auszustellen. Hinter den beiden Glastueren "
  "siehst Du eine Reihe von Einlegeboeden, die ebenfalls aus Glas bestehen. "+
  staub_descr());
  else return BS(staub_descr());
}


static string staub_descr(){
  string text;
  text="";
  switch(staub){
    case 0..1:
      text=
        "Die Vitrine ist hervorragend gepflegt. Du siehst keinerlei Staub "+
        "darin, und die Scheiben sind voellig klar.\n";
        AddDetail(({"staub"}),BS("Keiner zu sehen."));
    break;
    case 2..4:
      text=
        "Die Vitrine ist gut gepflegt. Es liegt kaum Staub darin, und die "+
        "Tueren sind klar.\n";
        AddDetail(({"staub"}),BS("Man sieht nur wenig davon."));
    break;
    case 5..7:
      text=
        "Die Vitrine ist in einem ordentlichen Zustand, allerdings sammelt "+
        "sich so langsam Staub auf den Einlegeboeden.\n";
        AddDetail(({"staub"}),BS("So langsam koennte man die "
        "Vitrine saeubern!"));
        AddDetail(({"zustand"}),BS("Noch ist er ordentlich."));
    break;
    case 8..10:
      text=
        "Die Vitrine wirkt schon etwas staubig. Man erkennt auch, dass "+
        "der ein oder andere Fingerabdruck die Tueren ziert.";
        AddDetail(({"staub","fingerabdruck","tueren","glastueren",
        "deckel","glastuer","tuer","zustand"}),
        BS("Es lohnt sich jetzt schon, die Vitrine zu saeubern."));
    break;
    case 11..15:
      text=
        "Du hast den Eindruck, dass die Vitrine langsam einmal gesaeubert "+
        "werden koennte. Die Einlegeboeden sind staubig und zahlreiche "+
        "Fingerabdruecke sind auf den Glastueren zu sehen.";
    break;
    case 16..20:
      text=
        "Die Vitrine macht einen recht ungepflegten Eindruck. Innen ist "+
        "sie schon sehr verstaubt und auch die Tueren sind schon recht "+
        "dreckig geworden.";
        AddDetail(({"staub"}),BS("Nun wird es aber wirklich Zeit, die "
        "Vitrine zu saeubern. Sonst sieht man bald nichts mehr darin."));
    break;
    case 21..25:
      text=
        "Die Vitrine ist total zugestaubt. Ueber den Glastueren sind "+
        "erste Spinnweben zu erkennen und die Scheiben sind schon fast "+
        "blind von all den Fingerabdruecken unaufmerksamer Gaeste.";
         RemoveDetail(m_indices(QueryProp(P_DETAILS)));
         AddDetail(({"spinnweben","dreck","vitrine","glastueren","glastuer",
         "tueren","tuer","scheibe","scheiben","deckel"}),
         "Unter dem ganzen Dreck ist leider nichts mehr zu erkennen.\n");
         AddDetail(({"gaeste","gast","fingerabdruck","fingerabdruecke"}),
         "Die Gaeste sind sicher schuld an dem Dreck.\n");
    break;
    case 26..30:
      text=
        "Du kannst die Vitrine unter der dicken Staubschicht kaum noch "+
        "erkennen und die Glastueren sind blind geworden.";
         AddDetail(({"staubschicht","schicht"}),
         BS("Die ist verdammt dick und undurchsichtig."));
    break;
    default:
      text=
        "Dass sich unter dieser Ansammlung von Spinnweben und Staub noch "+
        "eine Vitrine mit Glastueren verbergen soll, kannst Du kaum glauben. "+
        "Jedenfalls kann man durch die voellig blinden Scheiben nichts "+
        "mehr erkennen.";
      AddDetail(({"dreck","spinnweben","staub","ansammlung"}),"Pfui Deibel!\n");
  }
  return text;
}

static int saeuber_cmd(string str){
  string text;
  if(!str) return 0;
  if(strstr(str,"vitrine")<0) return 0;
  switch(staub){
    case 0:
      write("Das ist noch nicht noetig!\n");
      return 1;
    break;
    case 1..20:
      text="Du reinigst die Vitrine gruendlich. Das wurde auch langsam "
      "Zeit!";
      if(getuuid(TP)!=besitzer)
        text+="Du fragst Dich, wieso Du fremde Moebel reinigst.";

      write(BS(text));
      say(BS(TPN+" reinigt die Vitrine gruendlich. Das wurde auch langsam "
      "Zeit!\n"),TP);
      staub=0; //Hier darf noch jemand helfen!
    break;
    case 21..30:
      text="Du beginnst die Vitrine zu reinigen. ";
      if(getuuid(TP)!=besitzer){
        text+="Nach einiger Zeit fragst Du Dich, ob Du eigentlich spinnst, "+
        "denn das ist ueberhaupt nicht Deine Vitrine. Du laesst von dem "+
        "Vorhaben wieder ab.";
        say(BS(TPN+" faengt kurz an, die Vitrine zu reinigen, laesst es aber "
        "schnell wieder sein."),TP);
      }
      else{
        text+="Das dauert eine ganze Weile, aber dann strahlt die Vitrine "
        "wieder.";
        say(BS(TPN+" beginnt die Vitrine zu reinigen. Das dauert eine ganze "
        "Weile, aber dann strahlt sie wieder."),TP);
        staub=0;
      }
      write(BS(text));
    break;
    default:
      if(getuuid(TP)!=besitzer)
        write("Das dreckige Teil fasst Du nicht an!\n");
      else{
        write(BS(
          "Du schrubbst und schrubbst und es scheint kein Ende zu nehmen. "
          "Als Du schon langsam aufgeben willst, ist so langsam ein kleiner "
          "Erfolg zu sehen. Du nimmst Dir vor, die Vitrine in Zukunft "
          "haeufiger zu reinigen!"));
        say(BS(
          TPN+" schrubbt stundenlang an der Vitrine rum, bis sie endlich "
          "wieder richtig sauber ist."),TP);
        staub=0;
      }
  }
  if(staub==0){
    RemoveDetail(m_indices(QueryProp(P_DETAILS)));
    GetTrophyDetails();
    NeueDetails();
   }
  return 1;
}

void GetTrophyDetails(){
  string *trophyliste,*shorts;
  mapping trophydetails;

  shorts=({});

  trophyliste=MIRIL_TROPHYMASTER->GetPlacedTrophies(besitzer);

  foreach( string tr : trophyliste )
    shorts+=({ MIRIL_TROPHYMASTER->GetTrophyShort(tr) });

  shorts = sort_array(shorts, #'>);
  if(!sizeof(shorts))
    AddDetail(({"pokale","pokal","in vitrine"}),
                "Bislang ist die Vitrine noch leer.\n");
  else
  AddDetail(({"pokale","pokal","in vitrine"}),
    "Du siehst folgende Pokale in der Vitrine:\n"
    +implode(shorts,".\n")+".\n");

  foreach( string tr : trophyliste )
  {
    AddDetail( MIRIL_TROPHYMASTER->GetTrophyIds(tr),
               BS(MIRIL_TROPHYMASTER->GetTrophyLong(tr)));

    trophydetails = MIRIL_TROPHYMASTER->GetTrophyDetails(tr);
    if ( !sizeof(trophydetails) )
      continue;
    foreach ( mixed det, string det_desc : trophydetails )
      AddDetail( det, BS(det_desc) );
  }
  return;
}

string *GetAllTrophyNames(){
  int i,anzahl;
  string *trophynamen;

  trophynamen=({});

  foreach(string bla:MIRIL_TROPHYMASTER->GetTrophyListe())
    trophynamen+=MIRIL_TROPHYMASTER->GetTrophyIds(bla);

  return trophynamen;
}

static int stell_cmd(string str){
  string was,identifier;
  int resultat;
  object ob;
  if(!str) return 0;
  if(strstr(str,"vitrine")<0) return 0;
  notify_fail("Das ist nicht Deine Vitrine!\n");
  if(getuuid(TP)!=besitzer)
    return 0;
  notify_fail("Die Syntax ist: stelle <gegenstand> in vitrine!\n");
  if(sscanf(str,"%s in vitrine",was)!=1)
    return 0;

  notify_fail("Die Vitrine sollte schon offen sein.\n");
  if(QueryProp(P_CNT_STATUS)!=CNT_STATUS_OPEN && !IS_LEARNER(TP))
    return 0;
  notify_fail("Du solltest die Vitrine vorher erstmal saeubern!\n");
  if(staub>20)
    return 0;
  notify_fail("Das kannst Du leider nicht in die Vitrine stellen!\n");
  if(member(GetAllTrophyNames(),lower_case(was))<0
     && lower_case(was)!="pokal")
    return 0;
  notify_fail("Das hast Du nicht!\n");
  if(!ob=present(was,TP))
    return 0;
  notify_fail("Das kannst Du leider nicht in die Vitrine stellen!\n");
  if(!ob->id("\nmiril_pokal"))
    return 0;

  identifier=ob->QueryIdentifier();
  resultat=MIRIL_TROPHYMASTER->PlaceTrophy(getuuid(TP),identifier);

  notify_fail("Hier stimmt etwas nicht, sag bitte einem Erzmagier Bescheid! "
  "Resultat="+resultat+"\n");
  if(resultat!=1)
    return 0;
  write(BS("Du stellst den Pokal in die Vitrine und bewunderst ihn noch ein "
  "wenig.\n"));
  say(TPN+" stellt einen Pokal in die Vitrine.\n",TP);
  GetTrophyDetails();
  ob->remove();
  if(ob)
    destruct(ob);
  return 1;
}


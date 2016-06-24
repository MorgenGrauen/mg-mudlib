//--------------------------------------------------------------------------------
// Name des Objects:    Waffenschrank
// Letzte Aenderung:    22.03.2001
// Magier:              Swift
//--------------------------------------------------------------------------------
/* Changelog
   * 21.05.2007, Zesstra
     Kampfstaebe auf Hinweis von tassram ergaenzt.
*/
#pragma strong_types,rtt_checks

#include "schrankladen.h"
#include <ansi.h>
#include <class.h>
inherit LADEN("swift_std_container");

#define VERSION_OBJ "6"

string strall;

protected void create()
{
  if (!clonep(TO)) return;
  swift_std_container::create();
  SetProp(P_SHORT, "Ein Waffenschrank");
  SetProp(P_LONG, BS(
     "Mit den Waffen, die in diesen massiven Holzschrank passen, koenntest "
    +"Du ein ganzes Soeldnerheer ausruesten. Fuer jede Waffengattung gibt "
    +"es einen eigenen Platz, damit man auch alles gleich findet, sollte "
    +"man es brauchen.")+"@@cnt_status@@");
  AddId(({"waffenschrank", "holzschrank", "schrank"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Waffenschrank");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 100);     // Mehr sollte nicht sein, lt. Tiamak.
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_MISC_WOOD, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");
  SetProp("obj_cmd", "sliste [Objekt-Id] [Waffentyp|\"alles\"]\n"
    +"  Waffentypen: Schwerter, Speere, Aexte, Keulen, Kampfstaebe, Messer, Peitschen,\n"
    +"               Fernwaffen, Munition, Sonstiges, Unzuordbar\n");

  AD(({"platz"}), BS("Der Schrank bietet Platz fuer jede erdenkliche "
    +"Waffengattung."));

  AddCmd(({"sliste"}), "auflistung");
}

varargs int PreventInsert(object ob)
{
  // Keine Waffe? Hat in diesem Container nix verloren!
  if( !ob->QueryProp(P_WEAPON_TYPE) && !ob->QueryProp(P_WC) &&
      !ob->id("\nBumerang") && !ob->is_class_member(CL_AMMUNITION) &&
      !ob->id("\ngorseilpfeil") )
  {
    write( BS("In "+name(WEN,1)+" kannst Du nur Waffen + Munition legen!"));
    return 1;
  }
  else
    return ::PreventInsert(ob);
}

int auflistung2(string was, string nf_str1, string nf_str2, int alles)
{
  string obshort, suche, *strs;
  object *obs;
  int i;
  mapping suchobs;
  switch(was)
  {
    case "munition"   : suche=WT_AMMU; break;
    case "aexte"      : suche=WT_AXE; break;
    case "keulen"     : suche=WT_CLUB; break;
    case "messer"     : suche=WT_KNIFE; break;
    case "fernwaffen" : suche=WT_RANGED_WEAPON; break;
    case "speere"     : suche=WT_SPEAR; break;
    case "schwerter"  : suche=WT_SWORD; break;
    case "peitschen"  : suche=WT_WHIP; break;
    case "sonstiges"  : suche=WT_MISC; break;
    case "kampfstaebe": suche=WT_STAFF; break;
    case "unzuordbar" : suche="unzuordbar"; break;
    default           : write("Fehler: Ungueltiger Waffentyp. "
                             +"Folgende Typen gibt es:\n"+nf_str1+nf_str2);
                        return 1; break;
  }
  obs=all_inventory();
  suchobs=([]);
  for(i=0;i<sizeof(obs);i++)
  {
    obshort=to_string(obs[i]->QueryProp(P_SHORT));
    if(obshort=="Ein Bumerang@@fliegt@@")
      obshort="Ein Bumerang";
    else if(obshort=="Nichts besonderes" || obshort=="0") // keine P_SHORT ?
      obshort=obs[i]->Name(WER);     //   Hoffen wir mal dass das Obj. nen Namen hat.
    if(obs[i]->QueryProp(P_WEAPON_TYPE)==suche ||  // Gesuchter WeaponType gefunden...
       (!obs[i]->QueryProp(P_WEAPON_TYPE) && suche=="unzuordbar") )
    {
      if(suchobs[obshort])                         // P_SHORT des Obj. schon im mapping?
      {
        if( obs[i]->QueryProp(P_AMOUNT) )
          suchobs[obshort]+=obs[i]->QueryProp(P_AMOUNT); //   Dann den Zaehler um Amount hoch!
        else
          suchobs[obshort]+=1;                     //   Dann den Zaehler um 1 hoch!
      }
      else
      {
        if( obs[i]->QueryProp(P_AMOUNT) )
          suchobs+=([obshort: obs[i]->QueryProp(P_AMOUNT)]);
        else
          suchobs+=([obshort: 1]);
      }
    }
  }
  strs=m_indices(suchobs);
  if(sizeof(strs))
  {
    if(!alles)
    {
      if(suche=="unzuordbar")
        strall+=Name(WER,1)+" enthaelt folgende nicht zuordbare Waffen:\n";
      else
        strall+=Name(WER,1)+" enthaelt folgende "+CAP(was)+":\n";
      strall+="------------------------------------------------------------\n";
    }
    else
    {
      if(suche=="unzuordbar")
        strall+=ANSI_BOLD+"=== Nicht zuordbare Waffen:\n"+ANSI_NORMAL;
      else
        strall+=ANSI_BOLD+"=== "+CAP(was)+":\n"+ANSI_NORMAL;
    }
    for(i=0;i<sizeof(strs);i++)
    {
      if(suchobs[strs[i]] > 1)
        strall+=strs[i]+". ("+suchobs[strs[i]]+")\n";
      else
        strall+=strs[i]+".\n";
    }
  }
  else
    if(!alles)
      strall+=Name(WER,1)+" enthaelt keine "+CAP(was)+"!\n";
  return 1;
}

int auflistung(string str)
{
  string *strs, ob_id, was, nf_str1,nf_str2;
  strall="";
  nf_str1="   (Schwerter, Speere, Aexte, Keulen, Kampfstaebe, Messer, Peitschen,\n"
         +"    Fernwaffen, Munition, Sonstiges, Unzuordbar)\n";
  nf_str2="Syntax: sliste [Objekt-Id] [Waffentyp]\n"
         +"Bsp.:   sliste "+QueryProp(P_IDS)[1]+" "+"schwerter\n"
         +"        sliste "+QueryProp(P_IDS)[1]+" alles\n";
  notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str2);
  if(!str) return 0;
  if(present(str)==TO)   // Ueberpruefe, ob dieses Objekt gemeint ist!
  {
    write("Fehler: Es fehlt ein Waffentyp. Folgende Typen gibt es:\n"
         +nf_str1+nf_str2);
    return 1;
  }
  strs=old_explode(str, " ");
  notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"
             +nf_str2);
  if( sizeof(strs) < 2 ) return 0;   // Nur 1 Par. und der ist nicht die Objekt-Id
  if( sizeof(strs) == 2 )   // Anzahl Par. = 2
  {
    ob_id=strs[0];
    was=strs[1];
    if( IST_ZAHL(was) )     // Objekt-Id z.B. mit "schrank 2" angesprochen?
      if(present(ob_id+" "+was)==TO)   // Falls dieses Objekt gemeint ist und kein 3. Par!
      {
        write("Fehler: Es fehlt ein Waffentyp. Folgende Typen gibt es:\n"
              +nf_str1+nf_str2);
        return 1;
      }
      else
        return 0;
  }
  else if( sizeof(strs) == 3 )
  {
    ob_id=strs[0]+" "+strs[1];
    was=strs[2];
  }
  else        // Anzahl erforderlicher Parameter ueberschritten!
    return 0;
  if(present(ob_id)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;
  if(QueryProp(P_CNT_STATUS)!=CNT_STATUS_OPEN)
  {
    write("Dazu muesste "+name(WER,1)+" geoeffnet sein.\n");
    return 1;
  }
//--- Hier ist Parameterpruefung zu Ende.
  was=LOWER(was);
  if(was!="alles")
    auflistung2(was, nf_str1, nf_str2, 0);
  else
  {
    auflistung2("aexte", nf_str1, nf_str2, 1);
    auflistung2("keulen", nf_str1, nf_str2, 1);
    auflistung2("kampfstaebe", nf_str1, nf_str2, 1);
    auflistung2("messer", nf_str1, nf_str2, 1);
    auflistung2("speere", nf_str1, nf_str2, 1);
    auflistung2("schwerter", nf_str1, nf_str2, 1);
    auflistung2("peitschen", nf_str1, nf_str2, 1);
    auflistung2("fernwaffen", nf_str1, nf_str2, 1);
    auflistung2("munition", nf_str1, nf_str2, 1);
    auflistung2("sonstiges", nf_str1, nf_str2, 1);
    auflistung2("unzuordbar", nf_str1, nf_str2, 1);
  }
  TP->More(strall);
  return 1;
}

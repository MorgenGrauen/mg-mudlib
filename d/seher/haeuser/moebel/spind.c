//--------------------------------------------------------------------------------
// Name des Objects:    Spind
// Letzte Aenderung:    17.03.2001
// Magier:              Swift
//--------------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"
#include "ansi.h"
inherit LADEN("swift_std_container");

#define VERSION_OBJ "3"

string strall;

protected void create()
{
  if (!clonep(TO)) return;
  swift_std_container::create();
  SetProp(P_SHORT, "Ein Spind");
  SetProp(P_LONG, BS(
     "Dieser Stahlschrank ist vor allem fuer spartanische Raeume gedacht. Ebenso "
    +"wie sein Gegenstueck, der Kleiderschrank, verfuegt er ueber eine Menge "
    +"Regale, in denen er riesige Ausruestungsmengen aufnehmen kann.")
    +"@@cnt_status@@");
  AddId(({"spind", "schrank", "stahlschrank"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Spind");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 100);     // Mehr sollte nicht sein, lt. Tiamak.
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_MISC_METAL, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");
  SetProp("obj_cmd", "sliste [Objekt-Id] [Kleidungs-/Ruestungstyp]\n"
                    +"  Kleidungs-/Ruestungstypen: Helme, Umhaenge, Ruestungen, Hosen, Schuhe,\n"
                    +"                             Handschuhe, Guertel, Amulette, Ringe, Koecher,\n"
                    +"                             Schilde, Sonstiges\n");
  AD(({"regal", "regale"}), BS("Durch die Regale kommt etwas Ordnung in die "
    +"vielen Sachen, die man in den Schrank stecken kann. Du kannst Dir z.B. "
    +"eine Liste bestimmter Kleidungs-/Ruestungstypen des Schrankinhaltes "
    +"mit dem Befehl \"sliste\" anschauen. (Das geht natuerlich nur bei "
    +"offenem Schrank!"));

  AddCmd(({"sliste"}), "auflistung");
}

varargs int PreventInsert(object ob)
{
  // Nur Ruestung/Kleidung hat in diesem Container was verloren!
  if ((ob->IsClothing() || ob->QueryProp(P_ARMOUR_TYPE))
      && !ob->QueryProp(P_WEAPON_TYPE) )
  {
    return ::PreventInsert(ob);
  }

  write( BS("In "+name(WEN,1)+" kannst Du nur Ruestungs-/Kleidungsstuecke legen!"));
  return 1;
}

int auflistung2(string was, string nf_str1, string nf_str2, int alles)
{
  string obshort, suche, *strs;
  object *obs;
  int i;
  mapping suchobs;
  switch(was)
  {
    case "helme"      : suche=AT_HELMET; break;
    case "umhaenge"   : suche=AT_CLOAK; break;
    case "ruestungen" : suche=AT_ARMOUR; break;
    case "hosen"      : suche=AT_TROUSERS; break;
    case "schuhe"     : suche=AT_BOOT; break;
    case "handschuhe" : suche=AT_GLOVE; break;
    case "guertel"    : suche=AT_BELT; break;
    case "amulette"   : suche=AT_AMULET; break;
    case "ringe"      : suche=AT_RING; break;
    case "koecher"    : suche=AT_QUIVER; break;
    case "schilde"    : suche=AT_SHIELD; break;
    case "sonstiges"  : suche=AT_MISC; break;
    default           : write("Fehler: Ungueltiger Kleidungs-/Ruestungstyp. "
                             +"Folgende Typen gibt es:\n"+nf_str1+nf_str2);
                        return 1; break;
  }
  obs=all_inventory();
  suchobs=([]);
  for(i=0;i<sizeof(obs);i++)
  {
    obshort=obs[i]->QueryProp(P_SHORT);
    if(obshort=="Nichts besonderes") // keine P_SHORT ?
      obshort=obs[i]->Name(WER);     //   Hoffen wir mal dass das Obj. nen Namen hat.
    if(obs[i]->QueryProp(P_ARMOUR_TYPE)==suche)    // Gesuchter ArmourType gefunden...
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
      strall+=Name(WER,1)+" enthaelt folgende "+CAP(was)+":\n";
      strall+="------------------------------------------------------------\n";
    }
    else
      strall+=ANSI_BOLD+"=== "+CAP(was)+":\n"+ANSI_NORMAL;
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
  nf_str1="   (Helme, Umhaenge, Ruestungen, Hosen, Schuhe, Handschuhe,\n"
         +"    Guertel, Amulette, Ringe, Koecher, Schilde, Sonstiges)\n";
  nf_str2="Syntax: sliste [Objekt-Id] [Kleidungs-/Ruestungstyp|\"alles\"]\n"
         +"Bsp.:   sliste "+QueryProp(P_IDS)[1]+" "+"helme\n"
         +"        sliste "+QueryProp(P_IDS)[1]+" alles\n";
  notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str2);
  if(!str) return 0;
  if(present(str)==TO)   // Ueberpruefe, ob dieses Objekt gemeint ist!
  {
    write("Fehler: Es fehlt ein Kleidungs-/Ruestungstyp. Folgende Typen gibt es:\n"
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
        write("Fehler: Es fehlt ein Kleidungs-/Ruestungstyp. Folgende Typen gibt es:\n"
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
    auflistung2("helme", nf_str1, nf_str2, 1);
    auflistung2("umhaenge", nf_str1, nf_str2, 1);
    auflistung2("ruestungen", nf_str1, nf_str2, 1);
    auflistung2("hosen", nf_str1, nf_str2, 1);
    auflistung2("schuhe", nf_str1, nf_str2, 1);
    auflistung2("handschuhe", nf_str1, nf_str2, 1);
    auflistung2("guertel", nf_str1, nf_str2, 1);
    auflistung2("amulette", nf_str1, nf_str2, 1);
    auflistung2("ringe", nf_str1, nf_str2, 1);
    auflistung2("koecher", nf_str1, nf_str2, 1);
    auflistung2("schilde", nf_str1, nf_str2, 1);
    auflistung2("sonstiges", nf_str1, nf_str2, 1);
  }
  TP->More(strall);
  return 1;
}

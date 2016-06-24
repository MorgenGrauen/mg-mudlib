#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_clone, no_shadow

#include <defines.h>
#include <properties.h>
#include <wizlevels.h>
#include <items/kraeuter/kraeuter.h>
#include <items/kraeuter/trankattribute.h>
#include <living/comm.h>


#ifndef BS
#define BS(x)             break_string(x, 78)
#endif

// Weiter unten beim Parsen des Datenfiles werden SetProp() und Name()
// verwendet, daher erben wir thing.
inherit "/std/secure_thing";

// Liste aller moeglichen Zutaten, gemappt ueber einen key
// AN: irritierender Name, denn es sind nicht direkt die Zutaten enthalten
// sondern die ueber einen Key identifizierten Kraeutergruppen, die die
// Wirkungen/Funktionen festlegen.
// Format:
// ([ key : ({ attr_pos, attr_neg1, attr_neg2, Haltbark., Wirkdauer, 
//             Unterstuetzung, Stoerung, Haeufigkeit  }) ])
// Beispiel: [( "a7":({({2,400}),({11,150}),0,172800,0,({"n*"}),0,1}) ])
// HU: War ein Mapping, bleibt ein Mapping, und neu: Die Eintraege werden 
// auch Mappings!
private mapping ingredients;

// Mapping von id zum Key der Zutaten im ingredients-mapping: id2key[id]=key
// Die beiden Listen werden benoetigt, um zu den Kraut-IDs die jeweiligen
// Keys zu finden.
// HU: KEIN MAPPING. Ein Array von Strings. Aber das geht natuerlich.
private string *id2key;

// Mapping von key zu Kraeuter-ID der Zutaten.
private mapping key2id;

// Eigenschaften der Kraeuter:
// krautprops[id]=({ID, Demon, Gender, Name, Adjektiv(e), P_LONG, Raumdetail})
// krautprops[0] = 0; ID 0 darf nicht existieren
private mixed *krautprops;

// hier wird gespeichert welche Version einer Zutat fuer einen Spieler ist.
// AN/TODO: Wenn das fuer Nichtseher nutzbar sein soll, sollte hier besser
// mit getuuid() gearbeitet werden, statt getuid() zu verwenden.
// player enthaelt fuer jede Spieler-UID einen Bitstring. Gesetztes Bit
// bedeutet, dass der Spieler ueber das Kraut mit dieser ID etwas ueber
// die Verwendung und Wirkung weiss. Es gibt jedoch noch keine Lernmoeglich-
// keit und INSBESONDERE keine Funktionalitaet im Master, ueber die man 
// die Bits setzen koennte.
private mapping player;

// verstuemmeltes mapping fuer den VC in service (Save-Dummy)
private mapping map_ldfied;

// rooms enthaelt die Teileintraege des map_ldfied mapping vom VC
private mapping rooms;

// Enthaelt Daten zu den Raeumen, in denen das Trocknen von Kraeutern
// moeglich ist.
private mapping drying_data = ([:4]);

string build_plantname(mixed *props);

// struct-Templat fuer Trankattribute
// Fuer das SQL-Log MUSS die Reihenfolge der Trankattribute hier genau die
// sein, wie die Spalten in der Tabelle.
/* Currently not used.
struct trank_attrib_s {
    int car;
    int da;
    int dm;
    int du;
    int dn;
    int flt;
    int fro;
    int hI;
    int hP;
    int hK;
    int hL;
    int pa;
    int pm;
    int pu;
    int ss;
    int sp;
    int sd;
};
*/

#define allowed() (!process_call() && \
        IS_ARCH(RPL) && IS_ARCH(this_interactive()) )

#define POTIONFILES ({ TRANKITEM })

// eigenschaften im krautprops-array
#define PROP_ID           INGREDIENT_ID
#define PROP_DEMON        INGREDIENT_DEMON
#define PROP_GENDER       INGREDIENT_GENDER
#define PROP_NAME         INGREDIENT_NAME
#define PROP_ADJ          INGREDIENT_ADJ
#define PROP_LONG         INGREDIENT_LONG
#define PROP_ROOMDETAIL   INGREDIENT_ROOMDETAIL

// ATTR_ is immer ein array ({ attribut, ability })
#define ATTR_ARR_ATTRIB      0
#define ATTR_ARR_ABILITY     1

// maximal erlaubter Wert fuer eine Eigenschaft
#define MAX_ATTR_ABILITY      999

void debug(string str)
{
  //write("debug: "+str+"\n");
  if (this_player() && IS_ARCH(this_player()))
    this_player()->ReceiveMsg(str,536870912);
}

protected void create()
{
   seteuid(getuid());
   // AN/TODO: Variablen oben direkt initialisieren. Pruefen, ob davon
   // irgendwas nosave sein kann.
   if (!restore_object(__DIR__"/ARCH/krautmaster"))
   {
      ingredients=([]);
      id2key=({});
      key2id=([]);
      krautprops=({});
      player=([]);
      map_ldfied=([]);
      rooms=([]);
   }
   if (sl_open("/log/ARCH/krauttrank.sqlite") != 1)
   {
     raise_error("Datenbank konnte nicht geoeffnet werden.\n");
   }
   sl_exec("CREATE TABLE IF NOT EXISTS traenke(id INTEGER PRIMARY KEY, "
           "uid TEXT NOT NULL, rnd INTEGER, "
           "time INTEGER DEFAULT CURRENT_TIMESTAMP, "
           "receipe TEXT NOT NULL, "
           "quality TEXT NOT NULL, "
           "car INTEGER, "
           "da INTEGER, "
           "dm INTEGER, "
           "du INTEGER, "
           "dn INTEGER, "
           "flt INTEGER, "
           "fro INTEGER, "
           "hI INTEGER, "
           "hP INTEGER, "
           "hK INTEGER, "
           "hL INTEGER, "
           "pa INTEGER, "
           "pm INTEGER, "
           "pu INTEGER, "
           "ss INTEGER, "
           "sp INTEGER, "
           "sd INTEGER);"
           );
   sl_exec("CREATE INDEX IF NOT EXISTS idx_uid ON traenke(uid);");
   sl_exec("CREATE INDEX IF NOT EXISTS idx_receipe ON traenke(receipe);");
   sl_exec("CREATE TABLE IF NOT EXISTS rohdaten(id INTEGER PRIMARY KEY, "
           "uid TEXT NOT NULL, rnd INTEGER, "
           "time INTEGER DEFAULT CURRENT_TIMESTAMP, "
           "receipe TEXT NOT NULL, "
           "quality TEXT NOT NULL, "
           "car INTEGER, "
           "da INTEGER, "
           "dm INTEGER, "
           "du INTEGER, "
           "dn INTEGER, "
           "flt INTEGER, "
           "fro INTEGER, "
           "hI INTEGER, "
           "hP INTEGER, "
           "hK INTEGER, "
           "hL INTEGER, "
           "pa INTEGER, "
           "pm INTEGER, "
           "pu INTEGER, "
           "ss INTEGER, "
           "sp INTEGER, "
           "sd INTEGER);"
           );
   sl_exec("CREATE INDEX IF NOT EXISTS idx_uid_r ON rohdaten(uid);");
}

public string QueryPlantFile(int id)
{
  if (member(krautprops, id))
    return build_plantname(krautprops[id]);
  return 0;
}

// AN: Funktion ermittelt, ob ein Spieler pl das Kraut mit der ID id
// verwenden kann. Laut Kommentar unten muss man dafuer wohl was ueber das
// Kraut gelernt haben, von wem ist mir gerade noch nicht klar.
// AN/TODO: Es ist bisher keine Funktionalitaet vorhanden, um die IDs fuer
// den Spieler freizuschalten. Die Funktionsweise muss aus dem Beispielcode
// unten fuer das Learn-On-Demand abgelesen werden.
nomask int CanUseIngredient(object pl, int id)
{
  // Ich mach mal den harten Weg. -- Humni
  return 1;
  // Bitstring des Spielers aus der Liste auslesen.
   string str=player[getuid(pl)];
   if (!stringp(str)) str="";
   if (test_bit(str, id))
      return 1; // make the common case fast
   else {
     // letztenendes habe ich mich entschlossen einzubauen, das man nur die
     // Kraeuter verwenden kann, ueber die man schonmal etwas gelesen/gelernt
     // hat, aus diesem Grund, ist folgender Block auskommentiert.
     // Dieser Block bedeutet quasi ein "auto learning on demand" d.h.
     // wird ein Kraut verwendet wird geprueft ob fuer diese Gruppe bereits
     // eine ID freigeschaltet wurde - ansonsten wird eine freigeschaltet.
     /*
         // pruefen ob fuer den Key bereits ein Bit gesetzt ist, ggf. setzen
         if (id>=sizeof(id2key)) return 0; // illegale Id
         int idlist=key2id[id2key[id]];
         int i;
         for (i=sizeof(idlist)-1; i>=0; i--) {
           if (test_bit(str, idlist[i])) return 0; // Key bereits vorhanden
         }
         // Zufaellig ein Bit fuer den Key setzen
         i=random(sizeof(idlist));
         player[getuid(pl)]=set_bit(str, idlist[i]);
         save_object(object_name());
         return (i==id);
     */
     return 0;
   }
}

// Diese Funktion wird vom Metzelorakel aufgerufen, um die Belohnungen zu
// erzeugen, die man dort fuer erfolgreich absolvierte Metzelauftraege erhaelt
#define ALLOWED_CALLER ({ "/d/ebene/arathorn/orakel/secure/schamane" })
object get_plant_by_category(int npc_class)
{
  if ( member(ALLOWED_CALLER, load_name(previous_object()))<0 )
    raise_error("unauthorised call to get_plant_by_category()\n");

  // Uebergebene NPC-Klasse in Kraut-Kategorie umsetzen.
  // Kategorie 7 wird als 7-9 interpretiert (siehe unten).
  int category;
  switch(npc_class) {
    case 1: case 2:         category=4; break;
    case 3: case 4:         category=5; break;
    case 5: case 6: case 7: category=6; break;
    default:                category=7; break;
  }

  // Alle Kraeuter der ermittelten Kategorie raussuchen. Bei Kategorie 7
  // werden auch alle aus 8 und 9 dazugenommen.
  int *eligible_plant_ids=({});
  foreach( string grp, mapping grpdata : ingredients ) {
    if ( category == 7 && grpdata[T_ABUNDANCE]>=7 || 
          category == grpdata[T_ABUNDANCE] )
      eligible_plant_ids += key2id[grp];
  }

  // Krautnamen zu den Kraut-IDs ermitteln.
  string *plantfiles=map(eligible_plant_ids, function string (int plantid) {
    return build_plantname(krautprops[plantid]);});

  // Ein Kraut zufaellig auswaehlen, clonen und das Objekt zurueckgeben.
  object plant=clone_object(PLANTDIR+plantfiles[random(sizeof(plantfiles))]);
  plant->DryPlant(80+random(11));
  // Aufschreiben, wer welches Kraut mit welcher Qualitaet rausbekommt.
  log_file("ARCH/plant_by_category",
    sprintf("%s %-12s %-s Qual %d Kat %d Class %d\n", 
      strftime("%x %X",time()), getuid(PL),
      object_name(plant)[sizeof(PLANTDIR)..],
      plant->QueryProp(P_QUALITY), category, npc_class));
// sprintf("%24s: call from %O, player: %s (PL: %O), kategory: %d\n", ctime(),
// previous_object(), getuid(player), PL, kategory))
  return plant;
}

private nosave object simul_efun, plantvc;

// fuer SIMUL_EFUN_FILE
#include <config.h>

// AN/TODO: Klaeren, warum hier eine eigene Funktion get_cloner() existiert,
// wo doch der Kraeuter-VC schon eine hat.
nomask private string get_cloner()
{
   int i;
   object po;
   if (!simul_efun) {
      if (!(simul_efun=find_object(SIMUL_EFUN_FILE)))
         simul_efun=find_object(SPARE_SIMUL_EFUN_FILE);
   }
   // wenn sie jetzt nicht existiert - auch gut, dann gibt es halt keine
   // sefuns.

   if (!plantvc) plantvc=find_object(KRAEUTERVC);
   
   for (i=0; po=previous_object(i); i++) {
      if (po==master() || po==simul_efun || po==ME ||
          po==previous_object() || po==plantvc)
         continue;
      else return object_name(po);
   }
   return 0;
}

// AN: 
nomask string CheckPlant(int id)
{
   if (id<=0 || id>=sizeof(id2key)) return 0;
   if (!stringp(id2key[id])) return 0;
   return get_cloner();
}

// ueber diese Funktion kann die Liste der Id's updatet werden
// es wird <filename> eingelesen und durchgeparst.
// Diese Datei muss in jeder Zeile folgendes Format einhalten und darf keine
// leerzeilen enthalten! > Id,key,Gender,Name,Adjektiv
// AN: Das ist Quatsch. Das Format muss so aussehen:
// String-ID;Kraut-ID;demon;gender;P_NAME;adjektiv;P_LONG;roomdetail;
// HU: Diese Funktion lass ich so. Harhar.
// Update: Na gut. Fast.
nomask private int LoadIdList(string filename)
{
   int i, id, si, demon;
   string *lines, file;
   mixed *data;
   file=read_file(filename);
   if (!stringp(file)) raise_error("Missing File: "+filename+"\n");
   lines=explode(read_file(filename), "\n");
   si=sizeof(lines)-1;
   // AN/TODO: Warum verwendet man hier nicht auch einfach explode()?
   // Wenn "##ende##" als Trennzeichen enthalten ist, liefert explode()
   // als letzten Eintrag einen Leerstring, darauf kann man pruefen.
   // Allerdings muesste das vor dem explode() zur Zeilentrennung passieren.
   for ( ;si>=0; si--) 
   {
     string lili=lines[si];
     if (strstr(lines[si],"##ende##")>=0) break;
   }
   si--;
   if (si<0) raise_error("##ende## not found in id-list.\n");
   id2key=allocate(si+2);
   krautprops=allocate(si+2);
   // AN: Fuer das Auslesen der Werte aus dem Array "data" muessen die 
   // Indizierungs-Defines jeweils +1 verwendet werden, weil im Datenfile
   // schlauerweise ein Datenfeld *vorne* angefuegt wurde.
   // AN/TODO: vielleicht sollte man besser Element 0 zuerst auslesen und
   // dann das Array arr um ein Element kuerzen: arr = arr[1..];
   for (i=si; i>=0; i--) {
      data=explode(lines[i], ";");
      if (sizeof(data)!=8) raise_error("Wrong id-list format in line "+(i+1)+"\n");
      id=to_int(data[PROP_ID+1]);
      data[PROP_ID+1]=id;
      if (id>si+1) raise_error(sprintf("Line %d: id %d greater than array size %d.\n", i, id, si));
      id2key[id]=data[0];
      // AN: Ich sehe noch nicht so ganz, warum man von dem Datenfeld
      // PROP_DEMON nur das letzte Zeichen als Literal pruefen sollte.
      // Komplett geht doch genausogut?
      switch(data[PROP_DEMON+1][<1]) {
        case 'R': data[PROP_DEMON+1]=RAW; break;
        case '0': data[PROP_DEMON+1]=0;   break;
        case '1': data[PROP_DEMON+1]=1;   break;
        case '2': data[PROP_DEMON+1]=2;   break;
        default: raise_error("Unknown demonformat '"+data[PROP_DEMON+1]+
                   "' in idlist line "+(i+1)+"\n");
      }
      switch(data[PROP_GENDER+1][<1]) {
        case 'N': data[PROP_GENDER+1]=NEUTER; break;
        case 'F': data[PROP_GENDER+1]=FEMALE; break;
        case 'M': data[PROP_GENDER+1]=MALE;   break;
        default: raise_error("Unknown genderformat '"+data[PROP_GENDER+1]+
                   "' in idlist line "+(i+1)+"\n");
      }
      SetProp(P_GENDER, data[PROP_GENDER+1]);
      SetProp(P_NAME, data[PROP_NAME+1]);
      // AN/TODO: data[PROP_ADJ] muss man nicht unbedingt vorher auf 0 setzen
      if (!sizeof(data[PROP_ADJ+1])) data[PROP_ADJ+1]=0;
      if (data[PROP_ADJ+1])
         SetProp(P_NAME_ADJ, ({"ganz normal", data[PROP_ADJ+1]}));
      else SetProp(P_NAME_ADJ, "ganz normal");
      SetProp(P_ARTICLE, data[PROP_DEMON+1]!=RAW);
      demon=(data[PROP_DEMON+1]==RAW ? 0 : data[PROP_DEMON+1]);
      // AN: Wenn keine Langbeschreibung hinterlegt wurde, wird der Name
      // des Krautes als solche verwendet. Ebenso fuer das Raumdetail, das
      // beim Betrachten des Krautes im Raum ausgegeben wird.
      if (!sizeof(data[PROP_LONG+1])) {
         data[PROP_LONG+1] = Name(WER, demon)+".\n";
      }
      else data[PROP_LONG+1] = BS(data[PROP_LONG+1]);
// Humni: Offenbar kommen am Zeilenende manchmal Zeichen dazu. Ich gehe davon
// aus, dass keine Beschreibung kuerzer als 2 Zeichen ist.
      if (sizeof(data[PROP_ROOMDETAIL+1])<2) {
         data[PROP_ROOMDETAIL+1] = Name(WER, demon)+".\n";
      }
      else data[PROP_ROOMDETAIL+1] = BS(data[PROP_ROOMDETAIL+1]);
      krautprops[id]=data[1..];
   }
   SetProp(P_NAME, 0);
   SetProp(P_NAME_ADJ, "");
   
   // key2id-Cache neu aufbauen.
   key2id=([]);
   for (i=sizeof(id2key)-1; i>=0; i--) {
      if (member(key2id, id2key[i]))
         key2id[id2key[i]]+=({ i });
      else key2id[id2key[i]]=({ i });
   }
   return 1;
}

// Hilfsfunktion wird zum einparsen benötigt
// wandelt z.B. den string von "h 500" in ({ 3, 500 }) um
private nomask <string|int>* buildAttrArr(string attr)
{
   if (!attr) return 0;
   attr=trim(attr);
   <string|int>* res=explode(attr, " ")-({""});
   //debug(sprintf(" build Attr %O",res));
   if (sizeof(res)!=2) raise_error("Wrong attrib format!\n");
   //debug(sprintf("%O",T_KRAUT_MAP));
   return ({T_KRAUT_MAP[res[0]],(int)res[1]});
}

// AN: Hilfsfunktion zum Einlesen der Traenkeliste.
nomask private void LoadIndex(string filename)
{
   int i, j, si;
   string *lines, file;
   mixed  *data;

   file=read_file(filename);
   if (!stringp(file)) raise_error("Missing File: "+filename+"\n");
   
   // AN/TODO: Auch hier waere wieder die Verwendung von explode() zu
   // erwaegen.
   lines=explode(file, "\n")-({""});
   si=sizeof(lines);
   for (i=0; i<si; i++) if (lines[i]=="##start##") break;
   i++;
   if (i>=si) raise_error("missing ##start## in Index.\n");
   ingredients=([]);
   for (; i<si; i++) { // alle zeilen durchlaufen...
     // AN/TODO: Tabulatoren als Trennzeichen finde ich irgendwie bloed.
     // HU: Darum nun Semikolons
     // debug("Zeile "+lines[i]);
     data=old_explode(lines[i], ";"); // an Semikolons trennen...
     // debug(sprintf("%O",data));
     if (sizeof(data)!=9) 
       {
         //debug(sprintf("%O",data));
         raise_error("Wrong indexlist format in line "+(i+1)+"\n");
       }
     for (j=8; j>=0; j--) {
       // AN/TODO: if(data[j]=="" || data[j]=="-") data[j]=0;
       // es sind ohnehin ueberall Strings enthalten.
       // Wir machen aus "-" oder "0" eine echte int-Null.
       if (sizeof(data[j])==0 || data[j][0]=='-') data[j]=0;
     }
     // HU: Ab hier bau ich mal neu. Den Rest pack ich auskommentiert darunter, wenn jemand den alten Code
     // nachschauen will.
     // Ich will ein Mapping von dieser Kraeutergruppe.
     mapping mk=([]);
     // Dieses Mapping soll die Eintraege nun enthalten.
     mk[T_EFFECT_DURATION]=to_int(data[5]);
     mk[T_ABUNDANCE]=to_int(data[8]);
     // positive Effekte aufteilen
     //debug(sprintf("Vorposis %O - %O",mk,data));
     if (stringp(data[1]))
       {
         string* posis=explode(data[1],",");
         //debug(sprintf("%O - %O",mk,posis));
         foreach (string q:posis) {
           //debug(q);
           <string|int>* arr=buildAttrArr(q);
           //debug(sprintf("%O",arr));
           mk[arr[0]]=mk[arr[0]]+arr[1];
         }
       }
     //debug(sprintf("%O",mk));
     // Erster Negativer Effekt
     if (data[2]!=0)
       {
         <string|int>* arr=buildAttrArr(data[2]);
         mk[arr[0]]=mk[arr[0]]-arr[1];
       }
     //debug(sprintf("vorneg %O",mk));
     // Zeiter negativer Effekt
     if (data[3]!=0)
       {
         <string|int>* arr=buildAttrArr(data[3]);
         mk[arr[0]]=mk[arr[0]]-arr[1];
       }
     // Haltbarkeit wird umgerechnet
     string* sti=explode(data[4]," ")-({""});
     //debug("haltbar "+sti[0]);
     string stt=trim(sti[0]);
     int dur;
     if (stt[<1..]=="d") // Tage
       {
         //debug("Tage");
         // Der erste Teil ist die Dauer in Tagen.
         dur=to_int(sti[0][..<2]);
         dur=dur*24*60*60; // Sekunden
         mk[T_EXPIRE]=dur;
       }
     if (stt[<1..]=="h") // Stunden
       {
         //debug("Stunden");
         // Sonst ist es halt die Dauer in Stunden.
         dur=to_int(sti[0][..<2]);
         dur=dur*60*60;
         mk[T_EXPIRE]=dur;
       }
     //debug("ergibt "+dur);
     // Nun die lustigen Unterstuetzungen. Dazu benutzen wir unseren lustigen Glueckshasen.
     // Und ein Reh.
     string* glueckshase;
     string reh;
     // Alle Leerzeichen raus!
     //debug("Rehe");
     //debug(sprintf("Data: %O",data));
     if (stringp(data[6]))
       {
         reh=(explode(data[6]," ")-({""}))[0];
     
         glueckshase=explode(reh,",")-({});
         mk[T_SUPPORTER]=glueckshase;
       }
     else
       {
         mk[T_SUPPORTER]=0;
       }
     // Nun machen wir genauso die Blockaden.
     // Das tolle ist: Reh und Glueckshase koennen wir wiederverwenden! Das freut.
     if (stringp(data[7]))
       {
         reh=(explode(data[7]," ")-({""}))[0];
         glueckshase=explode(reh,",")-({});
         mk[T_BLOCKING]=glueckshase;
       }
     else
       {
         mk[T_BLOCKING]=0;
       }
     ingredients[trim(data[0])]=mk;
   }
   //debug("Wuff");
   //debug(sprintf("%O",ingredients));
}

nomask private void save()
{
  save_object(__DIR__"/ARCH/krautmaster");
}

// AN: erzeugt aus Namen + Adjektiv der Pflanzendaten den Objektnamen,
// z.B. waldrebe_gemein oder ackerklee_gelb, wobei Bindestriche auch
// durch Unterstriche ersetzt werden (acker_rettich).
string build_plantname(mixed *props)
{
   string key;
   // AN/TODO: erst PROP_NAME in key schreiben, dann ggf. PROP_ADJ dazu
   if (sizeof(props[PROP_ADJ])>0)
      key=lowerstring(props[PROP_NAME]+"_"+props[PROP_ADJ]);
   else key=lowerstring(props[PROP_NAME]);
   // AN/TODO: ersetzen durch regreplace();
   key=implode(old_explode(key, " "), "_");
   key=implode(old_explode(key, "-"), "_");
   return key;
}

public void UpdateVC()
{
  KRAEUTERVC->update(map_ldfied);
}

// AN: Daten neu parsen
// Nach dem Schreiben des Savefiles mittels save() wird auch das
// Kraeuter-Headerfile vom Kraeuter-VC neu geschrieben.
int _refresh()
{
   int i;
   string key;
   if (extern_call() && !allowed())
     return 0;
   
   LoadIdList(__DIR__"ARCH/kraeuterliste.dump");
   LoadIndex(__DIR__"ARCH/kraeuterindex.dump");
   map_ldfied=([]);
   for (i=sizeof(krautprops)-1; i>=0; i--)
   {
      if (sizeof(krautprops[i])<=PROP_ROOMDETAIL) continue;
      key = build_plantname(krautprops[i]);
      map_ldfied[key]=({ krautprops[i], rooms[key]||([]) });
   }
   save();
   UpdateVC();
   
   // Update Headerfile mit Kraeuterliste
   string *keys = sort_array(m_indices(map_ldfied), #'<);
   string headerfile =
     "// Automatisch generiertes File, nicht von Hand editieren!\n"
     "// Erzeugendes File: "+object_name()+"\n\n"
     "#define PLANTCOUNT "+to_string(sizeof(keys))+"\n\n"
     +"#define PLANT(x) \"/items/kraeuter/\"+x\n\n";
   foreach(key: keys)
   {
     headerfile += sprintf("#define %-30s PLANT(\"%s\")\n",
                     upperstring(key), key);
   }
   write_file(KRAEUTERLISTE, headerfile, 1);

   write("Inputfiles parsed. Save & Headerfiles updated!\n");
   return 1;
}

int _cloneplant(string str)
{
  if (allowed())
  {
    if (to_string(to_int(str)) == str)
      str = QueryPlantFile(to_int(str));
    clone_object(PLANTDIR+str)->move(this_player(), M_NOCHECK);
    write("Kraut " + str + " geclont.\n");
    return 1;
  }
  return 0;
}

#define MAX_ROOMS 10  /* kein Kraut ist in mehr als 10 Raeumen */
// AN: Ausgabe der Kategorienliste ueber das Planttool.
int _showplant(string str)
{
   int i, si, kat, secure;
   string *list, key;
   mixed *res, *arr;
   mapping props;

   secure=allowed();
   notify_fail("Es gibt folgende Kraeuterkategorien:\n"
    +" 0 - haeufig und an vielen Stellen im Mud anzufinden\n"
    +" 1 - etwas seltener zu finden, aber immer noch leicht\n"
    +" 2 - an wenigen gut versteckten Stellen in abgelegenen Gebieten\n"
    +" 3 - dito, jedoch muss das Kraut durch einen NPC (XP >= 500000) bewacht sein.\n"
    +" 4 - aeusserst selten und XP >= 1 mio\n"
    +" 5 - aeusserst selten und XP >= 2 mio\n"
    +" 6 - aeusserst selten und NPC bringt >= 5  Stupse\n"
    +" 7 - aeusserst selten und NPC bringt >= 10 Stupse\n"
    +" 8 - aeusserst selten und NPC bringt >= 20 Stupse\n"
    +" 9 - aeusserst selten und NPC bringt >= 50 Stupse\n"
    +"\nSyntax: showplant <kategorie>.\n");
   kat=to_int(str);
   if (kat<0 || kat>9) return 0;
   if (to_string(kat)!=str) return 0;
   list=m_indices(map_ldfied);
   // AN: *grummel*
   // res = allocate(MAX_ROOMS, ({}));
   res=map(allocate(MAX_ROOMS), #'allocate); // ({ ({}) ... ({}) })
   for (i=sizeof(list)-1; i>=0; i--) {
      arr=map_ldfied[list[i]];
      if (sizeof(arr)!=2) raise_error("Wrong map_ldfied-Format by "+list[i]+"\n");
      key=id2key[arr[0][PROP_ID]];
      if (!key) raise_error("Missing Key for id "+arr[0][PROP_ID]+"\n");
      props=ingredients[key];
      //if (!pointerp(props)) continue; // noch nicht eingetragen
      //if (sizeof(props)!=8)
      // printf("Warning: Wrong ingredient-content by "+key+"\n");
      //debug(sprintf("%O",props));
      if (props==0)
      {
        debug("Falscher Key: "+key);
      }
      else
      {
        if (props[T_ABUNDANCE]==kat)
        {
          si=sizeof(arr[1]);
          if (si<MAX_ROOMS) {
            if (stringp(arr[0][PROP_ADJ])) {
              SetProp(P_ARTICLE, 0);
              SetProp(P_NAME, arr[0][PROP_NAME]);
              SetProp(P_NAME_ADJ, arr[0][PROP_ADJ]);
              SetProp(P_GENDER, arr[0][PROP_GENDER]);
              key=Name(WER);
            }
            else key=arr[0][PROP_NAME];
            if (secure)
              res[si]+=({ sprintf("%3d %-40s: %d\n", arr[0][PROP_ID], key, si) });
            else res[si]+=({ sprintf("%-40s: %d\n", key, si) });
          }
        }
      }
   }
   for (i=0; i<MAX_ROOMS; i++) {
      sort_array(res[i], #'>);
      filter(res[i], #'write);
   }
   return 1;
}

// AN: Ausgabe der Raeume, in denen die Kraeuter zu finden sind.
// angesteuert ueber das Planttool.
int _showrooms(string str)
{
   int i, j, id;
   string *list, dummy;
   mixed *arr;
   if (!allowed()) return 0;
   notify_fail("Syntax: showrooms <id> oder showrooms all\n");
   if (str!="all") {
     id=to_int(str);
     if (to_string(id)!=str) return 0;
   }
   else id=-1;
   list=m_indices(map_ldfied);
   for (i=sizeof(list)-1; i>=0; i--) {
      arr=map_ldfied[list[i]];
      if (sizeof(arr)!=2) raise_error("Wrong map_ldfied-Format by "+list[i]+"\n");
      if (arr[0][PROP_ID]==id || id<0) {
         if (!sizeof(m_indices(arr[1]))) {
            if (id>=0) write("Fuer diese Id sind keine Raeume eingetragen.\n");
         }
         else if (id>=0) {
            write("Folgende Raeume sind fuer "+arr[0][PROP_ID]+" eingetragen.\n");
            filter(map(m_indices(arr[1]), #'+, "\n"), #'write);
            return 1;
         }
         else filter(map(m_indices(arr[1]), #'+, ": "+arr[0][PROP_ID]+", "+arr[0][PROP_NAME]+"\n"), #'write);
         if (id>=0) return 1;
      }
   }
   write("Fuer diese Id sind bisher keine Kraeuter eingetragen.\n");
   return 1;
}

// Nutzung der Kraeuter in  Gebieten liefert nur dann gueltige Kraeuter,
// wenn der Raum eingetragen ist.
int _addroom(string str)
{
   int id, i;
   string *list, vc;

   if (!allowed()) {
      write("Fuer das Eintragen der Raeume wende Dich doch bitte "
            "an einen EM.\n");
      return 1;
   }
   notify_fail("Syntax: addroom <krautnummer> <filename>\n");
   str=PL->_unparsed_args();
   if (!str || sscanf(str, "%d %s", id, str)!=2) return 0;
   if (str=="hier" || str=="here")
   {
     if (!this_player())
     {
       notify_fail("Kein Spielerobjekt, kann "
           "Raum nicht identifizieren.\n");
       return 0;
     }
     str=to_string(environment(this_player()));
   }
   if (str[<2..]==".c") str=str[0..<3]; // .c abschneiden
   if (file_size(str+".c")<=0) {
      list=explode(str, "/");
      vc=implode(list[0..<2], "/")+"/virtual_compiler.c";
      if (file_size(vc)<=0 || !call_other(vc, "Validate", list[<1])) {
         write("No such file \""+str+"\".\n");
         return 1;
      }
   }
   if (id<=0 || id>=sizeof(id2key)) {
      write("Illegal plantid "+id+".\n");
      return 1;
   }
   list=m_indices(map_ldfied);
   for (i=sizeof(list)-1; i>=0; i--) {
      if (map_ldfied[list[i]][0][PROP_ID]==id) {
         if (!member(map_ldfied[list[i]][1], str)) {
            map_ldfied[list[i]][1]+=([ str ]);
            rooms[list[i]]=(rooms[list[i]]||([]))+([ str ]);
            write("Raum Erfolgreich eingetragen!\n");
         }
         else write("Raum bereits eingetragen.\n");
         save();
         _refresh();
         return 1;
      }
   }
   write("Kraut mit id "+id+" nicht gefunden.\n");
   return 1;
}

int _delroom(string str)
{
   int i, done;
   string *list;

   if (!allowed()) {
      write("Fuer das Loeschen von Raeumen wende Dich doch bitte "
            "an einen EM.\n");
      return 1;
   }
   notify_fail("Syntax: delroom <filename>.\n");
   str=PL->_unparsed_args();
   if (!str) return 0;
   if (str[<2..]==".c") str=str[0..<3];
   list=m_indices(map_ldfied); done=0;
   for (i=sizeof(list)-1; i>=0; i--)
   {
      if (member(map_ldfied[list[i]][1], str)) {
         m_delete(map_ldfied[list[i]][1], str);
         m_delete(rooms[list[i]], str);
         write("Raum bei id "+map_ldfied[list[i]][0][PROP_ID]
               +" ausgetragen.\n");
         done=1;
      }
   }
   if (!done) {
      if (file_size(str+".c")<0)
         write("No such file \""+str+"\".\n");
      else write("Fuer "+str+" sind keine Kraeuter eingetragen!\n");
   }
   else {
      save();
      _refresh();
   }
   return 1;
}

// Veranlasst den Kraeuter-VC, eine phys. Datei aus den Kraeuterdaten eines
// Krautes zu erzeugen, falls man diese ausbeschreiben will.
int _createfile(string str)
{
  int id;
  if (!allowed()) {
    write("Diese Funktion wurde fuer Dich nicht freigegeben!\n");
    return 1;
  }
  id=to_int(str);
  if (to_string(id)!=str || id<=0 || id>=sizeof(id2key)) {
    write("Illegal plantid '"+str+"'.\n");
    return 1;
  }
  notify_fail("Unknown Function im kraeuterVC: _createfile()\n");
  return call_other(KRAEUTERVC, "_createfile", build_plantname(krautprops[id]));
}

// AN: Hilfsfunktionen, derzeit ebenfalls deaktiviert.
// i = 0..7, Position des Krautes, fuer das der Aufruf erfolgt, in der 
// Liste der in den Kessel einfuellten Kraeuter.
// keyLst ist die Liste der Kraeutergruppen, der die Kraeuter zugeordnet
// sind.
// An dieser Stelle kann also die Wirkung von Kraeutergruppen abgehaengt
// werden. Unklar ist mir aktuell nur, warum diese Funktion den Parameter
// "i" benoetigen wuerde.
// Idee: Es soll die Entscheidung davon abhaengig gemacht werden koennen,
// wie die Gesamtkombination aussieht, und zusaetzlich davon, aus welcher
// Gruppe das einzelne Kraut stammt.
nomask private int IsBlocked(int i, string *keyLst)
{
  return 0;
}

// AN: Diese Funktion muesste nach dem Code in make_potion() zu urteilen
// 0 oder 1 zurueckgeben, dann wird der Eigenschaftswert der Kraeutergruppe
// um den Faktor 1.5 verstaerkt.
nomask private int IsBoosted(int i, string *keyLst)
{
  return 0;
}

#define PRNG "/std/util/rand-glfsr"
// Individuelle Boni/Mali fuer Spieler. ploffset soll via Referenz uebergeben
// werden und wird von der Funktion gesetzt.
int calculate_mod(int krautindex, string plname, int ploffset)
{
  // Startoffset zufaellig ermittelt, aber immer gleich
  // fuer jeden Spielernamen
  PRNG->InitWithUUID(plname);
  ploffset = PRNG->random(16);
  // Jedes Kraut hat auch einen iOffset (der konstant bleibt und sich nicht
  // aendert). Der wird auch addiert. Bei Ueberschreiten von 30 wird nach 0
  // gewrappt.
  // Der Offset ist dann (spieleroffset + krautindex) % 16, d.h. alle Modifikatoren werden
  // der Reihe nach durchlaufen. So kriegt jeder Spieler - fast egal, bei welchem
  // Offset er startet - auch der Reihe nach alle Boni+Mali.
  int offset = ((ploffset + krautindex) % 16) * 2;
  // Am Ende wird das ganze noch nach 85 bis 115 verlegt.
  return offset + 85;
}

#define ZWEITIES "/secure/zweities"

mapping calculate_potion(int* plantids, int* qualities, string familie)
{
  // Man sollte ohne die Kraeuter nicht so einfach Wirkungen beliebig
  // berechnen koennen.
  if (extern_call() && !ARCH_SECURITY)
    return 0;

  // Hier speichern wir unser Ergebnis bzw. unser Zwischenergebnis.
  mapping attrib;
  // Hier speichern wir die Wirkgruppen, die aus den Plants gezogen werden.
  mapping* wirkungen=({});
  // Hier speichern wir gleich schon beim Erzeugen die wichtigsten Blockaden.
  string* unterstuetzungen=({});
  string* blockaden=({});
  int zufall;
  // Die Sortierung nach PlantID ist nur fuer das Tranklog wichtig.
  plantids = sort_array(plantids, #'<=);

  // PASS 1: Pflanzen durch Wirkungen ersetzen, dabei Unterstuetzer
  // und Blocker merken.
  foreach (int id, int qual : mkmapping(plantids,qualities))
    {
      //debug(sprintf("Gehe durch Plant: %d",id));
      string key=id2key[id];
      // Wirkungen dieses Krauts kopieren
      mapping ing=copy(ingredients[key]);
      //debug(sprintf("%O",ing));
      // Zu den Wirkungen noch den Key hinzufuegen.
      ing["key"]=key;
      // Die Qualitaet des Krautes wird noch mit dem spielerindividuellen
      // Modifikator skaliert.
      ing["quality"]=(qual * calculate_mod(id, familie, &zufall)) / 100;
      wirkungen+=({ing});
      if (pointerp(ing[T_SUPPORTER]))
        {
          foreach (string pi:ing[T_SUPPORTER])
            {
              unterstuetzungen+=({pi});
            }
        }
      if (pointerp(ing[T_BLOCKING]))
        {
          foreach (string pi:ing[T_BLOCKING])
            {
              blockaden+=({pi});
            }
        }
      debug(sprintf("Kraut %s ergibt Werte %O.",key,wirkungen));
    }
  // PASS 2: Jetzt die Unterstuetzungen auswerten
  foreach (mapping mar:wirkungen)
    {
      foreach (string pi:unterstuetzungen)
        {
          // Haben wir eine Unterstuetzung?
          if (mar["key"]==pi || ((pi[1]=='*') && (pi[0]==mar["key"][0])))
            {
              debug (sprintf("mar=%O, pi=%O",mar,pi));
              // ALLE ZAHLEN mal 1.5 nehmen. Mir ist klar, dass das nun auch
              // mit irgendwelchen Haeufigkeiten passiert, aber mal ehrlich,
              // das ist zur Berechnung egal.
              foreach (string kk, mixed val : &mar)
                {
                  if (intp(val))
                    {
                      val=15*val/10;
                    }
                }
            }
        }
    }
  // PASS 3: Jetzt die Blockaden auswerten
  foreach (mapping mar:wirkungen)
    {
      foreach (string pi:blockaden)
        {
          // Haben wir eine Blockade?
          if (mar["key"]==pi || ((pi[1]=='*') && (pi[0]==mar["key"][0])))
            {
              debug (sprintf("mar=%O, pi=%O",mar,pi));
              // Hier werden alle Zahlen auf Null gesetzt.
              foreach (string kk, mixed val : &mar)
                {
                  if (intp(val))
                    {
                      val=0;
                    }
                }
            }
        }
    }

  // PASS 3.5: Qualitaet der Kraeuter skalieren.
  foreach (mapping mar:wirkungen)
    {
      foreach (string kk, mixed val : &mar)
        {
          if (intp(val) && kk!="quality")
            {
              val=val*mar["quality"]/100;
            }
        }
    }

  // PASS 4: Nun addieren wir alles auf in das Mapping attrib.
  attrib=([]);
  foreach (mapping mar:wirkungen)
    {
      foreach (string kk:mar)
        {
          if (intp(mar[kk]))
            {
              attrib[kk]=attrib[kk]+mar[kk];
            }
        }
    }

  // Die Wirkungsdauer ist der Durchschnitt der Wirkungsdauern
  attrib[T_EFFECT_DURATION] /= sizeof(plantids);
  debug(sprintf("Duration: %d\n",attrib[T_EFFECT_DURATION]));

  // Die Haltbarkeit des Tranks ist die Haltbarkeit des kleinsten Krautes.
  int dur=10000000;
  foreach (mapping mar:wirkungen)
    {
      if (mar[T_EXPIRE]>0 && dur>mar[T_EXPIRE])
        {
          dur=mar[T_EXPIRE];
        }
    }
  if (dur==10000000)
    {
      dur=0;
    }
  attrib[T_EXPIRE]=dur;
  debug(sprintf("Expire: %d\n",dur));

  int maximum=0;
  // Effekte rausrechnen, die nicht maximal sind
  foreach (string kk, mixed val:attrib)
  {
    if (member(T_KRAUT_EFFECTS,kk)>=0)
    {
      if (val>0 && maximum<val)
      {
        maximum=val;
      }
    }
  }
  // Logeintrag erstellen.
  sl_exec("INSERT INTO rohdaten(uid, rnd, receipe, quality, car, da, dm, du, "
          "dn, flt, fro, hI, hP, hK, hL, pa, pm, pu, ss, sp, sd) "
          "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11, ?12, ?13, ?14, "
          "?15, ?16, ?17, ?18, ?19, ?20, ?21);",
          this_player() ? getuid(this_player()) : "<unknown>",
          zufall,
          implode(map(plantids, #'to_string), ", "),
          implode(map(qualities, #'to_string), ", "),
          attrib[T_CARRY], attrib[T_DAMAGE_ANIMALS],
          attrib[T_DAMAGE_MAGIC], attrib[T_DAMAGE_UNDEAD],
          attrib[T_EFFECT_DURATION], attrib[T_FLEE_TPORT],
          attrib[T_FROG], attrib[T_HEAL_DISEASE],
          attrib[T_HEAL_POISON], attrib[T_HEAL_SP],
          attrib[T_HEAL_LP], attrib[T_PROTECTION_ANIMALS],
          attrib[T_PROTECTION_MAGIC], attrib[T_PROTECTION_UNDEAD],
          attrib[T_SA_SPEED], attrib[T_SA_SPELL_PENETRATION],
          attrib[T_SA_DURATION]);

  // Maximal zwei positive Effekte.
  int cteff=0;
  foreach (string kk, mixed val : &attrib)
  {
      if (member(T_KRAUT_EFFECTS,kk)>=0)
      {
        // Nur die 2 staerksten positiven Wirkungen bleiben ueber (dazu
        // muessen sie wirklich den gleichen Zahlenwert haben, kann bei
        // Heilungen vorkommen, sonst eher unwahrscheinlich).
        if (val>0 && maximum>val)
        {
          val=0;
        }
        // Thresholds. Zu zu grosse Wirkungen haben die Grenze als
        // Auswirkung. Negative Wirkungen, die -T_MINIMUM_THRESHOLD nicht
        // ueberschreiben, fallen weg. Bei den positiven bleibt ja ohnehin nur
        // die staerkste Wirkung ueber, da gibt es erstmal keine
        // Mindestgroesse mehr.
        if (val>T_MAXIMUM_THRESHOLD)
        {
          val=T_MAXIMUM_THRESHOLD;
        }
        if (val < 0 && val > -T_MINIMUM_THRESHOLD)
        {
          val=0;
        }
        if (maximum==val && val>0)
        {
          cteff++;
          // Voellig willkuerlich, was hier getroffen wird. Ob reproduzierbar,
          // vermutlich ja, aber haengt mit der Mappingstruktur zusammen.
          // Harhar.
          if (cteff>2)
          {
            val=0;
          }
        }
      }
  }
  debug(sprintf(" TRANKERGEBNIS: %O",attrib));
  // Logeintrag erstellen.
  sl_exec("INSERT INTO traenke(uid, rnd, receipe, quality, car, da, dm, du, "
          "dn, flt, fro, hI, hP, hK, hL, pa, pm, pu, ss, sp, sd) "
          "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11, ?12, ?13, ?14, "
          "?15, ?16, ?17, ?18, ?19, ?20, ?21);",
          this_player() ? getuid(this_player()) : "<unknown>",
          zufall,
          implode(map(plantids, #'to_string), ", "),
          implode(map(qualities, #'to_string), ", "),
          attrib[T_CARRY], attrib[T_DAMAGE_ANIMALS],
          attrib[T_DAMAGE_MAGIC], attrib[T_DAMAGE_UNDEAD],
          attrib[T_EFFECT_DURATION], attrib[T_FLEE_TPORT],
          attrib[T_FROG], attrib[T_HEAL_DISEASE],
          attrib[T_HEAL_POISON], attrib[T_HEAL_SP],
          attrib[T_HEAL_LP], attrib[T_PROTECTION_ANIMALS],
          attrib[T_PROTECTION_MAGIC], attrib[T_PROTECTION_UNDEAD],
          attrib[T_SA_SPEED], attrib[T_SA_SPELL_PENETRATION],
          attrib[T_SA_DURATION]);

  return attrib;
}

mapping make_potion(object* plants)
{
  // -> mappt nicht-Objekt zu 0, aber 0 ist auch ne gueltige PlantID. Daher
  // müssen zerstoerten Objekte vorher raus.
  // TODO: drauf verlassen, dass nur intakte Objekt enthalten sind?
  if (member(plants, 0) >= 0)
    raise_error(sprintf("make_potion() got invalid object in plant array "
                        "%.50O\n",plants));

  int* plantids = (int*)plants->QueryPlantId();
  int* qualities = (int*)plants->QueryProp(P_QUALITY);

  return calculate_potion(plantids, qualities,
                          ZWEITIES->QueryFamilie(this_player()));
}

// AN: Sucht alle Pflanzen raus, in deren Namen der Suchbegriff "str"
// vorkommt und listet diese auf. Laeuft allerdings momentan noch in einen
// Fehler "index out of bounds", aber man muesste hier (TODO) sowieso mal
// von explode() auf strstr() umbauen, denke ich.
string _findplant(string str) {
  int i, k;
  string *ind, *tmp;

  if(!str) return "";
  write("Suche nach '"+str+"':\n\n");
  ind = m_indices(map_ldfied);
  for(i=0;i<sizeof(ind);i++) {
    tmp = map_ldfied[ind[i]][0];
    if( stringp(tmp[3]) && 
        old_explode(lower_case(tmp[3]),str)[0] != lower_case(tmp[3]) 
        ||
        stringp(tmp[4]) && 
        old_explode(lower_case(tmp[4]),str)[0] != lower_case(tmp[4]))
      write(" -  "+tmp[3]+
          (stringp(tmp[4])?" ("+tmp[4]+")":"")+"       - "+tmp[0]+"\n");
  }

  return "";
}

// AN: Funktion liefert das Ergebnisarray aus make_potion() fuer eine Liste
// von Kraeutern, die als ", "-getrennte Kraut-IDs uebergeben werden muessen.
mixed _checkTrank(string str)
{
  if (extern_call() && !allowed())
    return 0;

  string *ind, *args, name;
  object *objs;
  int k, l;

  objs = ({});
  if(!str) return "Keine Kraeuter uebergeben!";
  ind = old_explode(str,",");
//  ind = ({"180","11","53"});
  for(int i=0;i<sizeof(ind);i++)
  {
    name = build_plantname(krautprops[to_int(ind[i])]);
    write("Input: '"+name+"' ("+ind[i]+")\n");
    objs += ({clone_object(PLANTDIR+name)});
  }
  mapping ragtest = make_potion(objs);
  objs->remove();
/*  name="";
  for(int i=0;i<sizeof(ragtest);i++)
       name = name + ragtest[i]+",";
  write("Result: ({ "+name+" })\n");*/
  return sprintf("%O\n",ragtest);
}

#define QUAL_BASE  0
#define QUAL_RND   1
#define DELAY_BASE 2
#define DELAY_RND  3

#define ALLOWED_DRIER "/items/kraeuter/trockner"

// Liefert die Trocknungsdaten eines Raumes aus, mit denen der Kraeuter-
// trockner dann das Kraut bearbeiten kann.
int *QueryDryingData() {
  // Es muss allerdings das aufrufende Objekt ein Trockner-Clone sein, 
  // der in einem der zugelassenen Raeume stehen muss.
  // Wenn das nicht der Fall ist, wird der Trockner das leere Array, das 
  // zurueckgegeben wird, als Indiz werten, dass er im falschen Raum steht.
  if ( objectp(previous_object()) &&
       load_name(previous_object()) == ALLOWED_DRIER &&
       member(drying_data, load_name(environment(previous_object()))) &&
       clonep(previous_object()) )
  {
    // Raum ermitteln, Delay/Quali errechnen, Ergebnisarray zurueckgeben.
    string where = load_name(environment(previous_object()));
    int delay = drying_data[where,DELAY_BASE]+
                random(drying_data[where,DELAY_RND]);
    int qual  = drying_data[where,QUAL_BASE]+
                random(drying_data[where,QUAL_RND]);
    return ({ delay, qual });
  }
  return ({});
}

// Modifizieren der Trocknungsdaten.
// <room> muss der volle Dateiname des Raumes sein, ohne .c am Ende.
// <values> enthaelt die vier Parameter zu dem Raum in folgender Reihenfolge:
// ({ Quali-Basis, Quali-Zufallsanteil, Delay-Basis, Delay-Zufallsanteil })
// Wenn <values> nicht angeben wird oder 0 ist, werden die Daten zu <room>
// geloescht.
int|mapping SetDryingData(string room, int* values) 
{
  // keine Zugriffsrechte
  if ( !allowed() )
    return -1;
  
  // <values> wurde nicht uebergeben? Dann Daten loeschen.
  if ( !values ) 
  {
    m_delete(drying_data, room);
    return 1;
  }

  // Ansonsten muessen 4 Integer-Werte als <values> uebergeben werden.
  if ( sizeof(values) != 4 ) 
    return -2;

  if ( room[<2..<1] == ".c" )
    room = room[..<3];

  // Uebergebene Daten aendern direkt das Mapping der Trocknungsdaten.
  m_add(drying_data, room, values...);
  save();
  return ([ room : drying_data[room,0]; drying_data[room,1]; 
                   drying_data[room,2]; drying_data[room,3]]);
}

varargs mapping QueryDrying()
{
  return (allowed() ? drying_data : ([]) );
}

varargs int remove(int silent) 
{
  save();
  return ::remove(silent);
}

/*
#define DRYINGDATA "/secure/ARCH/kraeutertrocknungsdaten"

private void ReadDryingData() 
{
  mixed data = explode(read_file(DRYINGDATA), "\n")-({""});
  foreach(string line : data) 
  {
    if ( line[0] == '#' )
      continue;
    string *fields = explode(line,";");
    fields[1..] = map(fields[1..], #'to_int);
    m_add(tmp_drying_data, fields...);
  }
}*/


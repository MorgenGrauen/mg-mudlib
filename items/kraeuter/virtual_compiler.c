// (c) by Padreic (Padreic@mg.mud.de)

#pragma no_inherit,no_clone,strong_types,rtt_checks

#include <defines.h>
#include <properties.h>
#include <v_compiler.h>
#include <items/kraeuter/kraeuter.h>
#include <wizlevels.h>

inherit "/std/virtual/v_compiler";
inherit "/std/thing/language";
inherit "/std/thing/description";

// mit Hilfe dieses mappings kommt man sowohl an die ID und die Eigenschaften,
// als auch an die Liste der Raeume in denen das Kraut mit dem filenamen room
// gefunden werden kann.
// ([ "key": ({ ({eigenschaften}), ([raeume]) }) ])
private mapping krautdaten;

// AN: enthaelt die Liste der gueltigen Kraeuter-Dateinamen ohne .c
// am Ende. Ich vermute, dass es deswegen ein Mapping ist, damit in 
// Validate() einfach member() drauf gemacht werden kann und man nur 0/1
// als Rueckgabewerte pruefen muss, statt -1 bei nem member() auf ein Array.
private mapping validfiles;

public void update(mapping data)
{
  if (previous_object() == find_object(PLANTMASTER))
  {
    krautdaten = data;
    validfiles = mkmapping(m_indices(krautdaten));
  }
}

// Wird benutzt, um kurze IDs von Kraeutern zu raten. Diese IDs werden
// eingetragen, wenn der Krautname die ID als Teilstring enthaelt.
#define IDLIST ({ "klee", "rebe", "hahnenfuss", "rettich", "kraut", "wurz",\
                     "moos", "enzian", "rautenwicke", "pilz", "nelke",\
                     "lichtnelke", "wicke", "zwiebel", "hanf", "kresse"})

void create()
{
   seteuid(getuid());

   v_compiler::create();
   description::create();

   SetProp(P_COMPILER_PATH, __DIR__);
   SetProp(P_STD_OBJECT, PLANTITEM);
   
   PLANTMASTER->UpdateVC();
}

string Validate(string file)
{
   if (!stringp(file)) return 0;
   file = ::Validate(explode(file, "#")[0]);
#if MUDNAME == "MorgenGrauen"
   return (member(validfiles, file) ? file : 0);
#else
   return file;
#endif
}

private nosave object simul_efun;

// fuer SIMUL_EFUN_FILE
#include <config.h>

// AN: Funktion liefert das clonende Objekt als dessen Blueprint-Namen,
// indem es den Caller-Stack durchlaeuft und nach einem Objekt sucht,
// das weder der Master, noch ein Simul-Efun-Objekt, noch dieser VC selbst
// ist. Der Name des gefundenen Objekts wird zurueckgegeben, oder 0.
nomask private string get_cloner()
{
   int i;
   object po;

   if (!simul_efun)
   {
      if (!(simul_efun=find_object(SIMUL_EFUN_FILE)))
         simul_efun=find_object(SPARE_SIMUL_EFUN_FILE);
   }
   // wenn sie jetzt nicht existiert - auch gut, dann gibt es halt keine
   // sefuns.

   for (i=0; po=previous_object(i); i++)
   {
      if (po==master() || po==simul_efun || po==ME || po==previous_object())
         continue;
      return BLUE_NAME(po);
   }
   return 0;
}

// Konfiguriert das erzeugte Objekt entsprechend der dafuer im Kraeutermaster
// bekannten Daten. Vergibt auf die Plant-ID.
varargs string CustomizeObject(string file)
{
   if (previous_object()->QueryPlantId()) return 0; // bereits initialisiert
   
   if (stringp(file))
      file=Validate(file);
   else file=::CustomizeObject();
   if (!file) return 0;

   closure sp=symbol_function("SetProp", previous_object());
   mixed arr=krautdaten[file];
   if (pointerp(arr))
   {
      // Welches Objekt clont das File?
      string cloner = get_cloner();
      mapping rooms = arr[1];
      mixed props = arr[0];
      // Wird das Kraut legal von einem eingetragenen Cloner erzeugt? Nur dann
      // bekommt es eine gueltige Plant-ID.
     int legal=member(rooms, get_cloner()) || cloner==PLANTMASTER;
     if (!legal && this_interactive() && IS_ARCH(this_interactive()))
        legal=1;
     
      // Konfiguriert wird das Objekt dann, wenn es per VC erzeugt wird oder
      // ein Clone einer per VC erzeugten BP ist - d.h. wenn es nicht aus
      // einem real existierenden File auf der Platte existiert. Das ist dann
      // der Fall, wenn der Loadname gleich dem Standardplantobjekt des VC
      // ist.
      if (load_name(previous_object())==PLANTITEM)
      {
        if ((props[INGREDIENT_NAME]=="Klee") ||
            (props[INGREDIENT_NAME][<4..]=="klee")) {
           funcall(sp, P_NAME, ({ props[INGREDIENT_NAME],
                                  props[INGREDIENT_NAME]+"s",
                                  props[INGREDIENT_NAME],
                                  props[INGREDIENT_NAME]}));
        }
        else funcall(sp, P_NAME,     props[INGREDIENT_NAME]);
        funcall(sp, P_NAME_ADJ, props[INGREDIENT_ADJ]);
        funcall(sp, P_GENDER,   props[INGREDIENT_GENDER]);
        funcall(sp, P_LONG,     props[INGREDIENT_LONG]);
        funcall(sp, PLANT_ROOMDETAIL, props[INGREDIENT_ROOMDETAIL]);
        if (props[INGREDIENT_DEMON]==RAW) {
           funcall(sp, P_ARTICLE, 0);
           funcall(sp, P_SHORT, previous_object()->Name(WER));
           funcall(sp, P_ARTICLE, 1);
        }
        else funcall(sp, P_SHORT,
             previous_object()->Name(WER,props[INGREDIENT_DEMON]));
        previous_object()->AddId(lowerstring(props[INGREDIENT_NAME]));
        // bei zusammengesetzten Namen, auch den hauptnamen akzeptieren
        string str=lowerstring(props[INGREDIENT_NAME]);
        string *names=explode(str, "-");
        if (sizeof(names)>1) previous_object()->AddId(names[<1]);
        names=explode(str, " ");
        if (sizeof(names)>1) previous_object()->AddId(names[<1]);
        foreach(string id: IDLIST)
        {
          if (strstr(str, id)==-1) continue;
          previous_object()->AddId(id);
          break;
        }
        // Adjective vorher deklinieren
        str=props[INGREDIENT_ADJ];
        if (stringp(str))
        {
          str=DeclAdj(lowerstring(str), WEN, 0);
          previous_object()->AddAdjective(str);
        }
      }  // Ende Konfiguration eines VC-erzeugten Objekts
      // Plant-ID wird fuer alle Objekte auf irgendwas gesetzt.
      previous_object()->SetPlantId(legal ? props[INGREDIENT_ID] : -1);
   }
   // Keine Krautdaten bekannt...
   else
   {
     funcall(sp, P_NAME,     "Kraut");
     funcall(sp, P_GENDER,   NEUTER);
     funcall(sp, P_SHORT,    "Ein Testkraut ("+capitalize(file)+")");
     funcall(sp, P_LONG,     "Ein nicht naeher spezifiziertes Testkraut.\n");
     funcall(sp, PLANT_ROOMDETAIL,
         "Ein nicht naeher spezifiziertes Testkraut ("
         +capitalize(file)+").\n");
     previous_object()->AddId("kraut");
     previous_object()->SetPlantId(-1);
   }
   return file;
}

int NoParaObjects()
{   return 1;   }

// AN: Funktion erzeugt aus den vorliegenden Daten der Kraeuterliste ein
// physikalisch existierendes File in diesem Verzeichnis, zB wenn die Daten
// erweitert werden sollen. Die Kraeuterliste stellt nur generische Objekte
// zur Verfuegung, die keine Details haben. Wenn die Objekte ausgeschmueckt
// werden sollen, koennen diese auch als Datei hier liegen.
// Wird vom Plantmaster aus gerufen. Die Existenz von Klartext-
// Fehlermeldungen laesst darauf schliessen, dass diese Funktion dafuer
// vorgesehen war, vom Planttool aus gerufen zu werden. Dies wird dadurch
// bestaetigt, dass dort wie hier alle von Magiern benutzbaren Kommando-
// funktionen mit _ beginnen (_showplant(), _addroom() etc.), und die
// Kommandofunktion im Planttool generell in der Lage ist, alle _*() 
// im Plantmaster zu rufen, sofern existent und fuer den Magier freigegeben.
// AN/TODO: ggf. sollte man hier noch pruefen, ob die VC-Blueprint des
// angeforderten Krautes gerade existiert, denn sonst wuerde das auf der
// Platte liegende, scheinbar (nicht) geladene Objekt nicht mit dem
// VC-Objekt uebereinstimmen. Evtl. reicht es aus, die Blueprint einfach
// zu zerstoeren und neuzuladen.
int _createfile(string filename)
{
   int i;
   string str, short, long, gender, *name, roomdetail;
   string *ids;
   string plantfile;

/*   if (object_name(previous_object())!=PLANTMASTER) {
      write("Illegal usage of _createfile()!\n");
      return 1;   
   }*/
// ^^^ Zook, ggf.spaeter wieder Kommentar entfernen. 

   mixed arr;
   if (!pointerp(arr=krautdaten[filename])) {
      write("Unknown plant '"+filename+"'.\n");
      return 1;
   }
   if (file_size(PLANTDIR+filename+".c")>=0) {
      write("error: file "+PLANTDIR+filename+".c already exists.\n");
      return 1;
   }
   mixed props = arr[0];

   // Kurzbeschreibung erzeugen
   SetProp(P_NAME,     props[INGREDIENT_NAME]);
   SetProp(P_NAME_ADJ, props[INGREDIENT_ADJ]);
   SetProp(P_GENDER,   props[INGREDIENT_GENDER]);
   if (props[INGREDIENT_DEMON]==RAW) {
       SetProp(P_ARTICLE, 0);
       short=Name(WER);
       SetProp(P_ARTICLE, 1);
   }
   else short=Name(WER,props[INGREDIENT_DEMON]);
   ids = ({ lowerstring(props[INGREDIENT_NAME]) });
   // bei zusammengesetzten Namen, auch den hauptnamen akzeptieren
   str=lowerstring(props[INGREDIENT_NAME]);
   name=explode(str, "-");
   if (sizeof(name)>1) ids += ({ name[<1] });
   name=explode(str, " ");
   if (sizeof(name)>1) ids += ({ name[<1] });
   for (i=sizeof(IDLIST)-1; i>=0; i--) {
       if (strstr(str, IDLIST[i], 0)==-1) continue;
       ids += ({ IDLIST[i] });
       break;
   }
   switch(props[INGREDIENT_GENDER]) {
     case MALE:   gender="MALE"; break;
     case FEMALE: gender="FEMALE"; break;
     case NEUTER: gender="NEUTER"; break;
     default: gender=props[INGREDIENT_GENDER];
   }
   long="    \""+implode(old_explode(props[INGREDIENT_LONG], "\n"), 
                   "\\n\"\n   +\"")+"\\n\"";
   roomdetail="    \""+implode(
      old_explode(props[INGREDIENT_ROOMDETAIL], "\n"), "\\n\"\n   +\"")+
      "\\n\"";
   plantfile=
    "#pragma strong_types,rtt_checks\n\n"
    "#include <properties.h>\n"
    "#include <items/kraeuter/kraueter.h>\n"
    "#include <items/kraeuter/kraeuterliste.h>\n\n"
    "inherit STDPLANT;\n\n"
    "protected void create()\n"
    "{\n"
    "  ::create();\n";
   plantfile+="  customizeMe("+upperstring(filename)+");\n";
   plantfile+=
    "  SetProp(P_NAME,     \""+props[INGREDIENT_NAME]+"\");\n"
    "  SetProp(P_NAME_ADJ, \""+(props[INGREDIENT_ADJ]||"")+"\");\n"
    "  SetProp(P_GENDER,   "+gender+");\n"
    "  SetProp(P_LONG,     \n"+
    long+");\n"
    "  SetProp(PLANT_ROOMDETAIL, \n"+
    roomdetail+");\n"
    "  SetProp(P_SHORT,    \""+short+"\");\n";
   plantfile+="  AddId(({";
   for (i=sizeof(ids)-1; i>=0; i--)
     plantfile+=" \""+ids[i]+"\",";
   plantfile[<1]=' ';
   plantfile+="}));\n";
   // Adjective vorher deklinieren
   if (stringp(short=props[INGREDIENT_ADJ])) {
     short=DeclAdj(lowerstring(short), WEN, 0)[0..<2];
     plantfile+="  AddAdjective(\""+short+"\");\n";
   }
   plantfile+="}\n";
   write(plantfile);
   //write_file(PLANTDIR+filename+".c", plantfile);
   write("Filename: "+PLANTDIR+filename+".c\n");
   return 1;
}


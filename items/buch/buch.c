/*
  17.04.2007 Ennox angepasst fuer Blinde, automatisches filtern oder eigenes
  Verzeichnis fuer blindenfreundliche Version einzelner Seiten per 
  buchdir_noascii
 */
inherit "std/thing";

#include <properties.h>
#include <language.h>

#define TP this_player()
#define TPN this_player()->name()

int offen = 0;
int seite = 0;

int getseitzahl();

void create() {
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Buch");
  SetProp(P_LONG,
	"Du haeltst ein leinengebundenes Buch in Deinen Haenden. Der Titel heisst:\n"+
	"@@buchinfo@@.\n");
  SetProp(P_NAME,"Buch");
  AddId("buch");
  SetProp(P_GENDER, NEUTER);
  SetProp(P_WEIGHT,80);
  SetProp(P_NOBUY, 1);
  AddCmd("lies|lese&@ID", "lies", "Was willst Du lesen ? Syntax: LIES <ETWAS>.\n");
  AddCmd("oeffne&@ID","oeffne","Was willst Du oeffnen?");
  AddCmd("schliesse|schliess@ID","schliesse","Was willst Du schliessen?");
  AddCmd("blaettere","blaettere");
}

static int oeffne() {
  if (offen) 
    write("Es ist schon geoeffnet.\n");
  else {
    offen=1;
    seite=1;
    write("Du oeffnest das Buch auf Seite 1.\n");
    say(TPN + " oeffnet ein Buch.\n",TP);
  }
  return 1;
}

static int schliesse() {
  if (!offen) 
    write("Es ist schon geschlossen.\n");
  else {
    offen=0;
    seite=0;
    write("Du schliesst das Buch wieder.\n");
    say(TPN + " schliesst ein Buch wieder.\n",TP);
  }
  return 1;
}

static int blaettere(string str) {
  if (!offen || !stringp(str)) return 0;
  int zu_seite;
  if (sscanf(str,"zu seite %d",zu_seite)!=1) return 0;
  int seitenzahl = getseitzahl();
  if (zu_seite < 1 || zu_seite > seitenzahl) return 0;
  write("Du blaetterst zu Seite "+zu_seite+".\n");
  say(TPN + "blaettert in einem Buch.\n");
  seite=zu_seite;
  return 1;
}


string buchinfo() {
  return "'Standardbuch oder Wie man ein standardisiertes Buch schreibt'";
}

int getseitzahl() {
  return 3;
}

string buchdir() {
  return "/items/buch/";
}
// Zeigt gewuenschte Seite an, mit clear werden grafische
// Seiten aufgespalten und ohne Rahmen gezeigt

void zeige_seite(string file, status clear) {
  if (!clear) {
    TP->more(file,1);
    return;
  }
  string text=read_file(file);
  if (!stringp(text)) return;
  
  string* pages=({"",""});
  string* lines=explode(text,"\n");
  int len, count=sizeof(lines)-1;
  string line;

  for (int i=1;i<count;i++) {
    line=lines[i];
    len=sizeof(line);
    if (len>50) {
      pages[0]+=line[1..len/2-2]+"\n";
      pages[1]+=line[len/2+2..<2]+"\n";
    }
    else
      pages[0]+=line[1..<2]+"\n";
  }
  TP->More((pages[0]+pages[1]));
}

// Schaut ob es die Seiten-Datei mit dir/seite01 oder dir/seite1 findet
string find_file(string dir, string file, int page) {
  string filename=dir+file+(page? (page<10 ? "0"+page : page ) : "");
  if (file_size(filename)>=0) return filename;
  filename=dir+file+(page? page : "");
  if (file_size(filename)>=0) return filename;
  return 0;
}

// wenn keine geradzahlige seite vorhanden, dann wird ungerade angezeigt
string find_page(string dir, string file, int page) {
  string filename=find_file(dir,file,page);
  // eigentlich nur ungerade seiten gefordert
  if (!filename && seite%2==0) {
    return find_file(dir,file,page+1);
  }
  return filename;
}

// wenn _noascii Verzeichnis wird zuerst dort gesucht, wenn flag
// noascii gesetzt ist, da die files dann aufbereitet sind, wird das
// flag geloescht
string find_page_noascii(string dir, string file, int page, status noascii) {
  if (noascii) {
    string noascii_dir=dir[..<2]+"_noascii/";
    if (file_size(noascii_dir)==-2) {
      string filename=find_page(noascii_dir,file,page);
      if (stringp(filename)) {
	noascii=0;
	return filename;
      }
    }
  }
  return find_page(dir,file,page);
}

void lesseite(int seite) {
  string dir=buchdir();
  status noascii=0;
  if (TP->QueryProp(P_NO_ASCII_ART)) {
    noascii=1;
  }
  string buch_file=find_page_noascii(dir,seite ? "seite" : "titel",seite,&noascii);
  if (!buch_file) {
    write("Buchdaten nicht gefunden!\n");
    return;
  }
  zeige_seite(buch_file,noascii);
}
void lesbuch() {
  lesseite(seite);
  seite+=2;
  if (seite > getseitzahl()) {
    offen=0;
    seite=0;
    write("Du hast das Buch ausgelesen und schliesst es.\n");
    say(TPN + " hat ein Buch ausgelesen und schliesst es.\n",TP);
  } else {
    write("Du blaetterst um.\n");
    say(TPN + " blaettert um.\n",TP);
  }
}

void titel() {
  lesseite(0);
}

static int lies(string str) {
  string was;
  notify_fail("Zu welcher Seite willst Du blaettern?\n");
  if(!str) return 0;
  if(sscanf(str,"%s",was)!=1) return 0;
  if (!id(str)) return 0;
  say(TPN + " liest in einem Buch.\n",TP);
  if (offen) {
    lesbuch();
  } else {
    titel();
  }
  return 1;
}

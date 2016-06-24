// da dieses Objekt in /secure liegt und ROOTID inne hat, muessen besondere
// Sicherheitsvorkehrungen getroffen werden. Insbesondere duerfen keine Files
// aus /std/ inheritet werden, da es fuer diese Files keiner ROOTID bedarf.

#include <properties.h>
#include <defines.h>
#include "/secure/wizlevels.h"

static string in_use;

string thema;
string *moeglichkeiten;
string *ergebnis;

#undef SAY

private void SAY( string str )
{
  write ( str );
  write_file("/log/WAHL", str);
}

void create()
{
  if (clonep(ME)) destruct(ME);
  if (ergebnis) return;
  seteuid(getuid());
  in_use=0;
  ergebnis=({({}),({})});
  moeglichkeiten=({});
  restore_object("wahl/wahl");
  move_object(ME, "/gilden/abenteurer");
}

varargs string name(int casus, int demon)
{
  if (demon==RAW) return "Wahlmaschine";
  switch(casus) {
    case WEM:
    case WESSEN:
      if (demon==1)
        return "der Wahlmaschine";
      return "einer Wahlmaschine";
    default:
      if (demon==1)
        return "die Wahlmaschine";
      return "eine Wahlmaschine";
  }
}

varargs string Name(int casus, int demon)
{   return capitalize(name(casus, demon));  }


public string QueryPronoun(int casus)
{
  switch(casus)
  {
    case WER:
      return "sie";
      break;
    case WEM: 
      return "ihr";
  }
  return "sie";
}                                                                                                                   

string short()
{  return "Eine Wahlmaschine.\n";  }

string long()
{
  return "Eine Wahlmaschine. Mit ihr koennen geheime Wahlen durchgefuehrt werden.\n"+
    "Aktuelles Thema: "+(stringp(thema) ? thema : "keines")+"\n";
}

mixed Query(mixed param)
{
   if (!stringp(param)) return 0;
   switch (param) {
     case P_NOGET:   return 1;
     case P_GENDER:  return FEMALE;
     case P_NAME:    return "Wahlmaschine";
   }
   return 0;
}

mixed QueryProp(mixed param)
{  return Query(param);  }

int Set(mixed param)
{  return -1;  }

int SetProp(mixed param)
{  return -1;  }

varargs int id(string str, int lvl)
{  return (member(({"maschine", "waehler", "wahlmaschine"}), str)!=-1);  }

// ab hier kommt der eigentliche Code der Wahlmaschine

void init()
{
  add_action("waehle", "waehl",1);
  add_action("auswertung","auswertung");
  add_action("wahlthema", "wahlthema");
  add_action("moeglichkeit","moeglichkeit");
}

void save_me()
{  save_object("wahl/wahl");  }

static int waehle()
{
  int i;
  mixed second;

  if (!RPL) return 0;
  if (RPL->QueryProp(P_LEVEL)<10) {
    notify_fail("Man kann erst ab Stufe 10 mitwaehlen :(\n");
    return 0;
  }
  if (RPL->QueryProp(P_TESTPLAYER)) {
    notify_fail("Testspieler sind von der Wahl ausgeschlossen.\n");
    return 0;
  }
  if ((second=RPL->QueryProp(P_SECOND)) &&
      (!stringp(second) || (file_size("/save/"+lower_case(second[0..0])+"/"+lower_case(second)+".o")<=0))) {
    notify_fail("Nicht richtig markierte Zweities sind von der Wahl ausgeschlossen.\n");
    return 0;
  }
  if (in_use)
  {
    write(in_use+" waehlt gerade, warte also bitte einen Augenblick.\n");
    return 1;
  }
  in_use=capitalize(getuid(RPL));
  say(in_use+" tritt an die Wahlmaschine.\n");
  write(long());
  write("Das steht zur Auswahl:\n");
  for (i=0;i<sizeof(moeglichkeiten);i++)
    write(sprintf(" -- %d.) %s\n",i+1,moeglichkeiten[i]));
  write("Triff nun Deine Wahl (sie ist UNSICHTBAR): ");
  input_to("waehle_2",1);
  return 1;
}

private int falsch()
{
  write("Diese Auswahl ist ungueltig. Versuchs bitte gleich nochmal.\n");
  say(in_use+" hat ausgewaehlt.\n");
  in_use=0;
  return 1;
}

static int waehle_2(string str)
{
  int i;
  mixed name;

  if (!str) return falsch();
  
  name=RPL->QueryProp(P_SECOND);
  if (!stringp(name))
    name=getuid(RPL);
  else name=lower_case(name);
            
  if (member_array(str, moeglichkeiten)==-1) {
    if ( !sscanf( str, "%d", i ) ) return falsch();
    if ( i<1 || i>sizeof(moeglichkeiten) ) return falsch();
    ergebnis=insert_alist(name,moeglichkeiten[i-1],ergebnis);
  }
  else ergebnis=insert_alist(name,str,ergebnis);
  write("Ok.\n");
  save_me();
  say(in_use+" hat ausgewaehlt.\n");
  in_use=0;
  return 1;
}

static int auswertung()
{
  int i,j,max,all;
  mapping stimmen, data;
  mixed names;

  if (!ARCH_SECURITY || process_call()) return 0;

  rm("/log/WAHL");

  data = mkmapping(ergebnis[0], ergebnis[1]);
  names = m_indices(data);
  stimmen = ([]);

  SAY("\n\n\nWahlergebnisse zum Thema: "+thema+"\n");
  SAY("\nGewaehlt haben folgende Personen: \n");
  SAY(break_string(CountUp(map(sort_array(names, #'>/*'*/), #'capitalize/*'*/)), 78));
  SAY("\n");

  while(sizeof(names)) {
    stimmen[data[names[0]]]++;
    names[0..0] = ({});
    all++;
  }

  SAY("*****************************************\n");
  SAY("Ergebnis:\n");
  for (i=0;i<sizeof(moeglichkeiten);i++)
    SAY(sprintf(" -- %2d.) %20s: %3d (%' '3.2f%%)\n", i+1, moeglichkeiten[i],
                stimmen[moeglichkeiten[i]], 1.0 * stimmen[moeglichkeiten[i]] * 100.0 / (1.0 * all)));
  SAY(sprintf("Gesamtstimmen: %d\n", sizeof(m_indices(data))));
  return 1;
}

static int wahlthema(string str)
{
  if (!ARCH_SECURITY || process_call()) {
    write("NOT allowed");
    return 0;
  }
  if (!(str=this_interactive()->_unparsed_args())) return 0;
  thema=str;
  say(RPL->name(WER)+" setzt ein neues Abstimmungsthema fest.\n");
  ergebnis=({({}),({})});
  moeglichkeiten=({});
  write("Ok.\n");
  save_me();
  return 1;
}

static int moeglichkeit(string str)
{
  if (!ARCH_SECURITY || process_call()) {
    write("NOT allowed");
    return 0;
  }
  if (!(str=RPL->_unparsed_args())) return 0;
  if (member_array(str, moeglichkeiten) != -1) {
    write("Schon drin.\n");
    return 1;
  }
  moeglichkeiten+=({str});
  write("Ok.\n");
  save_me();
  return 1;
}

mixed _internal()
{
  if (!ARCH_SECURITY || process_call()) return 0;
  return ergebnis;
}

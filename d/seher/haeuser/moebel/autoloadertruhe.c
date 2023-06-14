//--------------------------------------------------------------------------------
// Name des Objects:    Aufbewahrungtruhe fuer Autoload-Objekte
//
// Magier:              Zesstra
//--------------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"
inherit LADEN("swift_std_container");

//#include <ansi.h>
#include <class.h>
#include <wizlevels.h>
#include "/d/seher/haeuser/haus.h"

#define IDS     0
#define NAME   1
#define ALDATA  2

#define LOG(x,y) log_file(x,sprintf("%s [%s, %O, %O]: %s\n",dtime(time()),\
      (uuid?uuid:" "),PL,object_name(ME),y))
#define STORELOG(x) LOG("zesstra/ALTRUHE_STORE.log",x)
#define PICKLOG(x) LOG("zesstra/ALTRUHE_PICK.log",x)

#define ITEMLOG(x) log_file("zesstra/ALTRUHE_ITEMS.log",\
    sprintf("%s [%O]: %s\n",dtime(time()),\
      this_interactive()||PL,x))

#define ERRLOG(x) LOG("zesstra/ALTRUHE.ERR",x)

#undef BS
#define BS(x) break_string(x,78)

#define VERSION_OBJ "5"

#ifdef MAINTAINER
#undef MAINTAINER
#endif
#define MAINTAINER ({"zesstra"})

// Savefile der Blueprint
#ifdef SAVEFILE
#undef SAVEFILE
#endif
#define SAVEFILE __FILE__[..<3]

#define DEBUG(x)    if (funcall(symbol_function('find_player),MAINTAINER[0]))\
          tell_object(funcall(symbol_function('find_player),MAINTAINER[0]),\
                      "ALTruhe: "+x+"\n")

#define ACCESS (my_secure_level() >= ARCH_LVL)

// Diese 4 sind fuer die Blueprint (Master aller Truhen)
mapping whitelist=([]); // erlaubte Autoloader, alle anderen nicht erlaubt.
mapping blacklist=([]); // bereits explizit durch EM+ abgelehnte Autoloader
mapping vorschlaege=m_allocate(1,2); // vorschlaege der Spieler
mapping data=([]);  // hier speichert die BP alle Daten der Truhen
                    // WICHTIG: dieses Mapping wird in /secure/memory
                    // abgelegt und muss auch beim Neuladen ggf. von dort
                    // wieder abgeholt werden. Ausserdem teilen sich alle
                    // Truhen eines Spielers und diese Blueprint die Mappings
                    // darin, damit Aenderungen sofort in allen Truhen und dem
                    // Master bekannt sind.

/* die einzelnen Truhen speichern in autoloader. Format: 
   3 Werte pro Key, Keys sind die Namen der Blueprints der Objekte:
   ([<blueprint>: <P_IDS>; <ob->name()>; <P_AUTOLOADOBJ> ])
   */
nosave mapping autoloader=m_allocate(1,3);
nosave string uuid; // UUID des Eigentuemers
nosave object ob_in_bewegung; // uebler Hack eigentlich. :-(

void NotifyInsert(object ob, object oldenv);
static mapping QueryData();
static mapping SetData(mixed data);
protected void save_me();
protected void check_content();

nomask private int my_secure_level(); //Args.

protected void create() {

  // Ja, es is Absicht, dass das create der BP erst spaeter abgebrochen wird!
  swift_std_container::create();

  seteuid(getuid(ME));

  // falls dies die BP ist:
  // 1. das Savefile zu laden.
  // 2. versuchen, die truhendaten von /secure/memory zu holen, damit nach dem
  //    Neuladen der Master und die Client immer nach _dieselben_ Mappings
  //    haben.
  if (!clonep(ME))
  {
    // Savefile restaurieren (auch wenn data im Memory ist, muss das sein,
    // damit die anderen Variablen wieder eingelesen sind).
    restore_object(SAVEFILE);
    // jetzt Daten aus Memory holen, sofern verfuegbar
    mapping tmp = "/secure/memory"->Load("truhendaten");
    if (mappingp(tmp))
    {
      // Daten aus Savefile durch die vom Memory ersetzen
      data = tmp;
    }
    else
    {
      // Keine Daten in Memory. Jetzt in jedem Fall den Pointer auf das Mapping data in
      // /secure/memory ablegen
      if ("/secure/memory"->Save("truhendaten",data) != 1)
      {
        raise_error("Could not save memory to /secure/memory.");
      }
    }
  }
  else
  {
    // brauchen Clones nicht.
    set_next_reset(-1);
    data=0; // brauchen Clones nicht.
  }

  SetProp(P_SHORT, "Eine kleine, magische Holztruhe");
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Holztruhe");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_NAME_ADJ,({"klein", "magisch"}));
  SetProp(P_LONG, BS(
     "Die kleine Holztruhe ist aus stabilem Eichenholz gefertigt. Eigentlich "
     "saehe sie recht unscheinbar aus, waeren da nicht die vielen kunstvollen "
     "Runen an den Seiten und auf dem Deckel.")
    +"@@cnt_status@@");

  SetProp(P_LEVEL, 1000); // wehrt bestimmte Zauber ab

  AddId(({"autoloadertruhe", "holztruhe", "truhe"}));

  // den Rest vom Create braucht die BP nicht.
  if (!clonep(ME)) return;

  SetProp(P_LOG_FILE,"zesstra/ALTRUHE.rep");

  SetProp(P_WEIGHT, 3000);         // Gewicht 5 Kg
  // die drei hier sind in diesme Fall eigentlich voellig ohne Bedeutung
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);
  SetProp(P_MAX_OBJECTS, 100);     // sind eh immer 0 echte Objekte drin.

  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ([MAT_OAK:49, MAT_MISC_MAGIC:50, MAT_JOFIUM: 1]) );
  SetProp(P_INFO, BS("In diese stabile Truhe kannst Du bestimmte "
        "Autoload-Objekte hineinlegen und sie lagern, wenn Du sie gerade "
        "nicht brauchst. (Mit 'deponiere <was>' kannst Du etwas in der "
        "Truhe zur Aufbewahrung deponieren, mit 'entnehme <was> oder "
        "'nimm <was> aus truhe' kannst Du einen Gegenstand wieder "
        "herausnehmen.)"));

  // Prop fuer rebootfeste Moebel
  // Der endgueltige Wert (UUID des Spielers) wird per SetBesitzer() gesetzt,
  // sobald die Truhe das erste Mal in ein Seherhaus bewegt wurde.
  SetProp(H_FURNITURE, 1);

  AD(({"platz","groesse"}), 
      BS("Die Truhe ist recht klein, aber merkwuerdigerweise "
        "scheint sie viel mehr an Gegenstaenden aufnehmen zu koennen, als "
        "es von ihrer Groesse her scheint."));
  AD(({"gegenstand","gegenstaende"}),
      BS("Die Truhe scheint zwar vielen Gegenstaenden Platz zu bieten, aber "
        "dafuer nimmt sie nicht jeden Gegenstand auf."));
  AD(({"holz","eichenholz"}),
      BS("Das Eichenholz ist sehr stabil. Es ist ganz glatt geschliffen und "
        "traegt viele kunstvolle Runen in sich."));
  AD(({"seiten","seite"}),
      BS("Die Truhe hat 4 Seiten, die allesamt mit Runen verziert sind."));
  AD(({"boden"}),
      BS("Der Boden der Truhe traegt keinerlei Runen."));
  AD(({"deckel"}),
      BS("Auch der Deckel der Truhe ist mit Runen verziert."));
  AD(({"runen","rune"}),
      BS("Die Runen bedecken alle 4 Seiten und den Deckel der Truhe. Man "
        "hat sie zweifellos in muehsamer Arbeit aus dem harten Holz "
        "geschnitzt. Anschliessend wurden sie offenbar mit einem "
        "Metall gefuellt. Du verstehst zwar ueberhaupt nicht, was "
        "die Runen bedeuten, aber sie sind bestimmt magisch, denn wann immer "
        "Du den Deckel oeffnest oder schliesst oder Gegenstaende hineinlegst "
        "oder herausnimmst, leuchten die Runen hell auf."));
  AD(({"metall"}),
      BS("Was das wohl fuer ein Metall sein mag? Zweifellos hat es auch was "
        "mit Magie zu tun."));
  AD(({"magie"}),
      BS("In dieser Truhe scheint viel Magie zu stecken, wenn selbst ein "
        "Weltuntergang ihr nichts anhaben kann."));
  AD(({"arbeit","fertigung"}),
      BS("Die Fertigung dieser Truhe muss sehr aufwendig gewesen sein. "
        "Kein Wunder, dass die Truhe so teuer ist."));
  AD(({"wunder"}),
      BS("Ein Wunder scheint es zu sein."));
  AD(({"weltuntergang","armageddon"}),
      BS("Dir schaudert beim Gedanken an den Weltuntergang."));
  AD(({"gedanken"}),
      BS("Denk doch lieber an was anderes..."));

  AddCmd("deponier|deponiere&@PRESENT","cmd_deponiere",
      "Was moechtest Du in der Eichenholztruhe deponieren?");
  AddCmd("entnimm|entnehme|entnehm","cmd_entnehmen");

  // bei dieser Truhe waere das Erlauben voellig sinnlos. ;-)
  RemoveCmd(({"serlaube"}));

}

// keine Truhen zerstoeren, die irgendeinen INhalt haben.
int zertruemmern(string str) {
  // aus swift_std_container
  string nf_str;
  nf_str="Syntax: zertruemmer [Objekt-Id]\n"
        +"Bsp.:   zertruemmer "+QueryProp(P_IDS)[1]+"\n";
  notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str);
  if(!str) return 0;
  notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"+nf_str);
  if(present(str)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;
  if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
  {
    write( BS("Nur "+QueryHausbesitzer()+" darf "+name(WEN,1)+" zertruemmern!"));
    return 1;
  }
  // Objekte enthalten? Wenn ja, abbruch.
  if (sizeof(autoloader)) {
    tell_object(PL,BS("Du willst gerade zum Schlag ausholen, um "
          +name(WEN,1)+ " zu zertruemmern, als Dir einfaellt, dass "
          +QueryPronoun(WER)+ " ja gar nicht leer ist! Nene, wer weiss, ob "
          "Du das nicht noch brauchen koenntest."));
    return 1;
  }
  // sonst geerbten Kram ausfuehren.
  return ::zertruemmern(str);
}

// Zesstra, 1.7.07, fuers Hoerrohr
string GetOwner() {return "zesstra";}

// Prueft das Objekt auf Eignung fuer die Truhe, speichert seine Daten und
// zerstoert es anschliessend. NODROP-Objekte koennen nur so in die Truhe
// gelegt werden, alle anderen koennen auch mit "stecke ... in truhe"
// deponiert werden, woraufhin ebenfalls PreventInsert() und NotifyInsert()
// durchlaufen werden.
protected int cmd_deponiere(string cmd, mixed args) {
  if (!objectp(PL)) return 0;

  notify_fail(Name(WER,1)+" ist doch geschlossen!\n");
  if(QueryProp(P_CNT_STATUS)!=CNT_STATUS_OPEN) return 0;
  
  notify_fail("Was moechtest Du in der Eichenholztruhe deponieren?\n");
  if (!stringp(cmd) || !sizeof(cmd) 
      || !pointerp(args) || !sizeof(args)) return 0;
  object ob=args[0];

  if (!objectp(ob)) return 0;

  // Nur Objekte aus dem Inventar deponieren. Absichtlich nicht ueber
  // @PUT_GET_DROP in AddCmd() geloest, damit die beiden Fehlerfaelle zwei
  // getrennte Meldungen bewirken.
  if (environment(ob) != PL) {
    tell_object(PL, BS("Du musst "+ob->name(WEN,1)+"schon bei Dir haben, um "+
      ob->QueryPronoun(WEN)+" deponieren zu koennen."));
    return 1;
  }

  // wuerde die Truhe das Objekt ueberhaupt aufnehmen? Fehlerausgabe durch
  // PrevenInsert()
  if (PreventInsert(ob)) return 1;
  // Ausziehen... Schade, dass DoUnwear nix vernuenftiges an Rueckgabewert
  // hat. :-(
  if (objectp(ob->QueryProp(P_WORN))) {
    ob->DoUnwear();
    if (objectp(ob->QueryProp(P_WORN))) {
      tell_object(PL, BS("Du musst "+ ob->name(WEN,1)+ "zunaechst einmal "
            "ausziehen!"));
      return 1;
    }
  }
  // wegstecken
  if (objectp(ob->QueryProp(P_WIELDED))) {
    ob->DoUnwield();
    if (objectp(ob->QueryProp(P_WIELDED))) {
      tell_object(PL, BS("Du musst "+ ob->name(WEN,1)+ "zunaechst einmal "
            "wegstecken!"));
      return 1;
    }
  }
  // NO_CHECK und Silent-Bewegung, Meldungen an den Spieler selber machen.
  tell_object(PL, BS("Du steckst "+ ob->name(WEN,1) + " in " + name(WEN,1) +
      "."));
    
  tell_room(environment(),BS(PL->Name(WER) + " legt " + ob->name(WEN,0) +
      " in " + name(WEN,1) + " hinein."),({PL}));
  ob->move(ME, M_NOCHECK|M_SILENT);
  return 1;
}

// alternative zum "nimm bla aus truhe". Spieler wollten was kurzes dafuer
// haben.
protected int cmd_entnehmen(string cmd) {
  int res;

  if (!objectp(PL)) return 0;

  notify_fail(Name(WER,1)+" ist doch geschlossen!\n");
  if(QueryProp(P_CNT_STATUS)!=CNT_STATUS_OPEN) return 0;

  notify_fail(BS("Was moechtest Du aus "+name(WEM,1)+ " entnehmen?\n"));
  if (!stringp(cmd) || !sizeof(cmd)) return 0;

  object *obs=present_objects(cmd);

  if (!sizeof(obs) || !objectp(obs[0]))
    return 0;

  // NOGET ist hier bloed. So ist es zwar auch nicht richtig doll... *seufz*
  // Die hier ist/waere aber nen uebler Hack, erstmal auskommentiert lassen.
  // also, P_NOGET sichern.
  /*mixed noget;
    if (!closurep(noget=ob->Query(P_NOGET,F_QUERY_METHOD))) {
    noget=ob->Query(P_NOGET,F_VALUE);
    ob->Set(P_NOGET,0,F_VALUE);
  }
  else {
    ob->Set(P_NOGET,0,F_QUERY_METHOD);
  }*/
  // nehmen.
  res=PL->pick_obj(obs[0]);
  // P_NOGET zurueckschreiben. (Ja, wenn eine Closure als F_VALUE drinsteht,
  // landet die jetzt als F_QUERY_METHOD im Objekt.
  /*if (closurep(noget)) {
    ob->Set(P_NOGET,noget,F_QUERY_METHOD);
  }
  else
    ob->Set(P_NOGET,noget,F_VALUE);
*/
  return(res);
}

// Hier wird auch das PreventInsert() von der Blueprint gerufen. Das
// erleichtert es, die Liste an erlaubten Objekten zu aendern, ohne dass man
// alle Clones ersetzen muss.
varargs int PreventInsert(object ob) {
  string oname;
  // Das Objekt, was die Truhe gerade selber bewegt, wird ignoriert.
  if (!objectp(ob) || ob_in_bewegung==ob) return 0;
  
  oname=BLUE_NAME(ob);
  
  // Pruefung in Clonen:
  if (clonep(ME))
  {
    // nur Eigentuemer
    if (!stringp(uuid) || !sizeof(uuid)) {
      if (objectp(PL))
        tell_object(PL,BS(sprintf("%s gehoert Dir nicht, daher kannst Du "
            "auch keine Gegenstaende in %s legen.",
            Name(WER,1),QueryPronoun(WEN))));
      return 1;
    }
    if (!objectp(PL) || getuuid(PL)!=uuid) {
      if (objectp(PL))
        tell_object(PL,BS(sprintf("Nur %s darf Gegenstaende in %s "
          "hineinlegen.",capitalize(explode(uuid,"_")[0]),name(WEN,1))));
      return 1;
    }
    // jedes Objekt nur einmal.
    if (member(autoloader,oname)) {
      if (objectp(PL))
        tell_object(PL,BS(Name(WER,1)+ " kann von einem Gegenstand jeweils "
          "nur ein Exemplar aufnehmen, es ist aber bereits "
          +ob->name(WER,0) + " in " + QueryPronoun(WEM) + "."));
      return 1;
    }
    // jetzt Erlaubnisliste der BP fragen.
    if (objectp(blueprint(ME)))
      return blueprint(ME)->PreventInsert(ob);
    else
      return load_object(load_name(ME))->PreventInsert(ob);
  }
  // Ende fuer Pruefung fuer Clone.

  // ab hier jetzt die Pruefung durch die BP.
  // Keine (freigegebener) Autoloader? Hat in diesem Container nix verloren!
  if( !ob->QueryProp(P_AUTOLOADOBJ) ) {
    if (objectp(PL))
      tell_object(PL,BS("In "+name(WEN,1)
            +" kannst Du nur Autoload-Objekte hineinlegen. "));
    return 1;
  }
  else if (member(blacklist,oname)) {
    if (objectp(PL))
      tell_object(PL,BS("In "+name(WEN,1)
            +" kannst Du nur dafuer erlaubte Autoload-Objekte hineinlegen. "
            +ob->Name(WER,1) + " wurde explizit als nicht erwuenscht "
            "befunden."));
    return 1;
  }
  else if (!member(whitelist,oname)) {
    if (!member(vorschlaege,oname)) {
      vorschlaege[oname,0]=ob->name(WER,0) || "";
      vorschlaege[oname,1]=ob->short() || "";
    }
    if (objectp(PL))
      tell_object(PL,BS("In "+name(WEN,1)
            +" kannst Du nur dafuer erlaubte Autoload-Objekte hineinlegen. "
            +ob->Name(WER,1) + " ist momentan nicht auf der Liste. Der "
            "Gegenstand wurde jetzt als Vorschlag gespeichert. Bitte hab "
            "etwas Geduld, bis sich jemand den Gegenstand angeschaut hat."));
    // ggf. reset reaktivieren und Maintainer informieren.
    if (!query_next_reset())
      set_next_reset(1);
    return 1;
  }
  // getragenes?
  if (ob->QueryProp(P_WORN)) {
      ob->DoUnwear(0);
      if (ob->QueryProp(P_WORN)) { //ARGL. GRUMMEL.
        if (objectp(PL))
          tell_object(PL,BS("Willst Du "+ob->name(WEN,1) + " nicht erstmal "
                "ausziehen?"));
        return 1;
      }
  }
  // enthaelt es irgendwelche anderen Objekte?
  if (sizeof(all_inventory(ob))) {
      if (objectp(PL))
        tell_object(PL,BS(ob->Name(WER,1) + " ist nicht leer."));
      return 1;
  }

  // andere Einschraenkungen, als hier geprueft werden, gibt es nicht.
  return 0;
}

// hier ist das Objekt jetzt in der Truhe, d.h. Daten speichern und Objekt
// destructen. ;)
void NotifyInsert(object ob, object oldenv) {

  // Das Objekt, was die Truhe gerade selber bewegt, wird ignoriert.
  if (!objectp(ob) || ob==ob_in_bewegung)
    return;
  STORELOG(sprintf("%s deponiert %s [%O], Daten: %O",getuid(PL),ob->name(WEN,0),
        ob,ob->QueryProp(P_AUTOLOADOBJ)));
  // noetig sind die IDs, den Namen und die AUTOLOADOBJ-Daten des Objekts.
  // ;-)
  autoloader[BLUE_NAME(ob),ALDATA]=ob->QueryProp(P_AUTOLOADOBJ);
  autoloader[BLUE_NAME(ob),IDS]=ob->QueryProp(P_IDS);
  // Objekte, die 0 als short liefern, kriegen eine Standard-Short und werden
  // sichtbar gemacht, da sie sonst nicht wiederzufinden sind. Objekte, die
  // unsichtbar sein sollen, duerfen nicht erlaubt werden.
  // TODO eine ID als Short nehmen?
  autoloader[BLUE_NAME(ob),NAME]=capitalize((ob->short()||"<Unbekannt>.\n")[..<3]);
  // nach dem Move die realen Objekte destructen.
  call_out(#'check_content, 0);
  save_me();
}

// Objekt wurde entnommen, also aus der Liste der enthaltenen Autoloader
// loeschen. Ausserdem Objekt konfigurieren. Dies wird erst hier gemacht und
// nicht in create_object(), damit es so aehnlich wie moeglich zum clonen der
// Autoloader beim erstellen des Spielerobjektes wird (dort wird erst bewegt,
// dann konfiguriert).
void NotifyLeave(object ob, object dest) {
  string oname;
 
  if (!objectp(ob))
    return;
  
  oname=BLUE_NAME(ob);
  if (!member(autoloader,oname)) {
    // Das sollte definitiv nicht passieren.
    ERRLOG(sprintf("Gegenstand (%O) wurde entnommen, der nicht "
      "gespeichert war!",ob));
    return;
  }

  PICKLOG(sprintf("Objekt (%O) wurde entnommen.",ob));
  m_delete(autoloader,oname);
  // speichern
  save_me();
}

protected void check_content() {
  // wenn Objekte noch in der Truhe sind, also nicht erfolgreich in
  // einen Spieler bewegt wurden, werden zerstoert. Datenverlust gibt es
  // hierbei nicht, weil die Daten der Autoloader nur durch NotifyLeave()
  // geloescht werden.
  foreach(object ob: all_inventory()) {
    ob->remove(1);
    if (objectp(ob)) destruct(ob);
  }
}

// hier nochmal schauen, ob das Objekt auch in den richtigen Spieler bewegt
// wird... Falls das move dann hier abgebrochen wird, wird das Objekt im
// naechsten HB von der Truhe zerstoert. (chk_contents())
varargs int PreventLeave(object ob, mixed dest) {
  object ob2;
  
  //DEBUG(sprintf("PreventLeave(): Ob: %O, Dest: %O (%O)",
  //        ob,dest,(objectp(dest)?"Objekt":"Non-Object")));

  if (!objectp(ob)) return 0; 
  // falls string uebergeben, erstmal zug. Spieler finden.
  if (stringp(dest) && sizeof(dest))
    dest=find_player(dest);

  // richtiges INteractive? Dann darf das Objekt raus. Sonst nicht. ;-)
  if (!objectp(dest) || !interactive(dest) || getuuid(dest)!=uuid)
      return 1;

  // pruefen, ob der Spieler schon ein Objekt dieser Blueprint dabei hat. Wenn
  // ja, Abbruch, das koennte sonst zuviele Probleme geben.
  if (objectp(ob2=present_clone(ob,dest))) {
    tell_object(dest,BS("Du hast bereits "+ob2->name(WEN,0) +
        " dabei. Zwei Gegenstaende dieser Art kannst du nicht gleichzeitig "
        "mit Dir herumtragen."));
    return 1; //nicht rausnehmen.
  }
  return 0;
}

// Objekte ausgeben, die hier angeblich drin sind. ;-) Gibt aber nur die
// Autoloader aus, keine echten Objekt, die Container sind. Allerdings sollte
// sowas eh nicht vorkommen. ;-)
// flags: 1 - return array, 2 - don't collect equal objects '
// flags: 4 - don't append infos for wizards
varargs mixed make_invlist(object viewer, mixed inv, int flags) {
  int iswiz;
  mixed objs;

  iswiz = IS_LEARNER( viewer ) && viewer->QueryProp(P_WANTS_TO_LEARN);
  // Mapping benutzen, um multiplen Overhead fuer allokation im foreach() zu
  // vermeiden.
  objs=m_allocate(sizeof(autoloader),1);
  foreach(string oname, string *ids, string sh: autoloader) {
    if (iswiz && !(flags & 4))
      objs[oname]=sh + ". ["+oname+"]";
    else
      objs[oname]=sh + ".";
  }
  if(flags & 1) return(m_values(objs)-({""}));
  if(!sizeof(autoloader)) return "";
  return sprintf("%"+(sizeof(objs) > 6 ? "#" : "=")+"-78s",
                 implode(m_values(objs)-({""}), "\n")) + "\n";
}

// erzeugt das benannte Objekt und liefert es zurueck. Liefert 0 im Fehlerfall.
// ausserdem bewegt es das Objekt in die Truhe, damit es ein Env hat und die
// Truhe via NotifyLeave() mitkriegt, dass es tatsaechlich entnommen wurde.
private object create_object(string oname) {
  string error;
  object ob;
  if (!member(autoloader,oname)) return 0;

  //Blueprint finden (ja, das ist nicht unbedingt noetig, man koennte auch
  //direkt clonen)
  if (error=catch(ob=load_object(oname);publish) ||
      !objectp(ob)) {
    ERRLOG(sprintf("Konnte %s nicht laden/finden. Fehler: %O",
          oname,error));
    return 0;
  }
  // clonen
  if (error=catch(ob=clone_object(oname);publish) ||
      !objectp(ob)) {
    ERRLOG(sprintf("Konnte %s nicht clonen. Fehler: %O",
        oname,error));
    return 0;
  }
  // konfigurieren
  error=catch(ob->SetProp(P_AUTOLOADOBJ,autoloader[oname,ALDATA]);publish);

  //Objekt bewegen, dabei Objekt in glob. Var. merken, damit PreventInsert()
  //und NotifyInsert() es ignorieren. *seufz*
  ob_in_bewegung=ob;
  ob->move(ME,M_NOCHECK);
  ob_in_bewegung=0;

  // jetzt noch nen Callout starten, damit das Objekt zerstoert wird, wenn es
  // nicht wirklich in einen Spieler bewegt wird.
  call_out(#'check_content,1);

  return(ob);
}

// Schauen, ob die truhe ein Objekt mit passender ID enthaelt. Wenn
// ja, das Objekt erzeugen und zurueckliefern. 
// Diese Funktion hat eine Reihe von Nachteilen bzw. Defiziten ggue. der
// normalerweise in Containern definierten Funktion:
// - Nur das erste Objekt, auf das id passt. Ich hab erstmal keine Lust,
//   hier mehrere Objekte zu erzeugen. Mal schauen, ob es so geht. Auch waere
//   es ein Problem, wenn viele Objekt die Evalgrenze bzw. nicht alle in den
//   Spieler bewegt werden, nachdem sie erzeugt wurden.
// - keine komplexen Ausdruecke, nur reine IDs.
// Ich halte den Aufwand fuer nicht gerechtfertigt.
object *present_objects( string complex_desc ) {
  object ob;
  if (!stringp(complex_desc) || !sizeof(complex_desc))
      return ({});
  // diese Funktion liefert nur Objete zurueck, wenn der richtige Interactive
  // versucht, sie zu entnehmen. ;-)
  if (!objectp(this_interactive()) ||
      getuuid(this_interactive())!=uuid)
      return ({});

  // "alles" liefert das erstbeste Objekt.
  if (complex_desc=="alles" && sizeof(autoloader))
  {
    string oname=m_indices(autoloader)[0];
    ob=create_object(oname);
    if (objectp(ob)) return ({ob});
  }

  // ueber alle Eintraege gehen, bis eine ID stimmt, erstes passendes Objekt
  // erzeugen und in einem Array zurueckliefern.
  foreach(string oname, string *ids: autoloader) {
    if (member(ids,complex_desc)==-1) continue;
    ob=create_object(oname);
    break; //objekt gefunden, fertig hier
  }
  if (objectp(ob)) return ({ob});
  return ({}); // nix gefunden
}



// ******************* Verwaltung *********************************

// registriert die Truhe auf den jeweiligen Eigentuemer.
protected void SetBesitzer(string unused, string newuuid) {
  if (!stringp(newuuid) || !sizeof(newuuid)) return;
  // wenn schon registriert, abbrechen
  if (stringp(uuid) && sizeof(uuid)) return;

  uuid=newuuid;
  Set(H_FURNITURE,uuid,F_VALUE); //Setmethode umgehen, sonst Rekursion 
  // ab jetzt nur noch von der Truhe selber.
  Set(H_FURNITURE,SECURED,F_MODE_AS);
  
  // Daten fuer den Benutzer aus der Blueprint holen (BP liefert KEINE Kopie
  // und es darf KEINE gemacht werden!):
  autoloader=({mapping})load_name()->GetData(uuid);
  
  // keine Daten gekriegt? -> Fehler loggen
  if (!mappingp(autoloader))
  {
    ERRLOG(sprintf("Keine gueltigen Daten vom Truhenmaster (BP) erhalten. "
        "initialisiere mit leerem Mapping. :-("));
    raise_error(sprintf(
          "Keine gueltigen Daten vom Truhenmaster (BP) fuer UUID %s "
          "erhalten.\n",uuid));
  }
}

// Set-Funktion
string _set_h_furniture(mixed arg) {
  if (stringp(arg))
  {
    SetBesitzer(0,arg); // bricht ab, wenn bereits registriert.
  }
  return(uuid);
}

// Falls das Speichern in der BP nicht klappte, rufen die Clones diese
// Funktion. Schreiben der Daten in in Logfile zur Restaurieren per Hand.
private void EmergencyStore(int res) {
    ERRLOG(sprintf("EmergencyStore() called. Rueckgabewert des "
        "Truhenmaster (BP) war: %O. Speichere Daten in Logfile. ",res));
    write_file(__DIR__+"ALTRUHE.NOTFALLDATEN",
        sprintf("Daten fuer %O:\n%O\n",uuid,autoloader));
}

protected void save_me() {
  int res;
  // nur BP speichern
  if (!clonep(ME))
    save_object(SAVEFILE);
  else
  {
    if (objectp(blueprint(ME)))
      res=({int})blueprint(ME)->StoreData(uuid,autoloader);
    else
      res=({int})load_object(load_name(ME))->StoreData(uuid,autoloader);

    if (res!=1)
      EmergencyStore(res); // Daten in einem Notfall-Logfile ablegen.
  }
}


// diese Funktion wird vom Seherhausraum gerufen, sobald ein rebootfestes
// Moebelstueck erzeugt und konfiguriert wurde.
void post_create() {
    if (!clonep()) return;

    // wenn jetzt kein Env: destructen
    if (!objectp(environment())) {
        remove(1);
        return;
    }
    //beim Schrankmaster registrieren
    SCHRANKMASTER->RegisterCnt(ME, QueryProp("cnt_version_std")
      +":"+QueryProp("cnt_version_obj"), environment()->QueryOwner(),
       environment());
}

// diese Funktion wird vom Schrankmaster gerufen, wenn dieser meint, dass
// dieses Objekt neu erstellt werden sollte.
int UpdateMe()
{
  if (!clonep())
    return 0;
  if (!objectp(environment()))
    return 1;
  object ob=clone_object(load_name(ME));
  if (objectp(ob)) {
    object oldenv=environment();
    // UUID uebertragen
    ob->SetProp(H_FURNITURE, uuid);
    // hierhier bewegen, dabei werden UUID und Daten von der neuen Truhe meist
    // automatisch geholt.
    ob->move(oldenv,M_NOCHECK);
    // jetzt erst post_create() rufen!
    ob->post_create();
    // dieses Objekt rausbewegen (damit das Seherhaus es austraegt).
    move("/room/void",M_NOCHECK);
    // Seherhausraum speichern (koennte teuer sein!)
    oldenv->Save(1);
    // selbstzerstoeren
    // __INT_MAX__ bedeutet: nicht speichern, die neue Truhe hat die daten
    // schon aus der BP abgefragt.
    remove(__INT_MAX__);
  }
  return(1);
}

// bei Selbstzerstoerung speichern bzw. Daten an die Blueprint uebermitteln
// und beim Schrankmaster abmelden.
varargs int remove(int silent) {
  string uid="";

  // Blueprint speichern im Savefile, Clones uebertragen die Daten hier an die
  // Blueprint. Clones nur, wenn nicht __INT_MAX__ uebergeben wurde.
  if (silent!=__INT_MAX__ || !clonep())
    save_me();

  if (clonep()) {
    // Clone melden sich beim Schrankmaster ab.
    if (objectp(environment())) {
        uid=environment()->QueryOwner();
    }
    //beim Schrankmaster deregistrieren
    SCHRANKMASTER->RemoveCnt(ME,uid);
  }
  return(::remove(silent));
}

// ***************** NUR BLUEPRINTS *********************************

// neuen Autoloader zulassen (nur EM+!)
varargs int AddAutoloader(string path,string nam) {
  object ob;
  if (clonep(ME)) return 0;
  if (!stringp(path) || !sizeof(path)) return -1;
  if (!ACCESS) return -2;
  //if (!ARCH_SECURITY) return -2;
  if (member(whitelist,path)) return -3;
  if (catch(ob=load_object(path);publish)
      || !objectp(ob))
      return -4;
  // wenn Name nicht angegeben und auch nicht aus BP ermittelbar: Abbruch.
  if (!stringp(nam) || !sizeof(nam)) {
      nam=ob->name(WER,0);
      if (!stringp(nam) || !sizeof(nam)) return -5;
  }
  ITEMLOG(sprintf("%s erlaubt: %s (%s)\n",getuid(this_interactive()),
          path,nam));
  whitelist+=([path:capitalize(nam)]);
  if (member(vorschlaege,path))
    m_delete(vorschlaege,path);
  save_me();
  return(1);
}

// Autoloader aus Erlaubnisliste entfernen (nur EM+!)
int RemoveAutoloader(string path) {
  if (clonep(ME)) return 0;
  if (!stringp(path) || !sizeof(path)) return -1;
  if (!ACCESS) return -2;
  //if (!ARCH_SECURITY) return -2;
  if (!member(whitelist,path)) return -3;
  ITEMLOG(sprintf("%s widerruft Erlaubnis: %s (%s)\n",getuid(this_interactive()),
          path,whitelist[path]));
  whitelist-=([path]);
  save_me();
  return(1);
}

// erlaubte Autoloader abfragen
mapping QueryAutoloader() {
  return(copy(whitelist));
}

// neuen Autoloader in Blacklist eintragen (nur EM+!)
varargs int AddBlacklist(string path, string nam) {
  object ob;
  if (clonep(ME)) return 0;
  if (!stringp(path) || !sizeof(path)) return -1;
  if (!ACCESS) return -2;
  //if (!ARCH_SECURITY) return -2;
  
  if (member(blacklist,path)) return -3;

  if (catch(ob=load_object(path);publish)
      || !objectp(ob))
      return -4;
  // wenn Name nicht angegeben und auch nicht aus BP ermittelbar: Abbruch.
  if (!stringp(nam) || !sizeof(nam)) {
      nam=ob->name(WER,0);
      if (!stringp(nam) || !sizeof(nam)) return -5;
  }
  // ggf. erlaubten entfernen.
  if (member(whitelist,path))
    RemoveAutoloader(path);
  ITEMLOG(sprintf("%s verbietet: %s (%s)\n",getuid(this_interactive()),
          path,nam));

  blacklist+=([path:capitalize(nam)]);

  if (member(vorschlaege,path))
    m_delete(vorschlaege,path);

  save_me();
  return(1);
}

// Autoloader aus Blacklist entfernen (nur EM+!)
int RemoveBlacklist(string path) {
  if (clonep(ME)) return 0;
  if (!stringp(path) || !sizeof(path)) return -1;
  if (!ACCESS) return -2;
  //if (!ARCH_SECURITY) return -2;
  if (member(blacklist,path)==-1) return -3;
  ITEMLOG(sprintf("%s loescht: %s (%s) von Blacklist\n",getuid(this_interactive()),
          path,blacklist[path]));
  blacklist-=([path]);
  save_me();
  return(1);
}

// gesperrte Autoloader abfragen
mapping QueryBlacklist() {
  return(copy(blacklist));
}

// vorschlaege abfragen
varargs mixed QueryVorschlaege(int format) {
  string res="\n";
  if (!format) return(copy(vorschlaege));

  foreach(string oname, string nam, string sh: vorschlaege) {
    res+=sprintf("%.78s:\n   %.37s,%.37s\n",oname,nam,sh); 
  }

  if (format==2 && objectp(PL))
    tell_object(PL,res);
  else
    return res;
  return 0;
}

// Wird diese funktion in der Blueprint gerufen, liefert sie die Daten fuer
// eine Truhe mit dieser UUID zurueck. Aber nur dann, wenn die auch von einer
// truhe abgerufen wird und keinem beliebigen anderen Objekt oder wenn ein EM
// abfragt.
mapping GetData(string uid) {
  if (clonep(ME)) return 0;
  if (extern_call())
  {
    if (!objectp(previous_object())) return 0;
    if (blueprint(previous_object()) != ME
        && load_name(previous_object()) != __FILE__[..<3]
        && !ACCESS)
        return(0);
  }
  if (!stringp(uid)) return ([]);

  if (!member(data, uid))
    data[uid] = m_allocate(1,3);

  // Absichtlich keine Kopie, damit die Truhen (falls jemand mehrere kauft)
  // sich alle eine Kopie des Mappings teilen.
  return data[uid];
}

// Loest die Speicherung der Daten im Savefile aus, denn irgendwelche Daten
// haben sich in einem Clone geaendert, welche noch auf die Platte muessen.
// Diese Funktion speicherte frueher Daten aus den Clones wieder im Mapping
// des Masters (der Blueprint). Dies ist heute nicht mehr so, weil alle Truhen
// (eines Benutzers) sich data[uid] teilen. Daher wird hier ein Fehler
// ausgeloest, wenn es von einer Truhe ein Mapping kriegt, welches nicht
// identisch (!) mit dem data[uid] ist.
// liefert 0 im Fehlerfall!
int StoreData(string uid, mapping tmp) {

  if (clonep(ME)) return 0;

  // Aber nur dann, wenn die auch von einer truhe abgerufen wird und keinem
  // beliebigen anderen Objekt oder wenn ein EM setzt.
  if (extern_call())
  {
    if (!objectp(previous_object()))
      return 0;
    if (blueprint(previous_object()) != ME 
        && load_name(previous_object()) != __FILE__[..<3]
        && !ACCESS)
      return 0;
  }
  if (!stringp(uid) || !mappingp(tmp))
    return(0);

  // tmp muss auf _dasselbe_ Mapping zeigen wie data[uid]. Wenn das nicht der
  // Fall ist, ist was schiefgelaufen. Jedenfalls wird dann hier nen Fehler
  // ausgeloest.
  if (tmp != data[uid])
  {
//    if(program_time(previous_object()) < 1400525694)
//      data[uid]=tmp;
//    else
      raise_error("StoreData() gerufen und Daten sind nicht identisch "
          "mit den bereits bekannten.\n");
  }

  // Absichtlich keine Kopie, damit sich alle Truhen das Mapping teilen.
  //data[uid]=tmp;

  // Savefile muss natuerlich geschrieben werden, fuer den naechsten Reboot.
  if (find_call_out(#'save_me)==-1)
      call_out(#'save_me,10); // Speichervorgaenge ggf. sammeln (upd -ar ...)
  return(1);
}

// Maintainer ueber Truhenvorschlaege informieren
void reset() {

  if (clonep() || !sizeof(vorschlaege)) {
    // ohne Vorschlaege ist auch kein reset noetig.
    set_next_reset(-1);
    return;
  }
  set_next_reset(12000); // alle 3-4h reicht.
  foreach(string uid: MAINTAINER) {
    object pl=find_player(uid);
    if (objectp(pl) && query_idle(pl) < 1800)
      tell_object(pl,BS("Es gibt neue Objektvorschlaege fuer die "
        "Autoloadertruhe. Bitt schau da doch bei Gelegenheit mal drueber."));
  }
}


// *************************************************************************

// **************** SONSTIGES **********************************************

// *seufz*
// secure_level() aus der simul_efun ist hier momentan nicht brauchbar, weil
// dort auch p/seher/moebel/autoloadertruhe in der Callerkette steht und das
// ein levle von 0 hat. *seufz*
nomask private int my_secure_level() {
  int *level;
  //kette der Caller durchlaufen, den niedrigsten Level in der Kette
  //zurueckgeben. Zerstoerte Objekte (Selbstzerstoerer) fuehren zur Rueckgabe
  //von 0.
  //caller_stack(1) fuegt dem Rueckgabearray this_interactive() hinzu bzw. 0,
  //wenn es keinen Interactive gibt. Die 0 fuehrt dann wie bei zerstoerten
  //Objekten zur Rueckgabe von 0, was gewuenscht ist, da es hier einen
  //INteractive geben muss.
  level=map(caller_stack(1),function int (object caller)
      {if (objectp(caller))
        return(query_wiz_level(geteuid(caller)));
       return(0); // kein Objekt da, 0.
      } );
  return(min(level)); //den kleinsten Wert im Array zurueckgeben (ggf. 0)
}


// debugkram
mixed _query_content() {
  //deep_copy, damit nicht jemand einfach so an den Daten rumbasteln kann.
  return(deep_copy(autoloader));
}

mixed _query_owner() {
  return(uuid);
}

int DeleteData(string uid) {
  if (clonep(ME)) return 0;
  if (!stringp(uid) || !sizeof(uid)) return -1;
  if (!ACCESS) return -2;
  //if (!ARCH_SECURITY) return -2;
  m_delete(data,uid);
  save_me();
  return(1);
}

mixed testfun() {return "bin da\n";}

public int HasAutoloader(string uuid, string loadname) {
  if (!sizeof(uuid) || !sizeof(loadname) || clonep() || !(uuid in data))
    return -1;

  return (loadname in data[uuid]);
}


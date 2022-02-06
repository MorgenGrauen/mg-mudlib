// MorgenGrauen MUDlib
//
// questmaster.c -- Questmaster, verwaltet die normalen Quests und
//                  die MiniQuests
//
// $Id: questmaster.c 9136 2015-02-03 21:39:10Z Zesstra $
//
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#include <config.h>
#include "/secure/wizlevels.h"
#include "/secure/questmaster.h"
#include "/secure/lepmaster.h"
#include "/secure/telnetneg.h" // P_TTY
#include <living/description.h> // P_LEVEL
#include <player/base.h> // P_TESTPLAYER
#include <daemon.h>
#include <ansi.h>
#include <events.h>

#define DEBUG(x) if (funcall(symbol_function('find_player),"arathorn"))\
          tell_object(funcall(symbol_function('find_player),"arathorn"),\
                      "QM: "+x+"\n")

#define ME this_object()
#define PL this_player()

#define MQ_DATA_POINTS 0
#define MQ_DATA_QUESTNO 1
#define MQ_DATA_TASKDESC 2
#define MQ_DATA_VISIBLE 3
#define MQ_DATA_ACTIVE 4
#define MQ_DATA_TITLE 5
#define MQ_DATA_DIARYTEXT 6
#define MQ_DATA_RESTRICTIONS 7
#define MQ_DATA_ASSIGNED_DOMAIN 8
#define MQ_DATA_QUERY_PERMITTED 9

private int max_QP = 0;
private int opt_QP = 0;
// Die Questliste mit allen Daten
private mapping quests = ([]);

// Das Mapping mit der MQ-Liste an sich und alle zugehoerigen Daten
private mapping miniquests = ([]);
// Nach MQ-Nummern indizierter Cache:
// Struktur ([int num : ({ int stupse, string mq-object }) ])
private nosave mapping by_num = ([]);
// Cache der Objekte, die die MQ-Listen der Regionen abfragen duerfen
// Struktur ([string path : ({ string mq_object }) ])
private nosave mapping mq_query_permitted = ([]);
// Cache fuer die MQ-Punkte von Spielern, wird fuer den jeweiligen Spieler
// beim Abfragen von dessen MQ-Punkten gefuellt. Spielername wird bei
// Aenderungen an seinen MQ-Punkten (Bestehen einer MQ, manuelles Setzen
// oder Loeschen einer seiner MQs) aus dem Cache ausgetragen
private nosave mapping users_mq = ([]);
// letzte vergebene MQ-Indexnummer. Es darf niemals eine MQ eine Indexnummer
// kriegen, die eine andere MQ schonmal hatte, auch wenn die geloescht wurde.
// (Zumindest nicht, ohne die entsprechenden Bits im Spieler zu loeschen, was
// zZ nicht passiert.
private int last_num = 0;


void save_info() {
  save_object(QUESTS);
}

// Caches aufbauen.
static void make_num(string mqob_name, int stupse, int index,
                     string taskdesc, int vis, int active, string title,
                     string donedesc, mapping restr, string domain, 
                     string *permitted_objs) {
  m_add(by_num, index, ({stupse, mqob_name}));
  foreach ( string obj: permitted_objs ) {
    if ( member(mq_query_permitted, obj) )
      mq_query_permitted[obj] += ({mqob_name});
    else
      mq_query_permitted[obj] = ({mqob_name});
  }
}

private void RebuildMQCache() {
  by_num = ([]);
  mq_query_permitted = ([]);
  walk_mapping(miniquests, #'make_num /*'*/ );
}

protected void create() {
  seteuid(getuid(ME));
  if (!restore_object(QUESTS)) {
    save_info();
  }

  RebuildMQCache();

  ({int})EVENTD->RegisterEvent(EVT_LIB_QUEST_SOLVED,"HandleQuestSolved",
      ME);
}

public int remove(int silent) {
  save_info();
  ({int})EVENTD->UnregisterEvent(EVT_LIB_QUEST_SOLVED, ME);
  destruct(ME);
  return 1;
}

// Schreibzugriff nur fuer interaktive EMs und ARCH_SECURITY.
private int allowed_write_access() {
  if (process_call())
    return 0;
  if (ARCH_SECURITY)  // prueft auch this_interactive() mit.
    return 1;
  return 0;
}

/*
 * (1) ABSCHNITT "NORMALE QUESTS"
 */

/* Die Quests werden in einem Mapping gespeichert. Der Schluessel ist dabei der
   Quest-Name, die Eintraege sind Arrays der folgenden Form:

   1. Element ist die Zahl der durch diese Quest zu erwerbenden Questpunkte.
   2. Element ist die Zahl der Erfahrungspunkte, die der Spieler bekommt,
      wenn er diese Quest loest.
   3. Element ist ein Array mit den Filenamen der Objekte, denen es gestattet
      ist, diese Quest beim Player als geloest zu markieren (Erzmagier duerfen
      das aber sowieso immer).
   4. Element ist ein String, der die Quest kurz beschreibt. Dieser String wird
      dem Spieler vom Orakel als Hinweis gegeben.
   5. Element ist eine Zahl zwischen -1 und 100, die den Schwierigkeitsgrad der
      Quest angibt, nach Einschaetzung des EM fuer Quests. Das Orakel kann dann
      evtl. sinnige Saetze wie "Diese Quest erscheint mir aber noch recht
      schwer fuer Dich.", oder "Hm, die haettest Du ja schon viel eher loesen
      koennen." absondern. :)

      Ein Wert von -1 bedeutet eine Seherquest. Diese zaehlt nicht zu den
      Maximalen Questpunkten, sondern zaehlt als optionale Quest
   6. Element ist ein Integer von 0 bis 5 und gibt die "Klasse" an;
      ausgegeben werden dort Sternchen
   7. Element ist ein Integer, 0 oder 1.
      0: Quest voruebergehend deaktiviert (suspendiert)
      1: Quest aktiviert
   8. Element ist ein String und enthaelt den Namen des Magiers, der die
      Quest "verbrochen" hat.
   9. Element ist ein String, der den Namen eines Magiers enthaelt, der
      evtl. fuer die Wartung der Quest zustaendig ist.
  10. Element ist eine Zahl von 0 bis 4, die der Quest ein Attribut
      gibt (0 fuer keines)
*/

// geaendert:
// 5  == diff geht nun von -1 bis 100
// 6  == klasse geht nun von 0 bis 5
// 10 == attribut geht nun von 0 bis 4

private int RecalculateQP() {
  int i;
  mixed q;

  if (!allowed_write_access())
    return -1;

  max_QP=0;
  opt_QP=0;

  q=m_values(quests);
    for (i=sizeof(q)-1;i>=0;i--)
      if (q[i][Q_ACTIVE]) {
        if (q[i][Q_DIFF]>=0)
          max_QP+=q[i][Q_QP];
        if (q[i][Q_DIFF]==-1)
          opt_QP+=q[i][Q_QP];
      }

  return max_QP+opt_QP;
}

int AddQuest(string name, int questpoints, int experience,
      string *allowedobj, string hint, int difficulty, int questclass,
      int active, string wiz, string scndwiz, int questattribute)
{
  int i;

  if (!allowed_write_access()) return 0;
  if (!stringp(name) || sizeof(name)<5) return -1;
  if (questpoints<1) return -2;
  if (!intp(experience)) return -3;
  if (!pointerp(allowedobj)) return -4;
  for (i=sizeof(allowedobj)-1;i>=0;i--)
    {
      if (!stringp(allowedobj[i]) || allowedobj[i]=="") return -4;
      allowedobj[i]=({string})master()->make_path_absolute(allowedobj[i]);
    }
  if (!stringp(hint) || hint=="") return -5;
  if (difficulty<-1 || difficulty>100) return -6;
  if (questclass<0 || questclass>5) return -11;
  if (active<0 || active>1) return -7;
  if (!stringp(wiz) || wiz=="" ||
      file_size("/players/"+(wiz=lower_case(wiz))) != -2) return -8;
  if (!stringp(scndwiz))
    scndwiz="";
  else if (file_size("/players/"+(scndwiz=lower_case(scndwiz))) != -2)
    return -9;
  if (questattribute<0 || questattribute>4)
    return -10;

  if(quests[name]&&(quests[name][5]==0||quests[name][5]==1)&&quests[name][6])
    max_QP-=quests[name][0];

  quests+=([name: ({questpoints,experience,allowedobj,hint,difficulty,
                    questclass,active,wiz, scndwiz,questattribute,
                    ({0.0,0}) }) ]);
  RecalculateQP();
  save_info();
  QMLOG(sprintf("add: %s %O (%s)",name,quests[name],
                getuid(this_interactive())));
  return 1;
}

int RemoveQuest(string name) {
  if (!allowed_write_access()) return 0;
  if (!quests[name]) return -1;
  QMLOG(sprintf("remove: %s %O (%s)",name,quests[name],
                getuid(this_interactive())));
  m_delete(quests,name);
  RecalculateQP();
  save_info();
  return 1;
}

int QueryNeededQP() {
  return REQ_QP;
}

int QueryMaxQP() {
  return max_QP;
}

int QueryOptQP() {
  return opt_QP;
}

int QueryTotalQP() {
  return max_QP+opt_QP;
}

mixed *QueryGroupedKeys() {
  string *qliste;
  mixed  *qgliste;
  int i, j;

  qgliste = allocate(sizeof(QGROUPS)+1); // letzte Gruppe sind die Seherquests
  qliste = m_indices(quests);

  for (i=sizeof(qgliste)-1;i>=0;i--)
    qgliste[i]=({});

  for (i=sizeof(qliste)-1;i>=0;i--)
    {
      // inaktive quest?
      if (!quests[qliste[i]][Q_ACTIVE])
        continue;
      // optionale quest? also Seherquest
        if (quests[qliste[i]][Q_DIFF]==-1)
          qgliste[sizeof(QGROUPS)] += ({qliste[i]});
        else {
          // dann haben wir also eine normale Quest und daher Einordnung
          // nach dem Schwierigkeitswert
          for (j=sizeof(QGROUPS)-1;
               j>=0 && QGROUPS[j]>=quests[qliste[i]][Q_DIFF];j--)
            ;
          qgliste[j] += ({qliste[i]});
        }
    }
  return qgliste;
}


// folgende funk brauch ich glaube ich nicht mehr:
int QueryDontneed(object pl) {
  raise_error("Ich glaub, die Func QueryDontneed() braucht kein Mensch mehr. "
  "(Zook)");
}

// Die folgende Func braucht man nicht mehr
int QueryReadyForWiz(object player) {
  raise_error("Die Func QueryReadyForWiz() braucht keiner mehr. (Zook)");
}

mixed *QueryQuest(string name) {
  if(!quests[name])
    return ({});
  if( extern_call() )
    return deep_copy( quests[name] );
  return quests[name];
}

int QueryQuestPoints(string name) {
  if( !quests[name] )
    return -1;

  return quests[name][Q_QP];
}

mixed *QueryQuests() {
  if( extern_call() )
    return ({m_indices(quests),map(m_values(quests),#'deep_copy /*'*/)});
  return ({ m_indices(quests), m_values(quests) });
}

string *QueryAllKeys() {
  return m_indices(quests);
}

int SetActive(string name, int flag) {
  mixed *quest;

  if (!allowed_write_access()) return 0;
  if (!(quest=quests[name])) return -1;
  switch(flag)
    {
    case 0:
      if (quest[Q_ACTIVE] == flag)
        return -2;
      quest[Q_ACTIVE] = flag;
      break;
    case 1:
      if (quest[Q_ACTIVE] == flag)
        return -2;
      quest[Q_ACTIVE] = flag;
      break;
    default:
      return -3;
    }
  quests[name]=quest;
  RecalculateQP();
  save_info();
  QMLOG(sprintf("%s: %s (%s)",(flag?"activate":"deactivate"),name,
                getuid(this_interactive())));
  return 1;
}

string name() {
  return "<Quest>";
}
string Name() {
  return "<Quest>";
}

void Channel(string msg) {
  if(!interactive(previous_object()))
    return;
  catch(({int})CHMASTER->send("Abenteuer", ME, msg);publish);
}

 /* quoted from /sys/mail.h: */
#define MSG_FROM 0
#define MSG_SENDER 1
#define MSG_RECIPIENT 2
#define MSG_CC 3
#define MSG_BCC 4
#define MSG_SUBJECT 5
#define MSG_DATE 6
#define MSG_ID 7
#define MSG_BODY 8

void SendMail(string questname, mixed *quest, object player) {
  mixed* mail;
  string text;

  mail = allocate(9);

  text =
    "Hallo "+capitalize(getuid(player))+",\n\n"+
    break_string("Nachdem Du gerade eben das Abenteuer '"+
                 questname +"' ("+quest[Q_QP]+" Punkte), das "+
                 capitalize(quest[Q_WIZ])+" fuer das "MUDNAME" entworfen hat, "
                 "mit Erfolg bestanden hast, sind "
                 "wir nun an Deiner Meinung dazu interessiert:", 78)+
    "\n  Hat Dir das Abenteuer gefallen und wieso bzw. wieso nicht?\n"
    "  Ist die Einstufung Deiner Meinung nach richtig? (AP und Stufe)\n"
    "  Gab es Probleme oder gar Fehler?\n"
    "  Hast Du Verbesserungsvorschlaege?\n\n";

  text += break_string("Diese Nachricht wurde automatisch verschickt, "
        "wenn Du mit dem 'r' Kommando darauf antwortest, geht die Antwort "
        "direkt an Zook als zustaendigem Erzmagier fuer Abenteuer.\n",78);

  if (quest[Q_SCNDWIZ]!="") {
    text += break_string(
        "Falls Du mit dem Magier sprechen willst, der zur Zeit das "
        "Abenteuer technisch betreut, kannst Du Dich an "
        +capitalize(quest[Q_SCNDWIZ])+ " wenden.",78);
  }

  mail[MSG_FROM] = "Zook";
  mail[MSG_SENDER] = "Zook";
  mail[MSG_RECIPIENT] = getuid(player);
  mail[MSG_CC]=0;
  mail[MSG_BCC]=0;
  mail[MSG_SUBJECT]="Das Abenteuer: "+questname;
  mail[MSG_DATE]=dtime(time());
  mail[MSG_ID]=MUDNAME":"+time();
  mail[MSG_BODY]=text;

  ({string*})"/secure/mailer"->DeliverMail(mail,0);
  return;
}

static int compare (mixed *i, mixed *j) {
  if (i[4] == j[4])
    return i[1] > j[1];
  else
    return i[4] > j[4];
}

#define FILTER_GELOEST   1
#define FILTER_UNGELOEST 2
varargs string liste(mixed pl, int geloest_filter)
{
  int qgroups, i, j;
  mixed *qlists, *qgrouped, *qtmp;
  string str;
  string ja, nein, format, ueberschrift;

  if(!objectp(pl))
    if(stringp(pl))
      pl=find_player(pl) || find_netdead(pl);
  if(!objectp(pl))
    pl=PL;
  if(!objectp(pl))
    return "Ohne Spielernamen/Spielerobjekt gibt es auch keine Liste.\n";

  if ( (({string})pl->QueryProp(P_TTY)) == "ansi")
  {
      ja = ANSI_GREEN + "ja" + ANSI_NORMAL;
      nein = ANSI_RED + "nein" + ANSI_NORMAL;
  }
  else
  {
      ja = "ja";
      nein = "nein";
  }

  str = "";
  // Festlegen des Ausgabeformates
  format = "%=-:30s %:3d %-:6s  %-:9s %:2s/%-:3s  %-:12s %-s\n";
  ueberschrift = sprintf("%-:30s %:3s %-:6s  %-:9s %-:6s  %-:12s %-:4s\n",
                 "Abenteuer", "AP", "Klasse", "Attribut",
                 "Stufe", "Autor", "Gel?");

  qgroups = sizeof(QGROUPS);
  qlists = allocate( qgroups+1 );
  for( i=qgroups; i>=0; i-- )
    qlists[i] = ({});

  qgrouped = QueryGroupedKeys();

  for (i=sizeof(qgrouped)-1;i>=0; i--)
    for (j=sizeof(qgrouped[i])-1;j>=0; j--)
    {
      qtmp = QueryQuest(qgrouped[i][j]);
      int geloest_status = ({int})pl->QueryQuest(qgrouped[i][j]);
      // Quest ausgeben, wenn "kein Filter" gegeben oder Quest geloest und
      // Filter "geloest" oder Quest ungeloest und Filter "ungeloest".
      if ( !(geloest_filter & (FILTER_GELOEST|FILTER_UNGELOEST))
          || ((geloest_filter & FILTER_GELOEST) && geloest_status == OK)
          || ((geloest_filter & FILTER_UNGELOEST) && geloest_status != OK)
         )
      {
        qlists[i] += ({ ({
          qgrouped[i][j],
          qtmp[Q_QP],
          QCLASS_STARS(qtmp[Q_CLASS]),
          capitalize(QATTR_STRINGS[qtmp[Q_ATTR]]),
          qtmp[Q_DIFF],
          (qtmp[Q_AVERAGE][1]>10
                    ? to_string(to_int(qtmp[Q_AVERAGE][0]))
                    : "-"),
          capitalize(qtmp[Q_WIZ]),
          geloest_status == OK ? ja : nein
        }) });
      }
    }

  for( i=0; i<qgroups; i++ )
  {
    if (sizeof(qlists[i])) {
      str += "\n" + ueberschrift;
      str += sprintf("Stufen %d%s:\n",
                     QGROUPS[i]+1,
                     i==qgroups-1?"+":sprintf("-%d", QGROUPS[i+1]));
      qlists[i] = sort_array( qlists[i], "compare", ME );
      for( j=0; j<sizeof(qlists[i]); j++ ) {
        if(qlists[i][j][Q_DIFF]>=0)
          str += sprintf( format,
                          qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
                          qlists[i][j][3], sprintf("%d",qlists[i][j][4]),
                          qlists[i][j][5],
                          qlists[i][j][6], qlists[i][j][7]);
      }
      str += "\n\n";
    }
  }

  qlists[qgroups] = sort_array(qlists[qgroups], "compare", ME);
  i = qgroups;
  if (sizeof(qlists[i])) {
    str += "\n" + ueberschrift;
    str += "Nur fuer Seher:\n";
    for( j=0; j<sizeof(qlists[qgroups]); j++ )  {
      if(qlists[i][j][Q_DIFF]==-1)
        str += sprintf( format,
                        qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
                        qlists[i][j][3], "S", qlists[i][j][5],
                        qlists[i][j][6],
                        qlists[i][j][7]);
    }
  }

  str +=
    "\nEine Erklaerung der einzelnen Spalten findest Du unter "
    "\"hilfe abenteuerliste\".\n";

  return str;
}


// mitloggen, mit welchen durchschnittlichen Leveln Quests so geloest
// werden...
void HandleQuestSolved(string eid, object trigob, mixed data) {
  string qname = data[E_QUESTNAME];

  if (!quests[qname] || !objectp(trigob)
      || ({int|string})trigob->QueryProp(P_TESTPLAYER) || IS_LEARNER(trigob))
    return;

  int lvl = ({int})trigob->QueryProp(P_LEVEL);

  if (lvl <= 0)
    return;

  // neuen Durchschnitt berechen.
  mixed tmp = quests[qname][Q_AVERAGE];
  float avg = tmp[0];
  int count = tmp[1];
  avg *= count;
  avg += to_float(lvl);
  tmp[1] = ++count;
  tmp[0] = avg / count;

  DEBUG(sprintf("%s: %f (%d)\n",qname,
        quests[qname][Q_AVERAGE][0],
        quests[qname][Q_AVERAGE][1]));

  save_info();
}

/*
 * (2) ABSCHNITT "MINI" QUESTS
 */

int ClearUsersMQCache() {
  if (!allowed_write_access())
    return 0;

  users_mq = ([]);

  return 1;
}

mixed QueryUsersMQCache() {
  if (!allowed_write_access())
    return 0;

  return users_mq;
}

/* Beschreibung
 *
 * Die MiniQuests werden in einem Mapping gespeichert.
 *
 * Der Key ist dabei der Name des Objektes, das die Quest im Spieler
 * markieren darf. Die Daten zu den Miniquests stehen in den Value-Spalten
 * des Mappings.
 *
 * 1. Spalte ist die Zahl der durch diese Quest zu erwerbenden Stufenpunkte
 * 2. Spalte ist die Nummer unter der die MiniQuest gefuehrt wird.
 * 3. Spalte ist ein String, der die Quest(aufgabe) kurz beschreibt.
 * 4. Spalte ist ein Integer, 0 oder 1:
 *     0 : Quest ist fuer Spieler nicht sichtbar
 *     1 : Quest ist fuer Spieler z.B. bei einer Anschlagtafel sichtbar
 *     Fuer Spieler unsichtbare MQs sollte es aber nicht mehr geben!
 * 5. Spalte ist ein Integer, 0 oder 1:
 *     0 : Quest voruebergehend deaktiviert
 *     1 : Quest aktiviert
 * 6. Spalte ist ein String, der den Kurztitel der Miniquest enthaelt
 * 7. Spalte ist ein String, der eine kurze Beschreibung dessen enthaelt,
 *     was der Spieler im Verlauf der Miniquest erlebt hat.
 * 8. Spalte ist ein Mapping, dessen Eintraege analog zu P_RESTRICTIONS
 *     gesetzt werden koennen, um anzugeben, welche Voraussetzungen erfuellt
 *     sein muessen, bevor ein Spieler diese Quest beginnen kann.
 * 9. Spalte ist die Zuordnung der MQ zu den Regionen; dies wird von der
 *    Bibliothek in der Fraternitas (/d/ebene/miril/fraternitas/room/bibliothek) 
 *    ausgewertet. Diese kennt (Stand 2019-07-30) folgende Regionen: 
 *    #define REGIONEN ({"wald","ebene","polar","wueste","inseln",\
 *                  "unterwelt","fernwest","dschungel","gebirge"})
 *    D.h. MiniQuests im Verlorenen Land sollten als "inseln" geschluesselt
 *    werden. 
 *10. Spalte ist ein Array aus Strings, das die Objekte enthaelt, die
 *    die Daten dieser Quest abfragen duerfen, um sie an Spieler auszugeben.
 */

int DumpMiniQuests(object who) {
  int sum_points;

  if (extern_call() && !allowed_write_access())
    return 0;

  if ( !objectp(who) || !query_once_interactive(who))
    who = this_interactive();

  MQMLOG(sprintf("DumpMiniQuests: PO: %O, TI: %O", previous_object(), who));
  rm(MQ_DUMP_FILE);

  write_file(MQ_DUMP_FILE, "MINIQUESTS: ("+dtime(time())+")\n\n"+
    "  Nr  Pkt  vis akt vergebendes Objekt\n");
  string *msg = ({});

  foreach(string obname, int stupse, int nummer, mixed descr, int vis,
      int active /*, string title, string donedesc, mapping restrictions,
      string domain, string *permitted_objs*/: miniquests)
  {
    msg += ({ sprintf("%4d %4d %4d %4d %s",
      nummer, stupse, vis, active, obname)});
    sum_points += stupse;
  }

  write_file(MQ_DUMP_FILE, implode(sort_array(msg, #'> /*'*/), "\n"));
  write_file(MQ_DUMP_FILE, sprintf("\n\n"
             "============================================================\n"
             +"MiniQuests: %d Miniquests mit %d Punkten.\n\n",
              sizeof(miniquests), sum_points));
  return 1;
}

public int AddMiniQuest(int mquestpoints, string allowedobj, string descr,
            int active, string title, string donedesc, mapping restrictions,
            string domain, string *permitted_objs) {

  if (!allowed_write_access())
    return 0;

  // Parameterpruefung: Questgeber, Restrictions, Region, Titel und
  // zugelassene Abfrageobjekte muessen gueltig angegeben werden, alles
  // weitere wird unten ggf. ausgenullt/korrigiert.
  if (!stringp(allowedobj) || !sizeof(allowedobj) || !mappingp(restrictions)
      || !stringp(domain) || !stringp(title) || !pointerp(permitted_objs))
    return -1;

  // Miniquest mit weniger als 1 Stups einzutragen ist wohl unsinnig.
  if (mquestpoints<1)
    return -2;

  // Mindestens ein Objekt muss eingetragen werden, das Spielern Informationen
  // ueber die Aufgabenstellung der MQ geben darf.
  if ( !sizeof(permitted_objs) )
    return -3;

  // Pruefen, ob die als Questgeber angegebene Datei existiert.
  if (allowedobj[<2..] == ".c")
    allowedobj = allowedobj[0..<3];
  allowedobj = explode(allowedobj, "#")[0];
  allowedobj = ({string})master()->make_path_absolute(allowedobj);
  if (file_size(allowedobj+".c") <=0)
    return -3;

  // Vergibt das angegebene Objekt schon eine MQ? Dann abbrechen.
  if (member(miniquests,allowedobj))
    return -4;

  if (!stringp(descr) || !sizeof(descr))
    descr = 0;
  if (!stringp(donedesc) || !sizeof(donedesc))
    donedesc = 0;

  // Eintrag hinzufuegen, visible ist per Default immer 1.
  // MQ-Nummer hochzaehlen
  int nummer = last_num + 1;
  m_add(miniquests, allowedobj, mquestpoints, nummer, descr, 1, active,
    title, donedesc, restrictions, lower_case(domain), permitted_objs);
  // und nummer als last_num merken.
  last_num = nummer;
  save_info();

  MQMLOG(sprintf("AddMiniQuest: %s %O (%s)", allowedobj, miniquests[allowedobj],
                 getuid(this_interactive())));

  ClearUsersMQCache();
  RebuildMQCache();
  if (find_call_out(#'DumpMiniQuests) == -1)
    call_out(#'DumpMiniQuests, 60, this_interactive());
  return 1;
}

int RemoveMiniQuest(string name) {
  if (!allowed_write_access())
    return 0;
  // Gibt es einen solchen Eintrag ueberhaupt?
  if (!member(miniquests,name))
    return -1;

  MQMLOG(sprintf("RemoveMiniQuest: %s %O (%s)",
    name, m_entry(miniquests, name), getuid(this_interactive())));

  // MQ aus der Miniquestliste austragen.
  m_delete(miniquests, name);
  save_info();

  // MQ-Punkte-Cache loeschen, da nicht feststellbar ist, welcher der
  // dort eingetragenen Spieler die gerade ausgetragene MQ geloest hatte.
  ClearUsersMQCache();
  RebuildMQCache();
  if (find_call_out(#'DumpMiniQuests) == -1)
    call_out(#'DumpMiniQuests, 60, this_interactive());
  return 1;
}

int ChangeMiniQuest(mixed mq_obj, int param, mixed newvalue) {
  if (!allowed_write_access())
    return 0;

  // MQ weder als Pfad, noch als Indexnummer angegeben?
  if ( !stringp(mq_obj) && !intp(mq_obj) && !intp(param))
    return MQ_KEY_INVALID;

  // gewaehlter Parameter ungueltig?
  if ( param < MQ_DATA_POINTS || param > MQ_DATA_QUERY_PERMITTED )
    return MQ_KEY_INVALID;

  // Indexnummer der MQ in den Pfad umwandeln
  if ( intp(mq_obj) )
    mq_obj = by_num[mq_obj][1];

  // Vergebendes Objekt nicht gefunden? Bloed, das brauchen wir naemlich.
  if (!stringp(mq_obj))
    return MQ_KEY_INVALID;

  if ( !member(miniquests, mq_obj) )
    return MQ_ILLEGAL_OBJ;

  switch(param) {
    // MQ_DATA_QUESTNO ist nicht aenderbar, daher hier nicht behandelt, so
    // dass Fallback auf default erfolgt.
    // Stufenpunkte muessen Integers sein.
    case MQ_DATA_POINTS:
      if ( !intp(newvalue) || newvalue < 1 )
        return MQ_KEY_INVALID;
      break;
    // Aufgabenbeschreibung, Titel, "geschafft"-Text und zugeordnete Region
    // muessen Strings sein
    case MQ_DATA_TASKDESC:
    case MQ_DATA_TITLE:
    case MQ_DATA_DIARYTEXT:
    case MQ_DATA_ASSIGNED_DOMAIN:
      if ( !stringp(newvalue) || !sizeof(newvalue) ) 
        return MQ_KEY_INVALID;
      if (param == MQ_DATA_ASSIGNED_DOMAIN)
        newvalue = lower_case(newvalue);
      break;
    // das Sichtbarkeits- und das aktiv/inaktiv-Flag muessen 0/1 sein.
    case MQ_DATA_VISIBLE:
    case MQ_DATA_ACTIVE:
      if ( !intp(newvalue) || newvalue < 0 || newvalue > 1 )
        return MQ_KEY_INVALID;
      break;
    // Die Voraussetzungen muessen als Mapping eingetragen werden, das aber
    // leer oder Null sein kann, wenn es keine Restriktionen gibt.
    case MQ_DATA_RESTRICTIONS:
      if ( !mappingp(newvalue) && newvalue != 0 )
        return MQ_KEY_INVALID;
      break;
    // Regionszuordnung muss ein nicht-leeres Array sein, das nur aus Strings
    // bestehen darf, die nicht leer sein duerfen.
    case MQ_DATA_QUERY_PERMITTED:
      if ( pointerp(newvalue) ) {
        newvalue = filter(filter(newvalue, #'stringp), #'sizeof);
        if (!sizeof(newvalue))
          return MQ_KEY_INVALID;
      }
      else
        return MQ_KEY_INVALID;
      break;
    default:
      return MQ_KEY_INVALID;
  }

  mixed *altemq = m_entry(miniquests, mq_obj);
  miniquests[mq_obj, param] = newvalue;

  save_info();

  MQMLOG(sprintf("ChangeMiniQuest: %s from %O to %O (%s)", mq_obj,
    altemq, m_entry(miniquests, mq_obj), getuid(this_interactive())));

  // nur bei einigen Aenderungen muessen die Caches angefasst werden.
  switch(param) {
    case MQ_DATA_POINTS:
      ClearUsersMQCache();
      // Fallthrough - in diesem Fall muss auch der MQ-Cache neu erstellt
      // werden.
    case MQ_DATA_QUERY_PERMITTED:
      RebuildMQCache();
      break;
  }

  if (find_call_out(#'DumpMiniQuests) == -1)
    call_out(#'DumpMiniQuests, 60, this_interactive());
  return 1;
}

mixed QueryMiniQuestByName(string name) {
  if (!allowed_write_access())
    return 0;
  return deep_copy(miniquests & ({name}));
}

mixed QueryMiniQuestByNumber(int nummer) {
  // Zugriffsabsicherung erfolgt dort auch, daher hier unnoetig
  return (by_num[nummer]?QueryMiniQuestByName(by_num[nummer][1]):0);
}

// Das vollstaendige MQ-Mapping nur als Kopie ausliefern.
mixed QueryMiniQuests() {
  return allowed_write_access() ? deep_copy(miniquests) : 0;
}

// De-/Aktivieren einer Miniquest, wirkt als Umschalter, d.h. eine aktive
// MQ wird durch Aufruf dieser Funktion als inaktiv markiert und umgekehrt.
int SwitchMiniQuestActive(string name) {
  if (!allowed_write_access())
    return -1;
  // Haben wir eine solche MQ ueberhaupt?
  if (!member(miniquests, name))
    return -2;

  // active-Flag invertieren
  miniquests[name, MQ_DATA_ACTIVE] = !miniquests[name, MQ_DATA_ACTIVE];
  save_info();

  MQMLOG(sprintf("%s: %s (%s)",
    (miniquests[name,MQ_DATA_ACTIVE]?"Activate":"Deactivate"), name,
    getuid(this_interactive()))
  );
  return miniquests[name,MQ_DATA_ACTIVE];
}

int GiveMiniQuest(object winner) {
  // Spieler muss existieren und interactive sein.
  if (!winner ||
      (this_interactive() && (this_interactive() != winner)) ||
      ((this_player() == winner) && !query_once_interactive(winner)))
    return MQ_ILLEGAL_OBJ;
  // Gaeste koennen keine Miniquests bestehen.
  if (({int})winner->QueryGuest())
    return MQ_GUEST;
  // Aufrufendes Objekt existiert gar nicht?
  if (!previous_object())
    return MQ_ILLEGAL_OBJ;

  string objname = load_name(previous_object());
  // Miniquest muss existieren
  if (!member(miniquests,objname))
    return MQ_KEY_INVALID;
  // Inaktive Miniquests koennen nicht vergeben werden.
  if (!miniquests[objname, MQ_DATA_ACTIVE])
    return MQ_IS_INACTIVE;

  string mq = (({string})MASTER->query_mq(getuid(winner)) || "");

  // Spieler hat die MQ schonmal bestanden? Dann keine weiteren Aktivitaet
  // noetig
  if (test_bit(mq, miniquests[objname, MQ_DATA_QUESTNO]))
    return MQ_ALREADY_SET;

  catch(mq = set_bit(mq, miniquests[objname,MQ_DATA_QUESTNO]);publish);
  ({int})MASTER->update_mq(getuid(winner), mq);

  MQSOLVEDLOG(sprintf("%s: %s, (#%d), (Stupse %d)",
    objname, geteuid(winner), miniquests[objname, MQ_DATA_QUESTNO],
    miniquests[objname, MQ_DATA_POINTS]));

  // Miniquest-Event ausloesen
  ({int})EVENTD->TriggerEvent( EVT_LIB_MINIQUEST_SOLVED, ([
            E_OBJECT: previous_object(),
            E_OBNAME: objname,
            E_PLNAME: getuid(winner),
            E_MINIQUESTNAME: miniquests[objname, MQ_DATA_TITLE] ]) );

  // Spielereintrag aus dem MQ-Punkte-Cache loeschen
  m_delete(users_mq, getuid(winner));

  return 1;
}

int QueryMiniQuestPoints(mixed pl) {
  string spieler;

  //if (!allowed_write_access())
  //  return 0;

  if (!pl)
    return -1;

  if (!objectp(pl) && !stringp(pl))
    return -2;

  if (objectp(pl) && !query_once_interactive(pl))
    return -3;

  if (objectp(pl))
    spieler = getuid(pl);
  else
    spieler = pl;

  if (!member(users_mq, spieler)) {
    int mqpoints;
    int p=-1;
    string s = (({string})MASTER->query_mq(spieler) || "");
    while( (p=next_bit(s, p)) != -1) {
      mqpoints+=by_num[p][0];
    }
    users_mq[spieler] = mqpoints;
  }
  return users_mq[spieler];
}

int HasMiniQuest(mixed pl, mixed name) {
  string mq, spieler;

  if (!pl || !name)
    return MQ_ILLEGAL_OBJ;

  if (!objectp(pl) && !stringp(pl))
    return MQ_ILLEGAL_OBJ;

  if (objectp(pl) && !query_once_interactive(pl))
    return MQ_ILLEGAL_OBJ;

  if (!objectp(name) && !stringp(name) && !intp(name))
    return MQ_ILLEGAL_OBJ;

  if (objectp(name))
    name = explode(object_name(name), "#")[0];

  if ( intp(name) )
    name = by_num[name][1];

  if (objectp(pl))
    spieler = getuid(pl);
  else
    spieler = pl;

  if (!member(miniquests,name))
    return MQ_KEY_INVALID;

  mq = (({string})MASTER->query_mq(spieler) || "");

  return test_bit(mq, miniquests[name, MQ_DATA_QUESTNO]);
}

// Zum Von-Hand-Setzen der MiniQuests
int SetPlayerMiniQuest(string pl, string name) {
  if(!allowed_write_access())
    return 0;
  if(!pl)
    return MQ_ILLEGAL_OBJ;
  if(!previous_object())
    return MQ_ILLEGAL_OBJ;

  if (!member(miniquests,name))
    return MQ_KEY_INVALID;

  string mq = (({string})MASTER->query_mq(pl) || "");

  if (test_bit(mq, miniquests[name,MQ_DATA_QUESTNO]))
    return MQ_ALREADY_SET;

  catch (mq = set_bit(mq, miniquests[name, MQ_DATA_QUESTNO]);publish);
  ({int})MASTER->update_mq(pl, mq);

  MQMLOG(sprintf("SetPlayerMiniQuest: %s %s (%s)",
                 pl, name, getuid(this_interactive())));
  // Spielereintrag aus dem MQ-Punkte-Cache loeschen
  m_delete(users_mq, pl);
  return 1;
}

int ClearPlayerMiniQuest(string pl, string name) {
  if (!allowed_write_access())
    return 0;
  if (!pl)
    return MQ_ILLEGAL_OBJ;
  if (!previous_object())
    return MQ_ILLEGAL_OBJ;

  if (!member(miniquests,name))
    return MQ_KEY_INVALID;

  string mq = (({string})MASTER->query_mq(pl) || "");

  if (!test_bit(mq, miniquests[name, MQ_DATA_QUESTNO]))
    return MQ_ALREADY_SET;

  catch (mq = clear_bit(mq, miniquests[name, MQ_DATA_QUESTNO]);publish);
  ({int})MASTER->update_mq(pl, mq);

  MQMLOG(sprintf("ClearPlayerMiniQuest: %s %s (%s)",
                 pl, name, getuid(this_interactive())));
  // Spielereintrag aus dem MQ-Punkte-Cache loeschen
  m_delete(users_mq, pl);
  return 1;
}

// Umtragen von Miniquests von Objekt <old> auf Objekt <new>
int MoveMiniQuest(string old_mqob, string new_mqob) {
  if ( !allowed_write_access() )
    return -1;

  // Haben wir ueberhaupt einen solchen Eintrag?
  if ( !member(miniquests, old_mqob) )
    return -2;

  // Pruefen, ob die als <new_mqob> angegebene Datei existiert.
  if (new_mqob[<2..] == ".c")
    new_mqob = new_mqob[0..<3];
  new_mqob = explode(new_mqob, "#")[0];
  new_mqob = ({string})master()->make_path_absolute(new_mqob);
  if (file_size(new_mqob+".c") <= 0)
    return -3;
  // Wenn das neue Objekt schon eine MQ vergibt, kann es keine weitere
  // annehmen.
  if ( member(miniquests, new_mqob) )
    return -4;

  // Der Miniquestliste einen neuen Key "new" mit den Daten des alten Keys
  // hinzufuegen. m_entry() liefert alle Values dazu als Array, und der
  // flatten-Operator "..." uebergibt dessen Elemente als einzelne Parameter.
  m_add(miniquests, new_mqob, m_entry(miniquests, old_mqob)...);
  m_delete(miniquests, old_mqob);
  // Nummer-index und Cache fuer Abfrageobjekte neu erstellen. Das by_num
  // koennte effizient mitgeaendert werden, aber das Mapping
  // mq_query_permitted ist unschoen zu bereinigen, weil man eh ueber alle
  // Keys laufen muss. Also einmal alles komplett neu.
  RebuildMQCache();
  return 1;
}

#define FRA_BIB "/d/ebene/miril/fraternitas/room/bibliothek"

// Erlaubt die Abfrage aller MQs einer bestimmten Region fuer die Bibliothek
// der kleinen und grossen Heldentaten in der Fraternitas.
// Gibt ein Mapping der Form ([ indexnummer : titel; erledigt_Beschreibung ])
// zurueck.
mapping QuerySolvedMQsByDomain(mixed pl, string region) {
  if ( !objectp(pl) && !stringp(pl) && 
       load_name(previous_object())!=FRA_BIB) /*|| !allowed_write_access())*/
    return ([:2]);

  mapping res = m_allocate(30,2);   // reicht vermutlich
  // Die angegebene Region muss in der Spalte MQ_DATA_ASSIGNED_DOMAIN
  // enthalten sein, und das abfragende Objekt muss die Fraternitas-Bib sein
  foreach(string mqobj, int mqp, int index, string task, int vis, int act,
    string title, string donedesc, mapping restr, string domain:
    miniquests) {
      // aktive MQs der angeforderten Region zusammentragen, die der Spieler
      // bestanden hat.
      if ( domain == region && act && HasMiniQuest(pl, mqobj) )
        m_add(res, index, title, donedesc);
  }
  //DEBUG(sprintf("%O\n",res));
  return res;
}
#undef FRA_BIB

// Abfrage der noch offenen MQs des angegebenen Spielers.
// Zurueckgegeben wird ein Mapping mit den Miniquest-Nummern als Keys und
// den Aufgabenbeschreibungen als Values, oder ein leeres Mapping, falls
// das abfragende Objekt keine Zugriffsberechtigung hat, oder das 
// uebergebene Spielerobjekt keine offenen Miniquests mehr hat.
mapping QueryOpenMiniQuestsForPlayer(object spieler) {
  // map() etwas beschleunigen
  closure chk_restr = symbol_function("check_restrictions",
                                      "/std/restriction_checker");
  // Cache-Eintrag fuer das abfragende Objekt holen
  string *list = mq_query_permitted[load_name(previous_object())];
  mapping res = ([:2]); 
  
  if (!pointerp(list) || !sizeof(list))
    return res;
  // Liste der MQ-Objekte umwandeln in deren MQ-Nummer plus 
  // Aufgabenbeschreibung, sofern der Spieler die MQ noch nicht bestanden
  // hat, aber die Voraussetzungen erfuellt.
  foreach ( string mq_obj : list ) 
  {
    // Ist die MQ nicht aktiv, ist der Rest hinfaellig.
    if(!miniquests[mq_obj,MQ_DATA_ACTIVE]) continue;

    // Nur wenn der Spieler die MQ noch nicht hat, kann er ueberhaupt einen
    // Tip dazu bekommen.
    if ( !HasMiniQuest(spieler, mq_obj) ) {
      // Restriction Checker fragen, ob der Spieler statt des Hinweises
      // eine Info bekommen muss, dass er eine Vorbedingung nicht erfuellt.
      string restr_result = funcall(chk_restr, spieler, miniquests[mq_obj,
        MQ_DATA_RESTRICTIONS]);
      // Wenn so eine Info dabei rauskommt, wird diese in die Ergebnisliste
      // uebernommen. In diesem Fall wird KEIN MQ-Hinweistext ausgeliefert.
      if ( stringp(restr_result) )
      {
        m_add(res, miniquests[mq_obj,MQ_DATA_QUESTNO], 0, restr_result);
      }
      // Anderenfalls wird der Hinweistext uebernommen; einen Eintrag 
      // bzgl. eines eventuellen Hinderungsgrundes gibt's dann nicht.
      else
      {
        m_add(res, miniquests[mq_obj,MQ_DATA_QUESTNO], 
          miniquests[mq_obj,MQ_DATA_TASKDESC], 0);
      }
    }
  }
  // Ergebnisliste zurueckgeben.
  return res;
}

// Datenkonverter fuer das bisherige MQ-Mapping von
// ([ obj : ({daten1..daten5}) ]) nach
// ([ obj : daten1; daten2; ...; daten9 ]), wobei daten6..9 als Leerspalten
// erzeugt werden.
/*void ConvertMQData() {
  if ( !allowed_write_access() )
    return;

  by_num=([]);
  // spaltenweise aus dem miniquests-Mapping ein Array aus Arrays erzeugen
  // Zunaechst die Keys des Mappings aufnehmen.
  mixed *mqdata = ({m_indices(miniquests)});

  // Dann das Datenarray spaltenweise dazu (jedes Array ist eine Spalte).
  mqdata += transpose_array(m_values(miniquests));
  // 1. Array: Keys, alle weiteren jeweils eine Wertespalte

  // Array erzeugen als Array aus 5 Array-Elementen, die alle soviele
  // Nullen enthalten, wie es Miniquests gibt,
  // ({ ({0,0,...}), ({0,0,...}), ({0,0,...}), ({0,0,...}), ({0,0,...}) })
  // dieses hinzufuegen. Sind die hinzukommenden 5 Spalten.
  mqdata += allocate(5, allocate(sizeof(miniquests),0));

  // Mapping erzeugen, indem mit dem flatten-Operator "..." die Einzel-
  // Arrays des Datenpakets uebergeben werden. Erzeugt auf diese Weise
  // ([ keys : daten1; daten2; daten3; daten4; daten5; 0; 0; 0; 0; 0,])
  miniquests=mkmapping(mqdata...);
}

// Neue Daten einlesen, Visible-Flag bei allen MQs per Default auf 1 setzen.
// Es gibt keine wirklich unsichtbaren MQs mehr.
void ReadNewData() {
  if ( !allowed_write_access() )
    return;

  string *import = explode(read_file("/players/arathorn/mqdata"),"\n")-({""});
  string *fields;
  foreach(string mqdata : import) {
    fields = explode(mqdata, "#")-({""});
    DEBUG(sprintf("%O\n", fields[0]));
    if ( miniquests[fields[4], MQ_DATA_QUESTNO] != to_int(fields[0]) ) {
      raise_error("MQ-Nummern stimmen nicht ueberein!\n");
      return;
    }
    // fields[4] ist der MQ-Objektpfad
    miniquests[fields[4], MQ_DATA_TITLE] = fields[7];
    miniquests[fields[4], MQ_DATA_DIARYTEXT] = fields[6];
    miniquests[fields[4], MQ_DATA_TASKDESC] = fields[5];
    miniquests[fields[4], MQ_DATA_VISIBLE] = 1; // Default: visible
    miniquests[fields[4], MQ_DATA_ASSIGNED_DOMAIN] = fields[8];
    miniquests[fields[4], MQ_DATA_QUERY_PERMITTED] = fields[9];
    if ( fields[3] != "0" ) {
      miniquests[fields[4], MQ_DATA_RESTRICTIONS] =
        restore_value(fields[3]+"\n");
    }
    else miniquests[fields[4], MQ_DATA_RESTRICTIONS] = 0;
    if ( fields[9] != "0" )
      miniquests[fields[4], MQ_DATA_QUERY_PERMITTED] =
        restore_value(fields[9]+"\n");
    else miniquests[fields[4], MQ_DATA_QUERY_PERMITTED] = 0;
  }
}
*/

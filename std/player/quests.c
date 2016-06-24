// MorgenGrauen MUDlib
//
// player/quests.c -- quest handler
//
// $Id: quests.c 9142 2015-02-04 22:17:29Z Zesstra $

// Dieses Modul enhaelt die Quest-spezifischen Teile der Playerobjekte.
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <player/life.h>
#include <player/quest.h>
#include <thing/properties.h>
#include <player/base.h>
#include <living/life.h>
#undef NEED_PROTOTYPES

#include "/secure/questmaster.h"
#include <wizlevels.h>
#include <daemon.h>
#include <language.h>
#include <mail.h>
#include <defines.h>
#include <new_skills.h>
#include <properties.h>
#include <events.h>

mixed quests;

int QueryQuest(string questname);
// local properties prototype
static mixed _query_quests();
static int   _query_questpoints();

protected void create() {
  Set(P_QUESTS, NOSETMETHOD, F_SET_METHOD);
  Set(P_QUESTS, quests = ([]), F_VALUE);
  Set(P_QUESTS, SECURED, F_MODE);
  Set(P_QP, SAVE, F_MODE);
  Set(P_QP, SECURED, F_MODE);
}

varargs int GiveQuest(string questname, string message) {
  mixed *quest = QM->QueryQuest(questname);

  // Questname ungueltig
  if (!quest||!pointerp(quest)||quest==({}))
    return GQ_KEY_INVALID;
  // Unbefugter Zugriff auf deaktivierte Quest
  if (!quest[6]&&!IS_ARCH(this_interactive()))
    return GQ_IS_INACTIVE;
  // Unbefugter Zugriff
  if (member(quest[2], load_name(previous_object()))==-1 &&
      !IS_ARCH(this_interactive()))
    return GQ_ILLEGAL_OBJ;

  // Gilde wird in jedem Fall informiert.
  string guild=GUILD_DIR+QueryProp(P_GUILD);
  if (find_object(guild) || file_size(guild+".c")>-1)
    catch( call_other(guild, "NotifyGiveQuest", ME, questname);publish );

  // Quest bereits gesetzt
  if (QueryQuest(questname))
    return GQ_ALREADY_SET;
  AddExp(quest[1]);
  quests += ([ questname : quest[0]; time() ]);
  force_save();
  // Event ausloesen
  EVENTD->TriggerEvent(EVT_LIB_QUEST_SOLVED,([
             E_OBJECT: ME,
             E_PLNAME: getuid(ME),
             E_ENVIRONMENT: environment(),
             E_QUESTNAME: questname,
             E_QP_GRANTED: quest[0] ]) );

  if (message && message!="") {
    if (message!="__silent__") {
      message=implode(explode(message,"@@name@@"),
          capitalize(query_real_name()));
    }
    else {
      message="";
    }
  }
  else
    message=capitalize(query_real_name())
      +" hat gerade ein Abenteuer bestanden: "+ questname+"\n";
  if(message!="")
    catch(QM->Channel(message);publish);
  catch(QM->SendMail(questname, quest, ME);publish);
  return OK;
}

int DeleteQuest(string questname) {
  // Quest ist nicht gesetzt
  if(!QueryQuest(questname))
    return DQ_NOT_SET;

  mixed *quest = QM->QueryQuest(questname);
  // Questname ungueltig
  if (!quest||!pointerp(quest)||quest==({}))
    return DQ_KEY_INVALID;
  // Unbefugter Zugriff
  if (!IS_ARCH(this_interactive()))
    return DQ_ILLEGAL_OBJ;
  AddExp(-quest[1]);
  m_delete(quests, questname);
  force_save();
  return OK;
}

int QueryQuest(string questname) {
  int dummy;

  // Gaeste haben keine Quests.
  if( sscanf( getuid(), "gast%d", dummy ) == 1 )
    return QQ_GUEST;
  // Uebergebener Parameter "questname" ungueltig oder leer?
  if(!questname || !stringp(questname) || questname == "")
    return QQ_KEY_INVALID;
  // Questname ist tatsaechlich in der Questliste enthalten? Alles klar!
  if ( member(quests, questname) )
    return OK;
  // Ansonsten war der Name wohl ungueltig.
  return QQ_KEY_INVALID;
}

int ModifyQuestTime(string qname, int when) {
  if ( process_call() )
    return -1;

  // Nur EM+ oder der Tagebuchmaster duerfen die Werte aendern.
  if ( !IS_ARCH(this_interactive()) &&
    load_name(previous_object())!="/d/wald/leusel/quest/objs/tagebuch-master")
    return -1;

  // Questliste ist unerwartet kein Mapping.
  if ( !mappingp(quests) )
    return -2;

  // Kein Questname angegeben, oder Spieler hat diese Quest ueberhaupt nicht
  // geloest.
  if ( !stringp(qname) || !member(quests, qname) )
    return -3;

  // Der Tagebuchmaster setzt Eintraege ggf. auf 0, wenn er keine Daten
  // findet, und EM+ wollen Eintraege auf -1 setzen koennen, um das Einlesen
  // der Daten noch einmal zu ermoeglichen, d.h. diese Werte sind zusaetzlich
  // zu gueltigen Zeitwerten erlaubt.
  if ( !intp(when) || when < -1 || when > time() )
    return -4;

  // Neuen Wert eintragen.
  quests[qname,1] = when;
  return 1;
}

int QueryQuestTime(string qname) {
  return quests[qname,1];
}

// Konvertiert Datenstruktur von quests in ein Mapping mit folgendem Aufbau:
// quests = ([ "questname" : Abenteuerpunkte; Zeit_des_Questabschlusses, ])
protected void updates_after_restore(mixed newflag) {
  // Ganz frischer Spieler? Dann keine Behandlung erforderlich.
  if ( newflag )
    return;
  // Wenn die Questliste noch kein Mapping ist, Konvertierung anstossen.
  if ( !mappingp(quests) ) {
    // Wenn noch keine Quests eingetragen sind, Leermapping eintragen.
    if ( !sizeof(quests) ) {
      quests = ([:2]);
      return;
    }
    // Vorsichtshalber Leereintraege rausziehen.
    quests -= ({({})});
    // Liste der Questzeiten aus dem Spieler auslesen. Wenn nicht vorhanden,
    // Array mit -1-Elementen passender Laenge erzeugen.
    // -1 an allen Stellen eintragen, wo bisher keine Daten vorliegen.
    // Diese werden dann vom Questtagebuch durch die Daten ersetzt.
    int *qtimes = QueryProp("questtime")||allocate(sizeof(quests), -1);
    // Falls die Questliste laenger ist als die Zeitenliste, wird letztere
    // um die fehlende Laenge in Form von -1-eintraegen ergaenzt, unter der
    // Annahme, dass die fehlenden Eintraege bisher lediglich nicht
    // eingelesen wurden. Im umgekehrten Fall werden alle Zeiten verworfen,
    // da dieser Fall eintritt, wenn einem Spieler eine Quest ausgetragen
    // wurde und die Listen in der Vergangenheit nicht synchron gehalten
    // wurden.
    if ( sizeof(qtimes) < sizeof(quests) ) {
      qtimes += allocate(sizeof(quests)-sizeof(qtimes), -1);
    }
    if ( sizeof(qtimes) > sizeof(quests) ) {
      qtimes = allocate(sizeof(quests), -1);
    }
    // Questdaten und Questzeiten zusammenpferchen. Ergibt folg. Mapping:
    // temp = ([ ({Questname1, QP1}) : Questzeit, ... ])
    mapping temp = mkmapping(quests, qtimes);
    quests = m_allocate(sizeof(quests),2);
    foreach(mixed qdata, int qtime : temp) {
      quests += ([ qdata[0] : qdata[1]; qtime ]);
    }
    if (QueryProp("questtime")) {
      SetProp("questtime",0);
      Set("questtime", SAVE, F_MODE_AD);
    }
  }
}

// **** local property methods
static int _query_questpoints() {
  int qp;

  if ( !mappingp(quests) || !sizeof(quests) ) {
    return 0;
  }

  closure qqp = symbol_function("QueryQuestPoints", QM);
  // Die aktuell gueltigen Abenteuerpunkte aus dem Questmaster holen und
  // die Questliste damit aktualisieren. qp wird als Referenz mit uebergeben
  // damit das Additionsergebnis auch nach dem Durchlauf ausserhalb der
  // Closure verfuegbar ist.
  // Falls Abenteuerpunkte < 0 existieren, wird die entsprechende Quest
  // aus der Liste ausgetragen.
  walk_mapping(quests,
    function void (string qname, int qpoints, int qtime, int sum)
    {
      qpoints = funcall(qqp, qname);
      if (qpoints<0)
        m_delete(quests,qname);
      else
        sum += qpoints;
    }, &qp);

  Set(P_QP, qp);
  return qp;
}

static mixed _query_quests() {
  return copy(quests);
}

/*  MorgenGrauen MUDlib
    /p/daemon/errord.c
    speichert Fehler und Warnungen
    Autor: Zesstra
    $Id: errord.c 9439 2016-01-20 09:48:28Z Zesstra $
    ggf. Changelog:
*/

#pragma strict_types,save_types,rtt_checks
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma pedantic
#pragma range_check
#pragma warn_deprecated

#include <config.h>
#include <wizlevels.h>
#include <defines.h>
#include <debug_info.h>
#include <commands.h>
#include <wizlevels.h>
#include <mail.h>
#include <tls.h>
#include <events.h>

inherit "/secure/errord-structs";

#define __NEED_IMPLEMENTATION__
#include "errord.h"
#undef __NEED_IMPLEMENTATION__

#define SAVEFILE  (__DIR__+"ARCH/errord")

#define TI this_interactive()

private int       access_check(string uid,int mode);
private varargs int set_lock(int issueid, int lock, string note);
private varargs int set_resolution(int issueid, int resolution, string note);

private int versende_mail(struct fullissue_s fehler);

nosave mapping lasterror;   // die letzen 5 jeder Art.


/* ******************* Helfer **************************** */

public int getErrorID(string hashkey)
{
  int** row=sl_exec("SELECT id from issues WHERE hashkey=?1;",
                    hashkey);
  //DEBUG(sprintf("getEID: %s: %O\n",hashkey,row));
  if (pointerp(row))
  {
    return row[0][0];
  }
  return -1;
}

// note->id muss auf einen Eintrag in issues verweisen, es erfolgt keine
// Pruefung.
int db_add_note(struct note_s note)
{
  sl_exec("INSERT INTO notes(issueid,time,user,txt) "
          "VALUES(?1,?2,?3,?4);",
          to_array(note)...);
  return sl_insert_id();
}

private struct frame_s* db_get_stack(int issueid)
{
  mixed rows = sl_exec("SELECT * FROM stacktraces WHERE issueid=?1 "
                       "ORDER BY rowid;", issueid);
  if (pointerp(rows))
  {
    struct frame_s* stack = allocate(sizeof(rows));
    int i;
    foreach(mixed row : rows)
    {
      stack[i] = to_struct(row, (<frame_s>));
      ++i;
    }
    return stack;
  }
  return 0;
}

private struct note_s* db_get_notes(int issueid)
{
  mixed rows = sl_exec("SELECT * FROM notes WHERE issueid=?1 "
                       "ORDER BY rowid;", issueid);
  if (pointerp(rows))
  {
    struct note_s* notes = allocate(sizeof(rows));
    int i;
    foreach(mixed row : rows)
    {
      notes[i] = to_struct(row, (<note_s>));
      ++i;
    }
    return notes;
  }
  return 0;
}

// einen durch id oder hashkey bezeichneten Eintrag als fullissue_s liefern.
private struct fullissue_s db_get_issue(int issueid, string hashkey)
{
  mixed rows = sl_exec("SELECT * FROM issues WHERE id=?1 OR hashkey=?2;",
                       issueid, hashkey);
  if (pointerp(rows))
  {
    // Einfachster Weg - funktioniert aber nur, solange die Felder in der DB
    // die gleiche Reihenfolge wie in der struct haben! Entweder immer
    // sicherstellen oder Ergebnisreihenfolge oben im select festlegen!
    struct fullissue_s issue = to_struct( rows[0], (<fullissue_s>) );
    if (issue->type == T_RTERROR)
        issue->stack = db_get_stack(issue->id);
    issue->notes = db_get_notes(issue->id);
    return issue;
  }
  return 0;
}

private struct fullissue_s filter_private(struct fullissue_s issue)
{
    //momentan wird F_CLI, also die Spielereingabe vor dem Fehler
    //ausgefiltert, wenn TI kein EM oder man in process_string() ist.

    //Wenn EM und nicht in process_string() oder die Spielereingabe gar nicht
    //im Fehlereintrag drinsteht: ungefiltert zurueck
    if (!issue->command ||
        (!process_call() && ARCH_SECURITY) )
        return issue;

    //sonst F_CLI rausfiltern, also Kopie und in der Kopie aendern.
    issue->command="Bitte EM fragen";
    return issue;
}

// setzt oder loescht die Loeschsperre.
// Prueft, ob <issueid> existiert und aendert den Zustand nur, wenn noetig.
// Rueckgabe: -1, wenn Issue nicht existiert, -2 wenn bereits resolved, -3
//            wenn keine Aenderung noetig, sonst den neuen Sperrzustand
int db_set_lock(int issueid, int lockstate, string note)
{
  int** rows = sl_exec("SELECT locked,resolved FROM issues WHERE id=?1;",
                       issueid);
  if (!rows)
    return -1;  // nicht vorhanden.

  if (rows[0][1])
      return -2; // bereits resolved -> Sperre nicht moeglich.


  if (lockstate && !rows[0][0])
  {
    // Sperren
//    sl_exec("BEGIN TRANSACTION;");
    sl_exec("UPDATE issues SET locked=1,locked_by=?2,locked_time=?3,mtime=?3 "
            "WHERE id=?1;",
            issueid, getuid(TI), time());
    db_add_note( (<note_s> id: issueid, time: time(), user: getuid(TI),
                           txt: sprintf("Lock gesetzt: %s", 
                                        note ? note : "<kein Kommentar>")) );
//    sl_exec("COMMIT;");
    return 1;
  }
  else if (!lockstate && rows[0][0])
  {
    // entsperren
//    sl_exec("BEGIN TRANSACTION;");
    sl_exec("UPDATE issues SET locked=0,locked_by=0,locked_time=0,mtime=?2 "
            "WHERE id=?1;", issueid, time());
    db_add_note( (<note_s> id: issueid, time: time(), user: getuid(TI),
                           txt: sprintf("Lock geloescht: %s", 
                                        note ? note : "<kein Kommentar>")) );
//    sl_exec("COMMIT;");
    return 0;
  }
  // nix aendern.
  return -3;
}

// markiert ein Issue als gefixt oder nicht gefixt.
// Prueft, ob <issueid> existiert und aendert den Zustand nur, wenn noetig.
// Rueckgabe: -1, wenn Issue nicht existiert, -3 wenn keine Aenderung noetig,
//            sonst den neuen Sperrzustand
int db_set_resolution(int issueid, int resolved, string note)
{
  int** rows = sl_exec("SELECT resolved FROM issues WHERE id=?1;",
                       issueid);
  if (!rows)
    return -1;  // nicht vorhanden.

  if (resolved && !rows[0][0])
  {
    // Als gefixt markieren.
//    sl_exec("BEGIN TRANSACTION;");
    sl_exec("UPDATE issues SET resolved=1,resolver=?2,mtime=?3 "
            "WHERE id=?1;",
            issueid, getuid(TI),time());
    db_add_note( (<note_s> id: issueid, time: time(), user: getuid(TI),
                           txt: sprintf("Fehler gefixt: %s", 
                                        note ? note : "<kein Kommentar>")) );
//    sl_exec("COMMIT;");
    return 1;
  }
  else if (!resolved && rows[0][0])
  {
    // als nicht gefixt markieren.
//    sl_exec("BEGIN TRANSACTION;");
    sl_exec("UPDATE issues SET resolved=0,resolver=0,mtime=?2 "
            "WHERE id=?1;", issueid, time());
    db_add_note( (<note_s> id: issueid, time: time(), user: getuid(TI),
                           txt: sprintf("Fix zurueckgezogen: %s", 
                                        note ? note : "<kein Kommentar>")) );
//    sl_exec("COMMIT;");
    return 0;
  }
  // nix aendern.
  return -3;

}

// Transferiert ein Issue zu einer neuen zustaendigen UID
// Prueft, ob <issueid> existiert und aendert den Zustand nur, wenn noetig.
// Rueckgabe: -1, wenn Issue nicht existiert, -3 wenn keine Aenderung noetig,
//            1, wenn erfolgreich neu zugewiesen
int db_reassign_issue(int issueid, string newuid, string note)
{
  string** rows = sl_exec("SELECT uid FROM issues WHERE id=?1;",
                       issueid);
  if (!rows)
    return -1;  // nicht vorhanden.

  if (!stringp(newuid))
    return(-2);

  if (newuid != rows[0][0])
  {
//    sl_exec("BEGIN TRANSACTION;");
    sl_exec("UPDATE issues SET uid=?2,mtime=?3 WHERE id=?1;",
            issueid, newuid,time());
    db_add_note( (<note_s> id: issueid, time: time(), user: getuid(TI),
                           txt: sprintf("Fehler von %s an %s uebertragen. (%s)",
                                        rows[0][0], newuid,
                                        note ? note : "<kein Kommentar>")) );
//    sl_exec("COMMIT;");
    return 1;
  }

  return -3;
}

// inkrementiert count und aktualisiert mtime, atime.
// Ausserdem wird ggf. das Loeschflag genullt - ein erneut aufgetretener
// Fehler sollte anschliessend nicht mehr geloescht sein. Geloeste
// (resolved) Eintraege werden NICHT auf ungeloest gesetzt. Vermutlich trat
// der Fehler in einem alten Objekte auf...
// Issue muss in der DB existieren.
int db_countup_issue(int issueid)
{
  sl_exec("UPDATE issues SET count=count+1,mtime=?2,atime=?2,deleted=0 WHERE id=?1;",
          issueid,time());
  return 1;
}

// Das Issue wird ggf. ent-loescht und als nicht resvolved markiert.
// Sind pl und msg != 0, wird eine Notiz angehaengt.
// aktualisiert mtime, atime.
// Issue muss in der DB existieren.
int db_reopen_issue(int issueid, string pl, string msg)
{
  int** row=sl_exec("SELECT deleted,resolved from issues WHERE id=?1;",
                    issueid);
  if (pointerp(row)
      && (row[0][0] || row[0][1]) )
  {
//    sl_exec("BEGIN TRANSACTION;");
    if (pl && msg)
    {
      db_add_note( (<note_s> id: issueid,
                             time: time(),
                             user: pl,
                             txt: msg) );
    }
    sl_exec("UPDATE issues SET "
        "deleted=0,resolved=0,resolver=0,mtime=?2,atime=?2 WHERE id=?1;",
        issueid,time());
//    sl_exec("COMMIT;");
  }
  return 1;
}

int db_insert_issue(struct fullissue_s issue)
{
  //DEBUG(sprintf("db_insert: %O\n", issue));

  mixed row=sl_exec("SELECT id from issues WHERE hashkey=?1;",
                    issue->hashkey);
  //DEBUG(sprintf("insert: %s: %O\n",issue->hashkey,row));
  if (pointerp(row))
  {
    issue->id=row[0][0];
    return db_countup_issue(issue->id);
  }
//  sl_exec("BEGIN TRANSACTION;");
  sl_exec("INSERT INTO issues(hashkey,uid,type,mtime,ctime,atime,count,"
          "deleted,resolved,locked,locked_by,locked_time,resolver,message,"
          "loadname,obj,prog,loc,titp,tienv,hbobj,caught,command,verb) "
          "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14,?15,"
          "?16,?17,?18,?19,?20,?21,?22,?23,?24);",
          (to_array(issue)[1..24])...); 
  issue->id=sl_insert_id();

  if (pointerp(issue->stack))
  {
    foreach(struct frame_s entry : issue->stack)
    {
      entry->id = issue->id;
      sl_exec("INSERT INTO stacktraces(issueid,type,name,prog,obj,loc,ticks) "
              "VALUES(?1,?2,?3,?4,?5,?6,?7);",
              to_array(entry)...);
    }
  }
  if (pointerp(issue->notes))
  {
    foreach(struct note_s entry : issue->notes)
    {
      entry->id = issue->id;
      sl_exec("INSERT INTO notes(issueid,time,user,txt) "
              "VALUES(?1,?2,?3,?4);",
              to_array(entry)...);
    }
  }
//  sl_exec("COMMIT;");

  return issue->id;
}


// loggt einen T_REPORTED_ERR, T_REPORTED_IDEA, T_REPORTED_TYPO,
// T_REPORTED_MD, T_REPORTED_SYNTAX
public string LogReportedError(mapping err)
{
    //darf nur von Spielershells oder Objekt in /obj/ (z.B. Fehlerteufel oder
    //vitem_proxy) gerufen werden.
    if (extern_call() && !previous_object()
        || (strstr(load_name(previous_object()),"/std/shells/") == -1
            && strstr(load_name(previous_object()), "/"LIBOBJDIR"/") == -1))
        return 0;

    //DEBUG("LogReportedError\n");
    // DEBUG(sprintf("%O\n",err));
    string uid = ({string})master()->creator_file(err[F_OBJ]);

    // default-Typ
    if (!member(err, F_TYPE)) err[F_TYPE] = T_REPORTED_ERR;

    // div. Keys duerfen nicht gesetzt sein.
    err -= ([F_STATE, F_READSTAMP, F_CAUGHT, F_STACK, F_CLI, F_VERB,
             F_LOCK, F_RESOLVER, F_NOTES]);

    // Errormapping in issue-struct umwandeln und befuellen.
    struct fullissue_s issue = (<fullissue_s>);
    issue->type = err[F_TYPE];
    issue->uid = uid;
    issue->mtime = issue->ctime = issue->atime = time();
    issue->count=1;
    issue->loadname = load_name(err[F_OBJ]);
    issue->message = err[F_MSG];
    issue->obj = objectp(err[F_OBJ]) ? object_name(err[F_OBJ]) : err[F_OBJ];
    // Normalisieren auf fuehrenden / und kein .c
    if (err[F_PROG]!="unbekannt")
        issue->prog = load_name(err[F_PROG]);
    else
        issue->prog = "unbekannt";
    issue->titp = getuid(this_interactive() || this_player());
    issue->tienv = object_name(environment(this_interactive() || this_player()));

    //DEBUG(sprintf("%O\n",issue));
    issue->hashkey = hash(TLS_HASH_MD5,
        sprintf("%d%s%s", issue->type, issue->loadname, issue->message));

    // ggf. vorhandenen Fehler suchen - zugegeben: sollte bei von Spielern
    // gemeldeten Dingen vermutlich nie vorkommen...
    int oldid = getErrorID(issue->hashkey);
    if (oldid >= 0)
    {
      // ggf. sicherstellen, dass er wieder eroeffnet wird.
      db_reopen_issue(oldid, "<ErrorD>",
                      "Automatisch wiedereroeffnet wegen erneutem Auftreten.");
      db_countup_issue(oldid);
      return issue->hashkey;
    }

    // sonst fuegen wir einen neuen Eintrag hinzu
    // Spielergemeldete Bugs werden erstmal vor automatischem Loeschen
    // geschuetzt, bis ein zustaendiger Magier ihn zur Kenntnis nimmt und
    // entsperrt.
    issue->locked = 1;
    issue->locked_by = getuid(TI || PL);
    issue->locked_time = time();

    // In DB eintragen.
    issue->id = db_insert_issue(issue);

    lasterror[issue->type]=issue->id;

    // Event triggern, aber nur eine Teilmenge der Daten direkt mitliefern.
    EVENTD->TriggerEvent(EVT_LIB_NEW_ERROR,
              ([F_TYPE: issue->type, F_HASHKEY:issue->hashkey,
               F_UID:issue->uid, F_ID: issue->id]));

    DEBUG(sprintf("LogReportedError: %s\n",issue->hashkey));

    return issue->hashkey;
}

//Fehler registrieren
//Diese Funktion darf nicht mehr als 200k Ticks verbrauchen und wird nur vom
//Master gerufen!
public int LogError(string msg,string prg,string curobj,int line,mixed culprit,
    int caught)
{
    //DEBUG(sprintf("LogError: Prog: %O, Obj: %O,\n",prg,curobj));

    //darf nur vom Master gerufen werden
    if (!extern_call() || 
        (previous_object() && previous_object() != master()))
        return 0;

    struct fullissue_s issue = (<fullissue_s>);

    //UID bestimmen, curobj is 0 for lwobjects, then the program is used.
    issue->uid=({string})master()->creator_file(curobj || prg);
    //DEBUG(sprintf("LogError: UID: %s\n",issue->uid));

    //Loadname (besser als BP, falls rename_object() benutzt wurde) bestimmen
    if (!stringp(curobj) || !sizeof(curobj))
        issue->loadname = curobj = "<Unbekannt>";
    else
    {
        //load_name nimmt Strings und Objects und konstruiert den loadname,
        //wie er sein sollte, wenn das Objekt nicht mehr existiert.
        issue->loadname=load_name(curobj);
    }
    if (!stringp(issue->loadname))
    {
        //hier kommt man rein, falls curobj ein 'kaputter' Name ist,
        //d.h. load_name() 0 liefert.
        issue->loadname="<Illegal object name>";
    }

    // Wenn curobj in /players/ liegt, es einen TI gibt, welcher ein Magier
    // ist und dieser die Prop P_DONT_LOG_ERRORS gesetzt hat, wird der FEhler
    // nicht gespeichert.
    if (this_interactive() && IS_LEARNER(this_interactive())
        && strstr(issue->loadname,WIZARDDIR)==0
        && ({int})this_interactive()->QueryProp(P_DONT_LOG_ERRORS))
    {
        return 0;
    }

    // prg und curobj auf fuehrenden / und ohne .c am Ende normieren.
    if (stringp(prg))
        issue->prog = load_name(prg);
    if (stringp(curobj) && curobj[0]!='/')
    {
      curobj="/"+curobj;
    }

    issue->obj = curobj;
    issue->loc = line;
    issue->message = msg;
    issue->ctime = issue->mtime = issue->atime = time();
    issue->type = T_RTERROR;
    issue->count = 1;
    issue->caught = caught;

    //Hashkey bestimmen: Typ, Name der Blueprint des buggenden Objekts,
    //Programmname, Zeilennr., Fehlermeldung
    //TODO: evtl. sha1() statt md5()?
    issue->hashkey=hash(TLS_HASH_MD5,
        sprintf("%d%s%s%d%s", T_RTERROR, issue->loadname||"",
                            issue->prog || "", issue->loc,
                            issue->message||"<No error message given.>"));
    DEBUG(sprintf("LogError: Hashkey: %s", issue->hashkey));

    // ggf. vorhandenen Fehler suchen
    int oldid = getErrorID(issue->hashkey);
    if (oldid >= 0)
    {
      db_reopen_issue(oldid, "<ErrorD>",
                      "Automatisch wiedereroeffnet wegen erneutem Auftreten.");
      db_countup_issue(oldid);
      return oldid;
    }

    //sonst fuegen wir einen neuen Eintrag hinzu
    //DEBUG(sprintf("LogError: OBJ: %s, BP: %s",curobj,loadname));
    // Wenn Fehler im HB, Objektnamen ermitteln
    if (objectp(culprit))
        issue->hbobj = object_name(culprit);

    //gibt es einen TI/TP? Name mit erfassen
    mixed tienv;
    if(objectp(this_interactive()))
    {
        issue->titp=getuid(this_interactive());
        tienv=environment(this_interactive());
    }
    else if (objectp(PL) && query_once_interactive(PL))
    {
        issue->titp=getuid(PL);
        tienv=environment(PL);
    }
    else if (objectp(PL))
    {
        issue->titp=object_name(PL);
        tienv=environment(PL);
    }
    if (objectp(tienv))
        issue->tienv=object_name(tienv);

    // Mal schauen, ob der Commandstack auch was fuer uns hat. ;-)
    mixed cli;
    if (pointerp(cli=command_stack()) && sizeof(cli))
    {
        issue->verb=cli[0][CMD_VERB];
        issue->command=cli[0][CMD_TEXT];
    }

    //stacktrace holen
    mixed stacktrace;
    if (caught)
        stacktrace=debug_info(DINFO_TRACE,DIT_ERROR);
    else
        stacktrace=debug_info(DINFO_TRACE,DIT_UNCAUGHT_ERROR);
    // gueltige Stacktraces haben min. 2 Elemente.
    // (leerer Trace: ({"No trace."}))
    if (sizeof(stacktrace) > 1)
    {
      int i;
      issue->stack = allocate(sizeof(stacktrace)-1);
      // erstes Element ist 0 oder HB-Objekt: kein frame, daher ueberspringen
      foreach(mixed entry : stacktrace[1..])
      {
        // frame->id will be set later by db_insert_issue().
        struct frame_s frame = (<frame_s> type : entry[TRACE_TYPE],
                                          name:  entry[TRACE_NAME],
                                          prog:  entry[TRACE_PROGRAM],
                                          obj:   entry[TRACE_OBJECT],
                                          loc:   entry[TRACE_LOC],
                                          ticks: entry[TRACE_EVALCOST]);
        issue->stack[i] = frame;
        ++i;
      }
    }

    issue->id = db_insert_issue(issue);

    lasterror[T_RTERROR]=issue->id;

    // Event triggern, aber nur eine Teilmenge der Daten direkt mitliefern.
    EVENTD->TriggerEvent(EVT_LIB_NEW_ERROR,
              ([ F_TYPE: T_RTERROR, F_HASHKEY: issue->hashkey, F_UID:
               issue->uid, F_ID: issue->id ]));

//    DEBUG(sprintf("LogError: Fehlereintrag:\n%O\n",
//          errors[uid][hashkey]));
//    DEBUG(sprintf("LogError: Verbrauchte Ticks: %d\n",
//          200000-get_eval_cost()));
    return issue->id;
}

//Warnungen registrieren
//Diese Funktion darf nicht mehr als 200k Ticks verbrauchen und wird nur vom
//Master gerufen!
public int LogWarning(string msg,string prg,string curobj,int line, int in_catch)
{
    //DEBUG(sprintf("LogWarning: Prog: %O, Obj: %O,\n",prg,curobj));

    //darf nur vom Master gerufen werden
    if (!extern_call() || 
        (previous_object() && previous_object() != master()))
        return 0;

    struct fullissue_s issue = (<fullissue_s>);

    //UID bestimmen, curobj is 0 for lwobjects, then the program is used.
    issue->uid=({string})master()->creator_file(curobj || prg);
    //DEBUG(sprintf("LogWarning UID: %s\n",uid));

    //Loadname (besser als BP, falls rename_object() benutzt wurde) bestimmen
    if (!stringp(curobj) || !sizeof(curobj))
        issue->loadname = curobj = "<Unbekannt>";
    else
    {
        //load_name nimmt Strings und Objects und konstruiert den loadname,
        //wie er sein sollte, wenn das Objekt nicht mehr existiert.
        issue->loadname=load_name(curobj);
    }

    if (!stringp(issue->loadname))
        //hier sollte man reinkommen, falls curobj ein 'kaputter' Name ist,
        //d.h. load_name() 0 liefert.
        issue->loadname="<Illegal object name>";

    // prg und curobj auf abs. Pfade normalisieren.
    if (stringp(prg))
        issue->prog=load_name(prg);
    if (stringp(curobj) && curobj[0]!='/')
    {
      curobj="/"+curobj;
    }

    //DEBUG(sprintf("LogWarning: OBJ: %s, BP: %s\n",curobj,blue));

    // Wenn curobj in /players/ liegt, es einen TI gibt, welcher ein Magier
    // ist und dieser die Prop P_DONT_LOG_ERRORS gesetzt hat, wird der FEhler
    // nicht gespeichert.
    if (this_interactive() && IS_LEARNER(this_interactive())
        && strstr(issue->loadname,WIZARDDIR)==0
        && ({int})this_interactive()->QueryProp(P_DONT_LOG_ERRORS)) {
        return 0;
    }

    //Hashkey bestimmen, Typ, Name der Blueprint des buggenden Objekts, Programm
    //Zeilennr., Warnungsmeldung
    issue->hashkey=hash(TLS_HASH_MD5,
        sprintf("%d%s%s%d%s", T_RTWARN, issue->loadname, issue->prog, line,
                           msg));
    //DEBUG(sprintf("LogWarning: Hashkey: %s",hashkey));


    // ggf. vorhandenen Fehler suchen
    int oldid = getErrorID(issue->hashkey);
    if (oldid >= 0)
    {
      db_reopen_issue(oldid, "<ErrorD>",
                      "Automatisch wiedereroeffnet wegen erneutem Auftreten.");
      db_countup_issue(oldid);
      return oldid;
    }

    //sonst fuegen wir einen neuen Eintrag hinzu
    // erstmal vervollstaendigen
    issue->obj = curobj;
    issue->message = msg;
    issue->ctime = issue->mtime = issue->atime = time();
    issue->loc = line;
    issue->count = 1;
    issue->type = T_RTWARN;
    issue->caught = in_catch;

    //gibt es einen TI/TP? Name mit erfassen
    mixed tienv;
    if(objectp(this_interactive()))
    {
        issue->titp=getuid(this_interactive());
        tienv=environment(this_interactive());
    }
    else if (objectp(PL) && query_once_interactive(PL))
    {
        issue->titp=getuid(PL);
        tienv=environment(PL);
    }
    else if (objectp(PL))
    {
        issue->titp=object_name(PL);
        tienv=environment(PL);
    }
    if (objectp(tienv))
        issue->tienv=object_name(tienv);

    // Mal schauen, ob der Commandstack auch was fuer uns hat. ;-)
    mixed cli;
    if (pointerp(cli=command_stack()) && sizeof(cli))
    {
        issue->verb=cli[0][CMD_VERB];
        issue->command=cli[0][CMD_TEXT];
    }

    issue->id = db_insert_issue(issue);

    lasterror[T_RTWARN]=issue->id;
    // Event triggern, aber nur eine Teilmenge der Daten direkt mitliefern.
    EVENTD->TriggerEvent(EVT_LIB_NEW_ERROR,
        ([F_TYPE: issue->type, F_ID: issue->id,
          F_UID: issue->uid, F_HASHKEY: issue->hashkey]) );

//    DEBUG(sprintf("LogWarning: Warnungseintrag:\n%O\n",
//          warnings[uid][hashkey]));
//    DEBUG(sprintf("LogWarning: Verbrauchte Ticks: %d\n",
//          200000-get_eval_cost()));
    return issue->id;
}

//Warnungen und Fehler beim Kompilieren  registrieren
//Diese Funktion darf nicht mehr als 200k Ticks verbrauchen und wird nur vom
//Master gerufen!
public int LogCompileProblem(string file,string msg,int warn) {

    //DEBUG(sprintf("LogCompileProblem: Prog: %O, Obj: %O,\n",file,msg));

    //darf nur vom Master gerufen werden
    if (!extern_call() || 
        (previous_object() && previous_object() != master()))
        return 0;

    struct fullissue_s issue = (<fullissue_s>);

    //UID bestimmen
    issue->uid=({string})master()->creator_file(file);
    //DEBUG(sprintf("LogCompileProblem UID: %s\n",uid));

    // An File a) fuehrenden / anhaengen und b) endendes .c abschneiden. Macht
    // beides netterweise load_name().
    issue->loadname = load_name(file);
    issue->type = warn ? T_CTWARN : T_CTERROR;

    //loggen wir fuer das File ueberhaupt?
    if (member(BLACKLIST,explode(issue->loadname,"/")[<1])>=0)
        return 0;

    //Hashkey bestimmen, in diesem Fall einfach, wir koennen die
    //Fehlermeldunge selber nehmen.
    issue->hashkey=hash(TLS_HASH_MD5,sprintf(
          "%d%s%s",issue->type,issue->loadname, msg));
    //DEBUG(sprintf("LogCompileProblem: Hashkey: %s",hashkey));

    // ggf. vorhandenen Fehler suchen
    int oldid = getErrorID(issue->hashkey);
    if (oldid >= 0)
    {
      db_reopen_issue(oldid, "<ErrorD>",
                      "Automatisch wiedereroeffnet wegen erneutem Auftreten.");
      db_countup_issue(oldid);
      return oldid;
    }

    // neuen Eintrag
    issue->message = msg;
    issue->count = 1;
    issue->ctime = issue->mtime = issue->atime = time();

    issue->id = db_insert_issue(issue);

    if (warn) lasterror[T_CTWARN]=issue->id;
    else lasterror[T_CTERROR]=issue->id;

//    DEBUG(sprintf("LogCompileProblem: Eintrag:\n%O\n",
//          (warn ? ctwarnings[uid][hashkey] : cterrors[uid][hashkey])));
//   DEBUG(sprintf("LogCompileProblem: Verbrauchte Ticks: %d\n",
//          200000-get_eval_cost()));
   return issue->id;
}

/* ****************  Public Interface ****************** */

//Einen bestimmten Fehler nach Hashkey suchen und als fullissue_s inkl. Notes
//und Stacktrace liefern.
struct fullissue_s QueryIssueByHash(string hashkey)
{
  struct fullissue_s issue = db_get_issue(0, hashkey);
  if (structp(issue))
    return filter_private(issue);
  return 0;
}
//Einen bestimmten Fehler nach ID suchen und als fullissue_s inkl. Notes
//und Stacktrace liefern.
struct fullissue_s QueryIssueByID(int issueid)
{
  struct fullissue_s issue = db_get_issue(issueid, 0);
  if (structp(issue))
    return filter_private(issue);
  return 0;
}

// den letzten Eintrag den jeweiligen Typ liefern.
struct fullissue_s QueryLastIssue(int type)
{
    if (!member(lasterror,type))
        return 0;
    //einfach den kompletten letzten Eintrag zurueckliefern
    return(QueryIssueByID(lasterror[type]));
}

// Liefert alle Issues, deren obj,prog oder loadname gleich <file> ist.
public struct fullissue_s* QueryIssuesByFile(string file, int type)
{
  mixed rows = sl_exec("SELECT * FROM issues "
                       "WHERE (loadname=?1 OR prog=?1 OR obj=?1) "
                       "AND deleted=0 AND resolved=0 AND type=?2"
                       "ORDER BY type,mtime;", file, type);
  if (pointerp(rows))
  {
    struct fullissue_s* ilist = allocate(sizeof(rows));
    int i;
    foreach(mixed row : rows)
    {
      // Einfachster Weg - funktioniert aber nur, solange die Felder in der DB
      // die gleiche Reihenfolge wie in der struct haben! Entweder immer
      // sicherstellen oder Ergebnisreihenfolge oben im select festlegen!
      struct fullissue_s issue = to_struct( row, (<fullissue_s>) );
      if (issue->type == T_RTERROR)
          issue->stack = db_get_stack(issue->id);
      issue->notes = db_get_notes(issue->id);
      ilist[i] = filter_private(issue);
      ++i;
    }
    return ilist;
  }
  return 0;
}

// Liefert eine Liste von allen IDs, Loadnames, UIDs und Typen fuer die
// angebenen <type> und <uid>.
varargs < <int|string>* >* QueryIssueListByFile(string file)
{
  mixed rows = sl_exec("SELECT id,loadname,obj,prog,loc FROM issues "
                       "WHERE (loadname=?1 OR prog=?1 OR obj=?1) "
                       "AND deleted=0 AND resolved=0 "
                       "ORDER BY type,mtime;", file);
  return rows;
}

// Liefert eine Liste von allen IDs, Loadnames, UIDs und Typen fuer die
// angebenen <type> und <uid>.
varargs < <int|string>* >* QueryIssueListByLoadname(string file, int type)
{
  mixed rows;
  if (type && file)
  {
     rows = sl_exec("SELECT id,loadname,uid,type FROM issues "
                       "WHERE loadname=?1 AND type=?2 AND deleted=0 "
                       "AND resolved=0 "
                       "ORDER BY type,mtime;", file, type);
  }
  else if (type)
  {
     rows = sl_exec("SELECT id,loadname,uid,type FROM issues "
                       "WHERE type=?1 AND deleted=0 AND resolved=0 "
                       "ORDER BY type,mtime;", type);
  }
  else if (file)
  {
    rows = sl_exec("SELECT id,loadname,uid,type FROM issues "
                       "WHERE loadname=?1 AND deleted=0 AND resolved=0 "
                       "ORDER BY type,mtime;", file);
  }
  return rows;
}


// Liefert eine Liste von allen IDs, Loadnames, UIDs und Typen fuer die
// angebenen <type> und <uid>.
varargs < <int|string>* >* QueryIssueList(int type, string uid)
{
  mixed rows;
  if (type && uid)
  {
    rows = sl_exec("SELECT id,loadname,uid,type FROM issues "
                   "WHERE type=?1 AND uid=?2 AND deleted=0 "
                   "AND resolved=0 "
                   "ORDER BY type,rowid;", type,uid);
  }
  else if (type)
  {
    rows = sl_exec("SELECT id,loadname,uid,type FROM issues "
                   "WHERE type=?1 AND deleted=0 AND resolved=0 "
                   "ORDER BY type,rowid;", type);
  }
  else if (uid)
  {
    rows = sl_exec("SELECT id,loadname,uid,type FROM issues "
                   "WHERE uid=?1 AND deleted=0 AND resolved=0 "
                   "ORDER BY type,rowid;", uid);
  }
  return rows;
}

varargs string* QueryUIDsForType(int type) {
    //liefert alle UIDs fuer einen Fehlertyp oder fuer alle Fehlertypen
    string** rows;

    if (type)
    {
      rows = sl_exec("SELECT uid FROM issues "
                     "WHERE type=?1 AND deleted=0 AND resvoled=0;", type);
    }
    else
      rows = sl_exec("SELECT uid FROM issues WHERE deleted=0 "
                     "AND resolved=0;");

    return map(rows, function string (string* item)
                     {return item[0];} );
}

//Wieviele unterschiedliche Fehler in diesem Typ?
varargs int QueryUniqueIssueCount(int type, string uid)
{
  int** rows;

  if (type && uid)
  {
    rows = sl_exec("SELECT count(*) FROM issues "
                   "WHERE type=?1 AND uid=?2 AND deleted=0 AND resolved=0;",
                   type, uid);
  }
  else if (type)
  {
    rows = sl_exec("SELECT count(*) FROM issues "
                   "WHERE type=?1 AND deleted=0 AND resolved=0;",
                   type);
  }
  else if (uid)
  {
    rows = sl_exec("SELECT count(*) FROM issues "
                   "WHERE uid=?1 AND deleted=0 AND resolved=0;",
                   uid);
  }
  else
    rows = sl_exec("SELECT count(*) FROM issues "
                   "WHERE deleted=0 AND resolved=0;");

  return rows[0][0];
}

//Einen bestimmten Fehler loeschen
varargs int ToggleDeleteError(int issueid, string note)
{
  mixed rows = sl_exec("SELECT uid,deleted from issues WHERE id=?1;",
                       issueid);
  if (!pointerp(rows))
    return -1;
  
  if (!access_check(rows[0][0], M_DELETE))
    //zugriff zum Schreiben nicht gestattet
    return -10;

//  sl_exec("BEGIN TRANSACTION;");
  if (rows[0][1])
  {
    // was deleted -> undelete it
    sl_exec("UPDATE issues SET deleted=0,mtime=?2 WHERE id=?1;",
            issueid,time());
    db_add_note((<note_s> id: issueid, time: time(), user: getuid(TI),
                          txt: sprintf("Loeschmarkierung entfernt. (%s)",
                                       note ? note: "<kein Kommentar>")
                ));
  }
  else
  {
    // was not deleted -> delete it.
    sl_exec("UPDATE issues SET deleted=1,mtime=?2 WHERE id=?1;",
            issueid, time());
    db_add_note((<note_s> id: issueid, time: time(), user: getuid(TI),
                          txt: sprintf("Loeschmarkierung gesetzt. (%s)",
                                       note ? note: "<kein Kommentar>")
                ));
  }
//  sl_exec("COMMIT;");
  return !rows[0][1];
}


// sperrt den Eintrag oder entsperrt ihn.
// Sperre heisst hier, dass der Fehler vom Expire nicht automatisch geloescht
// wird.
varargs int LockIssue(int issueid, string note) {
    return set_lock(issueid, 1, note);
}

varargs int UnlockIssue(int issueid, string note) {
    return set_lock(issueid, 0, note);
}

// einen Fehler als gefixt markieren
varargs int ResolveIssue(int issueid, string note) {
    return set_resolution(issueid, 1, note);
}
// einen Fehler als nicht gefixt markieren
varargs int ReOpenIssue(int issueid, string note) {
    return set_resolution(issueid, 0, note);
}

varargs int AddNote(int issueid, string note)
{

    if (!stringp(note) || !sizeof(note))
      return(-3);

    // existiert die ID in der DB?
    struct fullissue_s issue = db_get_issue(issueid,0);
    if (!issue)
      return -1;

    if (!access_check(issue->uid, M_WRITE))
        //zugriff zum Schreiben nicht gestattet
        return(-10);

    return db_add_note((<note_s> id: issueid, time: time(), user: getuid(TI),
                                 txt: note));
}

//Einen bestimmten Fehler einer anderen UID zuweisen.
//Hashkey ist zwar eindeutig, aber Angabe der
//der UID ist deutlich schneller. Weglassen des Typs nochmal langsamer. ;-)
//Potentiell also sehr teuer, wenn man UID oder UID+Typ weglaesst.
varargs int ReassignIssue(int issueid, string newuid, string note)
{
    struct fullissue_s issue = db_get_issue(issueid,0);
    if (!issue)
      return -1;

    if (!access_check(issue->uid, M_REASSIGN))
        //zugriff zum Schreiben nicht gestattet
        return(-10);

    return db_reassign_issue(issueid, newuid, note);
}

/* *********** Eher fuer Debug-Zwecke *********************** */
/*
mixed QueryAll(int type) {
    //das koennte ein sehr sehr grosses Mapping sein, ausserdem wird keine
    //Kopie zurueckgegeben, daher erstmal nur ich...
    if (!this_interactive() || 
        member(MAINTAINER,getuid(this_interactive()))<0)
        return(-1);
    if (process_call()) return(-2);
    if (!type) return(-3);
    return(errors[type]);
}

mixed QueryResolved()
{
    //das koennte ein sehr sehr grosses Mapping sein, ausserdem wird keine
    //Kopie zurueckgegeben, daher erstmal nur ich...
    if (!this_interactive() || 
        member(MAINTAINER,getuid(this_interactive()))<0)
        return(-1);
    if (process_call()) return(-2);
    return(resolved);
}
*/

/* *****************  Internal Stuff   ******************** */
void create() {
    seteuid(getuid(ME));

    if (sl_open("/"LIBDATADIR"/secure/ARCH/errord.sqlite") != 1)
    //if (sl_open("/errord.sqlite") != 1)
    {
      raise_error("Datenbank konnte nicht geoeffnet werden.\n");
    }
    sl_exec("PRAGMA foreign_keys = ON; PRAGMA temp_store = 2; ");
    //string* res=sl_exec("PRAGMA quick_check(N);");
    //if (pointerp(res))
    //{
    //  raise_error("");
    //}
    //sl_exec("CREATE TABLE issue(id INTEGER,haskey TEXT);");
    foreach (string cmd :
        explode(read_file("/secure/ARCH/errord.sql.init"),";\n"))
    {
      if (sizeof(cmd) && cmd != "\n")
      {
        sl_exec(cmd);
      }
    }
    sl_exec("ANALYZE main;");

    lasterror=([]);
}

string name() {return("<Error-Daemon>");}

void save_me(int now) {
  if (now)
    save_object(SAVEFILE);
  else if (find_call_out(#'save_object)==-1) {
    call_out(#'save_object, 30, SAVEFILE);
  }
}

varargs int remove(int silent) {
    save_me(1);
    destruct(ME);
    return(1);
}


// sperrt den Eintrag (lock!=0) oder entsperrt ihn (lock==0). 
// Sperre heisst hier, dass der Fehler vom Expire nicht automatisch geloescht
// wird.
// liefert <0 im Fehlerfall, sonst Array mit Lockdaten
private varargs int set_lock(int issueid, int lock, string note)
{
    struct fullissue_s issue = db_get_issue(issueid,0);
    if (!issue)
      return -1;

    if (!access_check(issue->uid, M_WRITE))
        //zugriff zum Schreiben nicht gestattet
        return(-10);

    return db_set_lock(issueid, lock, note);
}

//markiert einen Fehler als gefixt, mit 'note' als Bemerkung (res!=0) oder
//markiert einen Fehler wieder als nicht-gefixt (resolution==0)
//liefert < 0 im Fehlerfall, sonst den neuen Zustand.
private varargs int set_resolution(int issueid, int resolution, string note)
{
    struct fullissue_s issue = db_get_issue(issueid,0);
    if (!issue)
      return -1;

    // Fixen duerfen nur zustaendige
    if (resolution
        && !access_check(issue->uid, M_FIX))
      return -10;
    // ggf. Fix zurueckziehen darf jeder mit M_WRITE
    if (!resolution &&
        !access_check(issue->uid, M_WRITE))
      return -10;

    int res = db_set_resolution(issueid, resolution, note);

    if (res == 1)
    {
      // Fehler jetzt gefixt.
      versende_mail(db_get_issue(issueid,0));  // per Mail verschicken
    }

    return res;
}

//ist der Zugriff auf uid erlaubt? Geprueft wird TI (wenn kein TI, auch kein
//Schreibzugriff)
//mode gibt an, ob lesend oder schreibend
private int access_check(string uid, int mode) {

    if (mode==M_READ)
        return LEARNER_SECURITY;  //lesen darf jeder Magier

    // In process_string() schonmal gar nicht.
    if (process_call()) return(0);
    // EM+ duerfen alles loeschen.
    if (ARCH_SECURITY) return(1);
    // eigene UIDs darf man auch vollumfaenglich bearbeiten.
    if (secure_euid()==uid) return(1);

    // alles andere wird speziell geprueft
    switch(mode)
    {
      // M_WRITE ist zur zeit eigentlich immer nen Append - das duerfen zur
      // Zeit alle ab Vollmagier
      case M_WRITE:
        return LEARNER_SECURITY;
      // Loeschen und Fixen duerfen zur Zeit nur Zustaendige.
      case M_DELETE:
      case M_REASSIGN:
      case M_FIX:
        // Master nach UIDs fragen, fuer die der jew. Magier
        // zustaendig ist.
        if (member(({string*})master()->QueryUIDsForWizard(secure_euid()),uid) >= 0)
          return 1;

        break;
    }

    return(0);        //Fall-through, neinRNER_SECURITY
}

public string format_stacktrace(struct frame_s* stacktrace) {
    string *lines;

    if (!pointerp(stacktrace) || !sizeof(stacktrace))
        return("");

    lines=allocate(sizeof(stacktrace));
    int i;
    foreach(struct frame_s frame: stacktrace)
    {
      lines[i]=sprintf("Fun: %.20O in Prog: %.40s\n"
                       "   Zeile: %.8d [%.50s]\n"
                       "   Evalcosts: %d",
                       frame->name,frame->prog,
                       frame->loc,frame->obj,frame->ticks);
      ++i;
    }
    return(implode(lines,"\n"));
}

public string format_notes(struct note_s* notes)
{
  int i;
  string text="";
  foreach(struct note_s note: notes)
  {
    text+=sprintf("Notiz %d von %.10s am %.30s\n%s",
        ++i,capitalize(note->user),dtime(note->time),
        break_string(note->txt, 78,"  "));
  }
  return text;
}

public string format_error_spieler(struct fullissue_s issue)
{
  string txt;
  string *label;

  if (!issue)
    return 0;

  switch(issue->type)
  {
    case T_RTERROR:
      label=({"Laufzeitfehler","Dieser Laufzeitfehler"});
      break;
    case T_REPORTED_ERR:
      label=({"Fehlerhinweis","Dieser Fehlerhinweis"});
      break;
    case T_REPORTED_TYPO:
      label=({"Typo","Dieser Typo"});
      break;
    case T_REPORTED_IDEA:
      label=({"Idee","Diese Idee"});
      break;
    case T_REPORTED_MD:
      label=({"Fehlendes Detail","Dieses fehlende Detail"});
      break;
    case T_REPORTED_SYNTAX:
      label=({"Syntaxproblem","Dieses Syntaxproblem"});
      break;
    case T_RTWARN:
      label=({"Laufzeitwarnung","Diese Laufzeitwarnung"});
      break;
    case T_CTWARN:
      label=({"Ladezeitwarnung","Diese Ladezeitwarnung"});
      break;
    case T_CTERROR:
      label=({"Ladezeitfehler","Dieser Ladezeitfehler"});
      break;
    default: return 0;
  }

  txt=sprintf( "\nDaten fuer %s mit ID '%s':\n"
               "Zeit: %25s\n",
               label[0], issue->hashkey,
               dtime(issue->ctime)
              );

  txt+=sprintf("%s",break_string(issue->message,78,
                  "Meldung:    ",BS_INDENT_ONCE));

  if (pointerp(issue->notes))
      txt+="Bemerkungen:\n"+format_notes(issue->notes)+"\n";

  return txt;
}

public string format_error(struct fullissue_s issue, int only_essential)
{
  string txt;
  string *label;

  if (!issue)
    return 0;

  switch(issue->type)
  {
    case T_RTERROR:
      label=({"Laufzeitfehler","Dieser Laufzeitfehler"});
      break;
    case T_REPORTED_ERR:
      label=({"Fehlerhinweis","Dieser Fehlerhinweis"});
      break;
    case T_REPORTED_TYPO:
      label=({"Typo","Dieser Typo"});
      break;
    case T_REPORTED_IDEA:
      label=({"Idee","Diese Idee"});
      break;
    case T_REPORTED_MD:
      label=({"Fehlendes Detail","Dieses fehlende Detail"});
      break;
    case T_REPORTED_SYNTAX:
      label=({"Syntaxproblem","Dieses Syntaxproblem"});
      break;
    case T_RTWARN:
      label=({"Laufzeitwarnung","Diese Laufzeitwarnung"});
      break;
    case T_CTWARN:
      label=({"Ladezeitwarnung","Diese Ladezeitwarnung"});
      break;
    case T_CTERROR:
      label=({"Ladezeitfehler","Dieser Ladezeitfehler"});
      break;
    default: return 0;
  }

  txt=sprintf( "\nDaten fuer %s mit ID %d:\n"
               "Hashkey: %s\n"
               "Zeit: %25s (Erstmalig: %25s)\n",
               label[0], issue->id, issue->hashkey,
               dtime(abs(issue->mtime)),dtime(issue->ctime)
              );

  if (stringp(issue->prog))
      txt += sprintf(
               "Programm:   %.60s\n"
               "Zeile:      %.60d\n",
               issue->prog, issue->loc
               );

  if (stringp(issue->obj))
      txt+=sprintf("Objekt:     %.60s\n",
               issue->obj);

  txt += sprintf("Loadname:   %.60s\n"
                 "UID:        %.60s\n",
                 issue->loadname, issue->uid);

  txt+=sprintf("%s",break_string(issue->message,78,
                  "Meldung:    ",BS_INDENT_ONCE));
  if (stringp(issue->hbobj))
      txt+=sprintf(
               "HB-Obj:     %.60s\n",issue->hbobj);

  if (stringp(issue->titp)) {
      txt+=sprintf(
               "TI/TP:      %.60s\n",issue->titp);
      if (stringp(issue->tienv))
          txt+=sprintf(
               "Environm.:  %.60s\n",issue->tienv);
  }

  if (!stringp(issue->command) ||
      !ARCH_SECURITY || process_call())
  {
      // Kommandoeingabe ist Privatsphaere und darf nicht von jedem einsehbar
      // sein.
      // in diesem Fall aber zumindest das Verb ausgeben, so vorhanden
      if (issue->verb)
          txt+=sprintf(
              "Verb:        %.60s\n",issue->verb);
  }
  // !process_call() && ARCH_SECURITY erfuellt...
  else if (stringp(issue->command))
      txt+=sprintf(
          "Befehl:     %.60s\n",issue->command);

  if (issue->caught)
      txt+=label[1]+" trat in einem 'catch()' auf.\n";

  if (!only_essential)
  {
    if (issue->deleted)
        txt+=label[1]+" wurde als geloescht markiert.\n";

    if (issue->locked)
        txt+=break_string(
            sprintf("%s wurde von %s am %s vor automatischem Loeschen "
            "geschuetzt (locked).\n",
            label[1],issue->locked_by, dtime(issue->locked_time)),78);
    if (issue->resolved)
        txt+=label[1]+" wurde als erledigt markiert.\n";
  }

  txt+=sprintf("%s trat bisher %d Mal auf.\n",
               label[1],issue->count);

  if (pointerp(issue->stack))
      txt+="Stacktrace:\n"+format_stacktrace(issue->stack)+"\n";

  if (pointerp(issue->notes))
      txt+="Bemerkungen:\n"+format_notes(issue->notes)+"\n";

  return txt;
}

// letzter Aenderung eines Spieler-/Magiersavefiles. Naeherung fuer letzten
// Logout ohne das Savefile einzulesen und P_LAST_LOGOUT zu pruefen.
private int recent_lastlogout(string nam, int validtime)
{
  if (!nam || sizeof(nam)<2) return 0;
  return file_time("/"LIBSAVEDIR"/" + nam[0..0] + "/" + nam + ".o") >= validtime;
}

// Versendet Mail an zustaendigen Magier und ggf. Spieler, der den Eintrag
// erstellt hat.
// ACHTUNG: loescht issue->command.
private int versende_mail(struct fullissue_s issue)
{
  // Versendet eine mail mit dem gefixten Fehler.
  mixed *mail;
  string text, *empf;
  int res = -1;

  mail = allocate(9);
  mail[MSG_FROM] = "<Fehler-Daemon>";
  mail[MSG_SENDER] = getuid(TI);
  mail[MSG_BCC] = 0;
  mail[MSG_SUBJECT] = sprintf("Fehler/Warnung in %s behoben", issue->loadname);
  mail[MSG_DATE] = dtime(time());

  // auch wenn ein EM fixt, sollen die Empfaenger nicht automatisch die
  // Spielereingabe erhalten, also command loeschen.
  issue->command = 0;

  // erstmal eine Mail an zustaendige Magier.
  empf = ({string*})master()->QueryWizardsForUID(issue->uid);
  // lang (180 Tage) nicht eingeloggte Magier ausfiltern
  empf = filter(empf, #'recent_lastlogout, time() - 15552000);
  if (sizeof(empf))
  {

    text = break_string(
      sprintf(STANDARDMAILTEXT,capitalize(getuid(TI)))
      +format_error(issue, 1),78,"",BS_LEAVE_MY_LFS);

    mail[MSG_RECIPIENT] = empf[0];
    if (sizeof(empf)>1)
      mail[MSG_CC] = empf[1..];
    else
      mail[MSG_CC] = 0;
    mail[MSG_BODY]=text;
    mail[MSG_ID]=sprintf(MUDNAME": %d.%d",time(),random(__INT_MAX__));

    if (!sizeof(({string*})"/secure/mailer"->DeliverMail(mail,0)))
      res = -1; // an niemanden erfolgreich zugestellt. :-(
    else
      res = 1;
  }

  // Bei von Spielern gemeldeten Fehler werden Spieler bei
  // Erledigung informiert, wenn deren letzter Logout weniger als 180 Tage her
  // ist.
  if ( (issue->type &
        (T_REPORTED_ERR|T_REPORTED_TYPO|T_REPORTED_IDEA|T_REPORTED_MD|
        T_REPORTED_SYNTAX))
      && issue->titp
      && recent_lastlogout(issue->titp, time() - 15552000) )
  {
    text = break_string(
      sprintf(STANDARDMAILTEXT_ERRORHINT,
        capitalize(issue->titp), capitalize(getuid(TI)))
      +format_error_spieler(issue), 78,"",BS_LEAVE_MY_LFS);

    mail[MSG_ID]=sprintf(MUDNAME": %d.%d",time(),random(__INT_MAX__));
    mail[MSG_RECIPIENT] = issue->titp;
    mail[MSG_CC] = 0;
    mail[MSG_SUBJECT] = sprintf("Fehler/Idee/Typo wurde von %s bearbeitet",
                                getuid(TI));
    mail[MSG_BODY] = text;

    if (!sizeof(({string*})"/secure/mailer"->DeliverMail(mail,0)))
      res |= -1;
    else
      res |= 1;
  }

  return res;
}

void reset()
{
  // geloeschte Issues sofort, gefixte 30 Tage nach letzter Aenderung
  // loeschen.
  sl_exec("DELETE FROM issues WHERE deleted=1;");
  sl_exec("DELETE FROM issues WHERE resolved=1 AND mtime<?1;",
          time()-30*24*3600);
  set_next_reset(3600*24);
}

// Nicht jeder Magier muss den ErrorD direkt zerstoeren koennen.
public string NotifyDestruct(object caller) {
  if( (caller!=this_object() && !ARCH_SECURITY) || process_call() ) {
    return "Du darfst den Error-Daemon nicht zerstoeren!\n";
  }
  return 0;
}


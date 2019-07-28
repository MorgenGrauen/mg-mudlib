// MorgenGrauen MUDlib
//
// gmcp.c -- Verwaltung von GMCP im Spielerobjekt
//
// $Id$

#pragma strong_types,save_types
#pragma range_check
#pragma no_clone
#pragma no_shadow
#pragma pedantic

#include <regexp.h>
#include <telnet.h>

#define NEED_PROTOTYPES
#include <player/base.h>
#include <thing/properties.h>
#include <living/attributes.h>
#include <player/gmcp.h>
#include <thing/description.h>
#include <living/description.h>
#undef NEED_PROTOTYPES

#include <properties.h>
#include <new_skills.h>
#include <rooms.h>
#include <tls.h>

inherit "/secure/telnetneg-structs.c";

struct gmcp_mod_s {
  string id;        // Name des GMCP-Moduls (z.B. "MG.Char")
  int version;      // Version des aktivierten moduls
  closure sendcl;   // Handler zum Senden (lfun-closure)
  closure recvcl;   // Handler zum Empfangen (lfunc-closure)
  mixed data;       // optional data of the module
};

nosave mapping gmcpdata;
/* Struktur:
   Jedes Modul hat einen Schluessel im Toplevel, worunter ggf. seine Daten
   abgelegt sind. Die Daten sind eine struct gmcp_mod_s. Der Schluessel ist
   der Modulname OHNE Version.
   */
#define NEED_PROTOTYPES
#include "/secure/telnetneg.h"
#undef NEED_PROTOTYPES

//#define __GMCP_DEBUG__ 1
// Low priority debug messages
#define GMCP_DEBUG(pre,msg,prio) if (interactive(this_object()) \
                            && gmcpdata) \
                          GMCP_debug(pre,msg,prio);
// higher priority error messages
#define GMCPERROR(msg) if (interactive(this_object()) \
                       && gmcpdata) \
                     GMCP_debug("ERROR",msg,10);


// **************** API nach Aussen folgt ab hier ********************

// Wird vom Spielerobjekt gerufen, wenn sich Daten am Charakter veraendert
// haben, die gesendet werden sollten.
// Dies ist eigentlich nur ein Wrapper, der die Daten an den Handler eines
// Moduls weitergibt, welches vom Client aktiviert wurde. Hierzu kommen zur
// Zeit 2 in Frage: MG.Char (bevorzugt) und Char (minimaler Support).
/*protected*/ int GMCP_Char(mapping data) {

  if (!mappingp(gmcpdata)) return 0;

  // Als erstes schauen, ob der Client MG.Char aktiviert hat.
  struct gmcp_mod_s mod = gmcpdata["MG.char"];
  if (structp(mod) && closurep(mod->sendcl))
  {
    funcall(mod->sendcl, data);
    return 1;
  }
  // Dann noch das Modul char pruefen. Das ist aber ziemlich eingeschraenkt
  // und es gibt hoffentlich nicht viele Clients, die es benutzen.
  // (Aardwolf-Modul)
  mod = gmcpdata["char"];
  if (structp(mod) && closurep(mod->sendcl))
  {
    funcall(mod->sendcl, data);
    return 1;
  }
  // Dann noch das Modul Char pruefen. Das ist aber ziemlich eingeschraenkt
  // und es gibt hoffentlich nicht viele Clients, die es benutzen.
  // (IRE-Modul)
  mod = gmcpdata["Char"];
  if (structp(mod) && closurep(mod->sendcl))
  {
    funcall(mod->sendcl, data);
    return 1;
  }
  return 0;
}

/*protected*/ int GMCP_Channel(string msg, string channel, string sender) {
  if (!mappingp(gmcpdata)) return 0;
  // comm.channel Modul aktiv?
  struct gmcp_mod_s mod = gmcpdata["comm.channel"];
  if (structp(mod) && closurep(mod->sendcl))
  {
    funcall(mod->sendcl, (["chan":channel, "player": sender,
                           "msg": msg]) );
    return 1;
  }
  return 0;
}

/*protected*/ int GMCP_Room() {
  if (!mappingp(gmcpdata)) return 0;
  // MG.room Modul aktiv?
  struct gmcp_mod_s mod = gmcpdata["MG.room"];
  if (structp(mod) && closurep(mod->sendcl))
  {
    funcall(mod->sendcl, 0);
    return 1;
  }
  return 0;
}

// **************** Ab hier folgen eher die Lowlevel-Dinge ***********
private void GMCP_debug(string pre, string msg, int prio) {
  struct gmcp_mod_s mod = gmcpdata["Core"];
  if (mod && (mod->data)["Debug"] >= prio)
    tell_object(this_object(), sprintf("GMCP %s: %s\n",pre,msg));
}

private void GMCP_send(string cmd, mixed data)
{
  GMCP_DEBUG("GMCP_send",sprintf("%s %O",cmd,data), 30);
  send_telnet_neg_str(sprintf("%c%c%s %s", SB, TELOPT_GMCP,
                              cmd, json_serialize(data)), 1);
}

private void GMCP_unregister_module(string mod)
{
  int version;
  // Wenn nicht "mod version" Schema, ignorieren
  if (sscanf(mod, "%s %d", mod, version) != 2)
      return;

  if (mod=="Core") // darf nicht abgeschaltet werden.
      return;

  m_delete(gmcpdata, mod);
}

private void GMCP_register_module(string modname)
{
  int version;
  GMCP_DEBUG("register_module(): trying ... ",modname, 20);
  // Wenn nicht "mod version" Schema, ignorieren
  if (sscanf(modname, "%s %d", modname, version) != 2)
      return;

//  GMCP_DEBUG("register_module()",modname + " v" + version);

  // Modul (ggf. mit anderer Version) bereits aktiv?
  struct gmcp_mod_s mod = gmcpdata[modname];
  if (structp(mod)) {
    // wenn gleicher Name und Version, wird nix gemacht, bei anderer Version
    // wird ein neuer Handler eingetragen und die Daten geloescht.
    // Wenn nicht-existierende Modul/Version-Kombi angefordert wird, ist das
    // Modul hinterher aus.
    if (mod->id == modname && mod->version == version)
      return;
    else
      m_delete(gmcpdata,modname);
  }

  // Das GMCP-Modul ist nur verfuegbar, wenn es zu der Kombination aus mod und
  // version einen Handler zum Senden gibt...
  // Der Handler ist: GMCP_<mod>_v<version>_send, aber in <mod> werden alle
  // "." durch "_" ersetzt.
  string replacedname = regreplace(modname, "\\.", "_", RE_GLOBAL);
  closure sendcl = symbol_function(sprintf("GMCPmod_%s_v%d_send",
                      replacedname, version),
                   this_object());
  if (!sendcl)
    return;
  // Diese Closure darf 0 sein. Dann findet keine Behandlung von vom Client
  // gesendeten Kommandos statt. Was fuer die meisten Module auch in Ordnung
  // ist, da sie dem Client keine Kommandos anbieten.
  closure recvcl = symbol_function(sprintf("GMCPmod_%s_v%d_recv",
                      replacedname, version),
                   this_object());

  GMCP_DEBUG("register_module()",modname+" erfolgreich registriert.",10);

  mod = (<gmcp_mod_s> id: modname, version : version,
         sendcl : sendcl, recvcl: recvcl, data: ([]) );
  gmcpdata[modname] = mod;

  // Zum schluss noch den Senden-handler mal rufen, damit der mal alle
  // verfuegbaren daten sendet.
  funcall(mod->sendcl, 0);
}

// Handler fuer das Core Modul von GMCP
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_Core_v1_recv(string cmd, mixed args)
{
  struct gmcp_mod_s mod = gmcpdata["Core"];
  mapping data = mod->data;

/*  if (!mod)
  {
    GMCPERROR("Command %s for disabled module ignored.");
    return;
  }
  */
  GMCP_DEBUG("GMCPmod_Core_v1: ", cmd, 20);

  switch (cmd)
  {
    case "Core.Hello":
      if (mappingp(args))
        data["Hello"] = (["client": args["client"],
                          "version": args["version"] ]);
      break;
    case "Core.Supports.Set":
      if (pointerp(args))
      {
        // Alte Module abschalten/loeschen
        foreach(string m : data["Supports"])
          GMCP_unregister_module(m);
        data["Supports"] = args;
        // Versuchen, die neuen Module zu registrieren
        foreach(string m : args)
          GMCP_register_module(m);
      }
      else
          GMCP_DEBUG("GMCPmod_Core_v1: ",
              "Data for Core.Supports.Set is no array", 5);
      break;
    case "Core.Supports.Add":
      if (!pointerp(data["Supports"]))
        data["Supports"] = ({});
      if (pointerp(args))
      {
        foreach(string m: args)
          GMCP_register_module(m);
        data["Supports"] += args;
      }
      break;
    case "Core.Supports.Remove":
      if (!pointerp(data["Supports"]))
        break;
      if (pointerp(args))
      {
        foreach(string m: args)
          GMCP_unregister_module(m);
        data["Supports"] -= args;
      }
      break;
    case "Core.Supports.KeepAlive":
      break;  // this is ignored by us.
    case "Core.Ping":
      if (intp(args))
        data["Ping"] = args;
      // send a ping back
      GMCP_send("Core.Ping",0);
      break;
    case "Core.Debug":
      if (intp(args) && args >= 0)
        data["Debug"] = args;
      break;
    default:
      GMCPERROR(sprintf("Unknown GMCP Core cmd %s with args %O",
            cmd, args));
      break;
  }
}

// Handler fuer das Core Modul von GMCP
// Gerufen, wenn Daten zu senden sind.
protected void GMCPmod_Core_v1_send(mapping data)
{
  // Wenn Core registriert wird, wird diese Funktion gerufen und <data> als 0
  // uebergeben. Wir nutzen das zur Erkennung, dass GMCP aktiviert wurde und
  // senden die URI fuer das Client-UI-Package.
  // Bemerkung: ja...  Warum zur Hoelle macht Mudlet das so? Es sollte ein
  // Modul UI definiert werden, was vom Client angefordert wird, anstatt dass
  // wir auf Verdacht da etwas aus einem nicht-angeforderten Modul rauspusten,
  // sobald GMCP aktiviert wird.
  // Ausserdem: Mudlet erwartet da kein Key-Value-Paar, sondern Key und Value
  // zusammen in einem String im Key und dafuer keine Daten... Das wird
  // hoffentlich bald in Mudlet gefixt... Bis dahin machen wir das testweise
  // mal, so furchtbar es auch ist.
  // TODO: Mudlet pruefen und aendern!
  // *grrrrr*
  if (!data)
    send_telnet_neg_str(sprintf("%c%c%s", SB, TELOPT_GMCP,
             "Client.GUI 1\nhttp://mg.mud.de/download/testgui.mpackage"), 1);

  // Zur Zeit passiert hier weiter nix, spaeter mal Core.Goodbye senden.
}


// Handler fuer das MG.Char Modul
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_MG_char_v1_send(mapping data)
{
  mapping squeue = m_allocate(5,0);
  struct gmcp_mod_s mod = gmcpdata["MG.char"];
  // mod->data fungiert hier auch als Cache der Daten. Die muss man naemlich
  // leider immer alle senden, nicht nur die geaenderten.
  if (!mappingp(data))
  {
    // Alle verfuegbaren Informationen senden...
    mod->data = m_allocate(6);
    m_add(mod->data, "MG.char.base",
              ([P_NAME: Name(WER),
                P_GUILD: QueryProp(P_GUILD),
                P_PRESAY: QueryProp(P_PRESAY),  // TODO
                P_TITLE: QueryProp(P_TITLE),
                "wizlevel": query_wiz_level(this_object()),
                P_RACE: QueryProp(P_RACE)]) );  // TODO
    m_add(mod->data,"MG.char.vitals", 
              ([P_HP: QueryProp(P_HP),
                P_SP: QueryProp(P_SP),
                P_POISON: QueryProp(P_POISON) ]) );
    m_add(mod->data,"MG.char.maxvitals",
              ([P_MAX_HP: QueryProp(P_MAX_HP),
                P_MAX_SP: QueryProp(P_MAX_SP),
                P_MAX_POISON: QueryProp(P_MAX_POISON) ]) );
    m_add(mod->data,"MG.char.attributes",
              ([ A_STR: QueryAttribute(A_STR),
                 A_INT: QueryAttribute(A_INT),
                 A_DEX: QueryAttribute(A_DEX),
                 A_CON: QueryAttribute(A_CON) ]) );
    m_add(mod->data,"MG.char.info",
              ([P_LEVEL: QueryProp(P_LEVEL),
                P_GUILD_LEVEL: QueryProp(P_GUILD_LEVEL),
                P_GUILD_TITLE: QueryProp(P_GUILD_TITLE) ]) );
    m_add(mod->data,"MG.char.wimpy",
              ([P_WIMPY: QueryProp(P_WIMPY),
                P_WIMPY_DIRECTION: QueryProp(P_WIMPY_DIRECTION) ]) );
    m_add(squeue,"MG.char.base");
    m_add(squeue,"MG.char.vitals");
    m_add(squeue,"MG.char.maxvitals");
    m_add(squeue,"MG.char.attributes");
    m_add(squeue,"MG.char.info");
    m_add(squeue,"MG.char.wimpy");
    // dies wird direkt gesendet, weil es nicht gespeichert werden muss. (wird
    // nur beim Start des Moduls gesendet).
    GMCP_send("MG.char.infoVars", ([
          P_LEVEL: "Spielerstufe", P_GUILD_LEVEL: "Gildenstufe",
          P_GUILD_TITLE: "Gildentitel" ]) );
  }
  else
  {
    // nur die in data enthaltenen senden.
    // jetzt erstmal alles aus data so sortieren, wie es gesendet werden
    // muss... *seufz*
    foreach(string key, mixed val : data)
    {
      switch(key)
      {
        case P_HP:
        case P_SP:
        case P_POISON:
          (mod->data)["MG.char.vitals"] += ([key: val]);
          m_add(squeue,"MG.char.vitals");
          break;
        case P_MAX_HP:
        case P_MAX_SP:
        case P_MAX_POISON:
          (mod->data)["MG.char.maxvitals"] += ([key: val]);
          m_add(squeue,"MG.char.maxvitals");
          break;
        case P_NAME:
          (mod->data)["MG.char.base"] += ([key: Name(WER)]);
          m_add(squeue,"MG.char.base");
          break;
        case P_RACE:
        case P_PRESAY:
        case P_TITLE:
        case P_GUILD:
          (mod->data)["MG.char.base"] += ([key: val]);
          m_add(squeue,"MG.char.base");
          break;
        case A_DEX:
        case A_STR:
        case A_CON:
        case A_INT:
          (mod->data)["MG.char.attributes"] += ([key: val]);
          m_add(squeue,"MG.char.attributes");
          break;
        case P_LEVEL:
        case P_GUILD_LEVEL:
        case P_GUILD_TITLE:
          (mod->data)["MG.char.info"] += ([key: val]);
          m_add(squeue,"MG.char.info");
          break;
        case P_WIMPY:
        case P_WIMPY_DIRECTION:
          (mod->data)["MG.char.wimpy"] += ([key: val]);
          m_add(squeue,"MG.char.wimpy");
          break;
      }
    }
  }
  GMCP_DEBUG("GMCPmod_MG_char_v1_send()",
      sprintf("Data ready: %O, Sendqueue: %O",mod->data, squeue),50);

  // Jetzt die squeue senden...
  foreach(string key : squeue)
  {
    GMCP_send(key, (mod->data)[key]);
  }
}

// Handler fuer das MG.Char Modul
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_MG_char_v1_recv(string cmd, mixed args)
{
  // dieses Modul bietet dem Client keine Kommandos an, daher ignorieren.
  GMCP_DEBUG("GMCPmod_MG_Char_v1_recv","Client-Kommando ignoriert: "+cmd,20);
}

/*
// Handler fuer das MG.Room Modul von GMCP
// Gerufen, wenn Daten zu senden sind.
protected void GMCPmod_MG_Room_v1_send(mapping data)
{
}

// Handler fuer das Room Modul von GMCP
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_MG_Room_v1_recv(string cmd, mixed args)
{
  // dieses Modul bietet dem Client keine Kommandos an, daher ignorieren.
  GMCP_DEBUG("GMCPmod_MG_Room_v1_recv","Client-Kommando ignoriert: "+cmd,20);
}
*/

// Recv Handler fuer das comm.channel Modul von GMCP
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_comm_channel_v1_recv(string cmd, mixed args)
{
  GMCP_DEBUG("GMCPmod_comm_channel_v1_recv",
      "Client-Kommando ignoriert: "+cmd,20);
}

// Send Handler fuer das comm.channel Modul von GMCP
protected void GMCPmod_comm_channel_v1_send(mapping data)
{
  // Ganz simpel: einfach raussenden...
  // Core uebergibt beim Einschalten 0 als data. Dieses modul muss aber beim
  // Eisnchalten nix machen. Also nur ignorieren.
  if (mappingp(data))
    GMCP_send("comm.channel", data);
}

// Recv Handler fuer das MG.room Modul von GMCP
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_MG_room_v1_recv(string cmd, mixed args)
{
  GMCP_DEBUG("GMCPmod_MG_room_v1_recv",
      "Client-Kommando ignoriert: "+cmd,20);
}

// Send Handler fuer das comm.channel Modul von GMCP
protected void GMCPmod_MG_room_v1_send(mapping data)
{
  // Bekommt immer 0 als <data> uebergeben und sucht sich die Daten aus dem
  // Raum zusammen.

  // Baeh. Warum wird das denn ohne Env gerufen. :-(
  if (!environment())
    return;

  // Blind gibt es auch per GMCP nix.
  if (CannotSee(1))
    return;

  int restr = environment()->QueryProp(P_MAP_RESTRICTIONS);

  if (restr & MR_NOINFO)
    return; // gar keine info senden.

  // Anmerkung: int_short() waere cool. Dummerweise uebertraegt das auch
  // sichtbare Ausgange und Objekte. Insofern: geht nicht.
  // Ist das letzte Zeichen kein Satzzeichen einen Punkt anhaengen, sonst nur
  // den \n.
  string sh=process_string(environment()->QueryProp(P_INT_SHORT)||"");
  switch(sh[<1])
  {
    case ".":
    case "!":
    case "?":
      break;
    default:
      sh+=".";
      break;
  }
  data = ([
      P_SHORT: sh,
      "domain": environment()->QueryProp(P_DOMAIN) || "unbekannt",
      ]);

  // sichtbare Ausgaenge ausgeben
  mixed hide = environment()->QueryProp(P_HIDE_EXITS);
  if (hide && !pointerp(hide))
      data["exits"] = ({});   // alle verstecken
  else
  {
      // Query() verwenden, damit sowohl normale als auch Special Exits
      // kommen... Die Summe von beiden wuerde auch gehen, aber dann hat man
      // zwei unnoetige Filter in den Querymethoden. Hngl.
      mapping exits = environment()->Query(P_EXITS, F_VALUE) || ([]);
      if (pointerp(hide))
        data["exits"] = m_indices(exits) - hide;
      else
        data["exits"] = m_indices(exits);
  }

  if (restr & MR_NOUID)
    data["id"] = "";
  else
    data["id"] = hash(TLS_HASH_MD5, object_name(environment()));

  GMCP_send("MG.room.info", data);
}


// Handler fuer das "char" Modul von GMCP (Modul von Aardwolf)
// Gerufen, wenn Daten zu senden sind.
protected void GMCPmod_char_v1_send(mapping data)
{
  mapping squeue = m_allocate(4,0);
  struct gmcp_mod_s mod = gmcpdata["char"];
  // mod->data fungiert hier auch als Cache der Daten. Die muss man naemlich
  // leider immer alle senden, nicht nur die geaenderten.
  if (!mappingp(data))
  {
    // Alle verfuegbaren Informationen senden...
    mod->data = m_allocate(4);
    m_add(mod->data, "char.base", (["name": query_real_name(),
                                 "race": QueryProp(P_RACE)]) );
    m_add(mod->data,"char.vitals", (["hp": QueryProp(P_HP),
                                  "mana": QueryProp(P_SP)]) );
    m_add(mod->data,"char.stats", ([ "str": QueryAttribute(A_STR),
                               "int": QueryAttribute(A_INT),
                               "dex": QueryAttribute(A_DEX),
                               "con": QueryAttribute(A_CON) ]) );
    m_add(mod->data,"char.status", (["level": QueryProp(P_LEVEL) ]) );
    m_add(squeue,"char.base");
    m_add(squeue,"char.vitals");
    m_add(squeue,"char.stats");
    m_add(squeue,"char.status");
  }
  else
  {
    // nur die in data enthaltenen senden.
    // jetzt erstmal alles aus data so sortieren, wie es gesendet werden
    // muss... *seufz*
    foreach(string key, mixed val : data)
    {
      switch(key)
      {
        case P_HP:
          (mod->data)["char.vitals"] += (["hp": val]);
          m_add(squeue,"char.vitals");
          break;
        case P_SP:
          (mod->data)["char.vitals"] += (["mana": val]);
          m_add(squeue,"char.vitals");
          break;
        case P_NAME:
        case P_RACE:
          (mod->data)["char.base"] += ([key: val]);
          m_add(squeue,"char.base");
          break;
        case A_DEX:
        case A_STR:
        case A_CON:
        case A_INT:
          (mod->data)["char.stats"] += ([key: val]);
          m_add(squeue,"char.stats");
          break;
        case P_LEVEL:
          (mod->data)["char.status"] += ([key: val]);
          m_add(squeue,"char.status");
          break;
      }
    }
  }
  GMCP_DEBUG("GMCPmod_char_v1_send()",
      sprintf("Data ready: %O, Sendqueue: %O",mod->data, squeue),50);

  // Jetzt die squeue senden...
  foreach(string key : squeue)
  {
    GMCP_send(key, (mod->data)[key]);
  }
}

// Handler fuer das "char" Modul von GMCP (Modul von Aardwolf)
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_char_v1_recv(string cmd, mixed data)
{
  // dieses Modul bietet dem Client keine Kommandos an, daher ignorieren.
  GMCP_DEBUG("GMCPmod_char_v1_recv","Client-Kommando ignoriert: "+cmd,20);
}


// Handler fuer das "Char" Modul von GMCP (Modul von IRE)
// Gerufen, wenn Daten zu senden sind.
protected void GMCPmod_Char_v1_send(mapping data)
{
  mapping squeue = m_allocate(4,0);
  struct gmcp_mod_s mod = gmcpdata["Char"];
  // mod->data fungiert hier auch als Cache der Daten. Die muss man naemlich
  // leider immer alle senden, nicht nur die geaenderten.
  if (!mappingp(data))
  {
    // Alle verfuegbaren Informationen senden...
    mod->data = m_allocate(4);
    m_add(mod->data,"Char.Vitals", (["hp": QueryProp(P_HP),
                                     "mp": QueryProp(P_SP),
                                     "maxhp": QueryProp(P_MAX_HP),
                                     "maxmp": QueryProp(P_MAX_SP) ]) );
    m_add(mod->data,"Char.Status", (["level": QueryProp(P_LEVEL),
                                     "guild": QueryProp(P_GUILD) ]) );
    m_add(squeue,"Char.Vitals");
    m_add(squeue,"Char.Status");
    // dies wird direkt gesendet, weil es nicht gespeichert werden muss. (wird
    // nur beim Start des Moduls gesendet).
    GMCP_send("Char.StatusVars", ([
          "level": "Spielerstufe", "guild": "Gilde" ]) );
  }
  else
  {
    // nur die in data enthaltenen senden.
    // jetzt erstmal alles aus data so sortieren, wie es gesendet werden
    // muss... *seufz*
    foreach(string key, mixed val : data)
    {
      switch(key)
      {
        case P_HP:
          (mod->data)["Char.Vitals"] += (["hp": val]);
          m_add(squeue,"Char.Vitals");
          break;
        case P_SP:
          (mod->data)["Char.Vitals"] += (["mp": val]);
          m_add(squeue,"Char.Vitals");
          break;
        case P_MAX_HP:
          (mod->data)["Char.Vitals"] += (["maxhp": val]);
          m_add(squeue,"Char.Vitals");
          break;
        case P_MAX_SP:
          (mod->data)["Char.Vitals"] += (["maxmp": val]);
          m_add(squeue,"Char.Vitals");
          break;
        case P_LEVEL:
        case P_GUILD:
          (mod->data)["Char.Status"] += ([key: val]);
          m_add(squeue,"Char.Status");
          break;
      }
    }
  }
  GMCP_DEBUG("GMCPmod_Char_v1_send()",
      sprintf("Data ready: %O, Sendqueue: %O",mod->data, squeue),50);

  // Jetzt die squeue senden...
  foreach(string key : squeue)
  {
    GMCP_send(key, (mod->data)[key]);
  }
}

// Handler fuer das "char" Modul von GMCP (Modul von Aardwolf)
// Gerufen bei Empfang von Kommandos vom Client.
protected void GMCPmod_Char_v1_recv(string cmd, mixed args)
{
  // dieses Modul bietet dem Client keine Kommandos an, daher ignorieren.
  GMCP_DEBUG("GMCPmod_Char_v1_recv","Client-Kommando ignoriert: "+cmd,20);
}


// Handler, der von telnetneg.c gerufen wird.
private void _std_re_handler_gmcp(struct telopt_s opt, int action,
                                  int *optargs)
{
  switch(action)
  {
    case LOCALON:
      // super!
      GMCP_DEBUG("recv:", "LOCALON",10);
      gmcpdata = ([]);
      opt->data = gmcpdata;   // daten auch dort ablegen.
      // Coremodule in der Version 1 registrieren (es gibt nur eine).
      GMCP_register_module("Core 1");
#ifdef __GMCP_DEBUG__
      GMCPmod_Core_v1_recv("Core.Debug",30);
#endif
      break;
    case LOCALOFF:
      // alles abschalten und Daten loeschen
      GMCP_DEBUG("recv:", "LOCALOFF",10);
      opt->data = 0;
      gmcpdata = 0;
      break;
    case REMOTEON:
    case REMOTEOFF:
      // Huch. Auf Clientseite ist GMCP eigentlich nie an. Ignorieren...
      GMCP_DEBUG("recv:", "Huh? REMOTE state changed?",50);
      break;
    case SB:
      // Der eigentlich interessante Fall... GMCP-Kommandos
      if (!mappingp(gmcpdata)) return; // GMCP wohl nicht eingeschaltet...
      string cmd;
      mixed args;
      string payload=to_string(optargs);
      GMCP_DEBUG("recv", payload,10);
      if (sscanf(payload,"%s %s", cmd, args) != 2) {
        // ist vermutlich ein Kommando ohne daten (oder Muell)
        cmd = payload;
        //args = 0;
      }
      else
      {
        string err=catch(args = json_parse(args);nolog);
        if (err)
        {
          printf("\nFehler beim Parsen einer GMCP-Nachricht: %s. "
                 "Nachricht war: '%s'\n"
                 "Befehl: '%s', Argument: '%s'\n\n",err,payload,cmd,args||"");
          return;
        }
      }
      GMCP_DEBUG("recv", sprintf("Command: %s, Data: %O", cmd, args),20);

      string *cmdparts = explode(cmd, ".");
      struct gmcp_mod_s mod;
      string modname;
      // versuch, ein Modul fuer das Kommando zu finden. Anfangen mit der
      // Annahme, dass bis zum letzten Punkt der Modulname geht und dann
      // in jedem case einen Punkt kuerzer werdend.
      switch(sizeof(cmdparts))
      {
        case 4:
          modname = implode(cmdparts[0..2],".");
          GMCP_DEBUG("trying modname... ", modname, 20 );
          if (member(gmcpdata, modname)) {
            mod = gmcpdata[modname];
            funcall(mod->recvcl, cmd, args);
            break;
          }
          // Fall-through!
        case 3:
          modname = implode(cmdparts[0..1],".");
          GMCP_DEBUG("trying modname... ", modname, 20);
          if (member(gmcpdata, modname)) {
            mod = gmcpdata[modname];
            funcall(mod->recvcl, cmd, args);
            break;
          }
          // Fall-through!
        case 2:
          modname = implode(cmdparts[0..0],".");
          GMCP_DEBUG("trying modname... ", modname, 20);
          if (member(gmcpdata, modname)) {
            mod = gmcpdata[modname];
            funcall(mod->recvcl, cmd, args);
            break;
          }
          // Hier ists jetzt nen Fehler.
          GMCPERROR(sprintf("Unknown GMCP module for cmd %s",cmd));
          break;
        default:
          // zuviele oder zuwenig . ;-)
          GMCPERROR(sprintf("Illegal GMCP cmd %s with args %O",
                cmd, args));
          break;
      }
      // sbdata brauchen wir eigentlich nicht mehr.
      opt->re_wishes->sbdata = 0;
      break;
  } // switch (action)
}

// wird von base.c nach Konnektierung gerufen.
// Darf aber erst gerufen werden, wenn das Spielerobjekt fertig initialisiert
// und eingelesen ist.
protected void startup_telnet_negs()
{
  // evtl. war es ein reconnect, dann steht in gmcp noch alter kram drin. Der
  // muss weg, koennte ja auch sein, dass der Client (jetzt) kein GMCP
  // mehr
  // will.
  gmcpdata = 0;

  // Hack besonderer Sorte: GMCP soll lokal eingeschaltet sein. Auf
  // Clientseiten ist es laut Protokoll nicht vorgesehen, daher duerfen
  // (sollten?) wir kein DO an den Client senden. Wir brauchen aber einen
  // remote handler, um die Wuensche vom Client zu verarbeiten. Daher erstmal
  // nur den local handler binden (und gleichzeitig negotiation anstossen) und
  // dann direkt danach den remote handler auch binden (ohne erneute
  // negotiation zu starten). Achja und wir nehmen die gleiche Funktion als
  // Handler fuer remote und lokal.
  bind_telneg_handler(TELOPT_GMCP, 0, #'_std_re_handler_gmcp, 1);
  bind_telneg_handler(TELOPT_GMCP, #'_std_re_handler_gmcp,
                                   #'_std_re_handler_gmcp, 0);
}


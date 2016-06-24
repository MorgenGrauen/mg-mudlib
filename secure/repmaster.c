#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma warn_deprecated

#include <reputation.h>
#include <wizlevels.h>

/* Datenstruktur reputations:
 * (["id": (["uids": ({ "uid1", "uid2" }), "name":"", "changemsg":"",  
 *           "flags":x|x|x
 *         ]) 
 * ])
 * Bsp:
 * (["schaedlspalta": (["uids": ({"d.wald.nibel", "d.wald.kessa"}),
 *                      "name": "Schaedlspalta-Klan", "active": 1,
 *                      "changemsg":"beim Schaedlspalta-Klan" 
 *                    ])
 * ])
 */

private mapping reputations;

protected void create() {
  seteuid(getuid(this_object()));

  if(!restore_object(REP_SAVEFILE) || !mappingp(reputations)) 
    reputations = ([ ]);
}

// Zugriff auf Handlingmethoden fuer ROOT, EMs+
private int Sec() {
  if(process_call()) return 0;
  if(previous_object() && geteuid(previous_object()) == ROOTID) return 1;
  return objectp(previous_object()) && ARCH_SECURITY;
}


/* Argumente:
 * repid = id im mapping reputations
 * validuids = UIDs welche die Reputation veraendern duerfen, String
 *             oder String-Array
 * name = Name der Reputationsfraktion 
 * changemsg = Text der in die Default-Changemsg eingefuegt wird
 *             (Dein Ansehen %s hat sich ... verbessert.)
 *
 * Returns:
 *  1 = Reputation erfolgreich hinzugefuegt
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Reputation bereits vorhanden
 */
public int AddReputation(string repid, mixed validuids, string name,
                         string changemsg) {
  if(!Sec()) 
    return -1;
  if(stringp(validuids)) validuids = ({ validuids });
  if(!pointerp(validuids)) validuids = ({ });
  // validuids + changemsgs kann auch erstmal leer bleiben
  if(!stringp(repid) || !sizeof(repid) || !stringp(name) || !sizeof(name))
    return -2;
  if(member(reputations, repid))
    return -3;
  reputations += ([ repid : ([ 
    "uids": validuids,
    "name": name,
    "flags": REP_FLAG_ACTIVE,
    "changemsg": changemsg
  ]) ]);
  save_object(REP_SAVEFILE);
  tell_object(this_interactive(), sprintf("Reputation eingetragen:\n"
    "Name: %s\nValid UIDs: %O\nFlags: REP_FLAG_ACTIVE\nChangemsg: %s\n",
    reputations[repid]["name"], reputations[repid]["uids"],
    reputations[repid]["changemsg"]));
  return 1;
}

/* Argumente:
 * repid = id im mapping reputations
 *
 * Returns:
 *  1 = wurde erfolgreich deaktiviert
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Rep nicht vorhanden
 * -4 = Ist bereits deaktiviert
 */
public int DeactivateReputation(string repid) {
  if(!Sec())
    return -1;
  if(!stringp(repid) || !sizeof(repid))
    return -2;
  if(!member(reputations, repid))
    return -3;
  if(!(reputations[repid]["flags"] & REP_FLAG_ACTIVE)) 
    return -4;
  reputations[repid]["flags"] |= REP_FLAG_ACTIVE;
  save_object(REP_SAVEFILE);
  return 1;
}

/* Argumente:
 * repid = id im mapping reputations
 *
 * Returns:
 *  1 = wurde erfolgreich aktiviert
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Rep nicht vorhanden
 * -4 = Ist bereits aktiviert
 */
public int ActivateReputation(string repid) {
  if(!Sec())
    return -1;
  if(!stringp(repid) || !sizeof(repid))
    return -2;
  if(!member(reputations, repid))
    return -3;
  if(reputations[repid]["flags"] & REP_FLAG_ACTIVE) 
    return -4;
  reputations[repid]["flags"] ^= REP_FLAG_ACTIVE;
  save_object(REP_SAVEFILE);
  return 1;
}

/* Argumente:
 * repid = id im mapping reputations
 * uid = UID welche die Reputation veraendern darf
 *
 * Returns:
 *  1 = UID erfolgreich hinzugefuegt
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Rep nicht vorhanden
 * -4 = UID ist bereits vorhanden
 */
public int AddReputationUid(string repid, string uid) {
  if(!Sec())
    return -1;
  if(!stringp(repid) || !sizeof(repid) || !stringp(uid) || !sizeof(uid))
    return -2;
  if(!member(reputations, repid))
    return -3;
  if(member(reputations[repid]["uids"], uid) != -1) 
    return -4;
  reputations[repid]["uids"] += ({ uid });
  save_object(REP_SAVEFILE);
  return 1;
}

/* Argumente:
 * repid = id im mapping reputations
 * uid = UID welche die Reputation nicht mehr veraendern darf
 *
 * Returns:
 *  1 = UID erfolgreich geloescht
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Rep nicht vorhanden
 * -4 = UID nicht vorhanden
 */
public int RemoveReputationUid(string repid, string uid) {
  if(!Sec())
    return -1;
  if(!stringp(repid) || !sizeof(repid) || !stringp(uid) || !sizeof(uid))
    return -2;
  if(!member(reputations, repid))
    return -3;
  if(member(reputations[repid]["uids"], uid) == -1) 
    return -4;
  reputations[repid]["uids"] -= ({ uid });
  save_object(REP_SAVEFILE);
  return 1;
}

/* Argumente:
 * repid = id im mapping reputations
 * name = Neuer Name der Rep
 *
 * Returns:
 *  1 = Neuer Name erfolgreich gesetzt
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Rep nicht vorhanden
 * -4 = Name entspricht dem derzeitigen
 */
public int ChangeReputationName(string repid, string name) {
  if(!Sec())
    return -1;
  if(!stringp(repid) || !sizeof(repid) || !stringp(name) || !sizeof(name))
    return -2;
  if(!member(reputations, repid))
    return -3;
  if(reputations[repid]["name"] == name) 
    return -4;
  reputations[repid]["name"] = name;
  save_object(REP_SAVEFILE);
  return 1;
}

/* Argumente:
 * repid = id im mapping reputations
 * msg = Neuer String in der Default-Changemsg
 *
 * Returns:
 *  1 = Neue Msg erfolgreich gesetzt
 * -1 = Keine Zugriffsrechte
 * -2 = Falsche Argumente
 * -3 = Rep nicht vorhanden
 * -4 = Msg entspricht der derzeitigen
 */
public int ChangeReputationMsg(string repid, string msg) {
  if(!Sec())
    return -1;
  if(!stringp(repid) || !sizeof(repid) || !stringp(msg) || !sizeof(msg))
    return -2;
  if(!member(reputations, repid))
    return -3;
  if(reputations[repid]["changemsg"] == msg) 
    return -4;
  reputations[repid]["changemsg"] = msg;
  save_object(REP_SAVEFILE);
  return 1;
}

/* Argumente:
 * repid = id(s) im mapping reputations, ohne Argument werden alle aufgelistet,
 *         String oder String-array
 *
 * Returns:
 *  1 = Liste ausgegeben
 * -1 = Falsche Argumente
 * -2 = Rep nicht vorhanden
 */
public varargs int ViewReputationData(mixed repids) {
  // Fuer alle sichtbar, damit man ggf. weiss, wo man nachfragen muss
  if(!repids) repids = m_indices(reputations);
  else if(stringp(repids)) repids = ({ repids });
  if(!pointerp(repids) || !sizeof(repids))
    return -1;
  // Textausgabe fuer alle richtigen IDs
  foreach(string repid : repids) {
    if(!member(reputations, repid)) continue;
    tell_object(this_interactive(), sprintf("%s %s\n  Name: %s\n  Changemsg: "
      "%s\n  ValidUIDS: %O\n\n", repid, 
      (reputations[repid]["flags"] & REP_FLAG_ACTIVE ? "" :
      "(Inaktiv)"), reputations[repid]["name"], 
      reputations[repid]["changemsg"], reputations[repid]["uids"]));
  }
  // Min. 1 nicht vorhanden: return -2, sonst 1
  return (sizeof(repids - m_indices(reputations)) ? -2 : 1);
}

/* Argumente:
 * repid = id im mapping reputations
 *
 * Returns:
 * mapping = Daten der Reputation
 * 0 = Falsches Argument / Reputation nicht vorhanden
 */
public mapping GetReputationData(string repid) {
  if(!stringp(repid) || !sizeof(repid) || !member(reputations, repid))
    return 0;
  return deep_copy(reputations[repid]);
}

/* Argumente:
 * repid = id im mapping reputations
 * value = Wert um den die Rep veraendert wird (+ / -)
 *
 * Returns:
 * string = "Dein Ansehen %s hat sich ... .\n"
 * 0 = Falsche Argumente / Rep nicht vorhanden
 */
public string GetDefaultChangeMsg(string repid, int value) {
  string strength;
  if(!stringp(repid) || !sizeof(repid) || !intp(value) || !value ||
     !member(reputations, repid))
    return 0;
  switch(abs(value)) {
    case    0..25  : strength = "kaum merklich"; break;
    case   26..50  : strength = "leicht"; break;
    case   51..100 : strength = "deutlich"; break;
    case  101..250 : strength = "erheblich"; break;
    case  251..1000: strength = "sehr stark"; break;
    default:         strength = "ausserordentlich stark"; break;
  }
  return sprintf("Dein Ansehen %s hat sich%s%s.\n", 
    reputations[repid]["changemsg"], (!strength ? " " : " "+ strength +" "),
    (value < 0 ? "verschlechtert" : "verbessert"));
}

/* Argumente:
 * repid = id im mapping reputations
 * ob = Objekt, dessen UID geprueft wird, ohne Argument -> prev_ob()
 *
 * Returns:
 * 1 = UID valid
 * 0 = UID invalid
 * -1 = Falsche Argumente 
 * -2 = Rep nicht vorhanden 
 */
public int CheckValidUid(string repid, object ob) {
  if(!stringp(repid) || !sizeof(repid) || !objectp(ob))
    return -1;
  if(!member(reputations, repid))
    return -2;
  return (member(reputations[repid]["uids"], getuid(ob)) != -1);
}

/* Argumente:
 * repid = id im mapping reputations
 *
 * Returns:
 * 1 = Rep aktiv
 * 0 = Rep inaktiv
 * -1 = Falsche Argumente 
 * -2 = Rep nicht vorhanden 
 */
public int CheckRepActive(string repid) {
  if(!stringp(repid) || !sizeof(repid))
    return -1;
  if(!member(reputations, repid))
    return -2;
  // Verboolung
  return !!(reputations[repid]["flags"] & REP_FLAG_ACTIVE);
}

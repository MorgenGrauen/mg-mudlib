/* /std/channel_supervisor.c
//
// Der Standard-Supervisor fuer Ebenen. Sollte von allen Objekten geerbt
// werden, die Zugriffsrechte fuer Ebenen verwalten ("das Sagen" auf der Ebene
// haben).
// Das Objekt braucht ggf. eine eUID zum Dateizugriff. Zum Setzen dieser ist
// der Erbende verantwortlich.
// Standardmaessig wird /p/daemon/channeld.init[.testmud] eingelesen.
// Erbende koennen ihre Daten auch voellig anderweitig einlesen/ermitteln und
// Rechte voellig anders pruefen statt gegen ein Level zu pruefen.
// Das einzig wichtige fuer die Rechtepruefung ist die Funktion
// ch_check_access(), die vom CHANNELD gerufen wird. Sie muss 1 zurueckgeben,
// falls die Aktion erlaubt wird und 0 anderenfalls.
//
// Benutzung fuer Standardebenen:
//    Dieses Objekt erben, ggf. eine eUID setzen und ch_read_init_file()
//    rufen.
//    Zusaetzlich sollte dieses Objekt aber auch ein Namen haben, weil es
//    damit auf den Ebenen angezeigt wird. (D.h. minimal name() und Name()
//    sollten implementiert/geerbt sein.)
*/

#pragma strict_types, rtt_checks
#pragma no_shadow, no_clone

#include <wizlevels.h>
#include <regexp.h>
#include "/p/daemon/channel.h"
#include <living/description.h>

/* Speichert Sende- und Empfangslevel sowie Flags zu den einzelnen Channeln.
   mapping ch_access_data = ([ string channel_name : (<ch_access>) ]) */
struct ch_access {
    int recv;
    int send;
    int flags;
};
// nicht nosave hier, damit man es zwar nosave erben kann, aber ggf. auch
// speichern kann
protected mapping ch_access_data = m_allocate(0, 1);

// oeffentlicher Name des Supervisors
protected string ch_sv_name = "Generischer SV";

protected void ch_set_sv_name(string newname)
{
  ch_sv_name = newname;
}

public varargs string name(int casus,int demon)
{
  return ch_sv_name;
}

public varargs string Name(int casus, int demon)
{
  return capitalize(name( casus, demon )||"");
}

// ch_check_access() prueft die Zugriffsberechtigungen auf Ebenen.
//
// Wird vom CHANNELD gerufen.
// Gibt 1 zurueck, wenn Aktion erlaubt, 0 sonst.
//
// Verlassen (C_LEAVE) ist immer erlaubt. Die anderen Aktionen sind in zwei
// Gruppen eingeteilt:
// 1) RECV. Die Aktionen dieser Gruppe sind Suchen (C_FIND), Auflisten
//          (C_LIST) und Betreten (C_JOIN).
// 2) SEND. Die Aktion dieser Gruppe ist zur Zeit nur Senden (C_SEND).
//
// Aktionen werden zugelassen, wenn Spieler/MagierLevel groesser ist als die
// fuer die jeweilige Aktionsgruppe RECV oder SEND festgelegte Stufe.
// Handelt es sich um eine Magierebene (<accessflags> enthaelt das Flag
// CH_ACCESS_WIZARD), muss die Magierstufe des Spielers groesser sein als die
// Mindeststufe der Ebene. Ansonsten wird gegen den Spielerlevel geprueft.
// Enthaelt <accessflags> das Flag CH_ACCESS_NOGUEST, darf die Ebene nicht von
// Gaesten benutzt werden.
//
// Wenn RECV_LVL oder SEND_LVL auf -1 gesetzt ist, sind die Aktionen der
// jeweiligen Gruppen komplett geblockt.

public int ch_check_access(string ch, object user, string cmd)
{
  struct ch_access access = ch_access_data[ch];
  // <user> ist Gast, es sind aber keine Gaeste zugelassen? Koennen wir
  // direkt ablehnen.
  if ((access.flags & CH_ACCESS_NOGUEST) && ({int})user->QueryGuest())
    return 0;

  // Ebenso auf Magier- oder Seherebenen, wenn ein Spieler anfragt, der
  // noch kein Seher ist.
  if ((access.flags & CH_ACCESS_WIZARD) && query_wiz_level(user) < SEER_LVL)
    return 0;

  // Ebene ist Magierebene? Dann werden alle Stufenlimits gegen Magierlevel
  // geprueft, ansonsten gegen Spielerlevel.
  int level = (access.flags & CH_ACCESS_WIZARD
                  ? query_wiz_level(user)
                  : ({int})user->QueryProp(P_LEVEL));

  switch (cmd)
  {
    case C_FIND:
    case C_LIST:
    case C_JOIN:
      if (access.recv == -1)
        return 0;
      if (access.recv <= level)
        return 1;
      break;

    case C_SEND:
      if (access.send == -1)
        return 0;
      if (access.send <= level)
        return 1;
      break;

    // Verlassen ist immer erlaubt
    case C_LEAVE:
      return 1;
  }
  return 0;
}

// ch_store_access() - Angaben zu Zugriffsrechten fuer eine Ebene merken
//            Angaben kommen in folgender Reihenfolge:
//            string* chinfo = ({ channel_name, receive_level, send_level,
//                                adminflags, channelflags, description,
//                                supervisor })
//            mapping &data ist im Regelfall das <ch_access_data>, was per
//              Referenz uebergeben wird (auch wenn das nicht noetig ist,
//              macht es klar, was beabsichtigt ist).
protected void ch_store_access(string* chinfo, mapping data)
{
  object supervisor;

  // Nur die Angabe des SV (Index 6) im initfile ist optional, alle Elemente
  // davor muessen da sein. Wenn kein Supervisor angegeben ist, wird der
  // Default-SV angenommen.

  if (sizeof(chinfo) >= 7)
  {
    if (stringp(chinfo[6]) && sizeof(chinfo[6]))
      supervisor = find_object(chinfo[6]);
  }
  else if (sizeof(chinfo) == 6)
    supervisor = find_object(DEFAULTSV);
  else
    return;

  // Nur Daten merken, wenn wir auch der Supervisor fuer die Ebene sein
  // sollen.
  if (supervisor != this_object())
    return;

  struct ch_access access = (<ch_access>
                               recv: to_int(chinfo[1]),
                               send: to_int(chinfo[2]),
                               flags: to_int(chinfo[3])
                             );

  data[lower_case(chinfo[0])] = access;
  // Rest der Ebenendaten interessiert uns nicht.
}

// Liest ein channeld.init file ein. Wenn keines angegeben wird, wird der
// das Standardfile fuers Mud eingelesen. Die Angaben werden in das Mapping
// <ch_access_data> uebernommen, dieses vorher aber nicht geloescht.
// Der Rueckgabewert ist < 0 fuer Fehler, ansonsten die Groesse von
// <ch_access_data> nach Einlesen des Files.
protected varargs int ch_read_init_file(string fname)
{
  string ch_list;
  if (!fname)
  {
#if !defined(__TESTMUD__) && MUDNAME=="MorgenGrauen"
    fname = CHMASTER ".init";
#else
    fname = CHMASTER ".init.testmud";
#endif
  }

  ch_list = read_file(fname);
  if (!stringp(ch_list))
    return -1;

  // Channeldatensaetze erzeugen, dazu zuerst Datenfile in Zeilen zerlegen
  // "Allgemein:   0: 0: 0: 0: Allgemeine Unterhaltungsebene"
  // Danach drueberlaufen und in Einzelfelder splitten, dabei gleich die
  // Trennzeichen (Doppelpunkt, Tab und Space) rausfiltern.
  foreach(string ch : old_explode(ch_list, "\n"))
  {
    if (ch[0]=='#')
      continue;
    ch_store_access(regexplode(ch, ":[ \t]*", RE_OMIT_DELIM),
                    &ch_access_data);
  }
  return sizeof(ch_access_data);
}

// Nur falls es mal gebraucht wird und  damit es schon jetzt alle rufen
// koennen.
protected void create()
{
}

public varargs int remove(int silent)
{
  destruct(this_object());
  return 1;
}


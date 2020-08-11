// /std/channel_supervisor.c
//
// Der Standard-Supervisor fuer Ebenen. Wird genutzt vom channeld fuer die
// Ebenen, die der verwaltet.

#pragma strict_types,save_types, rtt_checks
#pragma no_shadow, no_clone

#include <wizlevels.h>
#include "/p/daemon/channel.h"
#include <living/description.h>

// Indizes fuer Zugriffe auf das Mapping <admin>.
#define RECV    0
#define SEND    1
#define FLAG    2

// Ebenenflags, gespeichert in admin[ch, FLAG]
// F_WIZARD kennzeichnet reine Magierebenen
#define F_WIZARD 1
// Ebenen, auf denen keine Gaeste erlaubt sind, sind mit F_NOGUEST markiert.
#define F_NOGUEST 2

/* Speichert Sende- und Empfangslevel sowie Flags zu den einzelnen Channeln.
 * Diese werden aber nur ausgewertet, wenn der Channeld die Rechtepruefung
 * fuer die jeweilige Ebene macht, d.h. in der Praxis bei Ebenen, bei denen
 * der CHANNELD der Supervisor ist.
 * Deswegen sind diese Daten auch nicht in der struct (<channel_s>) drin.
 * Wird beim Laden des Masters via create() -> initalize() -> setup() mit den
 * Daten aus dem Init-File ./channeld.init befuellt.
   mapping admin = ([ string channel_name : int RECV_LVL,
                                            int SEND_LVL,
                                            int FLAG ]) */
private mapping admin = m_allocate(0, 3);

// check_ch_access() prueft die Zugriffsberechtigungen auf Ebenen.
//
// Gibt 1 zurueck, wenn Aktion erlaubt, 0 sonst.
// Wird von access() gerufen; access() gibt das Ergebnis von
// check_ch_access() zurueck.
//
// Verlassen (C_LEAVE) ist immer erlaubt. Die anderen Aktionen sind in zwei
// Gruppen eingeteilt:
// 1) RECV. Die Aktionen dieser Gruppe sind Suchen (C_FIND), Auflisten
//          (C_LIST) und Betreten (C_JOIN).
// 2) SEND. Die Aktion dieser Gruppe ist zur Zeit nur Senden (C_SEND).
//
// Aktionen werden zugelassen, wenn Spieler/MagierLevel groesser ist als die
// fuer die jeweilige Aktionsgruppe RECV oder SEND festgelegte Stufe.
// Handelt es sich um eine Magierebene (F_WIZARD), muss die Magierstufe
// des Spielers groesser sein als die Mindeststufe der Ebene. Ansonsten
// wird gegen den Spielerlevel geprueft.
//
// Wenn RECV_LVL oder SEND_LVL auf -1 gesetzt ist, sind die Aktionen der
// jeweiligen Gruppen komplett geblockt.

public int check_ch_access(string ch, object pl, string cmd)
{
  // <pl> ist Gast, es sind aber keine Gaeste zugelassen? Koennen wir
  // direkt ablehnen.
  if ((admin[ch, FLAG] & F_NOGUEST) && ({int})pl->QueryGuest())
    return 0;

  // Ebenso auf Magier- oder Seherebenen, wenn ein Spieler anfragt, der
  // noch kein Seher ist.
  if ((admin[ch, FLAG] & F_WIZARD) && query_wiz_level(pl) < SEER_LVL)
    return 0;

  // Ebene ist Magierebene? Dann werden alle Stufenlimits gegen Magierlevel
  // geprueft, ansonsten gegen Spielerlevel.
  int level = (admin[ch, FLAG] & F_WIZARD
                  ? query_wiz_level(pl)
                  : ({int})pl->QueryProp(P_LEVEL));

  switch (cmd)
  {
    case C_FIND:
    case C_LIST:
    case C_JOIN:
      if (admin[ch, RECV] == -1)
        return 0;
      if (admin[ch, RECV] <= level)
        return 1;
      break;

    case C_SEND:
      if (admin[ch, SEND] == -1)
        return 0;
      if (admin[ch, SEND] <= level)
        return 1;
      break;

    // Verlassen ist immer erlaubt
    case C_LEAVE:
      return 1;

    default:
      break;
  }
  return 0;
}

// ggf. zum ueberschreiben
protected int ch_caller_allowed(object caller)
{
  if (caller == find_object(CHMASTER))
    return 1;
  return 0;
}

// stores the read and write levels and flags for a channel for
// usage in check_ch_access(). By default it is called by channeld::setup()
// during setup of a default channel in the supervisor configured for the
// channel.
// recv and send are minimum levels, flag is a combination of the F_* above.
visible void ch_supervisor_setup(string chname, int recv, int send, int flag)
{
  if (!extern_call() || ch_caller_allowed(previous_object()))
  {
    admin[chname, FLAG] = flag;
    admin[chname, SEND] = send;
    admin[chname, RECV] = recv;
  }
}


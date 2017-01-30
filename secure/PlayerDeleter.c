// MorgenGrauen MUDlib
//
// master.c -- master object
//
// $Id: master.c 7336 2009-11-19 20:37:31Z Zesstra $
#pragma strong_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
#pragma pedantic
#pragma range_check
#pragma warn_deprecated

#include <files.h>
#include <wizlevels.h>
#include <events.h>
#include <config.h>
#include <properties.h>

protected void create() {
  seteuid(getuid(this_object()));
}

public varargs int Delete(string plname, int nobanish, int noclone) {
  // definitiv nur fuer EM+ und nur Objekt mit Level 66+ in der Callchain.
  if (!ARCH_SECURITY || process_call())
    return -1;

  // existiert der Spieler?
  if (!stringp(plname)
      || !master()->find_userinfo(plname))
    return -2;
  // gibt es das Spielerobjekt?
  object plob = find_player(plname) || find_netdead(plname);
  // wenn kein Objekt: Dummyobjekt erzeugen
  if (!objectp(plob) && !noclone) {
    catch(plob = __create_player_dummy(plname));
  }

  int wlevel = query_wiz_level(plname);
  string part_filename="/"+plname[0..0]+"/"+plname+".o";

  // ggf. banishen
  if (!nobanish && wlevel >= LEARNER_LVL)
    master()->BanishName(plname, "So hiess mal ein Magier hier", 1);
  else if (!nobanish && wlevel >= SEER_LVL)
    master()->BanishName(plname, "So hiess mal ein Seher hier", 1);

  // Spielpausen aufheben
  master()->TBanishName(plname, 0);

  // loggen
  log_file("ARCH/ADMIN_USERDELETE",
      sprintf("%s: %s geloescht durch %s (%s)\n",
             strftime("%y%m%d-%H%M%S"),
             plname,
             secure_euid(),
             to_string(query_ip_number(this_interactive())) ));


  // Loesch-Event ausloesen
  if (plob) {
    EVENTD->TriggerEvent(EVT_LIB_PLAYER_DELETION, ([
            E_PLNAME: plname,
            E_ENVIRONMENT: environment(plob),
            E_GUILDNAME: plob->QueryProp(P_GUILD) ]) );

    // Spielerobjekt zerstoeren
    plob->move("/room/void", M_NOCHECK);
    plob->remove(1);
  }

  // Files loeschen
  if (file_size(LIBDATADIR"/"SECUREDIR"/save"+part_filename) > FSIZE_NOFILE)
    rm("/"SECUREDIR"/save"+part_filename);
  if (file_size("/"LIBSAVEDIR + part_filename) > FSIZE_NOFILE)
    rm("/"LIBSAVEDIR + part_filename);
  if (file_size("/"MAILDIR + part_filename) > FSIZE_NOFILE)
    rm("/"MAILDIR + part_filename);

  master()->RemoveFromCache(plname);

  return 1;
}


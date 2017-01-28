// MorgenGrauen MUDlib
/** \file /file.c
* Kurzbeschreibung.
* Langbeschreibung...
* \author <Autor>
* \date <date>
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types,save_types,rtt_checks
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic, range_check

#include <defines.h>
#include <events.h>
#include <wizlevels.h>
#include <lepmaster.h>
#include <properties.h>
#include <userinfo.h>

#define HOME(x) (__PATH__(0)+x)
#include <living/comm.h>
#define ZDEBUG(x) if (find_player("zesstra")) \
  find_player("zesstra")->ReceiveMsg(x,MT_DEBUG,0,object_name()+":",this_object())
//#define ZDEBUG(x)

object *players = ({});

protected void create()
{
  seteuid(getuid());
  if (sl_open(HOME("ARCH/topliste.sqlite")) != 1)
  {
    raise_error("Datenbank konnte nicht geoeffnet werden.\n");
  }
  // Tabellen und Indices anlegen, falls die nicht existieren.
  sl_exec("CREATE TABLE IF NOT EXISTS topliste(name TEXT PRIMARY KEY ASC, "
          "gilde TEXT NOT NULL, rasse TEXT NOT NULL, "
          "age DATETIME, wizlevel INTEGER, "
          "lastupdate DATETIME DEFAULT current_timestamp, "
          "lep INTEGER, qp INTEGER, xp INTEGER, level INTEGER"
          "hardcore INTEGER);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_gilde ON topliste(gilde);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_rasse ON topliste(rasse);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_hardcore ON topliste(hardcore);");
  
  // Login-Event abonnieren
  if (EVENTD->RegisterEvent(EVT_LIB_LOGIN,
                            "listen", this_object()) <= 0)
  {
    raise_error("Loginevent konnte nicht abonniert werden.\n");
  }
  if (EVENTD->RegisterEvent(EVT_LIB_ADVANCE,
                            "listen", this_object()) <= 0)
  {
    raise_error("EVT_LIB_ADVANCE konnte nicht abonniert werden.\n");
  }
  if (EVENTD->RegisterEvent(EVT_LIB_QUEST_SOLVED,
                            "listen", this_object()) <= 0)
  {
    raise_error("EVT_LIB_QUEST_SOLVED konnte nicht abonniert werden.\n");
  }
  if (EVENTD->RegisterEvent(EVT_LIB_MINIQUEST_SOLVED,
                            "listen", this_object()) <= 0)
  {
    raise_error("EVT_LIB_MINIQUEST_SOLVED konnte nicht abonniert werden.\n");
  }
}

private void process()
{
  players -= ({0});
  foreach(object pl : &players)
  {
    if (get_eval_cost() < 200000)
    {
      call_out(#'process, 2);
      return;
    }
    sl_exec("INSERT OR REPLACE INTO topliste(name, gilde, rasse, "
                "age, wizlevel, lastupdate, lep, qp, xp, level, hardcore) "
                "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11);",
                pl->query_real_name(),
                pl->QueryProp(P_GUILD) || "unbekannt",
                pl->QueryProp(P_RACE),
                pl->QueryProp(P_AGE),
                query_wiz_level(pl),
                time(),
                LEPMASTER->QueryLEPForPlayer(pl),
                pl->QueryProp(P_QP),
                pl->QueryProp(P_XP),
                pl->QueryProp(P_LEVEL),
                pl->query_hc_play()
               );
    pl=0;
  }
  players = ({});
}

public void listen(string eid, object trigob, mixed data)
{
  if (previous_object() != find_object(EVENTD)
      || !trigob
      || !query_once_interactive(trigob)
      || IS_LEARNER(trigob)
      || (mixed)trigob->QueryProp(P_TESTPLAYER)
      || trigob->QueryGuest()
      || trigob->QueryProp(P_NO_TOPLIST)
     )
    return;

  // Dieser handler hat nur 30k Ticks zur Verfuegung, dummerweise kann
  // QueryLEPForPlayer() locker ueber 30k kosten, wenn der Spieler nicht in
  // Caches drin ist. Daher muss das jetzt per call_out entkoppelt werden.
  // *seufz*
  if (member(players, trigob) == -1)
  {
    players += ({trigob});
    if (find_call_out(#'process) == -1)
      call_out(#'process, 2);
  }
}

public varargs < <string|int>* >* Liste(string rasse, string gilde,
                                     int limit, string sort)
{
  // Defaults:
  sort ||= "lep";
  if (!limit || limit > 100)
    limit=100;
  else if (limit < 1)
    limit=1;
  if (rasse && gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND gilde=?2 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse, gilde);
  else if (rasse)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse);
  else if (gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE gilde=?1 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        gilde);
  return sl_exec(
      "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
      "ORDER BY "+sort+" DESC LIMIT "+limit+";");
}

public varargs < <string|int>* >* SpielerListe(string rasse, string gilde,
                                     int limit, string sort)
{
  // Defaults:
  sort ||= "lep";
  if (!limit || limit > 100)
    limit=100;
  else if (limit < 1)
    limit=1;
  if (rasse && gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND gilde=?2 AND wizlevel=0 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse, gilde);
  else if (rasse)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND wizlevel=0 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse);
  else if (gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE gilde=?1 AND wizlevel=0 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        gilde);
  return sl_exec(
      "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
      "WHERE wizlevel=0 "
      "ORDER BY "+sort+" DESC LIMIT "+limit+";");
}

public varargs < <string|int>* >* SeherListe(string rasse, string gilde,
                                     int limit, string sort)
{
  // Defaults:
  sort ||= "lep";
  if (!limit || limit > 100)
    limit=100;
  else if (limit < 1)
    limit=1;
  if (rasse && gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND gilde=?2 AND wizlevel=1 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse, gilde);
  else if (rasse)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND wizlevel=1 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse);
  else if (gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE gilde=?1 AND wizlevel=1 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        gilde);
  return sl_exec(
      "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
      "WHERE wizlevel=1 "
      "ORDER BY "+sort+" DESC LIMIT "+limit+";");
}

public varargs < <string|int>* >* HardcoreListe(string rasse, string gilde,
                                     int limit, string sort)
{
  // Defaults:
  sort ||= "lep";
  if (!limit || limit > 100)
    limit=100;
  else if (limit < 1)
    limit=1;
  if (rasse && gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND gilde=?2 AND hardcore>0 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse, gilde);
  else if (rasse)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE rasse=?1 AND hardcore>0 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        rasse);
  else if (gilde)
    return sl_exec(
        "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
        "WHERE gilde=?1 AND hardcore>0 "
        "ORDER BY "+sort+" DESC LIMIT "+limit+";",
        gilde);
  return sl_exec(
      "select name,lep,qp,xp,level,age,rasse,gilde,wizlevel,hardcore from topliste "
      "WHERE hardcore>0 "
      "ORDER BY "+sort+" DESC LIMIT "+limit+";");
}

varargs int remove(int silent)
{
  EVENTD->UnregisterEvent(EVT_LIB_LOGIN, this_object());
  sl_close();
  destruct(ME);
  return 1;
}

public mixed sql_query(string query)
{
  if (ARCH_SECURITY)
    return sl_exec(query);
  return 0;
}

void reset()
{
  // Alle Eintraege loeschen, die seit 90 nicht mehr aktualisiert wurden.
//  sl_exec("DELETE FROM topliste WHERE lastupdate<?1;",
//                time()-90*24*3600);
  sl_exec("DELETE FROM topliste WHERE name IN (SELECT name FROM topliste "
          "ORDER BY lep DESC LIMIT 1000, -1);");
  set_next_reset(86400);
}


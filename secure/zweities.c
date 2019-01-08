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
#include <player/base.h>
#include <userinfo.h>
#include <config.h>

#define DBPATH  "/"LIBDATADIR"/"SECUREDIR"/ARCH/second.sqlite"

#define ZDEBUG(x) tell_room("/players/zesstra/workroom",\
                    sprintf("second: %O\n",x));

protected void create()
{
  seteuid(getuid());
  if (sl_open(DBPATH) != 1)
  {
    raise_error("Datenbank konnte nicht geoeffnet werden.\n");
  }
  // Tabellen und Indices anlegen, falls die nicht existieren.
  sl_exec("CREATE TABLE IF NOT EXISTS zweities(uuid TEXT PRIMARY KEY ASC, "
          "name TEXT NOT NULL, erstieuuid TEXT NOT NULL, "
          "erstie TEXT NOT NULL);");
  sl_exec("CREATE TABLE IF NOT EXISTS testies(name TEXT PRIMARY KEY ASC, "
          "magier TEXT NOT NULL, "
          "lastlogin DATETIME DEFAULT current_timestamp);");
  sl_exec("CREATE TABLE IF NOT EXISTS familien("
          "erstieuuid TEXT PRIMARY KEY ASC, familie TEXT NOT NULL);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_erstie ON zweities(erstie);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_name ON zweities(name);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_magiername ON testies(magier);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_familie ON familien(familie);");

  // Login-Event abonnieren
  if (EVENTD->RegisterEvent(EVT_LIB_LOGIN,
                            "listen", this_object()) <= 0)
  {
    raise_error("Loginevent konnte nicht abonniert werden.\n");
  }
}

public void listen(string eid, object trigob, mixed data)
{
  if (previous_object() != find_object(EVENTD)
      || !trigob
      || !query_once_interactive(trigob)
      || IS_LEARNER(trigob))
    return;
  // wenn testie, wird der Char als Testie behandelt und P_SECOND ignoriert.
  mixed testie=trigob->QueryProp(P_TESTPLAYER);
  if (stringp(testie)
      && strstr(testie,"Gilde")==-1)
  {
    testie=lower_case(testie);
    mixed plinfo = master()->get_userinfo(testie);
    if (pointerp(plinfo))
    {
      sl_exec("INSERT OR REPLACE INTO testies(name, magier, lastlogin) "
              "VALUES(?1,?2,?3);",
              trigob->query_real_name(),
              testie, time());
      return; // zweitie jetzt auf jeden Fall ignorieren.
    }
  }

  mixed erstie=trigob->QueryProp(P_SECOND);
  if (stringp(erstie))
  {
    erstie=lower_case(erstie);
    mixed plinfo = master()->get_userinfo(erstie);
    if (pointerp(plinfo))
    {
      sl_exec("INSERT OR REPLACE INTO zweities(uuid, name, erstieuuid, erstie) "
              "VALUES(?1,?2,?3,?4);",
              getuuid(trigob),
              trigob->query_real_name(),
              erstie + "_" + plinfo[USER_CREATION_DATE+1],
              erstie);
    }
    //ZDEBUG(sprintf("%O, %O, %O\n",eid,trigob,data));
  }
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

private string get_erstie_data(string datum, object zweitie)
{
  if (!zweitie) return 0;
  mixed res = sl_exec("SELECT " + datum + " FROM zweities WHERE uuid=?1",
                      getuuid(zweitie));
  if (sizeof(res))
    return res[0][0];

  return 0;
}

public varargs string QueryErstieName(object zweitie)
{
  return get_erstie_data("erstie", zweitie || previous_object());
}

public varargs string QueryErstieUUID(object zweitie)
{
  return get_erstie_data("erstieuuid", zweitie || previous_object());
}

private string* get_zweitie_data(string datum, object erstie)
{
  if (!erstie) return 0;
  mixed tmp = sl_exec("SELECT " + datum + " FROM zweities WHERE erstieuuid=?1",
                      getuuid(erstie));
  if (sizeof(tmp))
  {
    string *res=({});
    foreach(string *row: tmp)
      res+=({row[0]});
    return res;
  }
  return 0;
}

public varargs string* QueryZweities(object erstie)
{
  return get_zweitie_data("name", erstie || previous_object());
}

public varargs string QueryFamilie(object pl)
{
  string erstie = get_erstie_data("erstieuuid",
                                  pl || previous_object());
  // Wenn !erstie, dann ist pl kein Zweitie, also ist er selber erstie
  erstie ||= getuuid(pl);
  // jetzt noch gucken, ob ne explizite Familie fuer den erstie erfasst ist.
  mixed tmp = sl_exec("SELECT familie FROM familien WHERE "
                      "erstieuuid=?1",erstie);
  if (sizeof(tmp))
    return tmp[0][0];
  // wenn nicht, dann ist die Familie die Zweitieuuid 
  return erstie;
}

public string SetFamilie(object|string pl, string familie)
{
  if (!ARCH_SECURITY)
    return 0;

  // Wenn Spielerobjekt, UUID ermitteln
  if (objectp(pl) && query_once_interactive(pl))
    pl = getuuid(pl);

  sl_exec("INSERT OR REPLACE INTO familien(erstieuuid, familie) "
          "VALUES(?1, ?2);", pl, familie);

  mixed tmp = sl_exec("SELECT familie FROM familien WHERE "
                      "erstieuuid=?1", pl);
  if (sizeof(tmp))
    return tmp[0][0];

  return 0;
}

public int DeleteFamilie(object|string pl)
{
  if (!ARCH_SECURITY)
    return 0;

  // Wenn Spielerobjekt, UUID ermitteln
  if (objectp(pl) && query_once_interactive(pl))
    pl = getuuid(pl);

  sl_exec("DELETE FROM familien WHERE erstieuuid=?1;",
          pl);

  mixed tmp = sl_exec("SELECT familie FROM familien WHERE "
                      "erstieuuid=?1", pl);
  if (sizeof(tmp))
    return -1;

  return 1;
}

public int DeleteZweitie(object|string pl)
{
  if (!ARCH_SECURITY)
    return 0;

  // Wenn Spielerobjekt, UUID ermitteln
  if (objectp(pl) && query_once_interactive(pl))
    pl = getuuid(pl);

  sl_exec("DELETE FROM zweities WHERE uuid=?1;", pl);

  mixed tmp = sl_exec("SELECT name FROM zweities WHERE "
                      "uuid=?1", pl);
  if (sizeof(tmp))
    return -1;

  return 1;
}

public int DeleteTestie(object|string pl)
{
  if (!ARCH_SECURITY)
    return 0;

  // Wenn Spielerobjekt, UID ermitteln
  if (objectp(pl) && query_once_interactive(pl))
    pl = getuid(pl);

  sl_exec("DELETE FROM testies WHERE name=?1;", pl);

  mixed tmp = sl_exec("SELECT magier FROM testies WHERE "
                      "name=?1", pl);
  if (sizeof(tmp))
    return -1;

  return 1;
}


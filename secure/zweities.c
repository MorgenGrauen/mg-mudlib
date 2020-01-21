// MorgenGrauen MUDlib
/** \file /file.c
* Kurzbeschreibung.
* Langbeschreibung...
* \author <Autor>
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
  sl_exec("PRAGMA foreign_keys = ON; PRAGMA temp_store = 2; ");
  // Tabellen und Indices anlegen, falls die nicht existieren.
  sl_exec("CREATE TABLE IF NOT EXISTS zweities(uuid TEXT PRIMARY KEY ASC, "
          "name TEXT NOT NULL, erstieuuid TEXT NOT NULL, "
          "erstie TEXT NOT NULL);");
  sl_exec("CREATE TABLE IF NOT EXISTS testies(name TEXT PRIMARY KEY ASC, "
          "magier TEXT NOT NULL, "
          "lastlogin DATETIME DEFAULT current_timestamp);");
  sl_exec("CREATE TABLE IF NOT EXISTS familien("
          "erstieuuid TEXT PRIMARY KEY ASC, familie TEXT NOT NULL);");
  sl_exec("CREATE TABLE IF NOT EXISTS aliases("
          "familie TEXT NOT NULL, "
          "verb TEXT NOT NULL, alias TEXT NOT NULL,"
          "UNIQUE(familie,verb));");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_erstie ON zweities(erstie);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_name ON zweities(name);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_magiername ON testies(magier);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_familie ON familien(familie);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_aliasverb ON aliases(verb);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_aliasfamilie ON aliases(familie);");

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
    if (master()->find_userinfo(testie))
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
    if (master()->find_userinfo(erstie))
    {
      sl_exec("INSERT OR REPLACE INTO zweities(uuid, name, erstieuuid, erstie) "
              "VALUES(?1,?2,?3,?4);",
              getuuid(trigob),
              trigob->query_real_name(),
              erstie + "_"
                     + master()->query_userlist(erstie,USER_CREATION_DATE),
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
  if (!pl || !query_once_interactive(pl))
    return 0;
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

// *************** Aliase *******************************

// Nur die eigene Familie darf abgefragt/geaendert werden.
private int alias_access(string familie)
{
  if (ARCH_SECURITY)
    return 1;
  if (QueryFamilie(previous_object()) == familie)
    return 1;
  return 0;
}

// Familie ermitteln und ggf. Zugriffsrecht pruefen.
private string get_family(string familie)
{
  // Wenn Familie angegeben, darf das nur jemand abfragen, der dieser Familie
  // angehoert (oder EM+).
  if (familie)
  {
    if (!alias_access(familie))
      return 0;
    return familie;
  }
  else
    return QueryFamilie(previous_object());
  return 0;
}

// Die Familienaliase abfragen. Entweder ein bestimmtes oder alle. Eine
// Filterung ist hier (vorlaeufig) nicht eingebaut, das macht ggf. das
// Spielerobjekt.
public varargs mapping QueryFamilyAlias(string verb, string familie)
{
  // Familie ermitteln und Zugriffsrecht pruefen.
  familie = get_family(familie);
  if (!familie || !sizeof(familie))
    return 0;

  mixed tmp;
  if (verb)
    tmp = sl_exec("SELECT verb, alias FROM aliases WHERE "
                  "familie=?1 AND verb=?2", familie, verb);
  else
    tmp = sl_exec("SELECT verb,alias FROM aliases WHERE "
                  "familie=?1",familie);

  if (sizeof(tmp))
  {
    // Das Alias muss noch in das Aliasarray gesplittet werden.
    mapping res = m_allocate(sizeof(tmp));
    foreach(string* row : tmp)
    {
      m_add(res, row[0], restore_value(row[1]));
    }
    return res;
  }
  return ([]);
}

public varargs int AddOrReplaceFamilyAlias(string verb, <string|int>* alias,
                                           string familie)
{
  // Familie ermitteln und Zugriffsrecht pruefen.
  familie = get_family(familie);
  if (!familie || !sizeof(familie))
    return -1;

  // max. 100 Familienaliase fuer den Moment.
  mixed* tmp = sl_exec("SELECT COUNT(*) FROM aliases WHERE "
                       "familie=?1",familie);
  if (tmp[0][0] > 100)
    return -2;

  sl_exec("INSERT OR REPLACE INTO aliases(familie, verb, alias) "
          "VALUES(?1, ?2, ?3);", familie, verb, save_value(alias));
  return 1;
}

public varargs int DeleteFamilyAlias(string verb, string familie)
{
  // Familie ermitteln und Zugriffsrecht pruefen.
  familie = get_family(familie);
  if (!familie || !sizeof(familie))
    return -1;

  if (verb)
    sl_exec("DELETE FROM aliases WHERE familie=?1 AND verb=?2;",
            familie, verb);
  else // alle loeschen
    sl_exec("DELETE FROM aliases WHERE familie=?1;", familie);
  return 1;
}


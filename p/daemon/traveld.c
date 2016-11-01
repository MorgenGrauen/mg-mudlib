/** Doku:
public int AddRoute(string transporter, string* l_stops)
  Beschreibung:
    Setzt die aktuelle Route eines Transporters.
    Die Route hier ist allerdings nur die Liste von Haltepunkten - nicht der
    Rest, der in den Routen in den Transporter abgelegt ist.
    Eine neue Route ersetzt immer die bisherige Route
    Wird von /std/transport gerufen, wenn P_NO_TRAVELING nicht gesetzt ist.
  Rueckgabewerte:
    - 1: Transporter und Haltepunkt Eingetragen.
    - 0: Transporter oder Haltepunkt kein string|string*.
    - -1: Es wurde versucht einen Transporter in /p/ oder /players/
          einzutragen 
          und der Eintragende ist kein EM.

public int RemoveTransporter(string|object transporter)
public int RemoveStop(string|object stop)
  Beschreibung:
    Entfernt einen kompletten Transporter oder einen Haltepunkt aus dem
    Daemon.
  Rueckgabewerte:
    - 1: Erfolg.
    - 0: Transporter|Haltepunkt existiert nicht.
    - -1: Keine Berechtigung.

public varargs int|object* HasTransporter(object stop, string transporter_id)
  Beschreibung:
    Gibt eine Liste der Transporter aus, den den Haltepunkt anlaufen, oder
    prueft auf einen bestimmten Transporter.
  Rueckgabewerte:
    - 0: Haltepunkt ist kein Objekt, dem Haltepunkt sind keine Transporter
         zugeordnet,
         die Transporter sind nicht geladen
         oder, bei angabe von <transporter>: <transporter> faehrt den Hafen
         nicht an.
    - object*: Liste mit einem oder mehreren Transportern.
  Bemerkung:
    Auch bei der Angabe von <transporter_id> koennen mehrere Objekte
    zurueckgegeben werden, es wird mittels id() gefiltert.

public mixed RouteExists(object transporter, string dest)
  Beschreibung:
    Prueft auf korrekte Parameter und leitet die Anfrage an
    /std/transport weiter.
  Rueckgabewerte:
    - 0: <transporter> ist kein Objekt oder <dest> ist kein String.
    - Das Ergebnis von transporter->HasRoute(dest);

public mapping QueryTransporters()
public mapping QueryAllStops() 
public mapping QueryTransporterByStop(string stop)
public mapping QueryStopsByTransporter(string transporter)
  Beschreibung:
    Gibt das jewilige gefilterte oder ungefilterte Mapping aus.
  Rueckgabewerte:
    Mapping mit dem Ergebnis.
*/

#pragma strong_types,save_types,rtt_checks
#pragma pedantic,range_check,warn_deprecated
#pragma warn_empty_casts,warn_missing_return,warn_function_inconsistent
#pragma no_clone,no_shadow

#include <transport.h>
#include <wizlevels.h>
#include <daemon.h>

#define TRANSPORTER 0
#define STOP 1

#define MEMORY "/secure/memory"

// Key: string Schiff.
// Value0: string* Haefen, die von <Schiff> angefahren werden.
mapping transporters = ([]);
// Key: string Hafen
// Value0: string* Schiffe, die <Hafen> anfahren.
mapping stops = ([]);

protected void create()
{
  // Vom MEMORY die Daten abholen.
  // Wenn keine da, war vermutlich Reboot, dann muessen wir warten, bis die
  // Transporter sich wieder melden. Aber die Mappings muessen ins Memory
  // geschrieben werden.
  mapping tmp = MEMORY->Load("stops");
  if (mappingp(tmp))
  {
    stops = tmp;
  }
  else
  {
    if (MEMORY->Save("stops",stops) != 1)
      raise_error("Could not save memory to /secure/memory.");
  }
  tmp = MEMORY->Load("transporters");
  if (mappingp(tmp))
  {
    transporters = tmp;
  }
  else
  {
    if (MEMORY->Save("transporters",transporters) != 1)
      raise_error("Could not save memory to /secure/memory.");
  }
}

varargs int remove(int s)
{
  destruct(this_object());
  return 1;
}

// Loeschfunktion
private int _remove_data(string|object key, int what)
{
  mapping first,second;
  // Erstmal die Arbeitsmappings befuellen.
  if(what==TRANSPORTER)
  {
    first=transporters;
    second=stops;
  }
  else
  {
    first=stops;
    second=transporters;
  }

  // Key auf einen String normalisieren.
  if(objectp(key))
  {
    key=object_name(key);
  }

  if(!member(first,key))
  {
    return 0;
  }

  // Erstmal aus dem jeweils anderen Mapping austragen.
  foreach(string s : first[key])
  {
    // Nicht existent oder nicht zugeordnet -> weiter.
    if(!member(second,s) || member(second[s],key)==-1)
    {
      continue;
    }
    second[s]-=({key});
    if (!sizeof(second[s]))
      m_delete(second,s);
  }
  // Jetzt noch aus dem eigenen Mapping austragen.
  m_delete(first,key);
  return 1;
}

// Ein komplettes Schiff entfernen.
public int RemoveTransporter(object|string transporter)
{
  if (extern_call()
      && previous_object() != transporter
      && !IS_ELDER(getuid(previous_object())))
      return -1;
  return _remove_data(transporter,TRANSPORTER);
}

// Entfernt einen kompletten Hafen aus dem Daemon
public int RemoveStop(object|string stop)
{
  if (extern_call()
      && previous_object() != stop
      && !IS_ELDER(getuid(previous_object())))
      return -1;
  return _remove_data(stop,STOP);
}

// Setzt die aktuelle Route eines Transporters.
// Die Route hier ist allerdings nur die Liste von Haltepunkten - nicht der
// Rest, der in den Routen in den Transporter abgelegt ist.
// Wird von /std/transport gerufen, wenn P_NO_TRAVELING nicht gesetzt ist.
public int AddRoute(string transporter, string* l_stops)
{
  if (!stringp(transporter) || !pointerp(l_stops) || !sizeof(l_stops))
    return 0;

  if ((transporter[0..2] == "/p/" || transporter[0..8] == "/players/")
      && !IS_ARCH(getuid(previous_object()))) 
  {
    return -1;
  }

  // Damit sind alle Abfragen auf find_object(transporter|stop) ueberfluessig,
  // im Zweifelsfall knallt es naemlich hier. ;-)
  transporter = object_name(load_object(transporter));

  // Wenn die route bereits existiert, austragen. Dies ist noetig, weil die
  // Haltepunkte ja auch bereinigt werden muessen.
  if (member(transporters, transporter))
    RemoveTransporter(transporter);

  // Transporter eintragen, die Route wird dabei immer ueberschrieben, wenn
  // der Transporter schon bekannt ist. Ziel: hier ist immer die aktuell
  // konfigurierte Route drin.
  m_add(transporters, transporter, l_stops);

  // Nach obigen Schema, nur diesmal Haltepunkte und zugehoerige Schiffe 
  // eintragen.
  foreach(string stop : l_stops)
  {
    if (!member(stops,stop))
    {
      m_add(stops, stop, ({transporter}));
    }
    else if (member(stops[stop],transporter) == -1)
    {
      stops[stop] += ({ transporter });
    }
  }
  return 1;

}

// Abfrage ob ein Schiff einen Hafen anfaehrt.
public varargs int|object* HasTransporter(object stop,
                                          string transporter_id)
{
  if (!objectp(stop)) return 0;

  <string|object>* trans=stops[object_name(stop)];
  if (!pointerp(trans)) return 0;

  // Eigentlich sollten sich Transporter ordentlich abmelden... Aber zur
  // Sicherheit die nicht gefundenen Objekte ausfiltern.
  trans = map(trans, #'find_object) - ({0});

  // Wenn ne bestimmte ID gesucht wird, nur diese Transporter liefern.
  if (stringp(transporter_id))
    trans = filter_objects(trans, "id", transporter_id);

  return sizeof(trans) ? trans : 0;
}

public mixed RouteExists(object transporter, string dest)
{
  if (!objectp(transporter) || !stringp(dest))
  {
    return 0;
  }
  return transporter->HasRoute(dest);
}

public mapping QueryTransporters()
{
  return deep_copy(transporters);
}

public mapping QueryAllStops()
{
  return deep_copy(stops);
}

// Liefert alle Transporter mit ihren Haltepunkten zurueck, die diesen Halt ansteuern.
public mapping QueryTransportByStop(string stop)
{
  mapping res=m_allocate(4);
  foreach(string s, string* arr : transporters)
  {
    if(member(arr,stop)!=-1)
    {
      m_add(res,s,arr);
    }
  }
  return res;
}

// Liefert alle Haltepunkte mit ihren Transportern zurueck, die dieser
// transporter ansteuert.
public mapping QueryStopsByTransporter(string transporter)
{
  mapping res=m_allocate(4);
  foreach(string s, string* arr : stops)
  {
    if(member(arr,transporter)!=-1)
    {
      m_add(res,s,arr);
    }
  }
  return res;
}


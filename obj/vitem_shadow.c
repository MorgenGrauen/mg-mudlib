#pragma strong_types,rtt_checks

#include <moving.h>
#include <properties.h>
#include <container/vitems.h>

// Einschraenkungen: Alle _query_* Querymethoden und _set_* Setmethoden im
// Ziel werden nicht beschattet.


// Alle nicht explizit zum oeffentlichen Interface von /std/container
// gehoerenden (d.h. alle nicht explizit "public" definierten) Funktionen
// werden protected geerbt. Damit koennen sie von aussen nicht gerufen werden
// und der Effekt ist, dass sie nicht im Shadow gerufen werden, sondern im
// beschatteten Objekt.
protected functions nosave private variables inherit "/std/container";

// Mapping ohne Values. Keys zeigen an, welche Props dieses Objekt im
// Zielobjekt beschattet.
mapping props;

object cloner;

protected void create()
{
  cloner=previous_object();
  set_next_reset(7200);
}

// Einige spezielle Props (wie Details etc. und Props mit Arrays oder
// Mappings) werden gemerged mit dem Ziel, alle anderen ueberschrieben.
private void configure_object(object ziel, mapping newprops)
{
  props = ([:0]);
  foreach (string k, mixed v : newprops)
  {
    int reset_prop;
    if (k[0] == VI_RESET_PREFIX)
    {
      reset_prop=1;
      k=k[1..];
    }
    switch(k)
    {
      case P_READ_DETAILS:
        if (reset_prop)
          RemoveReadDetail(0);
        else
          SetProp(P_READ_DETAILS, ziel->QueryProp(P_READ_DETAILS));
        walk_mapping(v, #'AddReadDetail);
        break;
      case P_DETAILS:
        if (reset_prop)
          RemoveDetail(0);
        else
          SetProp(P_DETAILS, ziel->QueryProp(P_DETAILS));
        walk_mapping(v, #'AddDetail);
        break;
      case P_SMELLS:
        if (reset_prop)
          RemoveSmells(0);
        else
          SetProp(P_SMELLS, ziel->QueryProp(P_SMELLS));
        walk_mapping(v, #'AddSmells);
        break;
      case P_SOUNDS:
        if (reset_prop)
          RemoveSounds(0);
        else
          SetProp(P_SOUNDS, ziel->QueryProp(P_SOUNDS));
        walk_mapping(v, #'AddSounds);
        break;
      case P_TOUCH_DETAILS:
        if (reset_prop)
          RemoveTouchDetail(0);
        else
          SetProp(P_TOUCH_DETAILS, ziel->QueryProp(P_TOUCH_DETAILS));
        walk_mapping(v, #'AddTouchDetail);
        break;
      case P_IDS:
        if (reset_prop)
          SetProp(P_IDS, v);
        else
        {
          SetProp(P_IDS, ziel->QueryProp(P_IDS));
          AddId(v);
        }
      case P_CLASS:
        if (reset_prop)
          SetProp(P_CLASS, v);
        else
        {
          SetProp(P_CLASS, ziel->QueryProp(P_CLASS));
          AddClass(v);
        }
      case P_ADJECTIVES:
        if (reset_prop)
          SetProp(P_ADJECTIVES, v);
        else
        {
          SetProp(P_ADJECTIVES, ziel->QueryProp(P_ADJECTIVES));
          AddAdjective(v);
        }
        break;

      // Alle anderen Properties stumpf setzen.
      default:
        if (reset_prop)
          SetProp(k, v);
        else
        {
          mixed val = ziel->QueryProp(k);
          // Arrays und Mapping koennen addiert werden, alles andere wird
          // ueberschrieben.
          if (pointerp(val) || mappingp(val))
            SetProp(k, val + v);
          else
            SetProp(k, v);
        }
     } // switch
  } // foreach
}

/*
xcall 
/players/zesstra/vitem_shadow->set_shadow(find_object("/players/zesstra/seil#55513
2"),([P_MATERIAL:([MAT_ICE:100]),P_SHORT:"Eis-Seil"]))
*/
public void set_shadow(object target, mapping p)
{
  // wenn schon beschattend, ists nen Fehler
  if (query_shadowing(this_object()))
      raise_error(sprintf("set_shadow(): Ein Objekt wird schon beschattet: "
            "%O (Ziel jetzt: %O)\n",query_shadowing(this_object()), target));

  // der Aufrufer muss eine eUID gleich meiner UID haben, damit er das
  // Beschatten aktivieren kann. (Dieses Objekt hat immer die UID+eUID == eUID
  // vom Cloner des Shadows, d.h. vom Erzeuger des vItems.) D.h. das
  // Beschatten kann vom Erzeuger des vItems aktiviert werden.
  if (getuid(this_object()) == geteuid(previous_object()))
  {
    configure_object(target, p);
    if (shadow(target, 1) != target)
      raise_error(sprintf("Kann %O nicht beschatten.\n",target));
  }
  else
    raise_error(sprintf("Kann %O nicht beschatten, falsche eUID. "
                        "Meine: %s, Aufrufer: %s\n",
                        target, getuid(this_object()),
                        geteuid(previous_object()) ));
}


varargs int remove(int silent)
{
  // Sollten wir gerade etwas beschatten, ist der Aufruf von diesem remove()
  // evtl. auch der vom Ziel. Wir machen das ganz einfach: Aufruf von remove()
  // fuehrt zum Entsorgen von Ziel und diesem Objekt.
  object ziel=query_shadowing(this_object());
  // und ganz wichtig: nur wenn das Ziel noch ein virtuell anwesendes Items
  // (vitem) ist, d.h. *kein* Environment hat.
  if (ziel && !environment(ziel))
    ziel->remove(1);

  return ::remove(1);
}

// Dieses Objekt raeumt sich immer im ersten reset auf - es ist kurzlebig und
// transient und Aenderungen an seinen Daten bleiben nicht erhalten.
void reset()
{
  // BTW: Sollten wir gerade etwas beschatten, ist der Aufruf von diesem
  // reset() evtl. auch der vom Ziel. (remove kuemmer sich drum)
  remove(1);
}

// private ueberschreiben, damit es in diesem Objekt nicht gerufen werden kann
// und im Beschatteten gerufen wird.
private varargs void init(object origin)
{
}

public varargs int move( object|string dest, int method )
{
  object ziel=query_shadowing(this_object());
  int res;
  if (ziel)
  {
    res=ziel->move(dest, method);
    if (res == MOVE_OK)
    {
      // virtuellem environment bescheidsagen, dass das Objekt bewegt wurde.
      cloner->VItemMoved(ziel);
      // und nach Bewegung des vitems ist das kein vitem mehr und der Schatten
      // muss weg.
      remove(1);
    }
  }
  // Bewegen vom Schatten ist nicht. Und ohne Ziel braucht es den Schatten eh
  // nicht.
  else
  {
    remove(1);
    res = ME_WAS_DESTRUCTED;
  }
  return res;
}

// Query und QueryProp liefern die abweichend hier definierten Properties,
// oder ansonsten die aus dem beschatteten Objekt.
public mixed QueryProp( string name )
{
  object ziel=query_shadowing(this_object());
  if (ziel && !member(props,name))
  {
    return ziel->QueryProp(name);
  }
  return ::QueryProp(name);
}

public varargs mixed Query( string name, int Type )
{
  object ziel=query_shadowing(this_object());
  if (ziel && !member(props,name))
  {
    return ziel->Query(name, Type);
  }
  return ::Query(name, Type);
}

public varargs mixed Set( string name, mixed Value, int Type, int extern )
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,name);
  return ::Set(name, Value, Type, extern_call());
}

public mixed SetProp( string name, mixed Value )
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,name);
  return ::SetProp(name, Value);
}

// Details hinzufuegen muss die passende Prop so markieren, dass sie aus
// diesem Objekt geliefert wird.
public void AddDetail(string|string* keys, string|string*|mapping|closure descr)
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,P_DETAILS);
  m_add(props,P_SPECIAL_DETAILS);
  return ::AddDetail(keys, descr);
}

public void AddReadDetail(string|string* keys,
                   string|string*|mapping|closure descr )
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,P_READ_DETAILS);
  return ::AddDetail(keys, descr);
}

public void AddSounds(string|string* keys,
               string|string*|mapping|closure descr )
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,P_SOUNDS);
  return ::AddDetail(keys, descr);
}

public void AddSmells(string|string* keys,
               string|string*|mapping|closure descr )
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,P_SMELLS);
  return ::AddDetail(keys, descr);
}

public void AddTouchDetail(string|string* keys,
                    string|string*|mapping|closure descr )
{
  // Ab jetzt ist das unsere Prop, die aus dem Beschatteten ist egal.
  m_add(props,P_TOUCH_DETAILS);
  return ::AddDetail(keys, descr);
}

// Beides nicht unterstuetzt fuer vitems.
visible void AddSpecialDetail(string|string* keys, string functionname )
{
  raise_error("Nicht unterstuetzt fuer vitem-shadows.\n");
}
visible void RemoveSpecialDetail(string|string* keys )
{
  raise_error("Nicht unterstuetzt fuer vitem-shadows.\n");
}

// Wird ggf. auf GetDetail im echten Item umgeleitet, damit jenes FP an
// Details haben kann und man diese auch findet.
public varargs string GetDetail(string key, string race, int sense)
{
  object ziel=query_shadowing(this_object());
  if (!ziel)
    return ::GetDetail(key, race, sense);

  switch(sense)
  {
    case SENSE_SMELL: if (member(props, P_SMELLS))
                          return ::GetDetail(key, race, sense);
                      break;
    case SENSE_SOUND: if (member(props, P_SOUNDS))
                          return ::GetDetail(key, race, sense);
                      break;
    case SENSE_TOUCH: if (member(props, P_TOUCH_DETAILS))
                          return ::GetDetail(key, race, sense);
                      break;
    case SENSE_READ:  if (member(props, P_READ_DETAILS))
                          return ::GetDetail(key, race, sense);
                      break;

    default:          if (member(props, P_DETAILS))
                          return ::GetDetail(key, race, sense);
                      break;
  }
  // nix in diesem Objekt, GetDetail vom ziel darfs machen.
  return ziel->GetDetail(key, race, sense);
}

// Sollen aus dem Beschatteten kommen
public object *present_objects( string complex_desc )
{
  object ziel=query_shadowing(this_object());
  if (ziel)
    return ziel->present_objects(complex_desc);
  return ::present_objects(complex_desc);
}

// Sollen aus dem Beschatteten kommen
public object *locate_objects( string complex_desc, int info )
{
  object ziel=query_shadowing(this_object());
  if (ziel)
    return ziel->locate_objects(complex_desc, info);
  return ::locate_objects(complex_desc, info);
}

public object *AllVirtualEnvironments()
{
  if (cloner)
  {
    object *cloner_envs = all_environment(cloner)
                          || cloner->AllVirtualEnvironments();
    if (cloner_envs)
      return ({cloner}) + cloner_envs;
    return ({cloner});
  }
  return 0;
}


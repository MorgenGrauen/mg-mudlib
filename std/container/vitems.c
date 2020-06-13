// MorgenGrauen MUDlib
//
// container/vitems.c -- managing virtually present automatic items
//
#pragma strict_types,rtt_checks, range_check
#pragma no_clone

#include <properties.h>
#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <container/vitems.h>

// Werte im Mapping in P_VITEMS:
#define VI_PATH          0    // Pfad zur BP
#define VI_REFRESH       1    // Refresheinstellung, s. AddItem
#define VI_OBJECT        2    // Aktuelles Objekt des VItem
#define VI_PROPS         3    // Props fuer das echte Objekt
#define VI_SHADOW_PROPS  4    // Props fuer den shadow/vitem_proxy
#define VI_LAST_OBJ      5    // letztes mitgenommenes Objekt
#define VI_LENGTH        6

// VI_REFRESH hat aehnliche Bedeutung wie bei AddItem, mit einer Besonderheit:
// intern wird in Form eines negativen Wertes kodiert, ob das VItem gerade neu
// erzeugt werden darf (negativ: ja, positiv: nein).

/* Mehr zu VI_REFRESH:
 * vitems raeumen sich ggf. selber weg. D.h. wenn der Clone des vitems nicht
 * mehr ist, heisst das nicht, dass es jemand mitgenommen hat. In dem Fall
 * muss es sofort (wenn seine Praesenz abgefragt wird) neu erzeugt werden.
 * Die Logik ist wie folgt:
 * Der Standardzustand von VI_REFRESH ist ein negierter Wert. Solange dies der
 * Fall ist, wird ein vitem *immer* erzeugt, sofern es abgefragt wird, aber
 * nicht da ist.
 * Wird ein vitem hingegen bewegt/mitgenommen, wird VI_REFRESH auf den
 * positiven Wert geaendert. In diesem Fall wird es NICHT mehr neu erzeugt,
 * wenn es nicht als vitem praesent ist. Erst im naechsten reset() wird
 * geprueft, ob es in Zukunft wieder neu erzeugt werden darf. Wenn ja, wird
 * der Zahlenwert wieder negiert.
 * Es ist Absicht, dass VI_REFRESH_ALWAYS auch nur maximal einmal pro reset()
 * *mitgenommen* werden darf.
 */


//protected void create()
//{
//}

protected void create_super() {
      set_next_reset(-1);
}

public varargs void RemoveVItem(string key)
{
  mixed vitems=QueryProp(P_VITEMS);
  if (mappingp(vitems) && member(vitems, key))
  {
    // Es wird auch zerstoert, wenn das genommene Objekt gerade im Raum
    // rumliegt (weil Spieler es hat fallen lassen etc.)
    if (vitems[key, VI_OBJECT])
        ({int})vitems[key, VI_OBJECT]->remove(1);
    if (vitems[key, VI_LAST_OBJ]
        && environment(vitems[key, VI_LAST_OBJ]) == this_object())
        ({int})vitems[key, VI_LAST_OBJ]->remove(1);

    m_delete(vitems, key);
    SetProp(P_VITEMS, vitems);
  }
}

// TODO: braucht es dynamische refresh, shadowprops und props?
public varargs void AddVItem(string key, int refresh, mapping shadowprops,
                             string path, mapping props)
{
  if (!sizeof(key))
    return;
  // Wenn path gegeben, muss es eine ladbare Blueprint sind
  if (sizeof(path) && !load_object(path))
    return;
  if (!sizeof(path))
  {
    if (mappingp(props))
      raise_error("Reine vitems erlauben keine <props>\n");
  }

  refresh ||= VI_REFRESH_NONE;
  shadowprops ||= ([]);
  // Wenn reines vItem und keine IDs gesetzt, wird <key> als ID verwendet,
  // irgendwie muss es ja ansprechbar sein. (Wenn es ein Objekt mit Templat
  // ist, hat es normalerweise die IDs aus dem Templat. Wenn man das nicht
  // will, muss man es mit gezielter Angabe von P_IDS in den Shadowprops
  // ueberschreiben.) Gleiches fuer P_NAME (ohne ist ein "Ding") 
  // P_SHORT wird *nicht* automatisch gesetzt - ohne ist es zwar nicht
  // wahrnehmbar, aber untersuchbar und der Zustand ist gar nicht selten
  // gewuenscht.
  if (!path)
  {
    if (!member(shadowprops, P_IDS))
      shadowprops[P_IDS] = ({key});
    if (!member(shadowprops, P_NAME))
      shadowprops[P_NAME] = capitalize(key);
  }
  mixed vitems=QueryProp(P_VITEMS);
  if (!mappingp(vitems))
    vitems = m_allocate(1, VI_LENGTH);
  vitems[key, VI_PATH] = path;
  vitems[key, VI_REFRESH] = negate(refresh);
  vitems[key, VI_PROPS] = props;
  vitems[key, VI_SHADOW_PROPS] = shadowprops;
  SetProp(P_VITEMS, vitems);
}


private void configure_object(object ob, mapping props)
{
  foreach (string k, mixed v : props)
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
        if (reset_prop) ({void})ob->RemoveReadDetail(0);
        walk_mapping(v, "AddReadDetail", ob);
        break;
      case P_DETAILS:
        if (reset_prop) ({void})ob->RemoveDetail(0);
        walk_mapping(v, "AddDetail", ob);
        break;
      case P_SMELLS:
        if (reset_prop) ({void})ob->RemoveSmells(0);
        walk_mapping(v, "AddSmells", ob);
        break;
      case P_SOUNDS:
        if (reset_prop) ({void})ob->RemoveSounds(0);
        walk_mapping(v, "AddSounds", ob);
        break;
      case P_TOUCH_DETAILS:
        if (reset_prop) ({void})ob->RemoveTouchDetail(0);
        walk_mapping(v, "AddTouchDetail", ob);
        break;
      case P_IDS:
        if (reset_prop) ({string*})ob->SetProp(P_IDS, v);
        else ({void})ob->AddId(v);
      case P_CLASS:
        if (reset_prop) ({string*})ob->SetProp(P_CLASS, v);
        else ({void})ob->AddClass(v);
      case P_ADJECTIVES:
        if (reset_prop) ({string*})ob->SetProp(P_ADJECTIVES, v);
        else ({void})ob->AddAdjective(v);
        break;

      // Alle anderen Properties stumpf setzen.
      default:
        ({mixed})ob->SetProp(k, v);
    }
  }
}

// Clont ein vitem, falls noetig.
// Nebeneffekt ist aber in jedem Fall auch, dass ein nicht mehr virtuell
// anwesendes Objekt als VI_LAST_OBJ gespeichert und VI_OBJECT geloescht wird.
private void check_vitem(string key, string path, int refresh,
                         object obj, mapping props, mapping shadow_props,
                         object last_obj)
{
  // Ist es noch da? Ein vItem ist "da", wenn obj auf ein gueltiges Objekt
  // zeigt, welches *KEIN* Environment hat. (Hat es ein Environment, wurde es
  // mitgenommen.)
  if (obj)
  {
    if (!environment(obj))
      return;
    // wenn es mitgenommen wurde, ist obj in jedem Fall kein vItem mehr:
    // (eigentlich wird das in VItemMoved schon gemacht, aber mal fuer den
    // Fall, dass jemand den Shadow hart entsorgt hat o.ae.)
    last_obj = obj;
    obj = 0;
    // und wird ggf. unten neu erzeugt.
  }

  if (refresh < 0)
  {
    object sh;
    if (path)
    {
      obj=clone_object(path);
      ({int})obj->SetAutoObject(1);
      if (mappingp(props))
        configure_object(obj, props);
      // Schatten erzeugen, welcher die Beschreibung des Objekts im Container nach
      // den Props in shadow_props abaendert.
      sh = clone_object("/obj/vitem_shadow");
      ({void})sh->set_shadow(obj, shadow_props);
    }
    else
    {
      obj=clone_object("/obj/vitem_proxy");
      configure_object(obj, shadow_props);
      // no shadow needed in this case.
    }
  }
}

// Erzeugt Instanzen der vItems (sofern noetig und erlaubt durch
// Refresh-Parameter).
private mixed CheckVItems()
{
  mixed vitems=QueryProp(P_VITEMS);
  if (!mappingp(vitems))
    vitems = m_allocate(0, VI_LENGTH);
  walk_mapping(vitems, #'check_vitem);
  return vitems;
}

// Liefert alle in diesem Raum virtuell anwesenden Items
public object *GetVItemClones()
{
  mapping vitems = CheckVItems();
  return filter(m_values(vitems, VI_OBJECT), #'objectp);
}

public object present_vitem(string complex_desc)
{
  foreach(object o : GetVItemClones())
  {
    if (({int})o->id(complex_desc))
      return o;
  }
  return 0;
}

// wird aus dem Shadow fuer das VItem gerufen, wenn es genomment etc. wird.
// In dem Fall wird das Refresh "gesperrt", d.h. es wird fruehestens nach dem
// naechsten Reset wieder neu erzeugt - sofern im naechsten Reset die
// Vorraussetzungen erfuellt sind.
public void VItemMoved(object ob)
{
  if (load_name(previous_object()) == "/obj/vitem_shadow")
  {
    mapping vitems = QueryProp(P_VITEMS);
    if (!mappingp(vitems))
      return;
    // passendes vitem suchen
    foreach(string key, string path, int refresh, object o, mapping props,
            mapping shadow_props, object last_obj: &vitems)
    {
      if (ob != o)
        continue;
      else
      {
        // mitgenommenes Objekt merken und vitem-objekt loeschen
        last_obj = o;
        o = 0;
        // Sperren gegen sofortiges Neuerzeugen, wenn refresh nicht
        // VI_REFRESH_INSTANT ist.
        if (refresh != VI_REFRESH_INSTANT)
          refresh = negate(refresh);
        break;
      }
    }
  }
}

void reset()
{
  mapping vitems=QueryProp(P_VITEMS);
  if (!mappingp(vitems))
    return;
  foreach(string key, string path, int refresh, object obj,
          mapping props, mapping shadow_props, object last_obj : &vitems)
  {
    // Wenn negativ (d.h. vItem wurde noch nicht mitgenommen), 0 oder
    // REFRESH_NONE, muss hier nix gemacht werden.
    if (refresh <= REFRESH_NONE)
      continue;

    // Wenn last_obj nicht mehr existiert, darf (ausgenommen natuerlich
    // REFFRESH_NONE) immer neu erzeugt werden.
    if (!last_obj)
    {
      refresh=negate(refresh);
      continue;
    }

    // restliche Faelle
    switch(refresh)
    {
      case VI_REFRESH_MOVE_HOME:
        // Wenn das Objekt nicht mehr als vItem hier ist (auch wenn es hier im
        // Raum liegt!), wird es heim bewegt (sprich: zerstoert und neues
        // vitem).
        // (Da man hier nur hinkommt, wenn es mitgenommen wurde (refresh > 0),
        // wird es immer refresht...
        // Zu beachten: es soll auch nicht hier in diesem Container rumliegen
        // nach dem Heimbewegen, also zerstoeren!
        ({int})last_obj->remove(1);
        // Fallthrough
      case VI_REFRESH_REMOVE:
        // wenn nicht mehr als vItem hier ist (d.h. auch wenn es hier im Raum
        // rumliegt!) darf es neu erzeugt werden.
        // (Hierher kommt die Ausfuehrung nur her, wenn es mitgenommen
        // wurde, d.h. letztendlich: immer. d.h. Fallthrough.)
      case VI_REFRESH_ALWAYS:
        // neu erzeugen
        refresh=negate(refresh);
        break;
    }
  }
}


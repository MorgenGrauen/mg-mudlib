// MorgenGrauen MUDlib
/** @file
* Implementation fuer Sockelitems.
* Langbeschreibung... TODO
* @author Zesstra + Arathorn
* @date xx.05.2008
* @version $Id$
*/

/* Changelog:
*/
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma no_shadow
#pragma pedantic
#pragma range_check

#define NEED_PROTOTYPES
#include <thing/sockets.h>
#undef NEED_PROTOTYPES
#include <defines.h>
#include <lpctypes.h>

#ifndef DEBUG
/** Outputs debug message to Maintainer, if Maintainer is logged in. 
*/
#define DEBUG(x) __debug_msg__(x)
#endif


private void __debug_msg__(string x) {
  if (find_player("zesstra"))
      tell_object(find_player("zesstra"),"SDBG: "+x+"\n");
}

/** Setzt \a item in einen passenden Sockel ein.
  Wird vom Handwerker gerufen. Sofern die Pruefung mittels TestSocketItem()
  erfolgreich ist, werden die Props dieses Objekt durch die im Item
  gespeicherten in P_SOCKET_PROPS ergaenzt, ggf. P_RESTRICTION geaendert und
  Name der Blueprint, Name und P_SOCKET_PROPS in P_SOCKETS gespeichert, um
  spaeter nachvollziehen zu koennen, wie dieses Objekt bereits modifiziert
  wurde.
  @attention Am Ende der Funktion wird \a item zerstoert und darf nicht mehr
  benutzt werden!
  @param[in] item Objekt, welches in einen passenden Socket eingesetzt werden
  soll.
  @return 1, falls Einsetzen erfolgreich, <0 wenn nicht.
  @sa TestSocketItem(object)
 */
public int MountSocketItem(object item) {
  if (!objectp(item)) return SOCKET_NO_OBJECT;
  if ((int res=TestSocketItem(item)) != SOCKET_OK)
    return res;
  mapping idata=(mapping)item->QueryProp(P_SOCKET_PROPS);
  // TODO: Spezialbehandlung fuer Props, bei denen das Objekt erhalten bleiben
  // muss. (z.B. P_DEFEND_FUNC).

  // zu modifizierende Props addieren
  foreach(string propname, mixed pval: idata) {
    SetProp(propname, AddToEntity(QueryProp(propname), pval) );
  }
  // Restrictions hinzufuegen.
  SetProp(P_RESTRICTIONS, 
      AddToEntity(QueryProp(P_RESTRICTIONS), 
	item->QueryProp(P_SOCKET_RESTR_USE)) );

  // Daten ueber dieses Socketitem abspeichern.
  mixed sockets=QueryProp(P_SOCKETS)[item->QueryProp(P_SOCKET_TYPE)];
  // freier Sockel muss existieren (->TestSocketItem())
  int index=member(sockets, SOCKET_FREE);
  sockets[index] = idata + (["BLUE_NAME": load_name(item),
                           "DESCRIPTION": item->name(WER) ]);

  // ggf. Beschreibung aktualisieren?
  // ggf. Sonderbehandlung fuer Props, bei denen das Objekt noch gebraucht
  // wird (z.B. P_DEFEND_INFO)

  // Prop schuetzen, sobald Daten drin stehen.
  Set(P_SOCKETS, PROTECTED|NOSETMETHOD, F_MODE_AS);

  // item entsorgen
  if (!item->remove(1))
    raise_error(sprintf("MountSocketItem() in %O: %O hat Ausfuehrung von "
	  "remove() verweigert.\n", ME, item));
  return SOCKET_OK;
}

/** Prueft, ob \a item in einen Sockel eingesetzt werden kann.
  Prueft, ob es fuer das Item einen passenden Sockel gibt und ob 
  es die formalen Anforderungen erfuellt, dort eingesetzt zu werden. 
  Kann vom Handwerker vor dem echten Einsetzen gerufen werden.
  Wird auch von MountSocketItem(object) gerufen.
  @param[in] item Objekt, welches auf Eignung zum Einsetzen in einen Sockel
  getestet werden soll.
  @return 1, falls ein passender und freier Sockel existiert, 0 sonst.
  @sa MountSocket(object)
*/
public int TestSocketItem(object item) {

  if (!objectp(item)) return SOCKET_NO_OBJECT;
  
  // ist es ein Sockelitem und hat es einen gueltigen Typ?
  mapping idata = (mapping)item->QueryProp(P_SOCKET_PROPS);
  if (!mappingp(idata) || !sizeof(idata))
    return SOCKET_NO_DATA;
  string styp=(string)item->QueryProp(P_SOCKET_TYPE);
  if (!stringp(styp)
      || member(VALID_SOCKET_TYPES, styp) == -1)
    return SOCKET_INVALID_TYPE;

  // Hat dieses Item ueberhaupt Sockel? Und wenn ja, haben wir nen freien
  // Socke fuer den betreffenden Typ?
  mapping mysockets = QueryProp(P_SOCKETS);
  if (!mappingp(mysockets) || !sizeof(mysockets)) 
    return SOCKET_NO_SOCKETS;
  if (!member(mysockets, styp)
      || !member(mysockets[styp], SOCKET_FREE) == -1 )
    return SOCKET_NONE_AVAILABLE;

  // Handwerker pruefen.
  // TODO: Soll die Fehlermeldung komplett vom Aufrufer erledigt werden oder
  // soll es einen Heinweis geben, warum der Handwerker nicht geeignet ist?
  object craftsman = previous_object();
  mapping restr = (mapping)item->QueryProp(P_SOCKET_RESTR_MOUNT);
  if (!objectp(craftsman)
      || (mappingp(restr)
	&& "/std/restriction_checker"->check_restrictions(craftsman,
	  restr)))
    return SOCKET_NO_EXPERTISE;

  // da P_RESTRICTION nur beim Anziehen/Zuecken geprueft wird, darf man ein
  // Item nicht in getragenem Zustand modifizieren.
  if (objectp((object)item->QueryProp(P_WIELDED))
      || objectp((object)item->QueryProp(P_WORN)))
    return SOCKET_ITEM_INUSE;

  return SOCKET_OK;
}

/** Liefert Infos ueber die Sockets (Typ, was drin ist, etc.\ ).
  Primaer fuer Gilden gedacht.
*/
public mixed GetSocketInfo() {
}

/* **************************** private ************************* */

/** Addiert zwei Variablen, sofern sie kompatibel sind.
  Lassen sich die beiden Datentypen nicht sinnvoll addieren, erfolgt keine
  Addition und \a old wird zurueck geliefert (z.B. bei Objekten, Closures).
  Hierbei erfolgt z.B. bei Mappings eine wertweise Addition, keine blosse
  Ersetzung der Keys wie bei der normalen Mapping-Addition.
  @param[in,out] old Datum, zu dem addiert werden soll. Wird bei Mappings
  geaendert.
  @param[in] new Datum, welches addierten soll.
  @return Summe von old und new, falls die Datentypen kompatibel sind. Falls
  nicht, kann old zurueckgegeben werden. Der Datentyp von 
  @attention \b Rekursiv! Kann teuer werden.\n
    Kann (modifizierte) \a old oder \a new oder ganz neues Datum 
    zurueckliefern, d.h. der zurueckgelieferte Datentyp muss nicht dem
    Datentyp von old entsprechen. Ebenso erzeugt z.B. die Addition zweier
    Arrays ein neues Array.
    Wenn die korrespondierenden Werte nicht den gleichen Datentyp haben,
    findet u.U. keine Addition statt oder es wird eine Datentyp-Umwandlung
    durchgefuehrt, z.B. Int + Float == Float.\n
  */
private mixed AddToEntity(mixed old, mixed new) {

  // einfachste Faelle:
  if (!old)
    return new;
  else if (!new)
    return old;

  // Typ bestimmen
  int oldtype = typeof(old);
  int newtype = typeof(new);
  // Variablen gleichen Typs sind einfach.
  if (oldtype == newtype) {
    switch (oldtype) {
      // einige Typen werden stumpf addiert.
      case T_NUMBER:
      case T_STRING:
      case T_FLOAT:
      case T_POINTER: // TODO: anderes Verhalten?
	return old+new;
      // Mappings werden wertweise addiert.
      case T_MAPPING:
	// nur wenn die Breite des 2. Summanden <= der des 1. Summanden ist,
	// lassen sich Mappings hiermit addieren.
	if (widthof(new) > widthof(old))
	  return old;
	// new auf old addieren. Keys werden nicht ersetzt, sondern nach
	// Moeglichkeit die werte unter den keys jeweils addiert.
	// map() nur zum Uebergeben aller Keys+Value. AddToMapping aendert
	// direkt das uebergebene Mapping old.
	map(new, #'AddToMapping, old); 
	// alles hier nicht aufgefuehrte kann nicht addiert werden.
      default: return old;
    }
  }
  // Ints und Floats sind auch noch gut addierbar.
  else if ((oldtype == T_FLOAT && newtype == T_NUMBER) ||
           (oldtype == T_NUMBER && newtype == T_FLOAT) )
    return old + new;
  // Arrays lassen sich auch gut verwursten (new kann kein Array sein).
  // Umgekehrter Fall waere auch meoglich, aber der Datentyp von old waere
  // sehr deutlich unterschiedlich vom urspruenglichen.
  else if (oldtype == T_POINTER)
    return old+({new});
  // Strings und Zeichenliterale (Ints) sind Ansichtssache.
  else if (oldtype == T_STRING && newtype == T_NUMBER)
    return sprintf("%s%c",old,new);
  else if (oldtype == T_NUMBER && newtype == T_STRING)
    return sprintf("%c%s",old,new);

  // Fall-through
  return old;
}

/** Addiert einen Schluessel und seine Werte zu einem Mapping, ggf.\ auf die
 * bestehenden Werte des Mappings \a old.
 * Der Key und seine Werte ersetzen bestehende Werte in \a old nicht, wie es
 * die normale Mapping-Addition des Driver macht. Stattdessen wird versucht,
 * die neuen Werte auf die entsprechenden alten Werte zu addieren. Falls die
 * Datentypen zweier Werte inkompatibel sind, erfolgt keine Addition und der
 * alte Werte hat Bestand.
  @param[in] key (mixed) Wert, der in das Mapping addiert wird.
  @param[in] values (mixed)
  @param[in,out] old Mapping, in das addiert wird.
  @attention Die Breites des 2. Summanden darf \b nicht groesser sein als die
  Breite des 1, Summanden! \n
    Die korrespondieren Werte sollten den gleichen Datentyp haben, sonst
    findet u.U. keine Addition statt oder es wird eine Datentyp-Umwandlung
    durchgefuehrt, z.B. Int + Float == Float.\n
  */
private void AddToMapping(mixed key, mixed values, mapping old) {
  if (!mappingp(old)) return;
  if (!pointerp(values)) values = ({values});

  // wenn der Key noch nicht existiert, ists einfach.
  if (!member(old, key))
    m_add(old, key, values...); // flatten operator ... cool. ;-)
  else {
    // sonst muessen wir teure handarbeit machen. Insb. weil die Values einen
    // beliebigen Datentyp haben koennen, u.a. Mappings, weswegen wir wieder
    // rekursiv AddToEntity() rufen muessen.
    for(int i=sizeof(values) ; i-- ; ) {
      old[key,i] = AddToEntity(old[key,i], values[i]);
    }
  }
}


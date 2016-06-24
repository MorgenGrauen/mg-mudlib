// MorgenGrauen MUDlib
/** \file /secure/shadowmaster.c
* Objekt, welches eine Liste mit aktiven Shadows hat.
* Die simul_efun shadow() meldet das beschattende Objekt hier an. Nuetzlich,
* damit man als Magier endlich mal rauskriegen kann, welche Shadows es
* eigentlich gibt.
* \author Zesstra
* \date 07.08.2009
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types
#pragma no_clone
#pragma no_inherit
#pragma no_shadow
#pragma pedantic

#include <defines.h>
#include <wizlevels.h>

#define HOME(x) (__PATH__(0)+x)

/** shadows ist ein mapping, welches alle Schatten und ihre Opfer enthaelt.
 *
 * Struktur: Schatten als Schluessel (object) und Beschatteter (object) als
 * Wert.
*/
private mapping shadows = ([]);

#ifndef DEBUG
#define DEBUG(x)  if (find_player("zesstra"))\
            tell_object(find_player("zesstra"),\
                                      "SMDBG: "+x+"\n")
#endif

#define MEMORY "/secure/memory"

#define SHADOW_OK             1
#define SHADOW_ACCESS_DENIED -1
#define SHADOW_EXISTS        -2
#define SHADOW_UNKNOWN       -3

protected void create() {
  mixed tmp;
  seteuid(getuid());
  if (mappingp(tmp=MEMORY->Load("Schatten"))) {
    shadows = tmp;
    DEBUG("Daten aus MEMORY geladen.\n");
  }
  else {
    DEBUG("Keine Daten in MEMORY vorhanden - reinitialisiere.\n");
    if (MEMORY->Save("Schatten", shadows) != 1)
      raise_error("Konnte Daten nicht im Memory ablegen.\n");
  }
}

/** Liefert einen String mit allen Schatten und beschatteten Objekten zurueck.
  * @return string - Liste von Schatten und Opfern
  */
public void DumpShadows() {
  if (extern_call() && !ELDER_SECURITY ) return 0;

  string res = "";
  foreach(object schatten, object opfer: shadows)
    if (schatten && opfer)
      res += sprintf("%O -> %O\n",schatten,opfer);

  printf("Folgende Shadows sind bekannt: \n\n%s\n", res);
}

/** Findet zu einem beschatteten Objekt den zugehoerigen Schatten.
  @param[in] object - ein beschattetes Objekt.
  @return object - Den Schatten oder 0.
  @sa FindShadowsObject(), QueryObject(), QueryInfo()
*/
public object FindShadow(object opfer) {
  if (!objectp(opfer))
    return 0;

  foreach(object schatten, object victim: shadows) {
    if (opfer == victim)
      // Schatten gefunden.
      return schatten;
  }
  return 0;
}

/** Findet zu einem Schatten das beschattete Objekt.
  @param[in] object - ein beschattendes Objekt.
  @return object - Das beschattete Objekt oder 0.
  @sa FindShadow(), QueryObject(), QueryInfo()
*/
public object FindShadowedObject(object schatten) {
  return shadows[schatten];
}

/** Gibt Schatten und Beschatteten zurueck, falls ob eines von beiden ist.
  Ist ob ein Schatten oder ein beschattetes Objekt, wird ein Array aus
  Objekten geliefert. Hierbei werden ggf. alle Objekte in der
  Beschattungshierarchie geliefert, von der ob Bestandteil ist.
  @param[in] object - ein Objekt.
  @return object* - ({a, b, c, d}) oder 0.
  @sa FindShadow(), FindShadowedObject, QueryInfo()
*/
public object* QueryObject(object ob) {

  if (!objectp(ob)) return 0;
 
  object *res = ({ob});
  object sh = FindShadow(ob);

  while(sh) {
    // es gibt einen Schatten, also Kette nach oben verfolgen.
    res = ({sh}) + res;
    sh = FindShadow(sh);
  }

  object vic = FindShadowedObject(ob);
  while(vic) {
    // es gibt ein beschattetes Objekt, Kette nach unten verfolgen.
    res = res + ({vic});
    vic = FindShadowedObject(vic);
  }

  if (sizeof(res) < 2) {
    // Offenbar wird ob weder beschattet noch beschattet selber.
    // Moeglicherweise wurde jedoch durch wilde Zerstoerung die Hierarchie
    // getrennt. Falls ob nen Schatten ist, laesst sich das reparieren. Falls
    // ob beschattet wird, hilft eigentlich nur ein reset(). Den moechte ich
    // hier aber nicht machen, weils u.U. teuer sein koennte.
    if (query_shadowing(ob)) {
      shadows[ob] = query_shadowing(ob);
      // nochmal.
      return QueryObject(ob);
    }
    // scheinbar nicht.
    return 0;
  }

  return res;
}

/** Gibt einen String mit Infos ueber ob zurueck.
  Ist ob ein beschattetes Objekt oder ein Schatten, wird ein beschreibender
  String zurueckgeliefert (schatten -> beschattetes Objekt). Hierbei wird ggf.
  die gesamte Beschattungshierarchie angegeben (a -> b -> c -> d).
  @param[in] object - ein Objekt.
  @return string - String mit Infos ueber ob.
  @sa FindShadow(), FindShadowedObject(), QueryObject()
*/
public string QueryInfo(object ob) {
  object *shs = QueryObject(ob);
  if (!shs) return 0;

  return CountUp(map(shs, #'object_name), " -> ", " -> ");
}

/** Registriert einen Schatten und sein Opfer.
  Registriert den Schatten und sein Opfer. Sollte ausschliesslich durch die
  simul_efun oder spare_simul_fun gerufen werden.
  @sa UnregisterShadow()
*/
public int RegisterShadow(object schatten) {
  object opfer;

  //DEBUG(sprintf("[%O] %O\n", schatten, caller_stack()));

  // Irgendein Sicherheitscheck ist eigentlich nicht noetig hier, da das Opfer
  // ohnehin per efun ermittelt wird und kein Eintrag erfolgt, wenn kein
  // beschattetes Objekt zu finden ist.

  // das von schatten beschattete Objekt ermitteln 
  if (objectp(opfer=query_shadowing(schatten))) {
    if (shadows[schatten] == opfer)
      return SHADOW_EXISTS;

    // Neueintrag oder ggf. auch Aendern.
    shadows[schatten] = opfer;
    //DEBUG(DumpShadows());
    return SHADOW_OK;
  }
  return SHADOW_ACCESS_DENIED;
}

/** Traegt einen Schatten wieder aus.
  Der Schatten wird wieder ausgetragen. Sollte ausschliesslich durch die
  simul_efun oder spare_simul_fun gerufen werden.
  Werden Schatten oder beschattete Objekte zerstoert ohne vorher die
  Schattierung zu beenden, fuehrt dies zu Inkonsistenzen und zerbrochenen
  Beschattungshierarchien.
  @sa RegisterShadow(), UnregisterOpfer()
*/
public int UnregisterShadow(object caller) {
  object schatten, opfer;

  // Ein Sicherheitscheck fuer den Aufruf ist eigentlich nicht noetig, da ein
  // Eintrag nur entfernt wird, wenn die Beschattung nachweislich beendet
  // wurde.

  //DEBUG(sprintf("[%O] %O\n", caller, caller_stack()));
  if (!objectp(caller)) return SHADOW_UNKNOWN;

  // Schatten und beschatteten aus den lokalen Daten ermitteln.
  if (member(shadows, caller)) {
    schatten = caller;
    opfer = shadows[schatten];
  }
  else if (objectp(schatten = FindShadow(caller))) {
    opfer = caller;
  }
  // wenn nicht bekannt, ist jetzt eh Ende.
  if (!schatten) return SHADOW_UNKNOWN;

  //DEBUG(sprintf("%O -> %O (%O, %O)\n",
  //      schatten, opfer, caller, query_shadowing(schatten)));

  // Schattierung wirklich beendet? Wenn nicht -> Ende
  if (opfer && query_shadowing(schatten) == opfer)
    return SHADOW_ACCESS_DENIED;

  // war schatten in einer Beschattungshierarchie?
  object up = FindShadow(schatten);
  if (up && query_shadowing(up) == opfer) {
    shadows[up] = opfer; // Kette neu verlinken
  }

  // jetzt kann geloescht werden.
  m_delete(shadows,schatten);
  return SHADOW_OK;
}

public int ResetAll() {
  if (!ARCH_SECURITY) return SHADOW_ACCESS_DENIED;

  DEBUG("ResetAll() called.\n");

  shadows = ([]);

  if (MEMORY->Save("Schatten", shadows) != 1)
    raise_error("Konnte Daten nicht im Memory ablegen.\n");

  return SHADOW_OK;
}

/** Raeumt die Daten ueber die Schatten auf.
    Zerstoeren sich beschattete Objekte, werden die Schatten nicht
    ausgetragen, daher wird das von zeit zu zeit hier gemacht.
*/
public void reset() {
  foreach(object schatten, object opfer: shadows) {
    if (!objectp(opfer)) {
      // war schatten evtl. in einer Hierarchie und beschattet jetzt was
      // anderes?
      if (query_shadowing(schatten))
        shadows[schatten] = query_shadowing(schatten);
      else
        m_delete(shadows, schatten);
    }
  }
}


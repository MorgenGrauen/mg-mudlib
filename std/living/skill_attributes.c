// MorgenGrauen MUDlib
//
// living/skills_attributes.c - Verwaltung der Skillattribute von Lebewesen
//
// $Id: skills.c 6673 2008-01-05 20:57:43Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/util/executer";

#define NEED_PROTOTYPES
#include <living/skill_attributes.h>
#include <player/life.h>
#include <thing/properties.h>
#undef NEED_PROTOTYPES
#include <thing/properties.h>
#include <properties.h>
#include <defines.h>

//#define ZDEBUG(x) if (find_player("zesstra")) tell_object(find_player("zesstra"),x)
#define ZDEBUG(x)
//#define SASETLOG(x) log_file("SASET.LOG", x, 250000)

//#define __DEBUG__

// Variable fuer die Skill-Attribute, Datenstruktur:
/*  ([ SA_ATTR: ({Summe_Stat_Modifier, Zeitpunkt, AnzahlModifier, });
              ([ ob1:value;duration,
                 ob2:value;duration, ...]);  // stat. Modifier
              ([ ob1:closure;duration,
                 ob2:closure;duration, ...])     // dyn. Modifier
             ,
     SA_ATTR2: ({...}); ([]); ([]),
   ]) */
private nosave mapping skillattrs;

protected void create() {
  Set(P_SKILL_ATTRIBUTES, SECURED, F_MODE_AS);
}

// von aussen Prop setzen ist nicht...
mapping _set_skill_attr(mapping sa) {
  return deep_copy(skillattrs);
}
// und auch beim Abfragen nur kopien liefern. ;-)
mapping _query_skill_attr() {
  return deep_copy(skillattrs);

//TODO: Evtl. ext. Setzen von P_SKILL_ATTRIBUTE_OFFSETS mitloggen?
}

private void UpdateSACache(string *attrs) {
#ifdef __DEBUG__
    if (getuid(this_object()) == "zesstra")
    ZDEBUG(sprintf("%O\n",skillattrs));
#endif
  if (!mappingp(skillattrs)) return;
  if (!pointerp(attrs) || sizeof(attrs))
    attrs = m_indices(skillattrs); // alle
  // sonst schnittmenge aus existierenden und den uebergebenen.
  attrs = m_indices(skillattrs) & attrs;

  // und jetzt ueber alle gewuenschten SAs drueber
  foreach(string attr : attrs) {
    int *cache = skillattrs[attr, SAM_CACHE];
    mapping stat = skillattrs[attr, SAM_STATIC];
    mapping dyn = skillattrs[attr, SAM_DYNAMIC];
    int sum = 0;
    int timeout = __INT_MAX__;
 
    // ueber stat. Mods iterieren und Aufsummieren, kleinsten Timeout
    // ermitteln.
    foreach(object ob, int value, int duration: stat) {
      // gueltige Mods aufaddieren, abgelaufene rauswerfen
      if (duration >= time()) {
        sum += value;
        if (duration < timeout)
          timeout = duration;
      }
      else
        m_delete(stat, ob); // ja, geht im foreach ;-)
    }
    // Ablaufzeiten der dyn. Mods pruefen, waere hier zwar nicht unbedingt
    // noetig sondern koennte man ausschliesslich im QuerySkillAttribute()
    // machen, aber dann waere die Ermittlung der Summe der Mods schwieriger.
    foreach(object ob, closure value, int duration: dyn) {
      if (duration < time())
        m_delete(dyn, ob); // ungueltig, weg damit.
    }
    // gesamtzahl Mods?
    cache[SAM_COUNT] = sizeof(stat) + sizeof(dyn);
    if (!cache[SAM_COUNT]) {
      // keine mods da, Submapping fuer dieses SA komplett loeschen.
      m_delete(skillattrs, attr);
      continue;
    }
    // sonst die anderen Cache-Werte setzen.
    cache[SAM_SUM] = sum;
    cache[SAM_CACHE_TIMEOUT] = timeout;
  }
  // wenn alle Mods geloescht wurden.
  if (!sizeof(skillattrs)) skillattrs=0;
#ifdef __DEBUG__
  if (getuid(this_object()) == "zesstra")
    ZDEBUG(sprintf("%O\n",skillattrs));
#endif
}

private int InternalModifySkillAttribute(object caster, string atrname,
                                mixed value, int duration) {
  int zeit = utime()[1];
  int ticks = get_eval_cost();

  // nur existierende SAs...
  if (!stringp(atrname)
      || member(VALID_SKILL_ATTRIBUTES, atrname) == -1)
    return SA_MOD_INVALID_ATTR;

  if (!objectp(caster)) return SA_MOD_INVALID_OBJECT;

  if (!mappingp(skillattrs)) skillattrs=m_allocate(1,3);
  
  if (!member(skillattrs, atrname)) {
    skillattrs[atrname,SAM_CACHE] = ({0, 0, 0});
    // die meisten Mods sind statisch, daher auf Verdacht hier fuer einen
    // Eintrag Platz reservieren, aber nicht fuer den dyn. Teil.
    skillattrs[atrname,SAM_STATIC] = m_allocate(1,2);
    skillattrs[atrname,SAM_DYNAMIC] = m_allocate(0,2);
  }
  // pruefen, ob Maximalzahl an Eintraegen drin ist
  else if (skillattrs[atrname,SAM_CACHE][SAM_COUNT] >= SAM_MAX_MODS) {
    // letzte Chance: destructete Objekte drin?
    skillattrs[atrname,SAM_CACHE][SAM_COUNT] = 
      sizeof(skillattrs[atrname,SAM_STATIC])
    +sizeof(skillattrs[atrname,SAM_DYNAMIC]);
    // es kann sein, dass noch abgelaufene Objekte drinstehen,
    // aber die Pruefung ist mir gerade zu teuer. TODO
    // nochmal gucken (rest vom cache wird ggf. unten geprueft.)
    if (skillattrs[atrname,SAM_CACHE][SAM_COUNT] >= SAM_MAX_MODS)
      return SA_TOO_MANY_MODS; // dann nicht.
  }

  // Dauer darf nur ein int sein.
  if (!intp(duration))
    raise_error(sprintf("Wrong argument 3 to ModifySkillAttribute: "
    "expected 'int', got %.10O\n", duration));
  // Zeitstempel ermitteln
  duration += time();

  // statischer oder dyn. Modifier?
  if (intp(value)) {
    // Mod darf nicht zu gross oder zu klein sein. TODO: Grenzen?
    if (value < -1000)
      return SA_MOD_TOO_SMALL;
    else if (value > 1000)
      return SA_MOD_TOO_BIG;
    else if (!value)
      return SA_MOD_INVALID_VALUE; 
    // jedes Objekt darf nur einen mod haben. Wenn dieses schon einen dyn.
    // hat, muss der geloescht werden (stat. werden ja eh ersetzt).
    if (member(skillattrs[atrname,SAM_DYNAMIC], caster))
      m_delete(skillattrs[atrname,SAM_DYNAMIC], caster);
    // sonst eintragen
    skillattrs[atrname, SAM_STATIC] += ([caster: value; duration]);
  }
  else if (closurep(value)) {
    // nur ein Mod pro Objekt, s.o.
    if (member(skillattrs[atrname,SAM_STATIC], caster))
      m_delete(skillattrs[atrname,SAM_STATIC], caster);
    // direkt ohne weitere Pruefung eintragen
    skillattrs[atrname, SAM_DYNAMIC] += ([caster: value; duration]);
  }
  else
    raise_error(sprintf("Wrong argument 2 to ModifySkillAttribute(): "
    "expected 'int' or 'closure', got %.10O\n",value));

#ifdef SASETLOG
  if (query_once_interactive(this_object()))
    SASETLOG(sprintf("%s: %O, %s, %O, %O\n", 
        strftime("%y%m%d-%H%M%S"), this_object(), atrname, caster, value));
#endif
#ifdef SASTATD
  object daemon;
  if (query_once_interactive(ME)
      && objectp(daemon=find_object(SASTATD)))
    daemon->LogModifier(caster, atrname, value, duration);
#endif
  // noch den Cache fuer dieses SA neu berechnen
  // TODO: Cache nur invalidieren, damit er erst bei der naechsten Abfrage
  // aktualisiert wird. Spart Zeit, wenn bis dahin mehrere Mods
  // entfernt/addiert werden. Dafuer ist die gecache Anzahl an Mods
  // inkonsistent.
  UpdateSACache( ({atrname}) );

  ZDEBUG(sprintf("MSA: %O, Zeit: %d, Ticks: %d\n",
	this_object(),
	utime()[1]-zeit, ticks-get_eval_cost()));

  return SA_MOD_OK;
}

public int ModifySkillAttribute(string atrname, mixed value, 
                                    int duration) {
  return InternalModifySkillAttribute(
      (extern_call()?previous_object():ME), atrname, value, duration);
}

public int RemoveSkillAttributeModifier(object caster, string attrname) {
  if (!stringp(attrname) || !mappingp(skillattrs) || !objectp(caster)
      || !member(skillattrs, attrname))
    return SA_MOD_NOT_FOUND;
  // TODO: Berechtigung pruefen. ;-)

  if (member(skillattrs[attrname, SAM_STATIC], caster)) {
    m_delete(skillattrs[attrname, SAM_STATIC], caster);
  }
  else if (member(skillattrs[attrname, SAM_DYNAMIC], caster)) {
    m_delete(skillattrs[attrname, SAM_DYNAMIC], caster);
  }
  else
    return SA_MOD_NOT_FOUND;
  
  // TODO: Cache nur invalidieren, damit er erst bei der naechsten Abfrage
  // aktualisiert wird. Spart Zeit, wenn bis dahin mehrere Mods
  // entfernt/addiert werden. Dafuer ist die gecache Anzahl an Mods
  // inkonsistent.
  UpdateSACache( ({attrname}) );

  return SA_MOD_REMOVED;
}

public int QuerySkillAttribute(string atrname)
{
  mixed offsets, attr;
  int modsumme, qual, ret, cval;

  if (!stringp(atrname)) // VALID_SKILL_ATTRIBUTES beruecksichtigen?
    return 100;

  // wenn nicht SA_QUALITY gefragt ist, erstmal jenes ermitteln, weil es den
  // ersten Modifier auf alle anderen SAs darstellt. Sonst den Startwert fuer
  // SA_QUALITY (100) modifiziert durch evtl. Todesfolgen ermitteln.
  if ( atrname != SA_QUALITY )
    qual = QuerySkillAttribute(SA_QUALITY);
  else
    // bei SA_QUALITY gehen die Todesfolgen ein
    qual = to_int(100 * pow(0.9, death_suffering()/10.0));

  // Die Offsets sind sozusagen der Basiswert der SAs. Als erstes verwursten,
  // sofern vorhanden, nen int drinsteht und der offset != 0 ist.
  if ( mappingp(offsets = Query(P_SKILL_ATTRIBUTE_OFFSETS))
       && intp(attr=offsets[atrname]) 
       && attr)
    ret = attr;
  else
    ret = 100;

  // wenn keine Mods gesetzt sind, wars das jetzt. ;-)
  if ( !mappingp(skillattrs)
       || !member(skillattrs, atrname) )
    return (ret*qual)/100;

  // wenn Cache der stat. Mods abgelaufen oder offenbar Objekte zerstoert
  // wurden, muss der Cache neu berechnet werden.
  if ( skillattrs[atrname,SAM_CACHE][SAM_CACHE_TIMEOUT] < time()
      || sizeof(skillattrs[atrname,SAM_STATIC])
         +sizeof(skillattrs[atrname,SAM_DYNAMIC]) !=
           skillattrs[atrname,SAM_CACHE][SAM_COUNT] )
  {
    UpdateSACache( ({atrname}) );
    // UpdateSACache() loescht uU das SA-Mapping oder Eintraege daraus.
    if ( !mappingp(skillattrs)
         || !member(skillattrs, atrname) )
    return (ret*qual)/100;
  }
  // Summe der statischen Mods.
  modsumme = skillattrs[atrname,SAM_CACHE][SAM_SUM];

  // TODO! Evtl. andere Daten als ME an die Funktion uebergeben
  // TODO! bereits nach Addition des Funktionsrueckgabewertes pruefen, ob der
  //       Wertebereich des SA-Modifiers ueberschritten ist, oder freien
  //       Wertebereich erlauben und erst am Ende deckeln (aktuelle Variante)
  // Dynamische Modifier auswerten
  foreach( object ob, closure cl, int duration:
           skillattrs[atrname,SAM_DYNAMIC] )
  {
    if ( duration > time()               // Noch nicht abgelaufen und
         && intp(cval=funcall(cl, ME)) ) // Funktion liefert int zurueck
      modsumme += cval;
    else {
      m_delete(skillattrs[atrname,SAM_DYNAMIC], ob);
      skillattrs[atrname,SAM_CACHE][SAM_COUNT]--;
    }
  }
  ret = ((ret+modsumme)*qual)/100;
  if ( ret < 10 )
    ret = 10;
  else if ( ret > 1000 )
    ret = 1000;

  return ret;
}

public varargs mapping QuerySkillAttributeModifier(object caster, 
                           string *attrnames) {
  
  // auf abgelaufene Modifikatoren pruefen
  if (!pointerp(attrnames))
    UpdateSACache( ({}) );
  else
    UpdateSACache(attrnames);

  if (!mappingp(skillattrs))
    return ([]);
  if (!pointerp(attrnames) || !sizeof(attrnames))
    attrnames = m_indices(skillattrs); // alle durchsuchen
  else // schnittmenge der gew. und vorhandenen bilden
    attrnames = m_indices(skillattrs) & attrnames;
  
  mapping res=m_allocate(sizeof(attrnames), 1);

  foreach(string atr: attrnames) {
    res[atr] = m_allocate(5, 2); // mal fuer 5 Werte Platz reservieren
    if (!objectp(caster)) {
      // wenn kein bestimmter caster angefragt ist, alle mods liefern
      foreach(object c, int value, int dur: skillattrs[atr, SAM_STATIC]) {
	res[atr] += ([c: value; dur]);
      }
      foreach(object c, closure value, int dur: skillattrs[atr, SAM_DYNAMIC]) {
	res[atr] += ([c: value; dur]);
      }
    }
    else {
      // sonst nur den Mod von caster
      if (member(skillattrs[atr, SAM_STATIC], caster)) {
	res[atr] += ([caster: 
			 skillattrs[atr, SAM_STATIC][caster, SAM_VALUE];
	                 skillattrs[atr, SAM_STATIC][caster, SAM_DURATION]
		    ]);
      }
      else if (member(skillattrs[atr, SAM_DYNAMIC], caster)) {
	res[atr] += ([caster:
			 skillattrs[atr, SAM_DYNAMIC][caster, SAM_VALUE];
	                 skillattrs[atr, SAM_DYNAMIC][caster, SAM_DURATION]
		    ]);
      }
    }
  }
  return res;
}

// Kompatibilitaetsfunktion mit altem Interface. Ist nur ein Wrapper, der
// value umrechnet und 'alte' Rueckgabewerte liefert.

public varargs int ModifySkillAttributeOld(object caster, string atrname,
                          int value, int duration, mixed fun) {
  int res;
  // Caller ermitteln
  if (extern_call()) caster=previous_object();
  else caster=ME;

  // Closures koennen via ModifySkillAttributeOld() nicht mehr gesetzt werden,
  // da deren Rueckgabewert nicht sinnvoll umgerechnet werden koennen. (Man
  // weiss nicht, ob es eine neue oder alte Closure ist.)
  if (pointerp(fun) || closurep(fun))
      raise_error(sprintf("Closures for SA modifiers can't be set by "
      "ModifySkillAttributeOld()! Use ModifySkillAttribute()!\n"));
  
  res = InternalModifySkillAttribute(caster, atrname, value-100, duration);
  // die alte funktion hatte nur 0 fuer ungueltigen Wert und < 0 fuer zu
  // kleines Level als Rueckgabewert. Zu kleines Level gibt nicht mehr, also
  // bleibt nur 0 als Sammel-Fehlercode uebrig. *seufz*
  if (res < 0) return 0;
  return res;
}


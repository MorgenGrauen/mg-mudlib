// MorgenGrauen MUDlib
//
// armour/combat.c -- armour standard object
//
// $Id: combat.c 9092 2015-01-19 23:57:50Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/commands.h>
#include <thing/description.h>
#include <config.h>
#include <combat.h>
#include <language.h>
#include <defines.h>
#include <new_skills.h>


// Globale Variablen
private nosave closure defend_cl;
private nosave mapping resistance_strengths=([]);

void create() {
    // Einige Grundwerte setzen
    Set(P_ARMOUR_TYPE, AT_ILLEGAL, F_VALUE);
    Set(P_LAST_USE,time(),F_VALUE);
}

// Die Funktion, die den Schutzwert berechnet, den die Ruestung bietet
int QueryDefend(string|string* dam_type, int|mapping spell, object enemy)
{
    int     prot;
    mixed   def_func;
    object  pl;
    // Zum Cachen von spell[EINFO_DEFEND], haeufiges Lookup aus dem Mapping
    // koennte unnoetig Zeit kosten.
    mapping einfo;

    // AT_MISC-Ruetungen schuetzen normalerweise gar nicht...
    if (QueryProp(P_ARMOUR_TYPE)==AT_MISC) {
        // es sei denn, sie haben eine spezielle DefendFunc
        if (!closurep(defend_cl)) return(0);
    }
    else {
        // ansonsten Ruestungsschutz ermitteln und in EINFO_DEFEND vermerken.
        // (Beides fuer AT_MISC in jedem Fall unnoetig)

        // Ruestungen schuetzen nur gegen physikalischen Schaden
        if ((!spell || (mappingp(spell) && spell[SP_PHYSICAL_ATTACK]))
            && sizeof(filter(dam_type,PHYSICAL_DAMAGE_TYPES)))
        { 
          // Schutz bestimmen, Minimum 1, aber nur wenn P_AC > 0
          int pac = QueryProp(P_AC);
          if (pac > 0)
            prot = (pac/4 + random(pac*3/4 + 1)) || 1 ;
          object stat = find_object("/d/erzmagier/zesstra/pacstat");
          if (stat)
            stat->acstat(QueryProp(P_ARMOUR_TYPE),prot,
                         random(pac)+1);

          // ruestungschutz an defendfunc weitermelden
          if (mappingp(spell) &&
              mappingp(einfo=spell[EINFO_DEFEND])) {
              // Schutz d. akt. Ruestung vermerken.
              einfo[DEFEND_CUR_ARMOUR_PROT]=prot;
              // daten der Ruestung vermerken.
              if (mappingp(einfo[DEFEND_ARMOURS])) {
                einfo[DEFEND_ARMOURS][ME,DEF_ARMOUR_PROT]=prot;
              }
          } // ende vom if (mapping(spell) ...)
        } // ende vom if (phys Schaden)

    } // ende vom else (wenn kein AT_MISC) 

    // Ist eine DefendFunc gesetzt, wird diese ausgewertet
    if (closurep(defend_cl)) {
        if (!objectp(get_type_info(defend_cl,2))) {
            // Closure gesetzt, aber nicht gueltig, schauen, ob wir sie neu
            // erstellen koennen.
            if (objectp(def_func=QueryProp(P_DEFEND_FUNC))) {
                defend_cl=symbol_function("DefendFunc",def_func);
            }
            // sonst loeschen, um spaeter diesen Zweig ganz zu sparen.
            else defend_cl=0;
        }
        // BTW: Es ist ok, wenn defend_cl jetzt 0 ist, dann liefert funcall()
        // auch 0.
        // Bei Netztoten keine (zurueckschlagende) DefendFunc
        if (objectp(pl=QueryProp(P_WORN)) && (!query_once_interactive(pl) ||
                interactive(pl)) ) {
          // Der Rueckgabewert der DefendFunc wird zum Schutz addiert
          prot += funcall(defend_cl, dam_type, spell, enemy);
          // leider kann die DefendFunc jetzt auch noch das Objekt zerstoert
          // haben...
          if (!objectp(this_object()))
            return prot;
        }
    }

    // Zeitpunkt der letzten Benutzung ausgeben
    SetProp(P_LAST_USE,time());

    // Berechneten Schutz zurueckgeben
    return prot;
}

// Es duerfen nur "legale" Ruestungstypen gesetzt werden, ansonsten
// wird AT_ILLEGAL gesetzt.
static mixed _set_armour_type(mixed type ) {
    if (!COMBAT_MASTER->valid_armour_type(type))
    {
        Set(P_ARMOUR_TYPE, (type=AT_ILLEGAL), F_VALUE);
    }
    else
    {
        Set(P_ARMOUR_TYPE, type);
    }
    AddId(type);

    resistance_strengths=([]);
    return type;
}


// Wird etwas an P_DEFEND_FUNC geaendert, muss die zugehoerige closure
// neu erstellt werden.
static object _set_defend_func(object arg) {
  if (objectp(arg) &&
      closurep(defend_cl=symbol_function("DefendFunc",arg))) {  
    return Set(P_DEFEND_FUNC, arg, F_VALUE);
  }
  defend_cl=0;
  return(Set(P_DEFEND_FUNC, 0, F_VALUE));
}

// Auch Ruestungen koennen einen Schadenstyp haben. Dieser kann als string
// oder array angegeben werden, wird aber intern auf jeden Fall als
// array gespeichert.
static mixed _set_dam_type(mixed arg) {
    if (pointerp(arg))
    {
        return Set(P_DAM_TYPE, arg, F_VALUE);
    }
    else if (stringp(arg))
    {
        return Set(P_DAM_TYPE, ({ arg }), F_VALUE);
    }
    return Query(P_DAM_TYPE, F_VALUE);
}

// Ruestungen koennen Resistenzen setzen. Diese werden jedoch nicht wie
// "normale" Resistenzen gesetzt, sondern als Prozentwerte der fuer diesen
// Typ maximal zulaesigen Resistenz. Die Aenderung der Resistenzen darf
// nur durch die Ruestung selbst erfolgen.
// Beispiel: ([DT_FIRE: 100, DT_WATER: -150])
// max. Feuerresistenz, 1.5fache Anfaelligkeit
static mixed _set_resistance_strengths(mixed resmap) {
    float  max_res;
    object worn_by;

    // Es duerfen nur mappings angegeben werden
    if (!mappingp(resmap))
    {
        return -1;
    }

    // die Maxwerte muessen jeweils durch -100 geteilt sein, da hinterher
    // mit der Prozentzahl multipliziert wird und die angabe der Vorzeichen
    // hier umgekehrt wie bei den "normalen" Resistenzen ist. Der
    // Maximalwert ist vom Ruestungstyp abhaengig.
    switch (QueryProp(P_ARMOUR_TYPE))
    {
        case AT_CLOAK  :
        case AT_RING   :
        case AT_AMULET : max_res=-0.0010;
                         break;
        case AT_SHIELD :
        case AT_ARMOUR : max_res=-0.0015;
                         break;
        default        : max_res=-0.0005;
    }

    // Resistenz-Mapping aktualisieren
    resistance_strengths=([]);
    foreach(string damtype, int res: resmap)
    {
        if (!intp(res)) res=to_int(res);
        // Mehr als 100 Prozent ist nicht erlaubt
        if (res>100)
        {
            res=100;
        }
        else if (res<0)
        {
             res=(res/4)*5;
             // Man kann auch nicht beliebig negativ werden
             if (res<-1000)
             {
                 res=-1000;
             }
        }
        // Der Resistenzwert berechnet sich aus dem Produkt des
        // Maximalwertes und der Prozentzahl
        resistance_strengths[damtype]=res*max_res;
    }

    // Werden die Resistenzen bei einer getragenen Ruestung geaendert,
    // muss der Traeger davon natuerlich beeinflusst werden.
    if (objectp(worn_by=QueryProp(P_WORN)))
    {
        worn_by->AddResistanceModifier(resistance_strengths,
                                       QueryProp(P_ARMOUR_TYPE));
    }
   return resistance_strengths;
}

// Bei einem QueryProp(P_RESISTANCE_STRENGTHS) soll das aktuelle
// Resistenzen-Mapping zurueckgegeben werden
static mapping _query_resistance_strengths() {
  return (resistance_strengths||([]));
}


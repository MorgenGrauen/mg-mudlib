/*  Paracelsus: Eine Krankheit
**
**      Beispiel fuer die Verwendung von P_X_ATTR_MOD und P_X_HEALTH_MOD
**
**      Diese Krankheit setzt alle Attribute herab und erniedrigt
**      zusaetzlich P_MAX_HP und P_MAX_SP um jew. 20 (zusaetzlich zu den
**      Auswirkungen der Attributsenkungen).
**      Gestartet wird die Krankheit, indem sie einfach in das Opfer
**      bewegt wird.
**      Man beachte, dass im remove() die P_X_*-Properties ein leeres
**      Mapping zugewiesen bekommen und keine 0. Nur so werden die
**      Krankheitsfolgen rueckgaengig gemacht.
*/

// Diese Pragmas sorgen dafuer, dass der Driver darauf achtet, dass bei
// Funktionsargumenten, -Rueckgabewerten und teilweise bei Zuweisung von
// Werten an Variablen der richtige Datentyp verwendet wird (z.b. kein string
// anstelle eines int verwendet wird). Sollte in keinem Objekt fehlen.
#pragma strong_types, save_types, rtt_checks

inherit "/std/thing";

#include <properties.h>
#include <moving.h>
#include <language.h>
#include <class.h>

protected void create()
{
    ::create();

    // ausnahmsweise darf dieses Objekt mal kein P_SHORT+P_LONG haben...
    SetProp(P_SHORT,0);
    SetProp(P_LONG, 0);
    SetProp(P_INVIS, 1); // unsichtbar machen.
    SetProp(P_NAME,"Krankheit");
    SetProp(P_GENDER,FEMALE);
    // wichtig, sonst wiegt die Krankheit 1kg (Standardgewicht fuer thing) 
    SetProp(P_WEIGHT,0);
    SetProp(P_VALUE,0);
    SetProp(P_MATERIAL,MAT_MISC_MAGIC);
    SetProp(P_NODROP,1); // Nicht wegwerfbar.

// ----> Dieser Abschnitt sorgt fuer fiese Statabzuege

    SetProp(P_X_HEALTH_MOD,
    ([
        P_HP : -20,   // Max. Lebenspunkte um 20 runter
        P_SP : -20    // Max. Konzentrationspunkte um 20 runter
    ]) );
    SetProp(P_X_ATTR_MOD,
    ([
        A_CON : -1,   // Ausdauer um 1 runter, reduziert auch max. LP!
        A_DEX : -1,   // Geschicklichkeit um 1 runter
        A_INT : -2,   // Intelligenz um 2 runter, reduziert auch max. KP!
        A_STR : -4,   // Staerke um 4 runter
    ]) );

// <----

    AddId( ({"PARA\nSICK\nEXAMPLE"}) );

    AddClass(CL_DISEASE); // Damit Kleriker helfen koennen.
    SetProp(P_LEVEL,15);  // Aber nicht ganz so einfach zu entfernen
}


/* Paracelsus: Lebensring
**
**    Ein Beispiel fuer die Anwendung von P_M_HEALTH_MOD
**
**    Zieht ein Spieler diesen Ring an, so erhoehen sich seine maximalen
**    Lebenspunkte dabei um 10, waehrend seine max. Konzentrationspunkte
**    um 5 erniedrigt werden.
*/

// Diese Pragmas sorgen dafuer, dass der Driver darauf achtet, dass bei
// Funktionsargumenten, -Rueckgabewerten und teilweise bei Zuweisung von
// Werten an Variablen der richtige Datentyp verwendet wird (z.b. kein string
// anstelle eines int verwendet wird). Sollte in keinem Objekt fehlen.
#pragma strong_types, save_types, rtt_checks

inherit "/std/armour";

#include <properties.h>
#include <armour.h>
#include <language.h>
#include <combat.h>

protected void create()
{
    ::create();

    SetProp(P_SHORT,"Ein Lebensring");
    SetProp(P_LONG,
        "Ein kleiner Ring aus einem seltsamen, gruenen Material.\n");
    SetProp(P_INFO,
        "Dieser Ring unterstuetzt die Gesundheit.\n"+
        "Dabei schwaecht er leider den Geist.\n");
    SetProp(P_NAME,"Lebensring");
    SetProp(P_GENDER,MALE);
    SetProp(P_WEIGHT,80);
    SetProp(P_VALUE,2000+random(501));
    SetProp(P_ARMOUR_TYPE,AT_RING);
    SetProp(P_AC,0); // keine Schutzwirkung im Kampf
    SetProp(P_MATERIAL,MAT_MISC_MAGIC);

// ---->

    SetProp(P_M_HEALTH_MOD,
    ([
        P_HP : 10,      // Max. LP um 10 erhoehen
        P_SP : -5       // Max. KP um 5 verringern
    ]) );

// <----

    AddId( ({"ring","lebensring"}) );

    AddDetail( "material",
        "Es ist gruen.\n");
}


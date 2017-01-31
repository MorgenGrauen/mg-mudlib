P_EFFECTIVE_AC
==============

NAME
----
::

     P_EFFECTIVE_AC      "effective_ac"

DEFINIERT IN
------------
::

     <combat.h>

BESCHREIBUNG
------------
::

     Diese Property kommt sowohl in Ruestungen als auch in Waffen, die
     schuetzen sollen, zum Einsatz.

     In Ruestungen kann hier der Effektivwert der Ruestungsklasse vermerkt
     werden, wenn diese noch durch eine DefendFunc() modifiziert wird
     (soweit sich ein solcher Effektivwert angeben laesst).

     In einigen Gilden koennen Waffen auch als Ruestung eingesetzt werden
     (z.B. beim Parieren eines Angriffs). In dieser Property kann man die
     Ruestungsklasse eintragen, die die Waffe bei solchen Aktionen aufweisen
     soll. Dabei ist man an die ueblichen Beschraenkungen der
     Ruestungsklasse gebunden! (s. /sys/combat.h)

BERMERKUNGEN
------------
::

     Das Kaempferspellbook verwendet fuer Paraden etc. P_EFFECTIVE_AC anstatt
     P_AC, wenn verfuegbar.

BEISPIELE
---------
::

     * DefendFuncs: 
       Der doppelte Mittelwert der DefendFunc wird zur Basis-AC dazuaddiert,
       da sich der 'Schutzwert = random(Basis-AC) + absolut(DefendFunc-Wert)'
       berechnet.

     // #1 Eine Ruestung mit P_AC von 35 und randomisierter DefendFunc
        SetProp(P_AC, 35);
        SetProp(P_DEFEND_FUNC, this_object());

        int DefendFunc(...) {
          return random(20);                       // Mittelwert: 10
        }
        -> SetProp(P_EFFECTIVE_AC, 55);            // 35 + 2*10 = 55

     // #2 Eine Ruestung mit P_AC von 35 und teilrandomisierter DefendFunc
        SetProp(P_AC, 35);
        SetProp(P_DEFEND_FUNC, this_object());

        int DefendFunc(...) {
          return 20 + random(10);                  // Mittelwert: 20 + 5
        }
        -> SetProp(P_EFFECTIVE_AC, 85);            // 35 + 2*(20+5) = 85

     * Sonderfunktion im Kontext der Kaempfergilde:
       Auch wenn der eigentliche AC-Wert durch keine DefendFunc oAe
       modifiziert wird, sind abweichende Werte in P_EFFECTIVE_AC zB in der
       Kaempfergilde fuer Paraden oder aehnliches sinnvoll. Maximalwert ist
       dafuer der doppelte Wert des Basis-AC-Wertes.

     // #3 Ein schon sehr gutes Schild, welches bei der Schildparade aber
     //    noch besser schuetzen soll.
        SetProp(P_ARMOUR_TYPE, AT_SHIELD);
        SetProp(P_AC, 38);
        SetProp(P_EFFECTIVE_AC, 55);

     // #4 Ein sehr labbriges Schild schuetzt zwar gegen normale Schlaege,
     //    ist zum Parieren aber irgendwie ungeeignet weil unkontrollierbar.
        SetProp(P_ARMOUR_TYPE, AT_SHIELD);
        SetProp(P_AC, 38);
        SetProp(P_EFFECTIVE_AC, 20);

     * Waffen:
       P_EFFECTIVE_AC wird im Kaempferspellbook als Bonus dazugezaehlt! Daher
       sollten gute Parierwaffen auch einen niedrigeren P_WC-Wert haben.
       Reine Parierwaffen duerfen den maximalen AC-Wert von Schilden als
       Maximum gesetzt haben - die Balance klaert ggf, ob das auch auf den
       Gildenparierwert anwendbar ist.

     // #5 Eine maessige Hellebarde/Axtwaffe mit Parierhaken.
        SetProp(P_WEAPON_TYPE, WT_AXE);
        SetProp(P_WC, 100);
        SetProp(P_EFFECTIVE_AC, 25);

SIEHE AUCH
----------
::

     Waffen:     P_WC, P_TOTAL_WC, P_EFFECTIVE_WC, HitFunc()
     Ruestungen: P_AC, P_TOTAL_AC, DefendFunc()
     Files:      /std/weapon.c, /std/weapon/combat.c
     Balance:    waffen, ruestungen, properties, kaempferboni

6. Nov 2011 Gloinson


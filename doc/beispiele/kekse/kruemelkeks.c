/* Einfaches Beispiel zur Demonstration von Vererbung.
   ACHTUNG: Dieses Beispiel stellt _kein_ sinnvolles Lebensmittelobjekt dar.
            Hierfuer sollte /std/food verwendet werden, siehe auch
            /doc/beispiele/food/
*/

#include <properties.h>
inherit __DIR__"keks";

varargs int remove(int silent) {
  if(!silent && living(environment()))
    tell_object(environment(), Name(WER,1)+
                               " kruemelt ein letztes Mal.\n");
  return ::remove(silent);
}

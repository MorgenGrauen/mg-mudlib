/* Dieses Objekt dient zum Faken von Namen fuer den Channeld.
 * Hintergrund: der Channeld braucht beim send() ein Objekt (bzw. die in der
 * Verarbeitung dann folgenden Programme). Der Mudlibmaster sendet Meldung in
 * anderer Leute Namen (der darf das auch). Dafuer clont und konfiguriert er
 * dieses Objekt und uebergibt es an den channeld. Nach 3s raeumen sich Clone
 * dieses Objektes wieder auf.
 * Falls der Spieler/NPC existiert, wenn die meldung gesendet wird, wird vom
 * Master das richtige Objekt uebergeben, nicht dieses hier.
 */

#include <config.h>
#include <wizlevels.h>

inherit "/std/secure_thing";

// Envcheck-Mechanismus missbrauchen. ;-)
protected void check_for_environment(string cloner)
{
  if (clonep(this_object()))
    remove(1);
}

// Zur Sicherheit auch im Reset
void reset()
{
  if (clonep(this_object()))
    remove(1);
}

// Kopie aus /std/thing/properties.c. Man braucht davon wohl kein
// zugaengliches in jedem thing...
// Welche externen Objekte duerfen zugreifen?
nomask private int allowed()
{
    if ( (previous_object() && IS_ARCH(getuid(previous_object())) &&
          this_interactive() && IS_ARCH(this_interactive())) ||
         (previous_object() && getuid(previous_object()) == ROOTID &&
          geteuid(previous_object()) == ROOTID) )
        return 1;
    return 0;
}

// Sollte nur von ROOT oder EM+ manipuliert werden.
// Sprich, das tut so, als seien alle Props SECURED
public varargs mixed Set( string name, mixed value, int type, int extern )
{
  if ((extern || extern_call())
      && previous_object() != this_object()
      && !allowed()) // aus thing/properties.c
    return -1;

  return ::Set(name, value, type, extern);
}


inherit "std/room";
#include <properties.h>
#include <wizlevels.h>

void create()
{
  ::create();
  SetProp(P_LIGHT, 1 );
  SetProp(P_INT_SHORT, "Der Muellraum" );
  SetProp(P_INDOORS, 1);
  SetProp(P_NEVER_CLEAN, 1);
  SetProp(P_INT_LONG,break_string(
    "Dieser Raum ist vollkommen leer und anscheinend riiiiiiesig gross. "
    "Du kannst seine Aussmasse nichtmals abschaetzen. Naja. Muss wohl auch "
    "so sein, denn hier kommen alle Clones rein, die einige Sekunden "
    "nach ihrer Erstellung noch kein Zuhause (Environment) gefunden haben. "
    "Wenn Du das magische Kommando 'welt' gibst, wirst Du wieder in Deine Welt\n"
    "zurueckversetzt werden."));
  AddExit("welt", "/gilden/abenteurer");
}

// rekursiv zerstoeren
private void rec_remove(object ob) {
  object *inv=all_inventory(ob);
  if (sizeof(inv)) {
    filter(inv, #'rec_remove);
  }
  ob->remove(1);
  if (objectp(ob)) destruct(ob);
}

int clean_me() {
  object *inv=all_inventory(this_object());
  int isize=sizeof(inv);
  if (isize>100) {
    //aeltestes Objekt, was kein Spieler ist, zerstoeren. Pruefung auf Spieler
    //ist Paranoia, eigentlich sollte hier nie einer hinkommen koennen, ausser
    //magier, und um die waers dann nicht schade. *g*
    for (isize--; isize--; ) {
      if (!query_once_interactive(inv[isize])) {
	rec_remove(inv[isize]);
	return(1);
      }
    }
  }
  return 0;
}

varargs int PreventInsert(object pl) {

  if (!objectp(pl))
    return 1;
  clean_me();
  return 0;
}

varargs int PreventInsertLiving(object pl) {

  // keine Spieler. ;-)
  if (!objectp(pl) || 
      (query_once_interactive(pl) && !IS_LEARNER(pl)))
    return 1;

  clean_me();
  return 0;
}


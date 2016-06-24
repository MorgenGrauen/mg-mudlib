#pragma strong_types,rtt_checks

inherit "/std/room";
inherit "/std/hook_consumer";

#define OFFICE "/d/wueste/catweazle/room/office"

#include <properties.h>
#include <wizlevels.h>
#include <hook.h>

protected void create()
{
  ::create();
  SetProp(P_LIGHT, 1 );
  SetProp(P_INT_SHORT,"Eine Gefaengniszelle");
  SetProp(P_INT_LONG, break_string(
      "Die Zelle ist absolut eintoenig und hat keine Fenster. Lange hier drin"
      "zu sitzen ist bestimmt nicht angenehm. Wenn du hier heraus willst, "
      "kannst Du wohl nur den Sheriff, seinen Stellvertreter oder einen "
      "Erzmagier darum bitten.",78));
  SetProp(P_INDOORS,1);
}

void init()
{
  ::init();
  if (!query_once_interactive(this_player()))
    return;
  if ( IS_DEPUTY(this_player()) ) // EM+ sind sowieso Deputies
    return;
  add_action("bla","",1);
  input_to("murks",0);

  // Move-Hook anhaengen
  int ret = this_player()->HRegisterToHook(H_HOOK_MOVE, this_object(), 
                                 H_HOOK_LIBPRIO(1),H_HOOK_SURVEYOR,
				 -1);
  if ( ret <=0 )
      raise_error("Fehler: Move-Hook konnte nicht registriert werden. "
                  "HRegisterToHook-Ergebnis: " + ret + "\n");
}

// Move-Hook austragen.
void exit(object liv) {
  if (objectp(liv) && !IS_DEPUTY(liv))
    liv->HUnregisterFromHook(H_HOOK_MOVE,this_object());
}

int bla()
{
  string v;

  v=query_verb();
  input_to("murks",0);
  // sagen und schlafen erlaubt.
  if (stringp(v) && (v=="sag"||v=="sage"||v[0]=='\''
        ||v=="schlafe"||v=="schlaf"))
      return 0;
  write("Nix da.\n");
  return 1;
}

int murks(string str)
{
  if (!this_player() || environment(this_player())!=this_object()) 
      return 0;

  input_to("murks",0);

  if (!str||str=="") return 1;
  // Einschlafen erlaubt.
  if (str == "schlafe ein") {
      this_player()->command("schlafe ein");
      return 1;
  }
  if (str[0]=='\'') str="sag "+str[1..];
  if(str[0..3]=="sag "||str[0..4]=="sage ")
  {
       str=implode(old_explode(str," ")[1..]," ");
     write(break_string(str, 78, "Du sagst: "));
     tell_room(this_object(),break_string(str,78,
         capitalize(this_player()->name())+" sagt: "),({this_player()}));
     return 1;
  }
  write("Nix da.\n");
  return 1;
}

public varargs int remove(int silent) {
  // keine Zerstoerung, wenn Spieler drin sind.
  if (sizeof(filter(all_inventory(),#'query_once_interactive)))
    return 0;

  return ::remove(1);
}

public string NotifyDestruct(object caller) {

  if (previous_object() != master()
      || caller == this_object())
    return 0;

  // wenn keiner hier ist, ists egal.
  if (!sizeof(filter(all_inventory(),#'query_once_interactive)))
    return 0;

  // Direkter Destruct mit Anwesenden nur fuer EM+
  if (!process_call() && this_interactive()
      && IS_ARCH(this_interactive()))
    return 0;
      
  return "Direkte Zerstoerung des Jails nur fuer EM+ erlaubt.\n";
}


// keine anderen Move-Hooks erlaubt ausser diesem...
status HookRegistrationCallback(object registringObject, int hookid, object
    hookSource, int registringObjectsPriority, int registringObjectsType)
{
    if (hookid==H_HOOK_MOVE)
      return 0;

    return 1;
}
status HookCancelAllowanceCallback(object cancellingObject, int hookid, object
    hookSource, int cancellingObjectsPriority, mixed hookData)
{
    if (hookid==H_HOOK_MOVE && cancellingObject != this_object()
        && present(hookSource,this_object()))
      return 0;

    return 1;
}

status HookModificationAllowanceCallback(object modifyingObject, int hookid,
    object hookSource, int modifyingObjectsPriority, mixed hookData)
{
    if (hookid==H_HOOK_MOVE && modifyingObject != this_object()
        && present(hookSource,this_object()))
      return 0;

    return 1;
}

mixed HookCallback(object hookSource, int hookid, mixed hookData)
{
  // nur move hooks sind interessant hier.
  if (hookid != H_HOOK_MOVE)
    return ({H_NO_MOD, hookData});

  // das duerfte eigentlich nicht vorkommen, da der Hook im exit() geloescht
  // wird...
  if (environment(hookSource) != this_object()) {
    hookSource->HUnregisterFromHook(H_HOOK_MOVE,this_object());
    return ({H_NO_MOD, hookData});
  }

  // Bewegungen in den Netztotenraum sind ok.
  if (!interactive(hookSource)
      && pointerp(hookData) && sizeof(hookData) >= 1)
  {
    if ((objectp(hookData[0])
          && object_name(hookData[0]) == "/room/netztot")
        || (stringp(hookData[0]) && hookData[0]=="/room/netztot")
        )
    {
      return ({H_NO_MOD, hookData});
    }
  }

  // Deputy (oder hoeher), kein process_call(). Sonst wird die Bewegung
  // abgebrochen.
  if (!this_interactive() || !IS_DEPUTY(this_interactive()))
  {
      return ({H_CANCELLED, hookData});
  }

  return ({H_NO_MOD, hookData});
}

// wenn hier jemand durch Zerstoerung des Objektes rauskommen will, geht das
// schief.
varargs void NotifyRemove(object ob) {
  if (objectp(ob) && query_once_interactive(ob) && !IS_DEPUTY(ob))
  {
    ob->SetProp(P_START_HOME,"/room/jail");
    ob->save_me(0);
  }
}


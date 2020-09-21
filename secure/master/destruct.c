// MorgenGrauen MUDlib
//
// secure/master/destruct.inc -- module of the master object: stuff for destruct.
//
// $Id: master.c 7041 2008-10-13 18:18:27Z Zesstra $

#include "/sys/object_info.h"

// privilegierte Objekte, die das destruct() abbrechen duerfen (root objekte
// duerfen auch ohne, dass sie in dieser Liste erfasst sind):
private nosave string *deny_destruct_list = ({
    "/obj/shut", "/room/void", "/room/netztot", "/room/jail" });

// Helferfunktion fuer prepare_destruct()
private void recursive_remove(object ob, int immediate_destruct) {

  if (efun::object_info(ob, OI_ONCE_INTERACTIVE)) {
    // Spieler werden ins Void bewegt.
    int res;
    tell_object(ob, "Ploetzlich loest sich deine Welt in ihre " +
                  "Bestandteile auf. Zum Glueck wirst\nDu irgendwo " +
                  "hin geschleudert ...\n");
    // wenn Bewegung buggt oder nicht funktioniert und ob noch existiert,
    // rekursiv zerstoeren.
    object oldenv=environment(ob);
    if ( (catch(res=({int})ob->move("/room/void",M_TPORT|M_NOCHECK,0,"faellt");
           publish) || (ob && environment(ob) == oldenv) )
          && ob) {
            // Spieler speichern, dann erst Inventar entleeren, dann remove() und
        // und destruct() anwenden.
        catch(({void})ob->save_me(1); publish);
        filter(all_inventory(ob), #'recursive_remove, immediate_destruct);
        if (!immediate_destruct) 
          catch(({int})ob->remove(0);publish);
        if (ob) 
          destruct(ob);
    }
  }
  else {
    // kein Spieler. Rekursiv entfernen. Hierbei _zuerst_ rekursiv das
    // Inventar entfernen und dann ob selber, damit nicht erst das Inventar in
    // das Environment bewegt wird (soll ja eh zerstoert werden).
    filter(all_inventory(ob), #'recursive_remove, immediate_destruct);
    // ggf. zuerst remove versuchen
    if (!immediate_destruct)
      catch(({int})ob->remove(1);publish);
    if (ob)
      destruct(ob);
  }
}

// Zerstoerung von ob vorbereiten
protected mixed prepare_destruct(object ob)
{
  object old_env,env,item;
  int|string res;

  // zuerst das notify_destruct() rufen und ggf. abbrechen, falls ob
  // privilegiert ist.
  catch(res = ({int|string})ob->NotifyDestruct(previous_object()); publish);
  if (res &&
      (getuid(ob) == ROOTID ||
       (IS_ARCH(ob)) ||
       member(deny_destruct_list, object_name(ob)) >= 0)) {
    if (stringp(res) && sizeof(res))
      return res;
    else
      return sprintf("%O verweigert die Zerstoerung mittels destruct(). "
          "Fehlende Rechte von %O?\n",ob, previous_object());
  }

#if (__VERSION_MICRO__*100 + __VERSION_MINOR__ *10000 + __VERSION_MAJOR__ \
     * 1000000) <= 3060300
  // Workaround fuer Driver-Speicherleck in 3.6.2 und 3.6.3: vor dem
  // Zerstoeren das Encoding wechseln, damit der driver das Handle auf iconv
  // schliesst.
  if(interactive(ob))
    configure_interactive(ob, IC_ENCODING, "UTF-8");
#endif

  env = environment(ob);

  // Objekt hat kein Env: Alles zerstoeren, Spieler ins Void
  if (!env) {
    filter(all_inventory(ob), #'recursive_remove, 1);
  }
  else {
    // Ansonsten alles ins Environment 
    foreach(item : all_inventory(ob))
    {
      old_env=environment(item);
      // M_MOVE_ALL, falls item nen Unitobjekt ist. Sonst clonen die u.U. noch
      // wieder nen neues Objekt im alten Env.
      if(catch(({int})item->move(env, M_NOCHECK|M_MOVE_ALL);publish))
        recursive_remove(item, 1);
      else if (item && environment(item) == old_env)
        recursive_remove(item, 1);
    }
  }

  return 0; // Erfolg
}

// Anmerkung: liefert 0 zurueck, wenn die sefuns gerade geladen werden.
string NotifyDestruct(object caller) {
  // Nicht jeder Magier muss den Master entsorgen koennen.
  if ((caller != this_object() && 
        call_sefun("secure_level") < ARCH_LVL)
      || call_sefun("process_call") ) {
    return "Du darfst den Mudlib-Master nicht zerstoeren!\n";
  }
  return 0;
}


// MorgenGrauen MUDlib
//
// /std/npc/items.c -- Item-Verwaltung fuer NPCs
//
// $Id: items.c 8146 2012-10-30 18:18:14Z Zesstra $
//
// (c) by Padreic (Padreic@mg.mud.de)
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing/description.h>
#include <living/combat.h>
#undef NEED_PROTOTYPES
#include <npc.h>
#include <properties.h>
#include <combat.h>
#include <config.h>
#include <moving.h>
#include <sys_debug.h>

protected void create()
{
  // dieses flag ist nur bis zum ersten move gesetzt. Nimmt danach also auch
   Set(NPC_NEEDS_ITEM_INIT, 1);  // keinen Speicher mehr in Anspruch.
   // Set(P_ITEMS, ({})); // keine initialisierung spart Speicher  
}

protected void create_super() {
      set_next_reset(-1);
}

protected object* maybe_own_stuff() {
  object* obs = all_inventory(environment());
  object haufen = present("\nhaufen "+name(WEM),environment());
  if( objectp(haufen) ) obs += all_inventory(haufen);
  object leiche = present("\nleiche "+name(WEM),environment());
  if( objectp(leiche) ) obs += all_inventory(leiche);
  return obs;
}

// clont alle Objekte die geclont werden muessen...
protected void _clone_items()
{
  int i, j, mode, refresh;
  object ob, *inv1;
  mixed  props, *items;
  string file, *inv2;

  if (!pointerp(items=QueryProp(P_ITEMS))) return;
  if (environment()) { // Liste mit filenamen (inv2) erstellen.
    inv1 = maybe_own_stuff();
    inv2 = map(inv1, #'load_name);
  }
  else inv2=({}); // inv1 wird gar nicht mehr benoetigt

  for (i=sizeof(items)-1; i>=0; i--) {
    ob = items[i][RITEM_OBJECT];
    file = items[i][RITEM_FILE];
    mode = items[i][RITEM_REFRESH];
    refresh = mode & 0x0000ffff;
    props = items[i][RITEM_PROPS];
    if (props && intp(props)) { // unique Gegenstand?
      ob=find_object(file);
      if (ob) {
        // kann ob aufgenommen werden? Wenn das Objekt in inv1 drin ist, ist es
        // im Env oder in einer Leiche oder einem Haufen.
        if (member(inv1,ob) > -1) {
          if (environment()) {
            if (environment(ob) == environment())
              tell_room(environment(),
                  capitalize(name(WER))+" hebt "+ob->name(WEN)+" auf.\n");
            else
              tell_room(environment(),break_string(
                  capitalize(name(WER))+" nimmt "+ob->name(WEN)+ " aus "
                  +environment(ob)->name(WEM) + ".",78));
          }
          ob->remove();  // zerstoeren und neuladen.
          if (ob) destruct(ob);
          if (catch(ob=load_object(file);publish))             
              raise_error(sprintf(
                "_clone_items(): %O does not exist or is not loadable\n", file)); 
          ob->move(this_object(), M_NOCHECK);
          items[i][RITEM_OBJECT]=ob;
        }
        else switch( refresh ) {
          case REFRESH_DESTRUCT:
            if (environment(ob)) {
              ob=0; // nicht zuecken, tragen, initialisieren...
              break;
            }
          case REFRESH_ALWAYS:
          case REFRESH_REMOVE:                
            ob->remove();
            if (ob) destruct(ob);
            if (catch(ob=load_object(file);publish)) 
                raise_error(sprintf("_clone_items(): "
                      "%O does not exist or is not loadable\n", file));
            ob->move(this_object(), M_NOCHECK);
            items[i][RITEM_OBJECT]=ob;
            break;
          case REFRESH_NONE:
          default: ob=0; // nicht zuecken, tragen, initialisieren...
        }
      }
      else {
        if (catch(ob=load_object(file);publish)) raise_error(sprintf(
          "_clone_items(): %O does not exist or is not loadable\n", file));
        if (objectp(ob)) ob->move(this_object(), M_NOCHECK);
        items[i][RITEM_OBJECT]=ob;
      }
    } // if (props ...) // also nicht unique
    else {
      switch( refresh ) {
        case REFRESH_DESTRUCT: // erfuellt auch REFRESH_REMOVE Bedingung...
          if (ob) {
            ob=0; // nicht zuecken, tragen, initialisieren...
            break;
          }
        case REFRESH_REMOVE:
          if (ob) {
            if(present(ob, this_object())) {
              ob=0; // nicht zuecken, tragen, initialisieren...
              break;
            }
            else {
              if ((member(inv2, file))==-1) {  
                ob=0;
              }
            }
          }
        case REFRESH_NONE: // wird entfernt nach dem ersten clonen!!!
        case REFRESH_ALWAYS:
          // schauen ob der Gegenstand im environment liegt.
          if ((j=member(inv2, file))!=-1) {
            ob=inv1[j]; // inv1 kann leer sein, aber dann ist inv2 auch leer
            inv1[j]=0; // wichtig falls mehrere gleiche AddItems
            inv2[j]=0; // = 0 setzen um Array kopieren zu vermeiden
            if (environment()) {
              if (environment() == environment(ob))
                tell_room(environment(),
                    capitalize(name(WER))+" hebt "+ob->name(WEN)+" auf.\n");
              else
                tell_room(environment(),break_string(
                    capitalize(name(WER))+" nimmt "+ob->name(WEN)+" aus "
                    +environment(ob)->name(WEM) + ".",78));
            }
          }
          if (ob) {
            ob->remove();
            if (ob) destruct(ob);
          }
          if (catch(ob=clone_object(file);publish)) raise_error(sprintf(
            "_clone_items(): %O does not exist or is not loadable\n", file));
          ob->move(this_object(), M_NOCHECK);
          switch( refresh ) {
          case REFRESH_NONE:
            items[i] = 0; // Speicher freimachen
            break;
          case REFRESH_ALWAYS:
            items[i][RITEM_OBJECT] = 0; // Objekt "vergessen"
            break;
          default:
            items[i][RITEM_OBJECT] = ob;
          }
          break;
        default: ob=0; // nicht zuecken, tragen, initialisieren...
      }
    } // if (props ...) else 
    if (ob) {
      if (mappingp(props)) walk_mapping(props, symbol_function("SetProp", ob));
      // Eigentlich will man hier sowas wie command("zuecke schwert")
      // machen, aber das handling der id's kann nicht sicherstellen,
      // dass der NPC die richtige Waffe erwischt, deshalb machen wir
      // das tragen hier von Hand.
      if (mode & CLONE_WEAR) {
        if( mode & CLONE_NO_CHECK ) {
          object *armours;
          ob->DoUnwear(); // evtl. dem Vorgaenger abnehmen.
          UseHands(ob, ob->QueryProp(P_NR_HANDS));
          armours=QueryProp(P_ARMOURS)+({ ob });
          SetProp(P_ARMOURS, armours);
          SetProp(P_TOTAL_AC, QueryProp(P_TOTAL_AC)+ob->QueryProp(P_AC));
          ob->SetProp(P_WORN, this_object());
        } else {
          command( "trage \n"+object_name(ob) );
        }
      }
      if (mode & CLONE_WIELD) {
        if( mode & CLONE_NO_CHECK ) {
          ob->DoUnwield(); // evtl. dem Vorgaenger abnehmen. 
          UseHands(ob, ob->QueryProp(P_NR_HANDS));
          SetProp(P_WEAPON, ob);
          SetProp(P_TOTAL_WC, ob->QueryProp(P_WC));
          ob->SetProp(P_WIELDED, this_object());
        } else {
          command( "zuecke \n"+object_name(ob) );
        }
      }
    } // if (ob) 
  } // for i
  items-=({ 0 }); // REFRESH_NONEs von nicht unique Objekten 
  if (!sizeof(items)) items=0; // Speicher sparen...
  SetProp(P_ITEMS, items);
  Set(NPC_NEEDS_ITEM_INIT, 0);
}

public varargs object AddItem(mixed filename, int refresh, mixed props)
{
  // Aus Array ein Element auswaehlen
  if (pointerp(filename)) 
    filename = filename[random(sizeof(filename))];
 
  // Kein String? -> Fehler
  if (!stringp(filename)){
    raise_error("AddItem: filename or array of filenames expected.\n"); 
  } else {
    // Pfad normieren und eventuell vorhandenes ".c" entfernen
    filename = MASTER->_get_path(
                  filename[<2..]==".c"?filename=filename[0..<3]
                                        : filename,"?"); 
    // Property setzen
    SetProp(P_ITEMS, (QueryProp(P_ITEMS)||({}))+
                     ({ ({ 0, filename, refresh, props }) }));

  if (environment()) _clone_items();
  }
  return 0;
}

void reset()
// fuer REFRESH_ Objekte...
{
  _clone_items();
}


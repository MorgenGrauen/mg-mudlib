// MorgenGrauen MUDlib
//
// container/items.c -- creating extra items in room
//
// $Id: items.c 8811 2014-05-09 17:30:37Z Zesstra $

// extra items handling
//
//	AddItem(string filename, int refresh)
//	  Clones an item and puts it into the room. <refresh> may be
//	  on of the following:
//	  REFRESH_NONE: No refresh done until reboot.
//	  REFRESH_DESTRUCT: Refresh on reset if item was destructed.
//	  REFRESH_REMOVE: Refresh on reset if item was removed from room.
//	  REFRESH_ALWAYS: Create a new clone on every reset.
//
// The commands are implemented as properties P_ITEMS as mapping. They are 
// stored locally (_set_xx) as mapping to speed up the routines 
// in this module.
// 
// Format of key and data in the P_ITEMS mapping:
//
// ([ key1 : refresh1; obp1; arr1, ..., keyn : refreshn; obpn; arrn ])

#include <sys_debug.h>
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <rooms.h>
#include <container.h>
#undef NEED_PROTOTYPES
#include <defines.h>
#include <config.h>
#include <properties.h>
#include <moving.h>
#include <daemon.h>

protected void create()
{ 
    Set( P_ITEMS, ({}) );
    Set( P_ITEMS, SECURED, F_MODE_AS );
    
    OBJECTD->QueryObject();  // querying general objects
}

protected void create_super() {
  set_next_reset(-1);
}

/* Kram zum Aufraeumen von multiplen gleichen Items im Container. */
private object removeable_ob( object ob )
{
    if ( !query_once_interactive(ob) && !living(ob) )
        return ob;

    return 0;
}

protected varargs void remove_multiple(int limit, mixed fun)
{
    object *inh = all_inventory(this_object()) - ({0});

    inh = filter( inh, #'removeable_ob );
    foreach(mixed item : QueryProp(P_ITEMS))
      inh -= ({ item[0] });
 
    if (!stringp(fun) && !closurep(fun))
      fun = "description_id";
    inh = unique_array(inh, fun, 0);
    foreach(mixed arr: inh)
    {
      if (sizeof(arr) <= limit)
        continue;
      catch(call_other(arr[limit ..], "remove"); publish);
    }
}


/* Item handling */
public varargs object AddItem( mixed filename, int refresh, mixed props ) 
{
    string file;
    object ob;
    int i;

    if( pointerp(filename) ) {
        for( i = sizeof(filename); i--; )
            filename[i] = (string)master()->_get_path( filename[i], "?" );
        
        file = filename[random( sizeof(filename) )];
    }
    else 
        file = filename = (string)master()->_get_path( filename, "?" );
    
    if ( props == 1 )
        catch(ob = load_object( file); publish);
    else
	catch(ob = clone_object(file); publish);
    
    if (objectp(ob)) {
      ob->move( this_object(), M_NOCHECK|M_NO_ATTACK );
      // mit Absicht keine Pruefung aufs Move, wenns nicht geht, solls 2s
      // spaeter auf der Ebene buggen, weil praktisch niemand im create() das
      // Ergebnis vom AddItem() prueft.
    }

    // In P_ITEMS vermerken, es sei denn, REFRESH_NONE ist gegeben, in dem
    // Fall ist die Speicherung voellig unnoetig.
    // TODO: Pruefen, ob das wirklich problemlos geht. Bis dahin werden auch
    // TODO::REFRESH_NONE-Items vermerkt. (s. clean_up() in /std/room.c)
    //if (!(refresh & REFRESH_NONE)) {
      SetProp( P_ITEMS, QueryProp(P_ITEMS) +
             ({ ({ ob,        // RITEM_OBJECT
                   filename,  // RITEM_FILE
                   refresh    // RITEM_REFRESH
                       }) +
                    ((mappingp(props) || props == 1) ? ({ props }) : ({})) }) );
    //}

    if ( ob && mappingp(props) ) 
        walk_mapping( props, symbol_function( "SetProp", ob ) );
    
    return ob;
}


private void ri_rem_ob( object ob )
{
    object *inv;
    int i;
  
    if ( objectp(ob) && present(ob, this_object()) ) {
        inv = deep_inventory(ob);
        
        for ( i = sizeof(inv); i--; )
            if ( inv[i] ) {
                inv[i]->remove(1);
                
                if ( inv[i] )
                    destruct(inv[i]);
            }
        
        ob->remove(1);
        
        if ( ob )
            destruct(ob);
    }
}


private int ri_filter( mixed *ritem, mixed file )
{
    object ob, *inv;
    int i;
  
    ob = ritem[RITEM_OBJECT];
  
    if ( stringp(file) && ritem[RITEM_FILE] == file )
        return ri_rem_ob(ob), 0;
    else if ( pointerp(ritem[RITEM_FILE]) && pointerp(file) &&
              sizeof(file & ritem[RITEM_FILE]) == sizeof(ritem[RITEM_FILE]) )
        return ri_rem_ob(ob), 0;

    return 1;
}


public void RemoveItem( mixed filename )
{
    mixed *items;
    int i;

    if ( !pointerp(items = QueryProp(P_ITEMS)) || !sizeof(items) )
        return;

    if ( pointerp(filename) )
        for ( i = sizeof(filename); i--; )
            filename[i] = (string)master()->_get_path( filename[i], "?" );
    else
        filename = (string)master()->_get_path( filename, "?" );
  
    SetProp( P_ITEMS, filter( items, #'ri_filter/*'*/, filename ) );
}


private mixed _do_refresh( mixed item )
{
    string file;
    object ob;

    if ( !pointerp(item) || item[RITEM_REFRESH] == REFRESH_NONE )
        return item;

    if ( pointerp(item[RITEM_FILE]) ) 
        file = item[RITEM_FILE][random( sizeof(item[RITEM_FILE]) )];
    else
        file = item[RITEM_FILE];

    switch( item[RITEM_REFRESH] ){
    case REFRESH_MOVE_HOME:
        if ( objectp(item[RITEM_OBJECT]) &&
             environment(item[RITEM_OBJECT]) != ME ) {
            item[RITEM_OBJECT]->move( ME, M_GO|M_NO_ATTACK );
            break;
        }
        
    // fall through
    case REFRESH_DESTRUCT:
        if ( objectp(item[RITEM_OBJECT]) ) 
            break; // else FALL THROUGH
        
    case REFRESH_REMOVE:
        if ( objectp(item[RITEM_OBJECT]) &&
             environment(item[RITEM_OBJECT]) == ME )
            break; // else FALL THROUGH
        
    default:
        if ( sizeof(item) > RITEM_PROPS && item[RITEM_PROPS] == 1 ) {
            ob = load_object(file);
        }
        else
            ob = clone_object(file);

        ob->move( ME, M_NOCHECK|M_NO_ATTACK );
        break;
    }
    
    if ( ob ){
        item[RITEM_OBJECT] = ob;
        
        if ( sizeof(item) > RITEM_PROPS && mappingp(item[RITEM_PROPS]) )
            walk_mapping( item[RITEM_PROPS], symbol_function( "SetProp", ob ) );
    }
    
    return item;
}


// reset handling: check how the items should be refreshed.
void reset() 
{
    mixed *items;

    if ( !pointerp(items = QueryProp(P_ITEMS)) ){
        SetProp( P_ITEMS, ({}) );
        return;
    }
    
    SetProp( P_ITEMS, map( items, #'_do_refresh/*'*/ ) - ({0}) );
}


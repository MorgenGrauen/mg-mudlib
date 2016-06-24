//  losa.c -- Modul fuer Laden und Speichern der Hausdaten
//
//  (c) 1995 Wargon@MorgenGrauen
//      2006 Vanion@MorgenGrauen, fuer die rebootfesten Moebel
// $Id: losa.c,v 1.1.1.1 2000/08/20 20:22:42 mud Exp $
//
#pragma strong_types,rtt_checks

#define NEED_PROTOTYPES
#include "../haus.h"
#include <container.h>
#include <thing/properties.h>
#include <room/exits.h>
#include <thing/description.h>
#undef NEED_PROTOTYPES
#include <properties.h>
#include <wizlevels.h>
#include <moving.h>

static void start_reload_furniture();

private int csaved;

// Variablen zur Verwaltung der Moebel im Raum.
mapping furniture;
mapping broken_furniture;

protected void
create()
{
  Set(H_CHEST, SAVE, F_MODE);
  Set(H_CHEST, 0);

  Set(H_FURNITURE, SAVE, F_MODE);
  Set(H_FURNITURE, 0);

//  Set(H_SPECIAL, SAVE|SECURED, F_MODE);
//  Set(H_SPECIAL, ([:2]), F_VALUE);
}

/* Scheint nirgendwo benutzt zu werden...
varargs int AddSpecial(int typ, string key, string extra)
{
  if (!this_interactive() || !IS_ARCH(this_interactive()) ||
      !(PATH+"access_rights")->access_rights(geteuid(this_interactive()),""))
    return -1;

  if (typ != HS_EXIT && typ != HS_ITEM)
    return 0;

  Set(H_SPECIAL, Query(H_SPECIAL) + ([ key : typ; extra ]));
  Save();
  return 1;
}

void RemoveSpecial(string key)
{
  if (!this_interactive() || !IS_ARCH(this_interactive()) ||
      !(PATH+"access_rights")->access_rights(geteuid(this_interactive()),""))
    return;

  Set(H_SPECIAL, m_delete(Query(H_SPECIAL), key));
  Save();
}
*/

void
reset()
{
  if (QueryOwner() &&
      !sizeof(filter(all_inventory(),#'interactive)) &&
      !csaved)
    Save(1);
}

// crunched komprimiert das Savefile
varargs void
Save(int crunched)
{
  mixed o1, o2, o3;
  closure pc;
  object *obs;
  int i, found;

  o3 = 0;

  if (!(o1 = Query(P_DETAILS)))
    Set(P_DETAILS, QueryProp(P_DETAILS), F_VALUE);

  if (!(o2 = Query(P_READ_DETAILS)))
    Set(P_READ_DETAILS, QueryProp(P_READ_DETAILS), F_VALUE);

  if (csaved = crunched)
  {
    pc = symbol_function("PCrunch", VERWALTER);
    Set(P_DETAILS, funcall(pc, Query(P_DETAILS)), F_VALUE);
    Set(P_READ_DETAILS, funcall(pc, Query(P_READ_DETAILS)), F_VALUE);
    o3 = Query(H_COMMANDS, F_VALUE);
    Set(H_COMMANDS, funcall(pc, o3), F_VALUE);
  }

  // Autoload-Einrichtung identifizieren und speichern
  // Code in Anlehnung an dem Autoload-Mechanismus fuer Spieler
  furniture=([]);

  // Alle Autoloader filtern
  obs=filter_objects(all_inventory(this_object()), "QueryProp", H_FURNITURE);
  found = 0;

  // Ueber alle Moebel iteritieren
  for( i=sizeof(obs)-1;i>=0;i--)
  {
    if( clonep(obs[i]))
    {
      if ( ++found <= MAX_FURNITURE_PER_ROOM )
        furniture += ([ object_name(obs[i]):obs[i]->QueryProp(H_FURNITURE) ]);
    }
  }
  if (found > MAX_FURNITURE_PER_ROOM)
  {
    tell_object(this_player(), 
      break_string("Du hast "+found+" Moebelstuecke im Raum stehen. "
                   "Gespeichert werden nur "+MAX_FURNITURE_PER_ROOM+". "
                   "Du solltest Dich von einigen Einrichtungsgegenstaenden "
                   "trennen.",78));

  }
  HDEBUG("Saving "+ sizeof (furniture) +" (plus "+sizeof(broken_furniture)+
         " broken) objects in room "+
         object_name(this_object()) + ".");

  save_object( HAUSSAVEPATH+QueryOwner(1));

  Set(P_DETAILS, o1, F_VALUE);
  Set(P_READ_DETAILS, o2, F_VALUE);
  if (o3)
    Set(H_COMMANDS, o3, F_VALUE);
}

void
Load()
{
  mixed prop;
  int i;

  restore_object( HAUSSAVEPATH+QueryOwner(1));

  // Details und Kommandos werden beim Speichern de-dupliziert und in einem
  // speziellen Format abgespeichert (s. PCrunch() im Hausverwalter). Sie
  // muessen nach dem Laden durch die entsprechenden Add...()-Aufrufe
  // wieder eingetragen werden.
  prop=Query(P_DETAILS, F_VALUE);
  RemoveDetail(0);
  if (pointerp(prop))
  {
    foreach(<string*|string>* item : prop)
      AddDetail(item[0], item[1]);
  }
  else if (mappingp(prop))
  {
    foreach(string key, mixed val : prop)
      AddDetail(key, val);
  }
  else
    SetProp(P_DETAILS, prop);

  prop = Query(P_READ_DETAILS, F_VALUE);
  RemoveReadDetail(0);
  if (pointerp(prop))
  {
    foreach(<string*|string>* item : prop)
      AddDetail(item[0], item[1]);
  }
  else if (mappingp(prop))
  {
    foreach(string key, mixed val : prop)
      AddReadDetail(key, val);
  }
  else
    SetProp(P_READ_DETAILS, prop);

  prop = Query(P_EXITS, F_VALUE);
  RemoveExitNoCheck(0);
  if (mappingp(prop))
  {
    if (widthof(prop) <= 1)
    {
      foreach(string key, string dest : prop)
        AddExitNoCheck(key, dest);
    }
    else
    {
      foreach(string key, string dest, string msg : prop)
      {
        if (stringp(msg))
          _AddExit(key, dest, msg);
        else if (stringp(dest) && strstr(dest,"#") != -1)
          AddExitNoCheck(key, dest);
        else
          _AddExit(key, dest, 0);
      }
    }
  }

  prop=Query(H_COMMANDS, F_VALUE);
  if (pointerp(prop))
  {
    Set(H_COMMANDS, ([]), F_VALUE);
    for (i=sizeof(prop)-1; i>=0; i--)
      this_object()->AddUserCmd(prop[i][0], 0, prop[i][1], prop[i][2]);
  }

  if (environment())
    environment()->SetProp(P_NEVER_CLEAN, 1);

  if (previous_object() && object_name(previous_object())==VERWALTER)
  {
    if (Query(H_CHEST))
      this_object()->AddItem(PATH+"truhe",REFRESH_NONE,
                             ([ "owner" : QueryOwner() ]));
/* Das scheint nirgendwo benutzt zu werden und in allen Seherhaeusern leer zu
 * sein.
    mapping special = Query(H_SPECIAL, F_VALUE);
    if (special)
    {
      foreach(string key, int type, string extra : special)
      {
        switch(type)
        {
          case HS_ITEM:
            AddItem(SPECIALPATH + extra, REFRESH_DESTRUCT);
            break;
          case HS_EXIT:
            AddExitNoCheck(key, extra);
            break;
        }
      }
    }
*/
  }

  // Das Laden der Autoloader wird erst am Ende angestossen. 
  // Dann ist es nicht schlimm, wenn alle Eval Ticks verbraucht werden.
  start_reload_furniture();
}

// Mehrere Save-Anforderungen zusammenfassen.
static void queued_save()
{
        HDEBUG("QS");
        while (remove_call_out("Save")!=-1);
        call_out("Save",3);
}

static int reload_error(string file, mixed data,  string message)
{
        HDEBUG(message);
        broken_furniture+=([file:data]);
        log_file("seher/haeuser/autoloader_error",
                 dtime(time())+"\n"+
                 break_string(object_name(this_object())+" ("+QueryOwner(1)+")",78, " FILE: ",1)+
                 break_string(message, 78, " MSG:  ",1)+
                 break_string(sprintf("%O", data),78, " DATA: ",1)+"\n");
                 
        return 0; // 0 fuer das filter, damit dieser Eintrag 
                  // aus furniture geloescht wird.
}

// Laedt ein einzelnes Moebelstuecks
static int load_furniture_object( string file, mixed data )
{
    object ob;
    string error;
    string blueprint;
    closure pc;
    // mixed data;
    
    // Wenn genug Ticks frei sind, wird versucht, das Objekt zu erzeugen.
    // Ansonsten ist die Gefahr zu gross, dass ein Erzeugungs-Prozess abbricht.
    if (get_eval_cost() < 500000) 
    {
      // HDEBUG("Suspending Object: "+file+". Only "+to_string(get_eval_cost())+" ticks left.");
      return 1; // 1 bedeutet, dass dieser Eintrag es im Mapping bleibt.
    }

    // HDEBUG("Processing Object: "+file+" with Data: "+sprintf("%O",data)+".");
    
    // Nummern der Clones sind beim Speichern noetig, um die Identitaeten
    // der Objekte zu bestimmen (mehrere Objekte vom gleichen Blueprint 
    // speichern). Hier braucht man sie nicht mehr 
            
    blueprint = explode(file,"#")[0];
    
    // Data aus dem Mapping holen
    // data=furniture[file];
    
    // Muss ich die Blueprint suchen?
    ob = find_object(file);
    
    // Nein.
    if (!ob)
    {
            // Existiert die BP oder ein VC fuers File?
        if (file_size(blueprint+".c")<0&&
           file_size(implode(explode(blueprint,"/")[0..<2],"/")+
                     "/virtual_compiler.c")<0)
        {
           return reload_error(file, data, "Error in file: "+ file +
                               ". File does not exist.");

        }

        // File gefunden. Versuch, es zu laden.
        if (error = catch(call_other( blueprint,"???")))
        {
           return reload_error(file, data, "Error loading file: "+file+". "+error);
        }
    }
    
    // Clone erzeugen
    if ( error = catch(ob = clone_object(blueprint)) )
    {
        return reload_error(file, data, "Error cloning object: "+file+". "+error);
    }
 
    HDEBUG(sprintf("%O",furniture));
    // Autoload-Daten setzen
    HDEBUG(object_name(ob)+"->SetProp("+sprintf("%O", data)+")");
    if (ob)
        catch(ob->SetProp( H_FURNITURE, data ));
           
    // Furniture in das Seherhaus moven
    if ( error = catch(ob->move( this_object(), M_NOCHECK )) ) {
        ob->remove();
        if(ob) destruct(ob);
        return reload_error(file, data, "Error moving object: "+file+". "+error);
    }

    // post_create anstossen
    pc=symbol_function("post_create", ob);
    if (closurep(pc))
      call_out(pc, 1);
    
    return 0; // 0 bedeutet hier, dieses Objekt nicht noch einmal anstossen.
}

static void load_furniture()
{
        int i;
        string rv;
        string current_key;
        
        // Abbruchbedingung ist, dass nichts mehr zu laden ist.
        if (sizeof(furniture)==0) return; 
        
        // Anstoßen des naechsten Durchlaufs, falls die Ticks nicht reichen.
        while (remove_call_out(#'load_furniture) != -1);
        call_out(#'load_furniture, 1);
        
        // Laden aller Moebel anstoßen
    furniture=filter(furniture, #'load_furniture_object);
}

// Diese Funktion bereitet das Reloaden der Einrichtung vor
static void start_reload_furniture()
{  
  // Wenn es keine Moebel gibt, ist das Laden beendet.
  if (!mappingp(furniture)) return;
  if (broken_furniture==0) broken_furniture=([]);
  
  // Falls ein Key von furniture 0 ist, wird dieser geloescht. 
  m_delete(furniture,0);
  
  // Laden des Furniture anstossen
  load_furniture();  
}



// MorgenGrauen MUDlib
//
// virtual/v_compiler.c -- a general virtual compiler object
//
// $Id: v_compiler.c 9142 2015-02-04 22:17:29Z Zesstra $

// principle:
//  - inherit this object into your own 'virtual_compiler.c'
//  - customize Validate() and CustomizeObject() for you own sake
//  
//  * Validate() checks if a room filename given as argument (without path)
//    is valid and returns this filename with stripped '.c'!!
//  * CustomizeObject() uses the previous_object()->Function() strategy to
//    customize the standard object (for example to set a description)
//
// Properties: P_STD_OBJECT, P_COMPILER_PATH

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/thing/properties";

//#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <defines.h>
#include <v_compiler.h>
#include <exploration.h>
#include <sys_debug.h>
#include <living/description.h> //fuer P_PARA

// Der VC braucht das 'alte' object_name()-basierte BLUE_NAME, da sonst das
// Konfigurieren der von einem VC-Objekt geclonten Clones via
// CustomizeObject() nicht funktioniert (load_name() ermittelt den Namen des
// VC-Standardobjektes)
#ifdef BLUE_NAME
#undef BLUE_NAME
#endif
#define BLUE_NAME(ob) (explode(object_name(ob),"#")[0])

private nosave string last_loaded_file;
private nosave mapping objects;

void create()
{
  ::create();
  seteuid(getuid());
  SetProp(P_STD_OBJECT, "/std/room");
  SetProp(P_COMPILER_PATH, sprintf("/%s/",
        implode(old_explode(object_name(this_object()), "/")[0..<2], "/")));
  SetProp(P_PARA, ({}) ); // keine Para-VC-Objekte
  objects = ([]);
}

// von den erbenen VCs zu ueberschreiben...
// TODO: aus Standardobjekt entfernen, weil durch P_PARA und QueryValidObject
// obsolet. 
int NoParaObjects() { return 0; }

string Validate(string file)
{
  if(!file) return 0;
  if(file[<2..] == ".c") file = file[0..<3];
  EPMASTER->PrepareVCQuery(file);
  return file;
}

// Die Funktion bekommt einen Objektnamen uebergeben und muss entscheiden, ob
// dieser VC dafuer zustaendig ist, das Objekt zu generieren. Jeder Wert != 0
// zaehlt als 'zustaendig'. Es ist eine Art generalisiertem Validate(). Fuer
// maximale Nuetzlichkeit muss diese Funktion von den erbenden VCs
// ueberschrieben werden.
public int QueryValidObject(string oname) {
    string fname, path, *pelem;
    int para;
    mixed ppara;

    //erstmal Validate fragen
    pelem=explode(oname,"/");
    fname=pelem[<1];
    if (!fname=Validate(fname))
        return(0); //nicht zustaendig
    // nicht im richtigen Pfad?
    path=sprintf("%s/",implode(pelem[0..<2],"/"));
    if (path!=QueryProp(P_COMPILER_PATH))
        return(0);
    // Para-Objekt?
    if (sscanf(fname,"%s^%d",fname,para) > 1) {
        if (NoParaObjects())
            return(0); //direkt zurueck, keine Para-Objekte
        // bestimmte Para-Dimensionen explizit erlaubt? (Wenn P_PARA nicht
        // gesetzt ist, sind alle erlaubt!)
        if (ppara=QueryProp(P_PARA)) {
            if (pointerp(ppara) && member(ppara,para)!=-1)
                return(1);
            else if (intp(para) && ppara==para)
                return(1);
            // P_PARA gesetzt, aber gewuenschtes Para nicht enthalten...
            else return(0);
        }
    }
    return(1); //fall-through, offenbar zustaendig.
}

mixed CustomizeObject()
{
  string file;

  if(!clonep(previous_object()))
    return Validate(explode(BLUE_NAME(previous_object()), "/")[<1]);
  if(stringp(last_loaded_file)) file = last_loaded_file;
  else file = Validate(explode(BLUE_NAME(previous_object()), "/")[<1]);
  if(!file) return 0;
  last_loaded_file = 0;
  return file;
}

// add a new object to the object list if it compiles
private mixed AddObject(string file)
{
  object ob;
  string err;

  // clean up the object list
  objects = filter_indices(objects, function int (string f) {
      return (objectp(objects[f])); } );

  last_loaded_file = file;
  // register new object
  if(ob = clone_object(QueryProp(P_STD_OBJECT)))
    objects[file] = ob;
  return ob;
}

// try to create an object for the wanted file
mixed compile_object(string file)
{
  // validate if the file name is a correct one
  if(file = Validate(file))
    return AddObject(file);
  return 0;
}  

// return all cloned virtual objects
mixed QueryObjects()
{
  return m_values(objects)-({0});
}

// clean up rooms that have not been destructed yet
int remove() {

  if(!mappingp(objects)) return 0;

  //for(ob = QueryObjects(); sizeof(ob); ob = ob[1..])
  foreach(object ob: QueryObjects()) {
      ob->remove();
      if(objectp(ob)) destruct(ob);
  }
  return 1;
}


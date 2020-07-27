/** Letzte Aenderung:
// 01.04.2008 - Objekte des VC werden jetzt im memory gespeichert
//            - Kleinere Aenderungen im Code.
// 01.01.2007 - Grundlegende Ueberarbeitung, Vanion
//            - Verwaltung der Objekte in einer Liste
//              Dadurch kann ls die VC-Objekte darstellen
//            - Genauere Ueberpruefung der Dateinamen auf 
//              Korrektheit. Falls ein illegaler Zugriff
//              stattfindet, wirds geloggt.
//
*/

#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
#pragma warn_deprecated

#include "haus.h"
#include <thing/properties.h>
#include <wizlevels.h>

#define MEMORY "/secure/memory"

private nosave mapping objects;
private nosave int show_clone_list;

/**
 Initialisierung
*/
void create()
{
  seteuid(getuid());

  // Zeiger auf Cloneliste holen
  if (({int})call_other(MEMORY,"HaveRights")) {
    // Objektpointer laden
    objects = ({mapping})call_other(MEMORY,"Load","objects");

    // Hats geklappt?
    if (!mappingp(objects)) 
    // vermutlich noch nicht initialisiert.
    // Zeiger erzeugen
    objects = ([]); 
    call_other(MEMORY,"Save","objects",objects); // Und in den Memory schreiben
  } else {
    // Wenn ich keinen Zeiger habe, muss ich halt einen lokalen nehmen.

    objects = ([]);
  }

  show_clone_list = 0; 
}

/**
 Die Funktion wird vom GameDriver aufgerufen. Wenn die Namen-
 konventionen passen, erzeugt compile_object ein passendes Objekt
 und gibt es zurueck
 \param name Name des zu ladenden Objekts
 \return Das geladene und initialisierte Objekt
*/
object compile_object(string name)
{
  object ob;

  if (name[<6..<3] == "haus" )
  {
    ob = ({object})(VERWALTER)->_LadeHaus(name[0..<7]);
  }
  else if (name[<7..<4] == "raum" )
  {
    ob = ({object})(VERWALTER)->_LadeRaum(name[0..<8], to_int(name[<3..<3]));
  } 
  else 
  {
    log_file("seher/haeuser/vc_load_error", 
      dtime(time())+"\n Falscher Objektname: "+name+"\n"+
  	         " PO:  "+object_name(previous_object(1))+",\n"+
  	         " PPO: "+object_name(previous_object(2))+",\n"+
  	         " TI:  "+object_name(this_interactive())+".\n\n");
  	return 0;
  }
  
  // Objekt speichern (fuer ls)
  if (objectp(ob)) objects[name] = ob;

  return ob;
}

/**
  Soll die Liste der Objekte zum VC per QueryObjects ausgegeben werden oder nicht?
*/
  
void ToggleCloneList()
{
  show_clone_list ^= 1;
  tell_object(this_player(), "Die Clone List wird nun "+
        (show_clone_list?"":"nicht mehr ")+"angezeigt.\n");
}

/**
 return all cloned virtual objects, wird zum Beispiel fuer "ls" benutzt.
*/
mixed QueryObjects()
{
  if (show_clone_list)
    return m_values(objects)-({0});
  else 
    return ({});
}

/**
  Nur EM und Maintainer duerfen die sicheren Funktionen nutzen
*/
int secure()
{
	if (ARCH_SECURITY || IS_MAINTAINER(this_interactive())) 
	  return 1;
	else  
	  return 0;
}

/*
/**
 Die Funktion versucht, die Clones dieses VCs aus der Objekt List 
 zu regenerieren
*/
/*
int regenerate_clone_list()
{
	if (sizeof(objects)==0)
	HDEBUG("Tbd: regenerate_clone_list im vc");
	
	return 1;
}
*/

/**
  Die Funktion zerstoert alle bereits geclonten Seherhausobjekte
  Das gibt wirklich nur irre wenige Momente, wo das nicht schlecht ist.
*/
int desctruct_all_my_clones()
{
  mixed ob;

  if (!secure()) return 0;

  if(!mappingp(objects)) return 0;
  
  for(ob = QueryObjects(); sizeof(ob); ob = ob[1..])
  {
    if(objectp(ob[0]))
    {
      ob[0]->remove();
      if(objectp(ob[0])) destruct(ob[0]);
    }
  }
   
  return 1;
}


// MorgenGrauen MUDlib
//
// container/restrictions.c -- container restrictions
//
// $Id: restrictions.c 9020 2015-01-10 21:49:41Z Zesstra $

// This is a simple container to put objects in. It defines all functions
// which are necessary to describe an object which can be filled with
// other things.
//
// It will support restrictions for volume, weight etc.
//
// The following properties are defined:
// P_MAX_WEIGHT - maximum weight which container can carry
// P_TOTAL_WEIGHT - total weight, with contents.
// P_WEIGHT - the weight of the container without contents
//
// Functions for manipulation of weight
// MayAddWeight(weight) - Can <weight> be inserted?
// AddWeight(weight) - Add an amount of <weight>
//
// IMPORTANT: unit equals 1 gram

#pragma strict_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include <properties.h>
#include <defines.h>
#include <thing/description.h>

// local properties prototypes
static int _query_total_weight();


private nosave int LastWeightCalc, contents, LastObjectCount, objcount;
private nosave int last_content_change;


void create()
{
    Set( P_WEIGHT_PERCENT, 50 );
    Set( P_TOTAL_WEIGHT, NOSETMETHOD, F_SET_METHOD );
    Set( P_TOTAL_WEIGHT, PROTECTED, F_MODE );
    Set( P_MAX_OBJECTS, 100 );
    Set( P_TOTAL_OBJECTS, NOSETMETHOD, F_SET_METHOD );
    Set( P_TOTAL_OBJECTS, PROTECTED, F_MODE );
    LastWeightCalc = -1;
    LastObjectCount = -1;
}


static int _query_last_content_change()
{
    return last_content_change;
}


// absichtlich public, da es von der simul_efun.c *direkt* aufgerufen
// wird aus Performancegruenden!
public varargs int _set_last_content_change( mixed wert )
{
    // Der uebergebene Wert ist unerheblich. Die Property
    // P_LAST_CONTENT_CHANGE wird so oder so bei jedem SetProp()-
    // Aufruf um eins erhoeht, um der 2s-"Unschaerfe" von time()
    // aus dem Weg zu gehen.
    return ++last_content_change;
}


int query_weight_contents()
{
    object *objs;
    int w, w2, i;

    if ( last_content_change == LastWeightCalc )
        return contents;
    
    w = 0;
    objs = all_inventory(this_object());
    i = sizeof(objs);
    
    while ( i-- ){
        if ( !(w2 = (int)objs[i]->QueryProp(P_TOTAL_WEIGHT)) )
            w2 = (int)objs[i]->QueryProp(P_WEIGHT);
        w += w2;
    }
    
    LastWeightCalc = last_content_change;
    return contents = w;
}


static int _query_total_objects()
{
  if ( last_content_change == LastObjectCount )
      return objcount;

  objcount = sizeof( filter_objects( all_inventory(), "short" ) );
  LastObjectCount = last_content_change;
  return objcount;
}


// diese Funktion sollte von Raeumen natuerlich ueberschrieben werden...
int MayAddObject( object ob )
{
    if (ob) {
        if ( !ob->short() )
            return 1; // invis-Objekte duerfen immer
        
        if ( ob == ME || environment(ob) == ME)
            return 1; // objekt ist schon drin
    }
    
    return (QueryProp(P_TOTAL_OBJECTS) < QueryProp(P_MAX_OBJECTS));
}


#define ENV environment
#define PO previous_object()

int MayAddWeight( int w )
{
    int nw, aw;

    // was nix wiegt, passt
    if ( w <= 0 )
        return 0;

    nw = w; // Gewicht im neuen Container
    aw = 0; // Gewicht fuer das env()

    // Von Raum in Behaelter im Spieler: Container sieht volles Gewicht (nw=w),
    // Check im env() (Spieler) mit reduziertem Gewicht.
    if ( ENV() && PO && ENV(PO) != ENV() )
        aw = QueryProp(P_WEIGHT_PERCENT) * w / 100 || 1;

    if ( PO && ENV(PO) && ENV(ENV(PO)) ){
        // Wenn das Objekt ein env() hochbewegt wird, wird das Gewicht im alten
        // Container abgezogen. Weiterer Check im env() ist unnoetig (aw=0).
        if ( ENV(ENV(PO)) == ME )
            nw = w - (int)ENV(PO)->QueryProp(P_WEIGHT_PERCENT) * w / 100;
        // Eine Ebene tiefer bewegen: Test im neuen Container mit unveraendertem
        // Gewicht; Check im env() mit um Gewichtsreduktion verringertem Gewicht
        else if ( present( ME, ENV(PO) ) )
            aw = QueryProp(P_WEIGHT_PERCENT) * w / 100 - w;
        // Auf derselben Ebene verschieben (von Paket in Beutel):
        // Neuer Container sieht volles Gewicht (nw=w), Check im env() mit
        // Differenz aus Gewicht in Container1 und in Container2.
        else if ( ENV(ENV(ENV(PO))) && ENV() == ENV(ENV(PO)) )
            aw = QueryProp(P_WEIGHT_PERCENT) * w / 100
                - ((int)ENV(PO)->QueryProp(P_WEIGHT_PERCENT) * w / 100 || 1);
    }

    if ( query_weight_contents() + nw > QueryProp(P_MAX_WEIGHT) )
        // Container kann Gewicht nicht mehr aufnehmen
        return -1;
    
    if ( aw && ENV()->MayAddWeight(aw) < 0 )
        // Umgebung des Containers kann Gewicht nicht mehr aufnehmen
        return -2;
    
    return 0;
}


/* Redefine PreventInsert() to prevent inserting of special objects. If
 * a value greater 0 is returned the object ob can't be inserted in the
 * container.
 */
public int PreventInsert( object ob ) { return 0; }
public int PreventLeave( object ob, mixed dest ) { return 0; }
public int PreventInsertLiving( object ob ) { return 0; }
public int PreventLeaveLiving( object ob, mixed dest ) { return 0; }
public void NotifyInsert(object ob, object oldenv) { }

// **** local property methods
static int _query_total_weight() 
{ 
    return QueryProp(P_WEIGHT) +
        (QueryProp(P_WEIGHT_PERCENT) * query_weight_contents() / 100); 
}


// Hilfsfunktion
static int _behalten( object ob, string uid )
{
    return (string)ob->QueryProp(P_KEEP_ON_SELL) == uid;
}


/*
 *
 * get a list of all contained objects matching a complex description
 *
 */

#define POS_INVERS   0x01  /* nur zur funktionsinternen Verwendung */
#define POS_LETZTES  0x02  /* nur zur funktionsinternen Verwendung */

object *present_objects( string complex_desc )
{
    int i;          // Zaehlervariable
    int meth;       // 0x01 = invers?,  0x02 = letztes?
    object ob;      // einzelnes temporaeres Objekt
    object *obs;    // liste der ausgewaehlten Objekte
    object *erg;    // zum sammeln bis es nachher zurueckgegeben wird...
    string *strlst; // liste aller Gruppen die mit AND verknuepft sind
    object haufen;

    strlst = allocate(2);
    
    if ( sscanf( complex_desc, "%s ausser %s", strlst[0], strlst[1]) == 2 ){
        erg = present_objects( strlst[0] );
        obs = present_objects( strlst[1] );
        return erg-obs;
    }
    
   strlst = explode( complex_desc, " und " );
   erg = ({});
   
   for ( i = sizeof(strlst); i--; )
   {
       complex_desc = strlst[i];
       // auf letzte/letztes/letzten pruefen...
       if ( complex_desc[0..5] == "letzte" ){
           switch( complex_desc[6..6] ){
           case " ":
               meth |= POS_LETZTES;
               complex_desc = complex_desc[7..];
               break;
           case "s":
           case "n":
           case "m":
           case "r":
               if ( complex_desc[7..7] != " " )
                   break;
               meth |= POS_LETZTES;
               complex_desc = complex_desc[8..];
               break;
           default:
           }
       }

       // auf verneinung pruefen
       if ( complex_desc[0..5] == "nicht " ){
           meth |= POS_INVERS;
           complex_desc = complex_desc[6..];
       }

       obs=({});
       // nun nach Main-Ids (Gruppen) suchen...
       if ( meth & POS_LETZTES )
       { // geht es nur um den letzten Gegenstand?
           switch( complex_desc ){
           case "waffe":
               obs = filter_objects( all_inventory(), "QueryProp",
                                     P_WEAPON_TYPE );
               break;
               
           case "ruestung":
               obs = filter_objects( all_inventory(), "QueryProp",
                                     P_ARMOUR_TYPE );
               break;
          
           case "kleidung":
               obs = filter_objects( all_inventory(), "IsClothing");
               break;
               
           case "verschiedenem":
           case "verschiedenes":
               obs = all_inventory();
               obs -= filter_objects( obs, "QueryProp", P_WEAPON_TYPE );
               obs -= filter_objects( obs, "QueryProp", P_ARMOUR_TYPE );
               obs -= filter_objects( obs, "IsClothing");
               obs -= filter( obs, #'living/*'*/ );
               break;
        
           case "eigenes":
           case "meins":
               if (objectp(haufen=present("\nhaufen "+
                     this_player()->name(WEM)))) {
                  obs = all_inventory(haufen);
               }
               // kein break;, Fall-through!
           case "behaltenem":
           case "behaltenes":
               obs += filter( all_inventory(), "_behalten", ME,
                                   getuid(this_player() || previous_object()) );

               obs += (QueryProp(P_ARMOURS) || ({}))
                   + ({ QueryProp(P_WEAPON) }) - ({ 0 });
                   break;
                   
           case "gegenstand":
               obs = all_inventory() -
                   filter( all_inventory(), #'living/*'*/ );
               break;
               
           default:
               obs = filter_objects( all_inventory(), "id", complex_desc );
           }

           // unsichtbare objekte entfernen
           obs = filter_objects( obs, "short" );

           // letzten Gegenstand raussuchen
           obs = obs[0..0]; 
       } // if (letzter Gegenstand)
       else
       { // ganze Gruppen und nicht nur das letzte Objekt
           switch ( complex_desc )
           {
           case "allem":
           case "alles":
           case "jeden gegenstand":
           case "jedem gegenstand":
           case "gegenstaende":
           case "alle gegenstaende":
           case "allen gegenstaenden":
               if ( meth & POS_INVERS )
                   continue; // alles nicht = nichts :)
               
               obs = all_inventory() -
                   filter( all_inventory(), #'living/*'*/ );
               break;
               
           case "waffen":
           case "jede waffe":
           case "jeder waffe":
           case "alle waffen":
           case "allen waffen":
               obs = filter_objects( all_inventory(), "QueryProp",
                                     P_WEAPON_TYPE );
               break;
               
           case "ruestungen":
           case "jede ruestung":
           case "jeder ruestung":
           case "alle ruestungen":
           case "allen ruestungen":
               obs = filter_objects( all_inventory(), "QueryProp",
                                     P_ARMOUR_TYPE );
               break;
 
           case "kleidung":
           case "jede kleidung":
           case "jeder kleidung":
           case "alle kleidung":
           case "allen kleidung":
               obs = filter_objects( all_inventory(), "IsClothing");
               break;
              
           case "gegenstand":
               obs = filter_objects( all_inventory() -
                                     filter( all_inventory(),
                                                   #'living/*'*/ ),
                                     "short" )[0..0];
               break;
               
           case "verschiedenem":
           case "verschiedenes":
               obs = all_inventory();
               obs -= filter_objects( obs, "QueryProp", P_WEAPON_TYPE );
               obs -= filter_objects( obs, "QueryProp", P_ARMOUR_TYPE );
               obs -= filter_objects( obs, "IsClothing");
               obs -= filter( obs, #'living/*'*/ );
               break;
       
           case "eigenes":
           case "eigenem":
           case "eigenen":
           case "meins":
           case "alles eigene":
               if (objectp(haufen=present("\nhaufen "+
                       this_player()->name(WEM)))) {
                  obs = all_inventory(haufen);
               }
               // kein break;, Fall-through!
           case "behaltenem":
           case "behaltenen":
           case "behaltenes":
           case "alles behaltene":
               obs += filter( all_inventory(), "_behalten", ME,
                                   getuid(this_player() || previous_object()) );

               obs += (QueryProp(P_ARMOURS) || ({}))
                   + ({ QueryProp(P_WEAPON) }) - ({ 0 });
                   break;
                   
           default:
               if ( complex_desc[0..3] == "jede" ||
                    complex_desc[0..4] == "alle ")
               {
                   if ( complex_desc[4..4] == " " )
                   {
                       obs = filter_objects( all_inventory(), "id",
                                             complex_desc[5..] );
                       break;
                   }
                   else
                   {
                       switch( complex_desc[4..5] )
                       {
                       case "m ":
                       case "r ":
                       case "n ":
                       case "s ":
                           obs = filter_objects( all_inventory(), "id",
                                                 complex_desc[6..] );
                           break;
                           
                       default:
                           obs = 0;
                       }
                       if (obs)
                           break;
                   }
               }

               // Der Normalfall: einzelne ID...
               ob = present( complex_desc, ME );
               // Achtung: dieser Teil setzt das for() fort (continue) und
               // umgeht dabei die Pruefung auf Sichtbarkeit nach dem Ende vom
               // switch(). Aus diesem Grunde muss hier selber geprueft
               // werden.
               if ( meth & POS_INVERS )
               {
                   if ( ob && ob != ME )
                       erg += (filter_objects( all_inventory(), "short" )
                               - ({ ob }) );
                   else
                       erg += filter_objects( all_inventory(), "short" );
               }
               else if ( ob && ob != ME && !ob->QueryProp(P_INVIS) )
                   erg += ({ ob });   //Normalfall: einzelne ID

               continue;

           }  // switch
           // unsichtbare objekte entfernen
           obs = filter_objects( obs, "short" ); 
       } // else

       if ( meth & POS_INVERS )
           erg += ( filter_objects( all_inventory(), "short" ) - obs );
       else
           erg += obs;
   } // for
   return erg;
}

/*
 * returns a list of all found objects inside itself
 * may call same function in objects inside
 *
 * Funktion wird nicht mehr von put_and_get aufgerufen, stattdessen wird
 * direkt present_objects benutzt!
 */ 
object *locate_objects( string complex_desc, int info ) {
    string was, wo;

    if ( sscanf( complex_desc, "%s in %s", was, wo ) == 2 ){
      object *found_obs = ({});
      foreach(object invob: present_objects(wo)) {
        // || ({}) weil invob ein Objekt ohne locate_objects() sein koennte.
        found_obs += (object *)invob->locate_objects( was, info) || ({});
      }
      return found_obs;
    }
    // kein "in" gefunden
    return present_objects( complex_desc );
}


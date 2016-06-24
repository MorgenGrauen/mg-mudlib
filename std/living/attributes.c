// MorgenGrauen MUDlib
//
// living/attributes.c -- attributes for living objects
//
// $Id: attributes.c 8375 2013-02-12 21:52:58Z Zesstra $

#include <sys_debug.h>

// attribute handling
//
//   filter_ldfied: str, dex, con, int
//
// In den Attributen und Abilites werden Rassenspzifische und erworbene
// Faehigkeiten (mit autoload-Eigenschaft) abgepseichert.
//
// Funktionen:
//   SetAttribute( attr, val ) (Erzeuge und) setze das Attribut auf val.
//   QueryAttribute( attr )    Gebe den Wert des Attributes zurueck.
//
//   Wenn ein Objekt eine Funktion _filterattr_<name> beitzt, wird beim Setzen
//   des Attributes <name>, der vorgeschlagene Wert uebergeben und der von
//   dieser Funktion zurueckgegebene gesetzt. (fuer ueberpruefungszwecke)
//   Gleiches gilt fuer _filterabil_<name>.

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <attributes.h>
#include <player/gmcp.h>
#undef NEED_PROTOTYPES

#include <config.h>
#include <properties.h>

mapping attributes; // Dies sind die mit ZTs veraenderbaren Attribute
mapping attributes_modifier; // Modifier sollen gespeichert werden
nosave mixed* attributes_timed_mods; //P_TIMED_ATTR_MOD
nosave mapping attributes_offsets; // Offsets NICHT speichern!
nosave mapping used_attributes_offsets; // Zur Beschleunigung der Berechnung
nosave object* all_modifiers; // objekte mit P_X/M_ATTR_MOD, P_X/M_HEALTH_MOD
nosave object* invalid_modifiers; // objekte welche das limit ueberschreiten
nosave int cumulative_mod; // bilanz der P_X/M_ATTR_MOD
nosave int hp_off;
nosave int sp_off;


nomask public int SetTimedAttrModifier(string key, mapping modifier, 
    int outdated, object dependent, mixed notify) {
  if(!key || key=="" || !modifier || (outdated>0 && outdated<time()) || 
      (notify && !stringp(notify) && !objectp(notify)) ) {
    return TATTR_INVALID_ARGS;
  }
  
  
  if(member(attributes_timed_mods[TATTR_ENTRIES],key)) {
    // change entry
    attributes_timed_mods[TATTR_ENTRIES][key,TATTR_MOD]=modifier;
    attributes_timed_mods[TATTR_ENTRIES][key,TATTR_OUTDATED]=outdated;
    attributes_timed_mods[TATTR_ENTRIES][key,TATTR_DEPENDENT]=dependent;
    attributes_timed_mods[TATTR_ENTRIES][key,TATTR_NOTIFY]=notify;
  }
  else {
    // add entry
    attributes_timed_mods[TATTR_ENTRIES]+=([key:modifier;outdated;dependent;notify]);
  }
  
  // add outdate
  if(outdated>0 && member(attributes_timed_mods[TATTR_OUTDATE],outdated)==-1)
  {
    attributes_timed_mods[TATTR_OUTDATE]+=({outdated});
    attributes_timed_mods[TATTR_OUTDATE]=
      sort_array(attributes_timed_mods[TATTR_OUTDATE],#'<);
  }
  
  // add dependent
  if(objectp(dependent))
  {
  	if(member(attributes_timed_mods[TATTR_DEPENDENTS],key))
  	{
	  attributes_timed_mods[TATTR_DEPENDENTS][key]=dependent;
  	}
  	else
  	{
          attributes_timed_mods[TATTR_DEPENDENTS]+=([key:dependent]);
  	}
  }
  else
  {
  	// remove previously set dependent
  	if(member(attributes_timed_mods[TATTR_DEPENDENTS],key))
  	{
	  attributes_timed_mods[TATTR_DEPENDENTS]-=([key]);
  	}
  }
  
  UpdateAttributes();

  return TATTR_OK;
}

nomask public mapping QueryTimedAttrModifier(string key)
{
  int outdated;
  object dependent;
  mixed notify;
  mapping mod;
  
  if(!key || key=="")
  {
    return ([]);
  }

  if(!member(attributes_timed_mods[TATTR_ENTRIES],key))
  { 
    return ([]);
  }

  mod=deep_copy(attributes_timed_mods[TATTR_ENTRIES][key,TATTR_MOD]);
  outdated=attributes_timed_mods[TATTR_ENTRIES][key,TATTR_OUTDATED];
  dependent=attributes_timed_mods[TATTR_ENTRIES][key,TATTR_DEPENDENT];
  notify=attributes_timed_mods[TATTR_ENTRIES][key,TATTR_NOTIFY];
  
  return ([key:mod;outdated;dependent;notify ]);
}

nomask public int DeleteTimedAttrModifier(string key)
{
  if(!key || key=="")
  {
    return TATTR_INVALID_ARGS;
  }

  if(!member(attributes_timed_mods[TATTR_ENTRIES],key))
  { 
    return TATTR_NO_SUCH_MODIFIER;
  }
  
  attributes_timed_mods[TATTR_DEPENDENTS]-=([key]);
  attributes_timed_mods[TATTR_ENTRIES]-=([key]);
  UpdateAttributes();
  
  return TATTR_OK;
}

nomask protected void attribute_hb()
{
  int now,i,k,update,outdated;
  string* keys;
  mapping tonotify;
	
  // initialize
  now=time();
  tonotify=([]);

  keys=m_indices(attributes_timed_mods[TATTR_ENTRIES]);
  
  // delete outdated 
  for(i=sizeof(attributes_timed_mods[TATTR_OUTDATE])-1;i>=0;i--)
  {
  	outdated=attributes_timed_mods[TATTR_OUTDATE][i];
  	if(outdated>now)
  	{
  	  break;
  	}
  	
  	for(k=sizeof(keys)-1;k>=0;k--)
  	{
  	  if(attributes_timed_mods[TATTR_ENTRIES][keys[k],TATTR_OUTDATED]==outdated)
  	  {
  	    // bei fehlendem notifier wurde das zum verhaengnis
  	    // dank an gloinson
  	    /*
  	    if(objectp(attributes_timed_mods[TATTR_ENTRIES][keys[k],TATTR_NOTIFY]))
  	    {
  	    */
  	    tonotify+=([keys[k]:attributes_timed_mods[TATTR_ENTRIES][keys[k],TATTR_NOTIFY]]);
  	    //}
  	    
	    attributes_timed_mods[TATTR_DEPENDENTS]-=([keys[k]]);
 	    attributes_timed_mods[TATTR_ENTRIES]-=([keys[k]]);
  	    keys-=({keys[k]});
  	  }
  	}
  	
  	attributes_timed_mods[TATTR_OUTDATE]-=({outdated});
  }
  
  // delete depending
  keys=m_indices(attributes_timed_mods[TATTR_DEPENDENTS]);
  for(i=sizeof(keys)-1;i>=0;i--)
  {
  	if(!objectp(attributes_timed_mods[TATTR_DEPENDENTS][keys[i]]))
  	{
  	    // siehe oben
  	    /*
  	    if(objectp(attributes_timed_mods[TATTR_ENTRIES][keys[i],TATTR_NOTIFY]))
  	    {
  	    */
  	    tonotify+=([keys[i]:attributes_timed_mods[TATTR_ENTRIES][keys[i],TATTR_NOTIFY]]);
  	    //}

	    attributes_timed_mods[TATTR_DEPENDENTS]-=([keys[i]]);
 	    attributes_timed_mods[TATTR_ENTRIES]-=([keys[i]]);
  	    keys-=({keys[i]});
  	}
  }
  
  
  // update
  if(sizeof(tonotify))
  {
    UpdateAttributes();
    call_out(#'notifyExpiredModifiers,0,tonotify);
  }
}

nomask protected void notifyExpiredModifiers(mapping nots)
{
  int i;
  string* keys;
  while(remove_call_out(#'notifyExpiredModifiers)!=-1);
  
  if(!nots)
  {
    return;
  }
  
  keys=m_indices(nots);
  for(i=sizeof(nots)-1;i>=0;i--)
  {
  	if(nots[keys[i]] && 
	    ( objectp(nots[keys[i]])||stringp(nots[keys[i]]) ) )
  	{
  	   call_other(nots[keys[i]],"NotifyTimedAttrModExpired",keys[i]);
  	}
  }
}


// invalide modifier benachrichtigen
nomask protected void notifyInvalidModifiers() {

  while(remove_call_out(#'notifyInvalidModifiers)!=-1);
  
  if(!invalid_modifiers) {
    invalid_modifiers=({});
  }
  
  call_other(invalid_modifiers,"NotifyXMAttrModLimitViolation");

}


// welche Modifier / modif. Objekte wirken nun?
protected nomask void calculate_valid_modifiers() {
  closure qp;
  mapping res;	
  string key;	
  int wert;
  // Unterscheidung Bonus <-> Malus, weil der Malus voll eingehen soll	
  int hp_malus, sp_malus;
 
  used_attributes_offsets=([]);
  cumulative_mod=0;
  hp_off=sp_off=0;
  invalid_modifiers=({});

  // rassenspezifische boni P_ATTRIBUTES_OFFSETS  
  if ( mappingp(attributes_offsets) )
    used_attributes_offsets+=attributes_offsets;

  if (!pointerp(all_modifiers) || !sizeof(all_modifiers)) {
    // in diesem Fall koennen wir hier direkt mit dieser Funktion Schluss
    // machen. ;-)
    return;
  }
  else
    all_modifiers-=({0}); //zerstoerte Objekte rauswerfen.
    
  // einmal ueber alle modifizierenden Objekt iterieren und aufaddieren
  foreach(object ob: all_modifiers) {
    qp = symbol_function("QueryProp",ob);
 
    if (!objectp(ob) || environment(ob)!=this_object()) {
      all_modifiers-=({ob});
      continue;
    }
 		
    // ext. Attribut-Modifier
    if ( mappingp(res=funcall(qp,P_X_ATTR_MOD)) ) {		
      foreach(key, wert: res) {
        cumulative_mod+=wert;
      }
    }

    // magic Attribut-Modifier
    if ( mappingp(res=funcall(qp,P_M_ATTR_MOD))
        && ( funcall(qp,P_WORN)
            || funcall(qp,P_WIELDED) ) ) {		
      foreach(key, wert: res) {
        cumulative_mod+=wert;
      }
    }
    // Magic Health-Modifier 		
    if ( mappingp(res=funcall(qp,P_M_HEALTH_MOD))
        && ( funcall(qp,P_WORN)
            || funcall(qp,P_WIELDED) ) ) {
      if (res[P_HP] < 0)
        hp_malus += res[P_HP];
      else
        hp_off+=res[P_HP];

      if (res[P_SP] < 0)
        sp_malus += res[P_SP];
      else
        sp_off+=res[P_SP];
      }
	      
    // external Health Modifier
    if ( mappingp(res=funcall(qp,P_X_HEALTH_MOD)) ) {
      if (res[P_HP] < 0)
        hp_malus += res[P_HP];
      else
        hp_off+=res[P_HP];

      if (res[P_SP] < 0)
        sp_malus += res[P_SP];
      else
        sp_off+=res[P_SP];	    
    }	
  
  } // Ende 1. foreach()
  
 
  // und nochmal, soviele Objekt wieder rausschmeissen, bis das Limit wieder
  // unterschritten wird. (Verbesserungsvorschlaege erwuenscht.) :-(
  foreach(object ob: all_modifiers) {

    qp = symbol_function("QueryProp",ob);
 
    if ( mappingp(res=funcall(qp,P_X_ATTR_MOD)) ) {
      if(cumulative_mod>CUMULATIVE_ATTR_LIMIT) { 
        invalid_modifiers+=({ob});	  
	foreach(key, wert: res) {
          cumulative_mod-=wert;
        }
      }
      else {
        add_offsets(res);
      }
    }

    if ( mappingp(res=funcall(qp,P_M_ATTR_MOD))
        && ( funcall(qp,P_WORN)
            || funcall(qp,P_WIELDED) ) ) {
      if(cumulative_mod>CUMULATIVE_ATTR_LIMIT) {     
        if(member(invalid_modifiers,ob)==-1) {
          invalid_modifiers+=({ob});
        }			
	foreach(key, wert: res) {
          cumulative_mod-=wert;
        }
      }
      else {
        add_offsets(res);
      }
    }
  }

  // HEALTH_MOD werden durch eine Formel 'entschaerft', damit man nur schwer
  // das Maximum von 150 erreichen kann. (Formel von Humni, beschlossen auf 3.
  // EM-Treffen)
  // Mali gehen aber voll ein
  hp_off = (int)(150 - (150*150.0/(150 + hp_off))) + hp_malus;
  sp_off = (int)(150 - (150*150.0/(150 + sp_off))) + sp_malus;

  /* alte Version
    hp_off += hp_malus;
    sp_off += sp_malus;
  */
 
  // notify invalid modifiers
  if(sizeof(invalid_modifiers)>0) {
    call_out(#'notifyInvalidModifiers,0);
  }
}

// abmelden eines modifiers 
nomask public void deregister_modifier(object modifier)
{ 
  if (!pointerp(all_modifiers)) {
    return;
  }

  // valid object?
  if (!objectp(modifier) || member(all_modifiers,modifier)==-1) {
    return;
  }
  
  all_modifiers-=({modifier});
  if (invalid_modifiers) {
    invalid_modifiers-=({modifier});
  }
}

// anmelden eines modifiers 
nomask public void register_modifier(object modifier) { 
  closure qp;
  
  if (!pointerp(all_modifiers)) {
    all_modifiers=({});
  }

  // valid object?
  if (!objectp(modifier) || environment(modifier)!=this_object() || 
      member(all_modifiers,modifier)!=-1) {
    return;
  }
  
  qp = symbol_function("QueryProp",modifier);
  // modifier after all? Die P_M_* muessen getragen/gezueckt sein.
  if(mappingp(funcall(qp,P_X_ATTR_MOD)) ||
     mappingp(funcall(qp,P_X_HEALTH_MOD)) ||
    ((mappingp(funcall(qp,P_M_ATTR_MOD)) ||
       mappingp(funcall(qp,P_M_HEALTH_MOD))) &&
      (funcall(qp,P_WORN) || funcall(qp,P_WIELDED)) ) ) {
    all_modifiers+=({modifier});
  }
}

protected void add_offsets(mapping arr) {
  mixed *ind;
  int i;
  
  if ( !mappingp(arr) )
    return;

  foreach(string key, int wert: arr) {
    used_attributes_offsets[key]+=wert;
  }
}

public void UpdateAttributes() {
  mixed   *ind;
  int     i;
  
  // alle gueltigen Modifier ermitteln aus Objekten im Inventar.
  calculate_valid_modifiers();

  // persistente modifier drauf (frosch zb)
  // aus P_ATTRIBUTES_MODIFIERS
  if ( mappingp(attributes_modifier) ) {
    foreach(mixed key, mapping wert: attributes_modifier) {
      add_offsets(wert); // Modifier addieren...		
    }
  }
  // timed modifier drauf aus P_TIMED_ATTR_MOD
  ind=m_indices(attributes_timed_mods[TATTR_ENTRIES]);
  for ( i=sizeof(ind)-1 ; i>=0 ; i-- ) {
    // Modifier addieren...
    add_offsets(attributes_timed_mods[TATTR_ENTRIES][ind[i],0]);
  }
  // Bei Monstern werden die HP/SP ueblicherweise selbst gesetzt
  if ( !query_once_interactive(this_object()))
    return;

  SetProp(P_MAX_HP, QueryAttribute(A_CON)*8+42+hp_off);
  SetProp(P_MAX_SP, QueryAttribute(A_INT)*8+42+sp_off);
  
  if(QueryProp(P_HP)>QueryProp(P_MAX_HP)) {
    SetProp(P_HP,QueryProp(P_MAX_HP));
  }
  
  if(QueryProp(P_SP)>QueryProp(P_MAX_SP)) {
    SetProp(P_SP,QueryProp(P_MAX_SP));
  }

  GMCP_Char( ([ A_INT: QueryAttribute(A_INT),
                A_DEX: QueryAttribute(A_DEX),
                A_STR: QueryAttribute(A_STR),
                A_CON: QueryAttribute(A_CON) ]) );
}


protected void create() {
  hp_off=sp_off=0;
  used_attributes_offsets=([]);
  all_modifiers=({});
  invalid_modifiers=({});
  cumulative_mod=0;
  
  Set(P_ATTRIBUTES_MODIFIER, attributes_modifier=([])); // nicht geschuetzt
  Set(P_ATTRIBUTES_OFFSETS, attributes_offsets=([]));
  Set(P_ATTRIBUTES_OFFSETS, PROTECTED, F_MODE);
  Set(P_ATTRIBUTES, attributes=([]));
  Set(P_ATTRIBUTES, PROTECTED, F_MODE);

  Set(P_TIMED_ATTR_MOD, attributes_timed_mods=({ ({}),([]),([]) }));
  Set(P_TIMED_ATTR_MOD, NOSETMETHOD|SECURED, F_MODE_AS);
}

static mixed _query_timed_attr_mod() {
  mixed ret;
  return Set(P_TIMED_ATTR_MOD,	    
      ({attributes_timed_mods[0],
	deep_copy(attributes_timed_mods[1]),
	deep_copy(attributes_timed_mods[2])}));
}

static mapping _set_attributes(mapping arr) 
{
  Set(P_ATTRIBUTES, attributes=arr);
  UpdateAttributes();
  return arr;
}

static mapping _query_attributes() 
{
  return deep_copy(Set(P_ATTRIBUTES, attributes)); 
}

static mapping _set_attributes_offsets(mapping arr) 
{
  Set(P_ATTRIBUTES_OFFSETS, attributes_offsets=arr);
  UpdateAttributes();
  return attributes_offsets;
}

static mapping _query_attributes_offsets() 
{ 
  return deep_copy(Set(P_ATTRIBUTES_OFFSETS, attributes_offsets));
}

static mixed _set_attributes_modifier(mixed arr) 
{ string fn;
  mixed pre;
  mapping map_ldfied;
  
  if ( pointerp(arr) && (sizeof(arr)>=2) )
  {
    pre=arr[0];
    map_ldfied=arr[1];
  }
  else 
  {
    pre=previous_object();
    map_ldfied=arr;
  }

  if ( objectp(pre) )
    fn=old_explode(object_name(pre),"#")[0];
  else
    fn=pre;

  if ( !stringp(fn) )
    return 0;

  // wenn Modifier kein mapping oder ein leeres Mapping: loeschen
  if ( !mappingp(map_ldfied) || !sizeof(map_ldfied))
    m_delete(attributes_modifier,fn);
  else
    attributes_modifier[fn]=map_ldfied;

  Set(P_ATTRIBUTES_MODIFIER, attributes_modifier);
  UpdateAttributes();
  return attributes_modifier[fn];
}

static mapping _query_attributes_modifier() 
{
  return deep_copy(attributes_modifier);  
}

public int SetAttr(string attr, int val) 
{ closure filter_ldfied;

  if ( filter_ldfied = symbol_function("_filterattr_"+attr, this_object()) ) 
    val = funcall(filter_ldfied, val );

  attributes[attr] = val;
  UpdateAttributes();
  return val;
}

public int SetAttribute(string attr, int val) 
{
  return SetAttr(attr, val-used_attributes_offsets[attr]);
}

// Diese Funktion sollte zum Abfragen verwendet werden:
public int QueryAttribute(string attr) 
{ int re;

  re=attributes[attr]+used_attributes_offsets[attr];

  if ( query_once_interactive(this_object()) && (re>30) )
    re=30;

  return re;
}

public int QueryRealAttribute(string attr) 
{
  return attributes[attr];
}

public int SetRealAttribute(string attr, int val) 
{
  return SetAttr(attr, val);
}

public int QueryAttributeOffset(string attr) 
{
  return used_attributes_offsets[attr];
}

public status TestLimitViolation(mapping check)
{
  int k,test;
  mixed* ind;
    
  if(!check || !mappingp(check))
  {
    return 0;
  }

  test=0;
  ind=m_indices(check);
  for( k=sizeof(ind)-1 ; k>=0 ; k-- )
  {
    test+=check[ind[k]];
  }
  
  test+=cumulative_mod;
  if(test>CUMULATIVE_ATTR_LIMIT)
  {
    return 1;
  }
  
  return 0;
}

/*
 *------------------------------------------------------------
 * attributes compatibility functions
 */

protected int _filterattr_str(int val)
{
  return ( (val<0) ? 0 : (val>20) ? 20 : val );
}

protected int _filterattr_dex(int val)
{
  return ( (val<0) ? 0 : (val>20) ? 20 : val );
}

protected int _filterattr_int(int val)
{
  return ( (val<0) ? 0 : (val>20) ? 20 : val );
}

protected int _filterattr_con(int val)
{
  return ( (val<0) ? 0 : (val>20) ? 20 : val );
}

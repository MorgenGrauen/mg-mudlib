#pragma strong_types,save_types
#pragma no_clone,no_shadow

#include <wizlevels.h>

#define DB_NAME "/p/daemon/save/routingd"
#define MAX_LEN 50
#define TP this_player()

mapping domains;
mapping exits;
mapping targets;
static mapping directions;
static mapping reverse_directions;

void create() {
    seteuid(getuid());
  if (!restore_object(DB_NAME)) {
    domains=(["/d/":1]);
    exits=([]);
    targets=([]);
  }

  // Umsetzung zur Speicherersparnis:
  directions=(["norden":1,
	       "nordosten":2,
	       "osten":3,
	       "suedosten":4,
	       "oben":5,
	       "sueden":-1,
	       "suedwesten":-2,
	       "westen":-3,
	       "nordwesten":-4,
	       "unten":-5
	       ]);
  reverse_directions=([1:"norden",
		       2:"nordosten",
		       3:"osten",
		       4:"suedosten",
		       5:"oben",
		       -1:"sueden",
		       -2:"suedwesten",
		       -3:"westen",
		       -4:"nordwesten",
		       -5:"unten"
		       ]);
}

// Fuer Debug-Zecke:
mixed query_exits(string room) {return exits[room];}
mixed query_targets(string typ) {return targets[typ];}

int AddDomain(string dom, int val) {
  // Fuegt neue Domain hinzu, in der Routing moeglich sein soll.
  // +1: Einschliessen
  // -1: Ausschliessen
  if (!IS_ARCH(TP)
      || val<-1
      || val>1)
    return 0;
  if (!val)
    domains=m_copy_delete(domains,dom);
  else
    domains[dom]=val;
  save_object(DB_NAME);
  return 1;
}

int RegisterTarget(string type, string fname) {
  // Registriert Raum als zu einer Gruppe von Zielen zugehoerig
  // (z.B. RegisterTarget("pub","/d/ebene/room/PortVain/pub2"))
  mapping map_ldfied;

  if (!type || !fname)
    return 0;
  type=lower_case(type);
  if (type[0..1]!="##")
    type="##"+type;
  if (!mappingp(map_ldfied=targets[type]))
    map_ldfied=([]);
  map_ldfied[fname]=1;
  targets[type]=map_ldfied;
  return 1;
}

int RegisterExit(string start, mixed richtungen, string ziel) {
  // Registriert Ausgang in der angegebenen Richtung von Start nach Ziel,
  // wenn Routing dort moeglich sein soll.
  string *dirs;
  int i,sz,ok,pos;
  mixed ex,x,richtung;
  
  if (stringp(richtungen))
    richtungen=({richtungen});
  if (!start || !pointerp(richtungen) || !ziel)
    return 0;
  dirs=old_explode(start,"/");sz=sizeof(dirs)-1;x="/";ok=0;
  for (i=0;i<sz;i++) {
    x=x+dirs[i]+"/";
    if (domains[x]==-1) // Subdomain ausgeschlossen?
      return 0;
    if (domains[x]==1) // Domain erlaubt?
      ok=1;
  }
  if (!ok)
    return 0;

  if (!pointerp(ex=exits[start])) {
    if (file_size(start+".c")<=0) // Beim ersten Ausgang auf VC testen
      return 0; // Keine Virtual Compiler Raeume routen
    ex=({});
  }
  for (i=sizeof(richtungen)-1;i>=0;i--) {
    richtung=richtungen[i];
    if (!(x=directions[richtung]))
      x=richtung;
  
    pos=member(ex,x);
    if (pos>=0 && pos+1<sizeof(ex)) // Richtung schon eingetragen?
     ex[pos+1]=ziel; // Wurde umdefiniert
    else
      ex+=({x,ziel}); // ({ richtung, ziel })
  }
  exits[start]=ex;
  return 1;
}

// Low-Level FindRoute :-)
mapping do_find_route(string start, mapping is_target) {
  mapping erreicht;
  int i,j,k;
  mixed *newx, *actx, ex;
  string room, dir, x;
  
  erreicht=([start:"##START"]);
  newx=({start});
  for (k=MAX_LEN;k>=0;k--) {
    actx=newx; // In diesem Schritt die neu erreichten Raeume pruefen
    newx=({}); // Noch keine Raeume neu erreicht
    for (i=sizeof(actx)-1;i>=0;i--) {
      room=actx[i];
      // printf("%O\n",room);
      if (!pointerp(ex=exits[room]))
	continue;
      // printf("-> %O\n",ex);
      for (j=sizeof(ex)-1;j>=1;j-=2) {
	x=ex[j]; // Zielraum
	// printf("-> %O\n",x);
	if (!erreicht[x]) { // Wurde bisher noch nicht erreicht
	  erreicht[x]=({room,ex[j-1]});
	  if (is_target[x]) { // Ein Ziel wurde erreicht
	    erreicht["##ZIEL"]=erreicht[x]; // Kann ja auch Gruppe sein
	    return erreicht;
	  }
	  newx+=({x});
	}
      }
    }
  }
  return 0;
}

varargs mixed FindRoute(string start, string ziel, int etyp) {
  mapping res,ziele;
  string x;
  mixed ex,r,result;
  int k;
  
  if (!start || !ziel)
    return 0;
  if (!mappingp(ziele=targets[ziel]))
    ziele=([ziel:1]);
  if (!mappingp(res=do_find_route(start, ziele)))
    return 0; // nicht gefunden
  if (etyp)
    result=({});
  else
    result=([]);
  x="##ZIEL";
  for (k=MAX_LEN+2;k>=0;k--) {
    ex=res[x];
    if (ex=="##START") // Fertig.
      return result;
    if (!(r=reverse_directions[ex[1]])) // richtige Richtungen eintragen.
      r=ex[1];
    x=ex[0];
    if (etyp)
      result=({r})+result;
    else
      result[x]=r;
  }
  return 0;
}

void save() {
	save_object(DB_NAME);
}

void reset() {
	set_next_reset(43200); // Alle 12 Stunden speichern reicht
	save();
}

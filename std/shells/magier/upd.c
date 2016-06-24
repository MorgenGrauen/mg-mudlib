// MorgenGrauen MUDlib
//
// upd.c
//
// $Id: upd.c 8850 2014-06-13 21:34:44Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <magier.h>
#include <player.h>
#undef NEED_PROTOTYPES
#include <debug_info.h>
#include <wizlevels.h>
#include <moving.h>
#include <properties.h>
#include <logging.h>
#include <thing/properties.h>

varargs static int _make(string file, int flags, int recursive);

static mixed _query_localcmds()
{
  return ({({"upd","_upd",0,LEARNER_LVL}),
           ({"load","_load",0,LEARNER_LVL})});
}

//
// _save(): Spieler in Rettungsraum retten
// obj:      Spielerobjekt(?)
// inv_save: Rettungsraumname 
// Rueckgabe: 0 wenn kein Spielerobjekt
//            Spielerobjekt, falls doch
//

static mixed _save( object obj, object inv_saver )
{ 
    if ( query_once_interactive(obj) )
      { 
        obj->move( inv_saver, NO_CHECK );
        return obj;
      }
    return 0;
}


//
// _reload(): Objekt laden
// file:  Filename. Muss in der Form /xx/xx/xx.c vorliegen
// clone:  > 0 -> Es soll geclont werden, enthaelt Objektnummer des
//                Vorgaengerobjektes
// flags: Kommandozeilenoptionen
// err:   Leerstring als Referenz uebergeben. Enthaelt nach dem
//        Aufruf vom _reload() die Fehlermeldungen als String.
// Rueckgabe: Das neu erzeugte Objekt bzw. das schon vorhandene Objekt
//            bzw. 0, wenn Fehler auftrat.
//

private object _reload(string file, int clone, int flags, string err)
{
  object obj;
  
  if (!obj=find_object(file[0..<3]+(clone?("#"+clone):"")))
  {
    int pos,pos2;
    string bt;

    if(file_size(file)<0)
    {
      if (file_size(file)==-1)
        err = sprintf("upd: %s: Datei existiert nicht.\n", file);
      else // directory
        err = sprintf("upd: %s: Verzeichnisse koennen nicht geladen "
                      "werden.\n",file);
      return obj; // 0
    }
    pos = max(file_size(__DEBUG_LOG__),0);

    if ((err = (clone?catch(obj = clone_object(file)):
                catch(load_object(file)) )) && (flags & UPD_B))        
    {     
      if (( pos2=file_size(__DEBUG_LOG__)) > pos )
        err+=sprintf("\nBacktrace:\n%s",
                     read_bytes(__DEBUG_LOG__,pos, pos2-pos ));
      else
        err+=sprintf("\nKEIN BACKTRACE VERFUEGBAR!\n");
    }
    if (!err&&!obj&&(!obj = find_object(file[0..<3])))
      err += sprintf( "upd: %s: Blueprint nach dem Laden zerstoert.\n",file );
  }
  else
    err=sprintf("upd: Objekt existiert schon: %O\n",obj);
  return obj;
}


//
// _update(): File updaten -> Blueprint destructen
// file:  Filename
// dummy: simulieren? (1->wird nicht aktualisiert)
// flags: Kommandozeilenoptionen
// Rueckgabe: -1: Keine Vollzugriff
//             0: Objekt ist nicht geladen
//             1: Operation wird durchgefuehrt
//

private varargs int _update(string file, int dummy, int flags)
{
  object obj;
  string err;
  if (!dummy && !objectp(obj = find_object(file))) return 0;
  if (!IS_ARCH(this_object()))
  {
    // Schreibrechte nur pruefen, wenn echt aktualisiert werden soll.
    if(!dummy && !MAY_WRITE(file))
      return (printf("upd: %s: Keine Schreibrechte!\n",file), -1);
    if(!MAY_READ(file)) 
      return (printf("upd: %s: Keine Leserechte!\n", file), -1);
  }
  if (dummy) return 1;
  
  if ( flags & UPD_D )
  {
    object *inv;
    if (sizeof(inv = deep_inventory(obj)))
    {
      printf("upd: %s: Entferne Objekte im Inventar\n", file );
      if (!(flags&UPD_H))
      {
        if(err=catch(filter_objects( inv, "remove", 1 )))
          printf("upd: %s: Fehlgeschlagen. Grund:\n%s\n",
                 file,err);
      }
      if (sizeof(inv = deep_inventory(obj)))
        filter(inv, function void (object ob)
            {destruct(ob);});
    }
  }
  if (!(flags&UPD_H))
  {
    if(err = catch(obj->remove()))
      printf("upd: %s: remove() fehlgeschlagen. Aufruf von " +
             "destruct().\n",file);
  }
  if(objectp(obj)) destruct(obj);
  return 1;
}


//
// _instance_upd(): Alle Objekte nach Clones des Objekts durchsuchen
// file:  Filename des Objektes
// flags: Kommandozeilenargumente
// obj:   Aktuelles Objekt
// instances: Zahl der gefundenen Instanzen
//

private void _instance_upd(string file, int flags, mixed obj, int instances,
                           int firstcall)
{
 int i;
 if (firstcall)
    printf("upd: %s: %s Instanzen.\n",file,flags&UPD_A?"Aktualisiere":"Suche");
  
  while (get_eval_cost()>220000 && i < sizeof(obj))
  {
    if (!objectp(obj[i]))
      instances--;
    else
    {
      if (flags&UPD_F&&!(flags&UPD_S))
        printf( "upd: %O gefunden in %s\n", obj[i],
                environment(obj[i])?object_name(environment(obj[i])) 
                : "keiner Umgebung" );
      if (flags&UPD_A) _make(object_name(obj[i]), flags & ~(UPD_A|UPD_F),1 );
    }
    i++;
  }
  if (i < sizeof(obj)) 
    call_out( #'_instance_upd/*'*/,2,file,flags,obj[i..],instances,0);
  else
    printf( "upd: %s: %d Instanzen %s\n", file, instances,
            (flags & UPD_A) ? "aktualisiert" : "gefunden" );
  return;
}
  
  
//
// _do_make(): Alle geerbten Objekte bearbeiten (fuer -m/-v)
// file:  Name des Files
// clone: 0, wenn blueprint, ansonsten Clonenummer
// flags: Kommandozeilenparameter
// dep:   geschachteltes Array mit Meldungen (wg. Rekursion)
// ready: Array der schon bearbeiteten Objekte (wg. Rekursion)
// Rueckgabe: Array der Objektnamen, die bearbeitet wurden
//            (Array in Array in ...)
// 

varargs private int _do_make( string file,int clone,int flags,mixed dep,
                              string *ready )
{
  object obj;
  string err;
  string *ilist;
  mixed downdeps;
  int ret;
  
  if (!pointerp(ready)) ready = ({});
  ready += ({ file });
  
  if ( !(obj = _reload(file,clone,flags,&err)))
  {
    dep += ({ err });
    return 0;
  }
  
  ilist = inherit_list(obj)-ready;
  
  downdeps = ({});
  
  while (sizeof(ilist))
  {
    ret = _do_make( ilist[0],0,flags, &downdeps, &ready )||ret;
    ilist[0..0] = ({});
    ilist -= ready;
  }
  
  if ( ret||file_time(file)>program_time(obj)||(flags &UPD_I))
    if ( _make( file, flags & ~(UPD_M|UPD_I) ,1) < 0 ) 
      dep = ({ "{" + explode(file,"/")[<1] + "}", downdeps });
    else{
      dep = ({ "[" + explode(file,"/")[<1] + "]", downdeps });
      ret = 1;
    }
  else if (flags&UPD_V) dep += ({ explode(file,"/")[<1], downdeps });
  return ret;
}


//
// _make_dep(): Ausgabe des Ererbungsbaumes
// Objekte im Array dep
// prefix enthaelt Zeilenanfang (fuer Rekursion)
// Rueckgabe: String mit dem Vererbungsbaum des Objektes
//

private string _make_dep( mixed dep, string prefix )
{
  string ret;
  int i, size;
  
  ret="";
  size=sizeof(dep);
  for (i=0; i<size;i++)
    if (pointerp(dep[i]))
      ret += _make_dep(dep[i],prefix + (i < (size-1) ? "| ":" "));
    else 
      ret += prefix + "+-" + dep[i] + "\n";
  return ret;
}


//
// _illegal_closure(): ist closure in arg an objekt gebunden?
// arg: closure(-array/mapping)
// Rueckgabe: 0 wenn alles okay
//            1 wenn closure geloescht werden muss
//

private int _illegal_closure( mixed arg )
{
  int i, j;
  string *indices;
  
  if ( closurep(arg) && !objectp(query_closure_object(arg)) )
    return 1;
  
  if ( pointerp(arg) ){
    for ( i = sizeof(arg); i--; )
      if ( _illegal_closure(arg[i]) )
        return 1;
  }
  else if ( mappingp(arg) ){
    indices = m_indices(arg);
    for ( i = sizeof(indices); i--; )
      for ( j = get_type_info( arg, 1 ); j--; )
        if ( _illegal_closure(arg[indices[i], j]) )
          return 1;
  }
  return 0;
}

//
// _make(): Update file
// file:  Filename
// flags: Kommandozeilenargumente
//

varargs static int _make(string file, int flags,int recursive)
{
  string msg, err, blue;
  int inst;
  object obj, inv_saver;
  mixed tmp;
  
  msg = "";

  if (!file) return printf( "upd: Kein Filename uebergeben!\n" ), RET_FAIL;
    
// Filename in Blue, Objektname in blue, Instanznummer in inst

  if (sscanf(file,"%s#%d",blue,inst)==2) blue += ".c";
  else blue = file + (file[<2..]==".c" ? "" : ".c");

// Alle Instanzen durchsuchen im Falle von -a oder -f 

  if ((flags & UPD_LOAD)&&find_object(file))
    return printf("load: %s: Objekt ist schon geladen.\n",file),RET_OK;
  
  if ( flags & (UPD_F|UPD_A))
  {
    if (inst) return printf( "upd: %s: Eine Instanz kann keine " +
                             "Clones haben.\n",file ), RET_FAIL;
    if ((tmp=_update(file, 1,flags))==-1) 
      return printf( "upd: %s: Kein Vollzugriff auf die " +
                     "Datei erlaubt.\n",file), RET_FAIL;
    if (tmp==0) return RET_FAIL;

   tmp=clones(blue[0..<3],2);
    if (sizeof(tmp))
      call_out( #'_instance_upd/*'*/, 0, file,flags,tmp,sizeof(tmp),1);
    else
      printf( "upd: %s: Keine Clones vorhanden!\n", blue[0..<3]);

    if ( (flags & UPD_F) && !(flags &(UPD_R|UPD_L|UPD_LOAD))) return RET_OK; 
    // Nichts laden -> Auch kein Backup
  }

// Backupraum festlegen
   
  if( blue==INV_SAVE ) {
                printf("upd: Achtung: Raum zum Zwischenspeichern soll geladen werden?!\n");
        } 
  if ( !(inv_saver=load_object(INV_SAVE)) )
  {
    printf("upd: %s: Raum zum Zwischenspeichern des " +
           "Rauminhalts ist nicht ladbar.\n" +
           "         %s\n",file,INV_SAVE);
    return RET_FAIL;
  }

// Wenn das Objekt existiert bzw. Deep aktualisiert werden soll

  if ( (!(flags&UPD_LOAD))&&
       ((obj = find_object(file)) || (flags & (UPD_M|UPD_I))))
  {
    object *inv, env, *pl_inv;
    mapping pro;
    int i;
    mixed configdata;
    int restore_config;

    // Wenn Objekt existiert, dann Inhalt und ggf. Daten aus Configure() oder
    // Props sichern:
    if (obj)
    {
      // im Fall UPD_C erfolgt _kein_ Auslesen und _keine_ Restauration
      // mittels Configure()
      if ( ! (flags & UPD_C ) )
      {
        catch(restore_config=(mixed)call_resolved(&configdata,obj,
                                                  "Configure",0);
              publish);
        // Wenn UPD_CONF gesetzt wird, _muss_ das Objekt ein oeffentliches
        // Configure() definieren, sonst erfolgt Abbruch.
        if ((flags & UPD_CONF) && !restore_config)
        {
          printf("upd: %s: hat kein Configure(), Zerstoerung abgebrochen.\n",file);
          return RET_FAIL;
        }
      }
      if (!(flags&UPD_D)&&(flags&(UPD_L|UPD_R)))
      {
        if (i=sizeof(inv=(all_inventory(obj)-({0}))))
        {
          mixed items;
          // Herausbekommen, ob hier Items existieren, die per AddItem 
          // erzeugt werden. Die duerfen nicht gesichert werden.
          items=(mixed)obj->QueryProp(P_ITEMS); // mixed, da array of arrays
          if (pointerp(items)&&sizeof(items))
          {
            items=transpose_array(items)[0];
            while (i--)
              if (member(items, inv[i])==-1)
                    inv[i]->move(inv_saver,NO_CHECK);
          }
          else // In diesem Objekt sind keine Items gesetzt.
            while (i--) inv[i]->move(inv_saver,NO_CHECK);
        }
      }
      else
      {
        inv=map( deep_inventory(obj), #'_save/*'*/, inv_saver )-({0});
      }
      env = environment(obj);
      if ( flags & UPD_C )
      {
        pro = (mapping)(obj->QueryProperties());
      }
    }
    else inv = ({});

// Ererbte Objekte durchsuchen.
    if ( flags & (UPD_M|UPD_I) )
    {
      mixed dep;
      dep = ({});
      _do_make( blue, inst, flags & ~(UPD_M|UPD_L|UPD_R|UPD_F|UPD_A), &dep );
      printf( _make_dep( dep, "" ) + "\n" );
    }
   
// Tatsaechlichen Update durchfuehren
   
    if ( _update(file, 0, flags)< 0) return RET_FAIL;
    msg += (inst ? "zerstoert" : "aktualisiert");
    
// Neu laden ??
    if ( flags & (UPD_R|UPD_L) )
    {
      if ( obj = _reload( blue,inst,flags, &err ) )
        msg += ", " + (inst ? "neu geclont" : "neu geladen");
     
// Neu geladen: Properties wiederherstellen, Closures filtern 
      if (!err)
      {
        if (!obj) obj = find_object(file);
        // Wenn gewuenscht, Props zurueckschreiben (hat Prioritaet vor
        // Configure(), weil explizit vom Magier angefordert).
        if ( pro && (flags & UPD_C) )
        {
          string *names;
          
          names = m_indices(pro);
          
          // Closures in (mittlerweile) zerstoerten Objekten
          // rausfiltern, damit die (hoffentlich korrekten) Closures
          // im neu geclonten Objekt erhalten bleiben
          for ( i = sizeof(names); i--; )
            if ( _illegal_closure(pro[names[i], F_VALUE]) ||
                 _illegal_closure(pro[names[i], F_QUERY_METHOD]) ||
                 _illegal_closure(pro[names[i], F_SET_METHOD]) )
              m_delete( pro, names[i] );
          
          obj->SetProperties(pro);
          msg += ", Properties gesetzt";
        }
        // Wenn kein UPD_C, wird ggf. das Ergebnis von Configure() wieder
        // uebergeben.
        else if (restore_config)
        {
          int conf_res;
          if (!catch(conf_res=(int)obj->Configure(configdata); publish)
              && conf_res == 1)
          {
            msg += ", (re-)konfiguriert";
          }
          else
          {
            msg += ", (Re-)Konfiguration fehlgeschlagen";
            if (flags & UPD_CONF)
              printf("upd: Daten von %s konnten nicht rekonfiguriert werden: "
                           "%O\n", file, configdata);
          }
        }
        if (env)
        {
          if ( obj->move( env, NO_CHECK ) <= 0 )
            printf( "upd: /%O konnte nicht in /%O zurueckbewegt werden\n",
                    obj, env );
          else
            msg += ", bewegt";
        }
        if (i=sizeof(inv))
        {
          while(i--) if (inv[i]) inv[i]->move(obj, NO_CHECK );
          msg += ", Inhalt zurueckbewegt";
        }
      }
      else 
        return printf( "upd: %s: %s", file, err ), RET_FAIL;
    }
  }
  else 
    if ( !_update(file, 0, flags) && (flags & (UPD_L|UPD_LOAD)) )
      if ( !_reload( blue, inst, flags, &err ) )
        return printf( "%s: %s: %s", (flags&UPD_LOAD?"load":"upd"),file, err ),
          RET_FAIL;
      else
        msg += "geladen";
  
  if ( sizeof(msg)&&!(flags&UPD_S&&recursive) )
    printf("%s: %s: %s.\n",(flags&UPD_LOAD?"load":"upd"),file,msg);
  return RET_OK;
}

//
// _upd: Objekte laden, zerstoeren und aktualisieren
//

static int _upd(string cmdline)
{
  int flags;
  mixed *args;

  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,UPD_OPTS,1);
  if(flags==-1||!sizeof(args))
    return USAGE("upd [-"+UPD_OPTS+"] <datei> [<datei> ..]");
  if ((flags & UPD_C) && (flags & UPD_CONF))
  {
    printf("upd: -c und -C gleichzeitig werden nicht unterstuetzt.\n");
    return 1;
  }
  args=file_list(args,MODE_UPD,0,"/");
  if(!sizeof(args)) return printf("upd: Keine passende Datei gefunden!\n"),1;

  args=map(args,(: $1[FULLNAME] :))-({0});

  if(!sizeof(args))
  {
    printf("upd: Verzeichnisse koennen nicht aktualisiert werden!\n");
    return 1;
  }
  asynchron(args,#'_make,flags,0,0);
  return 1;
}

//
// _load: Objekte laden
//

static int _load(string cmdline)
{
  int flags;
  mixed *args;

  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,"",1);
  if(flags==-1||!sizeof(args))
    return USAGE("load <datei> [<datei> ..]");
  args=file_list(args,MODE_UPD,0,"/");
  if(!sizeof(args)) return printf("load: Keine passende Datei gefunden!\n"),1;
  args=map(args,(: (($1[FILESIZE]!=-2||find_object($1[FULLNAME]))?
                          $1[FULLNAME]:0) :))-({0});

  if(!sizeof(args))
    return printf("load: Verzeichnisse koennen nicht geladen werden!\n"),1;
  asynchron(args,#'_make,UPD_LOAD,0,0);
  return 1;
}

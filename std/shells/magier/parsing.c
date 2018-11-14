// $Id: parsing.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types, rtt_checks
#pragma range_check, pedantic
#pragma no_clone

protected functions virtual inherit "/std/util/path";

#include <files.h>
#include <wizlevels.h>
#include <logging.h>
#include <regexp.h>
#include <defines.h>
#define NEED_PROTOTYPES
#include <magier.h>
#include <thing/properties.h>
#include <player.h>

//
// glob2regexp: Argument von glob in Regexp umwandeln
// str: Argument (als Referenz)
//

static string glob2regexp(string str)
{
  str=regreplace(str,"([\\.\\^\\$\\[\\]\\(\\)])","\\\\\\1",RE_TRADITIONAL|1);
  str=regreplace(str,"\\*",".*",RE_TRADITIONAL|1);
  return sprintf("^%s$",regreplace(str,"?",".",RE_TRADITIONAL|1));
}

//
// to_filename: Argument in Dateinamen umwandeln
// str:  Argument
// Rueckgabe: Dateiname
//

static mixed to_filename(string str)
{
  string *tmp,p,newfn;
  int i;
// Testen ob .. in einem Filenamenabschnitt, falls Version <3.2.5
  tmp=explode(str,"/");
// Testen auf Pfadvariable
  if (sizeof(tmp[0]) && tmp[0][0]=='$' 
      && m_contains(&p,QueryProp(P_VARIABLES),tmp[0][1..]))
    tmp[0]=p;
// Pfad absolut machen (Hat danach noch Wildcards drinnen) oder auch nicht
  return normalize_path(implode(tmp,"/"), getuid(), 1);
}


//
// _parseargs(): Kommandozeilenabschnitt parsen
// str:         Kommandozeilenabschnitt
// line:        Array von geparsten Kommandozeilenabschnitten (Referenz)
// flags:       Als Referenz uebergebener Flag-Wert
// opts:        Erlaubte Flags
// build_fn:    Filenamen aendern
//

private void _parseargs(string str, string *line,int flags,string opts,
                     int build_fn )
{

// Strings in "" erhalten
  if(str[0] == "\""[0])  
  { 
    line += ({ str[1..<2] });
    return; 
  }  
//  Flags parsen
  if(str[0] == '-')  
  { 
    int i,j;
    i=sizeof(str);
    while(i--)
      if (str[i]!='-')
      {
        if((j = member(opts, str[i])) != -1)
          flags |= (1 << j);
        else
        {
          flags=-1;
          printf("Das Flag '-%c' wird von dem Befehl '%s' nicht "
                 "unterstuetzt.\n",str[i],query_verb()||"");
        }
      }
    return;
  }
  if (build_fn)
  {
    if (str=(string)to_filename(str)) line+=({ str });
  } 
  else
    line+= ({ str });
}


//
// parseargs() - zerlegt Kommandozeile in ein Array:
// cmdline:     Kommandozeile
// flags:       Als Referenz uebergebener leerer Integerwert. Enthaelt danach
//              die Flags
// opts:        String mit erlaubten Optionen
// build_fn:    Filenamen umbauen?
// Rueckgabe: Array der Kommandozeilenargumente (ausser Flags)
//

static string *parseargs(string cmdline,int flags, string opts,int build_fn)
{
  int i;
  string *line;
  line=({});
  if (!sizeof(cmdline)) return ({});
  map(regexplode(cmdline,"[\"][^\"]*[\"]| ", RE_TRADITIONAL)-({" ", ""}),
            #'_parseargs, &line, &flags,opts, build_fn);
  return line - ({""});
}

//
// _vc_map: VC-Objektnamen zu Arrays umwandeln ({ name, -1, program_time() })
//

private int *_vc_map(object ob,mixed *list)
{
  list+=({ explode(object_name(ob),"/")[<1],-1,program_time(ob) });
  return 0;
}


//
// _get_files: Filedaten beim Rekursiven kopieren erstellen
// dirname:    Bearbeitetes Verzeichnis
// mask:       Maske, der Dateinamen genuegen muessen
// mode:       Welche Daten werden benoetigt? (siehe magier.h)
// dest:       In welches Verzeichnis soll kopiert werden?
// Rueckgabe:  Alist mit den Dateiinformationen
//

private varargs mixed *_get_files(string dirname,string mask,int mode,
                                  string dest)
{
  //DEBUG("_GF: DIRNAME " + dirname);
  mixed *data=get_dir(dirname+"*",GETDIR_NAMES|GETDIR_SIZES|GETDIR_DATES);
  if(!sizeof(data)) return ({});
  mixed *files=({});

  while(1)
  {
    mixed *tmp=({});
    string base=data[BASENAME];
    string fullname=dirname+base;
    if (base!="." && base!=".."
        && (!(mode==MODE_GREP && base=="RCS"))
        && ((data[FILESIZE]==FSIZE_DIR
             && sizeof(tmp=_get_files(fullname+"/",mask,mode,dest+base+"/"))
             && mode!=MODE_RM)
            || !mask
            || sizeof(regexp(({ base }),mask, RE_TRADITIONAL))) )
    {
      //DEBUG("_GF: ADDING FILE " + fullname);
      files+= ({ data[0..2]+({ fullname,dirname,dest+base,
                               sizeof(tmp) }) });
    }
    if (sizeof(files)+sizeof(tmp)>MAX_ARRAY_SIZE)
       raise_error("Zu viele Files (>3000)!! Abgebrochen!\n");
    files+=tmp;
    if (sizeof(data) > 3)
      data=data[3..];
    else
      break;
  }

  if(sizeof(files)>300&&!IS_ARCH(this_object()))
    // Tod allen Laggern :o)
    raise_error("Zu viele Files (>300)!! Abgebrochen!\n");
  return files;
}
 

//
// _get_matching: Rekursive Funktion zum ermitteln der Files, wenn mit Maske
//                gearbeitet wird (z.B. cp -m /bla/*/* /ziel *.c)
// pathmask: Array, das die einzelnen Elemente der Maske (Arg1) enthaelt
//                   (per efun:old_explode(arg1,"/"))
// depth:     Aktuelles Element von pathmask
// path:      implode(pathmask[0..depth],"/");
// mode:      Welche Daten werden benoetigt? (siehe magier.h)
// flags:     Welche Flags wurden gesetzt?
// dest:      Zielverzeichnis (beim kopieren/moven)
// filemask:  Maske, der die Files genuegen muessen
// Rueckgabe: Alist mit den Dateiinformationen
//

private mixed *_get_matching(string *pathmask, int depth, string path, 
                    int mode, int recursive, string dest,string filemask)
{
  //DEBUG("_GM: PM: " + pathmask[depth]);
  //DEBUG("_GM: FM: " + filemask);

  // Pfad normalisieren (ggf. Platzhalter expandieren)
  string p=normalize_path(path+pathmask[depth++], getuid(), 1);
  mixed *data=get_dir(p, GETDIR_NAMES|GETDIR_SIZES|GETDIR_DATES)||({});
  if (!sizeof(data))
    return ({});

  // Bemerkung: path beginnt als "/" und wird nach und nach mit base
  // verlaengert, daher muss das nicht explizit normalisiert werden. dest
  // wurde bereits vom Aufrufer normalisiert und wird hier nur durchgereicht.

  mixed *files=({});
  while(sizeof(data))
  {
    string base=data[BASENAME];
    if (base=="." || base=="..")
    {
      data=data[3..];
      continue;
    }
    string full=path+base;
    //DEBUG("_GM: FULL: " + full);
    mixed *tmp;
    if ((data[FILESIZE]==FSIZE_DIR) && (sizeof(pathmask)>depth)
        && (!(mode==MODE_GREP && base=="RCS")))
    {
      //DEBUG("DESCEND INTO " + full);
      tmp=_get_matching(pathmask,depth,full+"/",mode,recursive,
                        (recursive ? dest+base+"/" : dest),filemask);
    }
    else tmp=({});

    //DEBUG("DEPTH: " + depth + " : " + sizeof(pathmask));
    if((!filemask && (depth==sizeof(pathmask)))
        || (filemask && (depth+2 > sizeof(pathmask))
            && sizeof(regexp(({ base }),filemask,RE_TRADITIONAL)))
        || ((mode==MODE_CP || mode==MODE_MV
             || (filemask && (mode==MODE_RM)
                 && sizeof(regexp(({ base}),filemask,RE_TRADITIONAL))))
             && sizeof(tmp)) )
    {
      //DEBUG("ADDING: " + base+ " : "+ full );
      files+=({ data[0..2]+({ full, path, dest+base,sizeof(tmp)}) });
    }
    if (sizeof(files) + sizeof(tmp) > MAX_ARRAY_SIZE)
       raise_error("Zu viele Files (>3000)!! Abgebrochen!\n");
    files+=tmp;
    if(sizeof(data)>3)
      data=data[3..];
    else
      break;
  }
  if(sizeof(files)>300&&!IS_ARCH(this_object()))
    // Tod allen Laggern!
    raise_error("Zu viele Files (>300)!! Abgebrochen!\n");
  return files;
}


//
// get_files: Basisroutine zum Ermitteln der zu bearbeitenden Dateien
// filename:  Pfadmaske, Verzeichnisname, Dateiname, der bearbeitet werden
//            soll
// mode:      Welche Daten werden benoetigt? (siehe magier.h)
// recursive: Auch Unterverzeichnisse bearbeiten?
// dest:      Wenn kopiert werden soll: wohin?
// filemask:  Maske, der die Dateinamen genuegen muessen
// Rueckgabe: Alist mit den Dateiinformationen
//

static varargs mixed *get_files(string filename, int mode, int recursive,
                                string dest, string filemask)
{
  // Bemerkung. dest wurde bereits vom Aufrufer normalisiert.

  // DEBUG("GF: " + filename);
  // DEBUG("REC: " + recursive + " MODE: " + mode);
  // if (dest[<1..<1]!="/") DEBUG("DEST: " + dest);
  if (filename=="/")
  {
      switch (mode)
      {
        case MODE_LSA: return ({({ "", -2, 0,"","","",0 })});
        default: if (!recursive) return ({});
                 break;
      }
  }

  // Normalisiertes Pfadarray besorgen
  string *patharray=master()->path_array(filename);
  // und daraus auch filename neu erzeugen
  filename=implode(patharray, "/");

  mixed *data=get_dir(filename,GETDIR_NAMES|GETDIR_SIZES|GETDIR_DATES)||({});
  if(!sizeof(data)
     && (mode==MODE_UPD || mode==MODE_MORE || mode==MODE_ED))
    data=get_dir(filename+".c",GETDIR_NAMES|GETDIR_SIZES|GETDIR_DATES) || ({});

  if ((mode==MODE_LSA||mode==MODE_LSB))
  {
    string *vrooms;
    object vcompiler =
      find_object(implode(patharray[0..<2],"/")+"/virtual_compiler");
    if (vcompiler && pointerp(vrooms=(mixed *)vcompiler->QueryObjects()))
    map(vrooms,#'_vc_map,&data);
  }
  mixed *files=({});
  if (sizeof(data)) // passende files
  {
    mixed *subfiles;
    subfiles=({});
    string path=implode(patharray[0..<2],"/")+"/";
    while (sizeof(data))
    {
      subfiles=({});
      string base=data[BASENAME];
      if (mode==MODE_LSB||(base!="."&&base!=".."))
      {
        //DEBUG("PATH: " + path+" BASE: " + base + " MODE: " + mode);
        string full=path+base;
        string dest2;
        if (dest=="/" || file_size(dest[0..<2])==FSIZE_DIR)
          dest2 = dest+base;
        else
          dest2 = dest[0..<2];

        //DEBUG("DEST: " + dest);
        if (recursive && data[FILESIZE]==FSIZE_DIR) // Verzeichnis, Rekursiv
          subfiles=_get_files(full+"/",filemask,mode,dest2+"/");
        if (!(filemask && !sizeof(subfiles)
              && !sizeof(regexp(({ base }),filemask,RE_TRADITIONAL))))
        {
          if (!filemask || mode!=MODE_RM)
            files+=({ data[0..2]+({ full, path, dest2,sizeof(subfiles)}) });
          if (sizeof(files)+sizeof(subfiles) > MAX_ARRAY_SIZE)
            raise_error("Zu viele Files (>3000)!! Abgebrochen!\n");
          files+=subfiles;
        }
      }
      if (sizeof(data)>3)
        data=data[3..];
      else
        break;
    }
    return files;
  }
// File existiert nicht -> Wildcard oder tatsaechlich nicht existent
// Behandeln je nach mode
  switch(mode)
  {
    case MODE_CP:
    case MODE_MV:
    case MODE_CD:
    case MODE_LSA:
      files=_get_matching(patharray+(filemask?({ "*" }):({})),1,"/",mode,
                           recursive,dest,filemask);
      break;
    default: break; 
  }
  return files;
}


//
// file_list(): Liste der Fileinformationen
// files:     Array der Filenamen MIT Wildcards
// mode:      Welche Daten werden benoetigt
// recursive: Rekursiv listen?
// dest:      Zielverzeichnis
// mask:      Maske (regexp, wenn definiert)
// Rueckgabe: Liste der betroffenen Files
//

static varargs mixed *file_list(string *files, int mode, int recursive,
                                string dest, string mask)
{
  string *list=({});
  if (mask) mask=glob2regexp(mask);
  // dest muss einmal normalisiert werden, dann muss es das in den gerufenen
  // (rekursiven) Funktionen nicht immer nochmal gemacht werden.
  dest=normalize_path(dest, getuid(), 1);

  foreach(string file: files)
  {
    // Abschliessenden / von Pfadnamen abschneiden, weil in diesem Fall 
    // die Inhalte der ersten Unterverzeichnisebene mit ausgegeben
    // wurden. Ursache hierfuer ist ein Fix an full_path_array() im 
    // Masterobjekt, der dazu fuehrte, dass get_dir() den abschliessenden /
    // des uebergebenen Pfades jetzt korrekt behandelt. \o/
    if ( sizeof(file) > 1 && file[<1] == '/' )
    {
      file = file[0..<2];
      if ( !sizeof(file) )
        continue;
    }
    string err=catch(list += get_files(file,mode,recursive,dest,mask));
    if (err)
    {
      printf("Fehler aufgetreten: %s\n",err);
      log_file(SHELLLOG("FILE_LIST"),
               sprintf("%s fuehrte folgendes Kommando aus: (Zeit: %s)\n"
                       "  >>%s %s<<\n"
                       "  Folgender Fehler trat dabei auf:\n"
                       "  %s\n\n",
                       capitalize(getuid())||"<Unbekannt>",dtime(time()),
                       query_verb()||"*",_unparsed_args()||"*",err||"*"));
      return ({});
    }
  }
   return list;
}

// $Id: parsing.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#include <files.h>
#include <wizlevels.h>
#include <logging.h>
#include <regexp.h>
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
  return master()->make_path_absolute(implode(tmp,"/"));
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

private varargs mixed *_get_files(string dirname,string mask,int mode,string dest)
{
  mixed *files,*tmp,*data;
  string fullname,base;

  //DEBUG("_GF: DIRNAME " + dirname);
  data=get_dir(dirname+"*",7);
  if(!sizeof(data)) return ({});
  files=({});
 
  while(sizeof(data))
  {
    tmp=({});
    base=data[BASENAME];
    fullname=dirname+base;
    if (base!="."&&base!=".."&&(!(mode==MODE_GREP&&base=="RCS"))&&
        ((data[FILESIZE]==-2&&
       sizeof(tmp=_get_files(fullname+"/",mask,mode,
      dest+base+"/"))&&mode!=MODE_RM)||!mask||sizeof(regexp(({ base }),mask, RE_TRADITIONAL))))
    {
      //DEBUG("_GF: ADDING FILE " + fullname);
      files+= ({ data[0..2]+({ fullname,dirname,dest+base,
                               sizeof(tmp) }) });
    }
    if (sizeof(files)+sizeof(tmp)>MAX_ARRAY_SIZE)
       raise_error("Zu viele Files (>3000)!! Abgebrochen!\n");
    files+=tmp;
    data=data[3..];
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
  mixed *data,*tmp,*files;
  string base,full;

  //DEBUG("_GM: PM: " + pathmask[depth]);
  //DEBUG("_GM: FM: " + filemask);
  data=get_dir(path+pathmask[depth++],GETDIR_NAMES|GETDIR_SIZES|GETDIR_DATES)||({});
  if (!sizeof(data)) return ({});
  files=({});
  while(sizeof(data))
  {
    if ((base=data[BASENAME])=="."||base=="..")
    {
      data=data[3..];
      continue;
    }
    full=path+base;
    //DEBUG("_GM: FULL: " + full);
    if ((data[FILESIZE]==-2)&&(sizeof(pathmask)>depth)&&
        (!(mode==MODE_GREP&&base=="RCS")))
    {
      //DEBUG("DESCEND INTO " + full);
      tmp=_get_matching(pathmask,depth,full+"/",mode,recursive,
                        (recursive?dest+base+"/":dest),filemask);
    }
    else tmp=({});
    //DEBUG("DEPTH: " + depth + " : " + sizeof(pathmask));
    if((!filemask&&(depth==sizeof(pathmask)))||
        (filemask&&(depth+2>sizeof(pathmask))&&
        sizeof(regexp(({ base }),filemask,RE_TRADITIONAL)))||
       ((mode==MODE_CP||mode==MODE_MV||(filemask&&
        (mode==MODE_RM)&&sizeof(regexp(({ base}),filemask,RE_TRADITIONAL))))&&
        sizeof(tmp)))
    {
      //DEBUG("ADDING: " + base+ " : "+ full );
      files+=({ data[0..2]+({ full, path, dest+base,sizeof(tmp)}) });
    }
    if (sizeof(files)+sizeof(tmp)>MAX_ARRAY_SIZE)
       raise_error("Zu viele Files (>3000)!! Abgebrochen!\n");
    files+=tmp;
    data=data[3..];
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

static varargs mixed *get_files(string filename, int mode, int recursive, string dest, string filemask)
{ 
  string full,path,base,*patharray,*vrooms,dest2;
  object vcompiler;
  mixed *files,*data;

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
  patharray=explode(filename,"/");
  if(!sizeof(data=get_dir(filename,7)||({}))&&
     (mode==MODE_UPD||mode==MODE_MORE||mode==MODE_ED))
    data=get_dir(filename+".c",7)||({});
  if ((mode==MODE_LSA||mode==MODE_LSB)&&
      (vcompiler = find_object(implode(patharray[0..<2],"/")+"/virtual_compiler")) &&
      pointerp(vrooms=(mixed *)vcompiler->QueryObjects()))
    map(vrooms,#'_vc_map,&data);
  files=({});
  if (sizeof(data)) // passende files
  {
    mixed *subfiles;
    subfiles=({});
    path=implode(patharray[0..<2],"/")+"/";
    while (sizeof(data))
    {
      subfiles=({});
      base=data[BASENAME];
      if (mode==MODE_LSB||(base!="."&&base!=".."))
      {
        //DEBUG("PATH: " + path+" BASE: " + base + " MODE: " + mode);
        full=path+base;
        dest2=((dest=="/"||file_size(dest[0..<2])==-2)?
               (dest+base):(dest=="/"?"/":dest[0..<2]));
        //DEBUG("DEST: " + dest);
        if (recursive&&data[FILESIZE]==-2) // Verzeichnis, Rekursiv
          subfiles=_get_files(full+"/",filemask,mode,dest2+"/");
        if (!(filemask&&!sizeof(subfiles)&&!sizeof(regexp(({ base }),filemask,RE_TRADITIONAL))))
        {
          if (!filemask||mode!=MODE_RM)
            files+=({ data[0..2]+({ full, path, dest2,sizeof(subfiles)}) });
          if (sizeof(files)+sizeof(subfiles)>MAX_ARRAY_SIZE)
            raise_error("Zu viele Files (>3000)!! Abgebrochen!\n");
          files+=subfiles;
        }
      }
      data=data[3..];
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

static varargs mixed *file_list(string *files, int mode, int recursive, string dest, string mask)
{
  string *list,err,*result;
  int i,j;
  list=({});
  if (mask) mask=glob2regexp(mask);
  j=sizeof(files);
  for(i=0;i<j;i++)
  {
    // Abschliessenden / von Pfadnamen abschneiden, weil in diesem Fall 
    // die Inhalte der ersten Unterverzeichnisebene mit ausgegeben
    // wurden. Ursache hierfuer ist ein Fix an full_path_array() im 
    // Masterobjekt, der dazu fuehrte, dass get_dir() den abschliessenden /
    // des uebergebenen Pfades jetzt korrekt behandelt. \o/
    if ( sizeof(files[i]) > 1 && files[i][<1] == '/' )
    {
      files[i] = files[i][0..<2];
      if ( !sizeof(files[i]) )
        continue;
    }
    if (err=catch(list+=get_files(files[i],mode,recursive,dest,mask)))
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

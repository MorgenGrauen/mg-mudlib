// MorgenGrauen MUDlib
//
//fileedit.c
//
// $Id: fileedit.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#include <wizlevels.h>
#include <input_to.h>

#define NEED_PROTOTYPES
#include <magier.h>
#include <player.h>
#include <files.h>

//                        ###################
//######################### INITIALISIERUNG #############################
//                        ###################

mixed _query_localcmds()
{
  return ({ ({"cp","_cp",0,WIZARD_LVL}),
            ({"mv","_cp",0,WIZARD_LVL}),
            ({"rm","_rm",0,WIZARD_LVL}),
            ({"rmdir","_rmdir",0,WIZARD_LVL}),
            ({"mkdir","_mkdir",0,WIZARD_LVL}),
            ({"ed","_ed",0,WIZARD_LVL})});
}

//                               ######
//################################ ED ###################################
//                               ######


//
// _ed_file: Mehrere Files hintereinander editieren
//

private nosave string *_ed_cache;

private void _ed_file()
{
  if (!sizeof(_ed_cache)) return;
  printf("ed: Naechste Datei: %s\n",_ed_cache[0]);
  ed(_ed_cache[0],"_ed_file");
  _ed_cache=_ed_cache[1..];
  return;
}

#if __VERSION__ < "3.2.9"
private mixed _ed_size_filter(mixed *arg)
{
  if (arg[FILESIZE]>=-1) return arg[FULLNAME];
  printf("%s ist ein Verzeichnis.\n",arg[FULLNAME]);
  return 0;
}
#endif

//
// _more: Dateien anzeigen
// cmdline: Kommandozeile
//

static int _ed(string cmdline)
{
  mixed *args,*args2;
  int flags,i,arg_size;
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,"",1);
  if (flags==-1||!(arg_size=sizeof(args)))
    return USAGE("ed <datei> [<datei2>..]");
  while(arg_size--)
  {
    if (sizeof(args2=file_list(args[arg_size..arg_size],MODE_ED,0,"/")))
      args[arg_size..arg_size]=args2;
    else
      args[arg_size]=({ "" , -1, 0 , (string)
              call_other(master(),"_get_path",args[arg_size],
                  getuid())});
  }
#if __VERSION__ < "3.2.9"
  args=map(args,#'_ed_size_filter)-({0});
#else
  args=map(args,(:
          if ($1[FILESIZE]>=-1) return $1[FULLNAME];
          printf("%s ist ein Verzeichnis.\n",$1[FULLNAME]);
          return 0; :))-({0});
#endif
  if (flags==-1||!sizeof(args)) return USAGE("ed <datei> [<datei2>..]");
  _ed_cache=args;
  _ed_file();
  return 1;
}


//                             ###########
//############################## CP & MV ################################
//                             ###########

static void _cp_ask_copy(mixed *filedata,int dummy, int flags);
static void _cp_ask_overwrite(mixed *filedata, int mode, int flags);
static void _mv_ask_overwrite(mixed *filedata, int mode, int flags);

static mixed cp_file(mixed filedata,int move,int flags, mixed *do_delete)
{
  string source,dest;
  source=(string)filedata[FULLNAME];
  dest=(string)filedata[DESTNAME];
  if (source==dest) return ERROR(SAME_FILE,source,RET_FAIL);
  if (!MAY_READ(source)) return ERROR(NO_READ,source,RET_JUMP);
  if (!MAY_WRITE(dest)) return ERROR(NO_WRITE,dest,RET_JUMP);
  if (filedata[FILESIZE]==-1) return ERROR(DOESNT_EXIST,source,RET_JUMP);
  if (filedata[FILESIZE]==-2) // Quelle ist Verzeichnis
  {
    switch(file_size(dest))
    {
      case -1:
        if (move)
        {
          if (rename(source,dest)) return ERROR(NO_CREATE_DIR,dest,RET_JUMP);
          if (flags&CP_V) printf(FILE_MOVED,source);
          return RET_JUMP;
        }
        if (!mkdir(dest)) return ERROR(NO_CREATE_DIR,dest,RET_JUMP);
        if (flags&CP_V) printf(DIR_CREATED,dest);
      case -2:
        if (!move) return RET_OK;
        if (filedata[SUBDIRSIZE]>0) return RET_DELETE;
        if (!rmdir(source)) return ERROR(NO_DELETE,source,RET_FAIL);
        if (flags&MV_V) printf("mv: %s: Quellverzeichnis wurde "
                               "geloescht.\n",source);
        return RET_OK;    
      default:  return ERROR(NO_DIRS,dest,RET_JUMP);
    }
  }
  switch(file_size(dest))
  {
    case -2: return ERROR(DEST_IS_DIR,dest,RET_FAIL);
    default:
      if (flags&CP_F)
      {
        if (!rm(dest)) return ERROR(DEST_NO_DELETE,dest,RET_FAIL);
        if (flags&CP_V) printf(FILE_DELETED,dest);
      }
      else
      {
        if (move) return #'_mv_ask_overwrite;
        else return #'_cp_ask_overwrite;
      }
    case -1:
      if (move)
      {
        if (rename(source,dest)) return ERROR(NO_MOVE,source,RET_FAIL);
        if (flags&CP_V) printf(FILE_MOVED,source);
        return RET_OK;
      }
      if (copy_file(source,dest)) return ERROR(NO_COPY,source,RET_FAIL);
      if (flags&CP_V) printf(FILE_COPIED,source);
      return RET_OK;
  }
  return 0; // not-reached
}

static void _cp_ask_overwrite2(string input, mixed *filedata,
                               int interactive,int flags,int move)
{
  if (!sizeof(input)) input=" ";
  input=lower_case(input);
  switch(input[0])
  {
    case 'q':
      printf("%s abgebrochen!\n",move?"Bewegen":"Kopieren");
      return;
    case 'a':
      flags|=CP_F;
      if (!(flags&CP_I))
      {
        asynchron(filedata,#'cp_file,move,flags,0);
        return;
      }
    case 'y':
    case 'j':
      if (!rm(filedata[0][DESTNAME]))
        printf(DEST_NO_DELETE "Uebergehe Datei...\n",filedata[0][DESTNAME]);
      else
      {
        if (flags&CP_V) printf(FILE_DELETED,filedata[0][DESTNAME]);
        if (move)
        {
          if (rename(filedata[0][FULLNAME],filedata[0][DESTNAME]))
            printf(NO_MOVE "Uebergehe Datei...\n",filedata[0][FULLNAME]);
        }
        else
        {
          if (copy_file(filedata[0][FULLNAME],filedata[0][DESTNAME]))
            printf(NO_COPY "Uebergehe Datei...\n",filedata[0][FULLNAME]);
        }
      }
    case 'n':
      if (flags&CP_I)
        _cp_ask_copy(filedata[1+filedata[0][SUBDIRSIZE]..],move,flags);
      else
        asynchron(filedata[1+filedata[0][SUBDIRSIZE]..],
                                     #'cp_file,move,flags,0);
      return;
    default:
      printf("Kommando nicht verstanden.\n");
      _cp_ask_overwrite(filedata,interactive,flags);
      return;
  }

}

static void _cp_ask_overwrite(mixed *filedata, int interactive, int flags)
{
  printf("Die Datei '%s' existiert schon.\n",
         filedata[0][DESTNAME]);
  input_to("_cp_ask_overwrite2",INPUT_PROMPT,"Ueberschreiben? (j,n,a,q): ",
      filedata,interactive,flags,0);
  return;
}

static void _mv_ask_overwrite(mixed *filedata, int interactive, int flags)
{
  printf("Die Datei '%s' existiert schon.",
         filedata[0][DESTNAME]);
  input_to("_cp_ask_overwrite2",INPUT_PROMPT,"Ueberschreiben? (j,n,a,q): ",
      filedata,interactive,flags,1);
  return;
}

static void _cp_ask_copy2(string input,mixed *filedata,int mode,
                           int flags,int move)
{
  if (!sizeof(input)) input=" ";
  input=lower_case(input);
  switch(input[0])
  {
    case 'y':
    case 'j':
      if (mode==1)
      {
        if (!(flags&CP_F))
        {
          if (move) _mv_ask_overwrite(filedata,1,flags);
          else      _cp_ask_overwrite(filedata,1,flags);
          return;
        }
        if (!rm(filedata[0][DESTNAME]))
        {
          printf(DEST_NO_DELETE "Uebergehe Datei...\n",
                 filedata[0][DESTNAME]);
          _cp_ask_copy(filedata[1..],move,flags);
          return;
        }
        if (flags&CP_V) printf(FILE_DELETED,filedata[0][DESTNAME]);
      }
      if (mode<2)
      {
        if (move) rename(filedata[0][FULLNAME],filedata[0][DESTNAME]);
        else copy_file(filedata[0][FULLNAME],filedata[0][DESTNAME]);
        _cp_ask_copy(filedata[1..],move,flags);
        return;
      }
      if (mode==2)
      {
        if (move)
        {
          if (rename(filedata[0][FULLNAME],filedata[0][DESTNAME]))
            printf(NO_MOVE "Uebergehe Verzeichnis...\n",
                   filedata[0][FULLNAME]);
          _cp_ask_copy(filedata[1+filedata[0][SUBDIRSIZE]..],move,flags);
          return;
        }
        if (mkdir(filedata[0][DESTNAME]))
        {
          _cp_ask_copy(filedata[1..],0,flags);
          return;
        }
        printf(NO_CREATE_DIR "Uebergehe Verzeichnis...\n",
               filedata[0][DESTNAME]);
      }
    case 'n':
      _cp_ask_copy(filedata[(1+filedata[0][SUBDIRSIZE])..],0,flags);
      return;
    case 'q':
      printf("Kopieren abgebrochen!\n");
      return;
    case 'a':
      flags&=~CP_I;
      asynchron(filedata,#'cp_file,move,flags,0);
      return;
    default:
      printf("Kommando nicht verstanden.\n");
      _cp_ask_copy(filedata,0,flags);
      return;
  }
}

static void _cp_ask_copy(mixed *filedata,int move, int flags)
{
  mixed data;
  string dest,source;
  int delete_subs,jump;

  if(!sizeof(filedata))
  {
    printf("%s: abgeschlossen.\n",move?"mv":"cp");
    return; 
  }
  dest=filedata[0][DESTNAME];
  source=filedata[0][FULLNAME];
  switch(0)   // break wirkt damit wie ein goto end_of_switch
  {
    default:
    case 0: // Sinnlos, aber er compiliert sonst nicht :(
      jump=filedata[0][SUBDIRSIZE];
      if (source==dest)
      {
        printf(SAME_FILE,source);
        break;
      }
      if (!MAY_READ(source))
      {
        printf(NO_READ,source);
        break;
      }
      if (!MAY_WRITE(dest))
      {
        printf(NO_WRITE,dest);
        jump=0;
        break;
      }
      if (filedata[0][FILESIZE]==-1)
      {
        printf(DOESNT_EXIST,source);
        break;
      }
      if (filedata[0][FILESIZE]==-2) // Quelle ist Verzeichnis
      {
        if (file_size(dest)>-1)
        {
          printf(NO_DIRS,dest);
          break;
        }
        if (file_size(dest)==-2)
        {
          jump=0;
          break;
        }
        printf("Verzeichnis '%s' %s?\n",source,
               move?"bewegen":"kopieren");
        input_to("_cp_ask_copy2",INPUT_PROMPT, "(j,n,a,q) ",
            filedata,2,flags,move);
        return;
      }
      if (file_size(dest)==-2)
      {
        printf(DEST_IS_DIR,dest);
        break;
      }
      printf("'%s' %s?\n",source,move?"bewegen":"kopieren");
      input_to("_cp_ask_copy2",INPUT_PROMPT, "(j,n,a,q) ",
          filedata,(file_size(dest)!=-1),flags,move);
      return;
  }
  _cp_ask_copy(filedata[1+jump..],move,flags);
  return;
}


static int _cp(string cmdline)
{
  mixed *args;
  int flags;
  string mask;
  string dest,*dest2;
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,CP_OPTS,0);
  if (flags==-1||!sizeof(args))
    return USAGE(query_verb()+" [-" CP_OPTS
                 "] <datei/verz> [<datei2/verz2> ... ] <ziel> [<maske>]");
  if (flags&CP_M)
  {
    mask=args[<1];
    args=args[0..<2];
  }
  if (!dest=to_filename(args[<1]))
     return USAGE(query_verb()+" [-" CP_OPTS
          "] <datei/verz> [<datei2/verz2> ... ] <ziel> [<maske>]");
  if (file_size(dest)==-1)
  {
    dest2=explode(dest,"/");
    if (file_size(implode(dest2[0..<2],"/"))==-2)
    {
      if (dest2[<1]=="*")
        dest=implode(dest2[0..<2],"/");
      else
        if (member(dest2[<1],'*')>-1||
            member(dest2[<1],'?')>-1)
         return notify_fail(
          sprintf("%s: Keine * und ? im Zielnamen erlaubt.\n",query_verb())),0;
    }
    else
      return notify_fail(
        sprintf("%s: Der angegebene Zielpfad existiert nicht.\n",
                query_verb())),0;
  }
  args=args[0..<2];
  if (file_size(dest)!=-2&&sizeof(args)>1)
    return notify_fail(
        sprintf("%s: Bei mehreren Quellen muss das Ziel ein Verzeichnis "
                "sein.\n",query_verb())),0;
  if (!sizeof(args=map(args,#'to_filename)-({0})))
    return USAGE(query_verb()+" [-" CP_OPTS
          "] <datei/verz> [<datei2/verz2> ... ] <ziel> [<maske>]");
  // DEBUG("DEST: " + dest + " : FLAGS: " + flags);
  args=file_list(args,MODE_CP,(flags&CP_R),dest+"/",mask);
  if (!sizeof(args))
    return notify_fail(sprintf("%s: Keine passenden Dateien gefunden.\n",
                               query_verb())),0;

  if (sizeof(args)>1&&(args[0][FILESIZE]>=0)&&file_size(dest)!=-2)
      return notify_fail(
        sprintf("%s: Bei mehreren Quellen muss das Ziel ein Verzeichnis "
                "sein.\n",query_verb())),0;
  if (sizeof(args)==1&&file_size(dest)!=-2)
    args[0][DESTNAME]=dest;
  if (!(flags&CP_I))
  {
    asynchron(args,#'cp_file,(query_verb()=="mv"),flags,0);
    return 1;
  }
  if (query_verb()=="cp")
    _cp_ask_copy(args,0,flags);
  else
    _cp_ask_copy(args,1,flags);
  return 1;
}

//                              #########
//############################### RMDIR #################################
//                              #########


//
// _rmdir: Verzeichnis loeschen
// cmdline: Kommandozeilenargumente
//


#if __VERSION__ < "3.2.9"

private int _dir_filter(mixed arg)
{
  return (arg[FILESIZE]==-2);
}

#endif

static int _rmdir(string cmdline)
{
  string dest,tmp;
  int flags;
  mixed *args;
  
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,RMDIR_OPTS,1);
  if (flags==-1||!sizeof(args))
    return USAGE("rmdir [-" RMDIR_OPTS "] <Verzeichnis>");
  if (sizeof(args)>1)
    return
    notify_fail("Mit 'rmdir' kann nur jeweils EIN Verzeichnis geloescht "
         "werden.\nDer Befehl 'rm' bietet erweiterte Moeglichkeiten.\n"),0;
  dest=args[0];
  if (dest!="/")
  {
    args=file_list(({dest}),MODE_RMDIR,0,"/");
#if __VERSION__ < "3.2.9"
    args=filter(args,#'_dir_filter);
#else
    args=filter(args,(: ($1[FILESIZE]==-2) :));
#endif
    if (!sizeof(args))
      return notify_fail(
        sprintf("rmdir: %s: Kein solches Verzeichnis gefunden.\n",dest)),0;
    if (sizeof(args)>1)
      return notify_fail(
                sprintf("rmdir: %s: Maske ist nicht eindeutig.\n",dest)),0;
    dest=args[0][FULLNAME];
    if (!MAY_WRITE(dest)) return ERROR(NO_WRITE,dest,1);
    if (!rmdir(dest))
    {
      if (sizeof((get_dir(dest+"/*")||({}))-({".",".."})))
        printf("rmdir: %s: Verzeichnis ist nicht leer.\n",dest);
    }
    else
    {
      if (flags&&RMDIR_V) printf(FILE_DELETED,dest);
    }
    return 1;
  }
  return ERROR(NO_DELETE,dest,1);
}

//                              #########
//############################### MKDIR #################################
//                              #########


//
// _mkdir: Verzeichnis erstellen
// cmdline: Kommandozeilenargumente
//

static int _mkdir(string cmdline)
{
  string dest,tmp;
  int flags,i;
  string *args;
  
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,MKDIR_OPTS,1);
  if (flags==-1) return 0;
  if (!sizeof(args))
    return USAGE("mkdir [-" MKDIR_OPTS "] <Verzeichnis>");
  if (sizeof(args)>1)
    return notify_fail("Mit 'mkdir' kann nur jeweils EIN Verzeichnis "
                       "erstellt werden.\n"),0;
  dest=args[0];
  
  if ((i=file_size(implode((args=explode(dest,"/"))[0..<2],"/")))==FSIZE_DIR)
  {
    if (!mkdir(dest)) return ERROR(NO_CREATE_DIR,dest,1);
    if (flags&MKDIR_V) printf(DIR_CREATED,dest,1);
    printf("mkdir: abgeschlossen.\n");
    return 1;
  }
  
  if (i==FSIZE_NOFILE)
  {
    if (flags&MKDIR_R)
    {
      if (mkdirp(dest) != 1)
        return ERROR(NO_CREATE_DIR,dest,1);
      if (flags&MKDIR_V)
        printf(DIR_CREATED,implode(args[0..i],"/"));
      printf("mkdir: abgeschlossen.\n");
      return 1;
    }
    return ERROR(DOESNT_EXIST,implode(args[0..<2],"/"),1);
  }
  return ERROR(ALREADY_EXISTS,dest,1);
}

//                               ######
//################################ RM ###################################
//                               ######

private void _rm_ask_delete(mixed *filedata, int flags);

static void _rm_ask_delete2(string input,mixed *filedata,int flags)
{
  int i;
  if (!sizeof(input)) input=" ";
  input=lower_case(input);
  switch(input[0])
  {
    case 'q':
      printf("Loeschen abgebrochen!\n");
      return;
    case 'y':
    case 'j':
      if (filedata[0][FILESIZE]==-2)
      {
        if (i=filedata[0][SUBDIRSIZE]) // Dir-Eintrag nach hinten schieben
        {
          mixed temp;
          int j;
          temp=filedata[0];
          temp[SUBDIRSIZE]=0;
          for(j=0;j<i;j++) filedata[j]=filedata[j+1];
          filedata[j]=temp;
          _rm_ask_delete(filedata,flags);
          return;
        }
        if (!rmdir(filedata[0][FULLNAME]))
          printf(NO_DELETE,filedata[0][FULLNAME]);
        else if (flags&RM_V) printf(FILE_DELETED,filedata[0][FULLNAME]);
      }
      else // Datei existiert
      {
        if (!rm(filedata[0][FULLNAME]))
          printf(DEST_NO_DELETE "Uebergehe Datei...\n",
                 filedata[0][FULLNAME]);
        else if (flags&RM_V) printf(FILE_DELETED,filedata[0][FULLNAME]);
        
      }
    case 'n':
      _rm_ask_delete(filedata[1+filedata[0][SUBDIRSIZE]..],flags);
      return;
    default:
      printf("Kommando nicht verstanden.\n");
      _rm_ask_delete(filedata,flags);
      return;
  }
  return;
}

private void _rm_ask_delete(mixed *filedata, int flags)
{
  int i;
  mixed temp;
  if (!sizeof(filedata))
  {
    printf("rm: abgeschlossen.\n");
    return;
  }
  switch(filedata[0][FILESIZE])
  {
    case -1:
      if (flags&RM_V) printf(DOESNT_EXIST,filedata[0][FULLNAME]);
      _rm_ask_delete(filedata[1..],flags);
      return;
    case -2:
      if (i=filedata[0][SUBDIRSIZE])
        printf("Ins Verzeichnis '%s' hinabsteigen?\n",
          filedata[0][FULLNAME]);
      else
        printf("Verzeichnis '%s' loeschen?\n",
               filedata[0][FULLNAME]);
      input_to("_rm_ask_delete2",INPUT_PROMPT, "(j,n,q) ",
          filedata,flags);
      return;
    default:
      printf("'%s' loeschen? (j,n,q)\n",
         filedata[0][FULLNAME]);
      input_to("_rm_ask_delete2",INPUT_PROMPT, "(j,n,q) ",
          filedata,flags);
      return;
  }
}


private void rm_file(mixed filedata, mixed notused, int flags)
{
  string dest;
  dest=filedata[FULLNAME];
  if (!MAY_WRITE(dest))
  {
    printf(NO_WRITE,dest);
    return;
  }
  switch(filedata[FILESIZE])
  {
    case -1:
      if (flags&RM_V) printf(DOESNT_EXIST,dest);
      return;
    case -2:
      if (!rmdir(dest)) printf(DEST_NO_DELETE,dest);
      else
      {
        if (flags&RM_V) printf(FILE_DELETED,dest);
      }
      return;
    default:
      if (!rm(dest)) printf(DEST_NO_DELETE,dest);
      else
      {
        if (flags&RM_V) printf(FILE_DELETED,dest);
      }
      return;
  }
}

static int _rm(string cmdline)
{
  mixed *args,*args2;
  int flags,i;
  string mask;
  
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,RM_OPTS,0);
  if (flags==-1||!sizeof(args))
    return USAGE("rm [-" RM_OPTS
                 "] <datei/verz> [<datei2/verz2> ... ] [<maske>]");
  if (flags&RM_M)
  {
    mask=args[<1];
    args=args[0..<2];
  }
  args=map(args,#'to_filename)-({0});
  args=file_list(args,MODE_RM,(flags&RM_R),"/",mask);
  if (!(i=sizeof(args)))
    return printf("Keine passende Datei gefunden.\n"),1;
  if (!(flags&RM_I))
  {
    if (i>1) // Umdrehen
    {
      mixed temp;
      i>>=1;
      while(i)
      {
        temp=args[<(i--)];
        args[<(i+1)]=args[i];
        args[i]=temp;
      }
    }
    asynchron(args,#'rm_file,args,flags,0);
    return 1;
  }
  _rm_ask_delete(args,flags);
  return 1;
}

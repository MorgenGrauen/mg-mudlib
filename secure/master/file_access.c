#pragma strict_types, no_warn_deprecated

#include "/secure/master.h"

static mapping projects=([]);
static string *insecure,*deputy_files;

int ReloadInsecureFile()
{
    insecure = efun::explode( read_file("/secure/INSECURE") || "", "\n" );
    insecure -= ({""});
    insecure = map( insecure, #'lower_case/*'*/ );
    return(1);
}

void LoadDeputyFileList()
{
  // Leseberechtigungen fuer /log/ARCH/* setzen
  deputy_files = efun::explode( read_file("/log/ARCH/DEPUTY_FILELIST") || "",
                                "\n" ) - ({""});
  return;
}

#define PATH_ARRAY(x) (explode(x, "/")-({"."}))
string *full_path_array(string path, string user) {
  string *strs;
  string cwd;

  if(!path)
    return ({"",""}); // additional "" to yield "/" later.

  // remove multiple '/'
  path = regreplace(path, "/+", "/", 1);

  switch(path[0]) {
    case '/':
      if(!path[1]) return ({"",""}); //additional "" to yield "/" later
      strs=PATH_ARRAY(path);
      if (!sizeof(strs))
          strs = ({"",""});
      break;
    case '+':
      if(!path[1]) return ({"","d"});
      strs=({"","d"})+PATH_ARRAY(path[1..<1]);
      break;
    case '~':
      if(user && !path[1])
        return ({"","players",user});
      if(user && path[1]=='/')
        strs=({"","players",user})+PATH_ARRAY(path[2..<1]);
      else
        strs=({"","players"})+PATH_ARRAY(path[1..<1]);
      break;
    default:
      if(user && TP && getuid(TP) == user
          && (cwd=(string)TP->QueryProp(P_CURRENTDIR)))
        strs=PATH_ARRAY(cwd + "/" + path);
      else
        strs=PATH_ARRAY(path);
  }
  // /../ behandeln. Einschraenkungen der RegExp:
  // /a/b/../../d wuerde nur durch Wiederholung aufgeloest.
  // /../d wird nicht richtig aufgeloest.
  //regreplace("/d/inseln/toeter/room/hoehle/../wald/bla.c","(.*)\/[^/]+/\.\.
  //    /(.*)", "\\1\/\\2", RE_GLOBAL)
  
  // dies sieht schlimmer aus als es ist (member ist O(n)), solange das Array
  // nicht gross wird.
  int p;
  while((p=member(strs, "..")) != -1)
      strs = strs[0..p-2]+strs[p+1..];

  return strs;
}
#undef PATH_ARRAY

string _get_path(string path, string user) {
  string *p_arr = full_path_array(path, user);
  // make path absolute
  if (p_arr[0] != "")
      p_arr = ({""}) + p_arr;

  return implode(p_arr,"/");
}

static int project_access(string user, string project)
{
  mixed *lines;
  string s;
  mapping tmp;

  if (!member(projects,project))
  {
    s=read_file(PROJECTDIR+"/"+project+"/ACCESS_RIGHTS");
    if(!s||s=="")
      return 0;
    tmp=([]);
    for (lines=explode(s,"\n")-({""});sizeof(lines);lines=lines[1..])
    {
      if (lines[0][0]=='*')
        tmp[lines[0][1..]]=2;
      else
        tmp[lines[0]]=1;
    }
    projects[project]=({tmp,time()});
    return tmp[user];
  }
  projects[project][1]=time();
  return projects[project][0][user];
}

void OutdateProjectCache(string project)
{
  m_delete(projects,project);
}

static void _cleanup_projects() {
  int i;
  mixed *users;

  for (users=m_indices(projects),i=sizeof(users)-1;i>=0;i--)
    if((time()-projects[users[i]][1])>1800)
      m_delete(projects,users[i]);
}

int access_rights(string *p_arr, string euid)
{
  int i;

  i = sizeof(p_arr) - 2;

  while ( i >= 0 &&
          file_size( implode(p_arr[0..i], "/") + "/access_rights.c" ) < 0 )
      i--;

  if ( i < 0 )
      return 0;

  if ( !catch(i = (int)call_other( 
          implode(p_arr[0..i], "/") + "/access_rights",
          "access_rights", euid,
          implode(p_arr[i+1..], "/") ); publish) )
      return i;

  return 0;
}

string make_path_absolute(string str) {
  return _get_path(str, getuid(TP));
}


mixed valid_write(string path, string euid, string fun, object obj)
{
  int s,lvl;
  string *strs;
  int *date;

  if (member(path,' ')!=-1) return 0;

  // Unter LIBDATADIR (/data) sollen komplett identische Schreibrechte
  // vergeben werden.
  if (sizeof(path) > 6
      && path[0..5] == "/"LIBDATADIR"/")
    return valid_write(path[5..], euid, fun, obj) != 0;

  switch(fun) {
    case "log_file":
      strs=full_path_array("/"+path, 0);
      path = implode(strs, "/");
      strs -= ({""}); // remove trailing and leading "/".
      if (sizeof(strs)>1 && strs[0]=="log") return path;
      return 0;
    case "save_object":
      if (!path) return 0;
      strs=full_path_array("/"+path, 0);
      break;
    case "ed_start":
      if (path && path[0])
        strs=full_path_array(path, euid);
      else strs=({"players",euid,".err"});
      break;
    default:
      strs=full_path_array(path, euid);
  }

  if (!euid || euid=="NOBODY" || euid=="ftp" || euid=="anonymous") return 0;

  // Pfad soll ab jetzt auf jeden Fall absolut sein.
  if (strs[0] != "")
      path = "/" + implode(strs, "/");
  else
      path=implode(strs, "/");

  // fuer die Rechtebestimmung "/" an Anfang und Ende entfernen
  strs -= ({""});

  //Root-Objekte und Master duerfen immer.
  if (euid == ROOTID || obj==TO) return path;

  lvl=query_wiz_level(euid);

  //Toplevel: nur EM+
  if ((s=sizeof(strs))<=1) return lvl>=ARCH_LVL;

  switch(strs[0]) {
    case TMPDIR:
      return 1;

    case STDDIR:
    case SYSDIR:
    case LIBOBJDIR:
    case ETCDIR:
    case LIBROOMDIR:
      return lvl>=ARCH_LVL;
    
    case LIBITEMDIR:
      return lvl>=ELDER_LVL;

    case SPELLBOOKDIR:
      // wenn unterhalb eines unterverzeichnisses des Stils "gilde", dann ists
      // einfach
      if (sizeof(strs) > 2
          && guild_master(euid, strs[1]))
        return 1;

      // sonst nur EMs bzw. access_rights.c fragen.
      return ((lvl>=ARCH_LVL) || access_rights(strs,euid));

    case GUILDDIR:
      // wenn unterhalb eines unterverzeichnisses des Stils "filde.gilde",
      // dann ists einfach
      if (sizeof(strs) > 2) {
        string *tmp = explode(strs[1],"files.");
        if (sizeof(tmp) == 2) {
          if (guild_master(euid, tmp[1]))
            return 1;
        // Objekte dort sollen auch schreiben duerfen.
          if (euid == ("GUILD."+tmp[1]))
            return 1;
        }
      }
      // sonst nur EMs bzw. access_rights.c fragen.
      return ((lvl>=ARCH_LVL) || access_rights(strs,euid));

    case DOCDIR:
      if ( s > 1 && (strs[1] == "balance") )
        return ((lvl>=ARCH_LVL) || access_rights(strs,euid));
      else
        return ((lvl>=SPECIAL_LVL) || access_rights(strs,euid));

    case FTPDIR:
        if ( s > 1 && (strs[1] == "incoming" || strs[1] == "tmp" ||
                       s == 3 && strs[1] == "News" && strs[2] == euid+".news") )
            return 1;

        if (lvl>=ELDER_LVL)
            return 1;

        return 0;

    case MAILDIR:
      if (euid==MAILID || strs[1]=="spool") break;
      return 0;

    case LIBSAVEDIR:
      if (lvl>=ARCH_LVL) return 1;
      if (s==3 && strs[1] == euid[0..0] &&
          (strs[2]==euid+".o" || strs[2]==euid)) break;
      return 0;

    case LIBLOGDIR:
      /* auf /log/ARCH/ haben wirklich nur EMs Zugriff */
      if (strs[1]=="ARCH" && lvl<ARCH_LVL) return 0;

      /* alles andere duerfen auch Weise... */
      if (lvl>=ELDER_LVL) return 1;

      /* Allgemeine logfiles in /log duerfen wirklich nur geschrieben werden */
      if (s==2 && strs[1][0]>='A' && strs[1][0]<='Z' && fun != "write_file")
         return 0;

      /* Unterhalb von /log/syslog darf nur geschrieben werden */
      if (s>1 && strs[1]=="syslog" && fun != "write_file")
         return 0;

      /* loggen ins eigene repfile immer erlauben */
      if (s==3 && strs[1]=="report" &&
          strs[2]==explode(euid, ".")[<1]+".rep") break;

      /* in fremden Verzeichnissen hat niemand was zu loggen */
      if (get_wiz_level(strs[1]) >= WIZARD_LVL &&
          strs[1] != efun::explode(euid, ".")[<1])
         return 0;
      break;

    case WIZARDDIR:
      /* kein Zugriff auf Objekte mit hoeheren Rechten */
      if (query_wiz_level(strs[1]) > lvl) return 0;

      /* Magier selbst oder Weise duerfen hier schreiben */
      if ((IS_WIZARD(euid) && euid==strs[1])||(lvl>=ELDER_LVL)) break;

      /* Es steht jedoch frei, auch anderen Schreibrechte zu geben... */
      return access_rights(strs,euid);
      
    case DOMAINDIR:
      /* neue Regionen duerfen nur Erzmagier anlegen */
      if (s<2 && lvl<ARCH_LVL) return 0;

      /* kein Zugriff auf Objekte mit hoeheren Rechten */
      if (s>2 && query_wiz_level(creator_file(path)) > lvl)
         return 0;

      /* Auf Regionfiles von erzmagier haben nur EMs Zugriff */
      if (creator_file(path)=="erzmagier" && lvl<ARCH_LVL) return 0;

      /* innerhalb der Region haben RMs und Weise volle Schreibrechte */
      if (lvl>=ELDER_LVL || domain_master(euid,strs[1])) break;

      /* neue Verzeichnisse in der Region kann nur RM+ anlegen */
      if (s<=3 && (fun=="mkdir" || fun=="rmdir")) return 0;

      /* Magier auf ihre eigenen Files... */
      if (s>2 && strs[2]==euid) break;

      /* Files der Magier in der Region in ihre eigenen Verzeichnisse... */
      if (s>2 && euid==sprintf("d.%s.%s", strs[1], strs[2])) break;

      /* Files mit Regionsuid */
      if (euid==strs[1]) break;

      /* Es ist moeglich anderen Magiern Rechte bei sich einzuraeumen. */
      if (access_rights(strs,euid)>0) break;

      /* Auf das Verzeichniss 'alle' haben alle Regionsmitglieder Zugriff.
         Ausser, wenn sie ueber access_rights.c explizit ausgeschlossen
         werden (Returncode < 0). */
      if (s>2 && strs[2]=="alle" && domain_member(euid, strs[1]) &&
          access_rights(strs,euid)>=0) break;
      return 0;

    case PROJECTDIR:
      /* Nur Erzmagier duerfen neue Projektverzeichnisse anlegen... */
      if (s<3 && lvl<ARCH_LVL) return 0;

      /* alles weitere duerfen auch Weise tun */
      if (lvl>=ELDER_LVL) break;

      /* in den Serviceverzeichnissen muss lediglich der Name stimmen */
      if (s>3 && strs[1]=="service" && euid==strs[2]) break;

      /* Objekte eines Projektes haben Schreibzugriffe auf ihr Projekt */
      if (s>3 && (implode(strs[0..1], ".") == euid
                  || implode(strs[0..2], ".") == euid) ) return 1;

      /* Schreibrechte koennen natuerlich auch vergeben werden... */
      if (project_access(euid,strs[1])) break;
      // Alternativ besteht neben dem ACCESS_RIGHTS auch noch die
      // Moeglichkeit, per access_rights.c Rechte einzuraeumen.
      if (access_rights(strs,euid)>0) break;

     return 0;

    default: return 0;
  }
  return path;
}

mixed valid_read(string path, string euid, string fun, object obj)
{
  int s, lev;
  string *strs, suf;

  if (member(path,' ')!=-1) return 0;

  // Unter LIBDATADIR (/data) sollen komplett identische Leserechte
  // vergeben werden.
  if (sizeof(path) > 6
      && path[0..5] == "/"LIBDATADIR"/")
    return valid_read(path[5..], euid, fun, obj) != 0;

  if (!euid) euid="-";

  strs=full_path_array(path, euid);
  // Pfad soll ab jetzt auf jeden Fall absolut sein.
  if (strs[0] != "")
      path = "/" + implode(strs, "/");
  else
      path=implode(strs, "/");

  // fuer die Rechtebestimmung "/" an Anfang und Ende entfernen
  strs -= ({""});

  if (!sizeof(strs) || !sizeof(path) || euid == ROOTID || obj==TO) return path;

  if ((s=sizeof(strs)) <= 1) return path;

  lev=query_wiz_level(euid);

  switch(strs[0]) {
    case "core":
    case "wahl":
      return 0;

    case NEWSDIR:
      if (strs[1] == "archiv.pub") // oeffentliches archiv
        return path;
      else if (strs[1] == "archiv.magier" // Magier-Archiv
               && lev >= WIZARD_LVL)
        return path;

      return 0; // kein Zugriff auf /news/ oder alles andere.

    case "maps":
      if (lev<=WIZARD_LVL) return 0;

    case "":
    case ETCDIR:
    case STDDIR:
    case SYSDIR:
    case DOCDIR:
    case LIBOBJDIR:
    case LIBROOMDIR:
    case LIBITEMDIR:
    case FTPDIR:
      return path;

    case MAILDIR:
      return (euid==MAILID);

    case SECUREDIR:
      if (strs[1]=="save") return 0;
      if (path[<2..]==".o" || path[<5..]==".dump") return 0;
      if (strs[1]!="ARCH" || lev>=ARCH_LVL) return path;

    case LIBLOGDIR:
      if ( strs[1] == "ARCH" && !IS_DEPUTY(euid) )
          return 0;

      if ( strs[1] == "ARCH" && lev < ARCH_LVL && s == 3 &&
           strs[2] != "*" && strs[2][0..2] != "bb." &&
           member( deputy_files, strs[2] ) < 0 )
          return 0;

      if ( lev > WIZARD_LVL )
          return path;

      if ( s == 2 && (strs[1] == "report" || strs[1] == "error") )
          return path; // fuer 'cd'

      // /log sollte jeder auflisten koennen
      if ( s == 2 && strs[1]=="*" && fun=="get_dir")
          return path;

      // unter /log/report/, /log/error und /log/warning/ duerfen alle lesen
      if ( s==3 && 
          (strs[1] == "report" || strs[1] == "error"  
           || strs[1] =="warning"))
          return path;

      // /log/<Magiername> darf von <Magiername> natuerlich auch
      // gelesen werden
      if ( s >= 2 && strs[1] == euid )
          return path;

      return 0;

    case "backup":
    case LIBSAVEDIR:
      if (lev>WIZARD_LVL) return path;

      /* Objekte in /p/* haben bisher leider wizlevel 0 */
      //if (lev==0) return path;

      if (fun=="file_size") return path;

      // das eigene Savefile darf man natuerlich immer. ;-)
      if (s==3 && (strs[2]==euid+".o" || strs[2]==euid))
        return path;
      return 0;

    case PROJECTDIR:
      /* Pruefen ob ein File existiert darf jeder... */
      if (fun=="file_size") return path;

      /* Die Service-Verzeichnisse darf jeder lesen */
      if (s>3 && strs[1]=="service") return path;

      //Weise duerfen in /p/ schreiben, also auch lesen. (Zesstra, 04.11.06)
      if (lev>=ELDER_LVL) return path;

      /* wer hier schreiben darf, darf natuerlich auf jeden Fall lesen */
      //Anmerkung v. Zesstra: das prueft nur, ob jemand in ACCESS_RIGHTS
      //steht, nicht ob jemand (ggf. aus anderen Gruenden schreiben darf)
      if (project_access(euid,strs[1])) return path;
      //Alternativ kann man explizite Schreibrechte auch via access_rights.c
      //vergeben. (und damit natuerlich auch Leserechte)
      if (access_rights(strs,euid)>0) return path;

      /* Objekte eines Projektes haben Lesezugriff auf ihr Projekt */
      if (s>3 && (implode(strs[0..1], ".") == euid
                  || implode(strs[0..2], ".") == euid) ) return path;

      /* Fall-Through */

    case GUILDDIR:
      /* "secure"-Verzeichnisse darf nur lesen, wer da auch schreiben darf */
      if ( s > 3 && strs[<2] == "secure" && member( insecure, strs[1] ) < 0
           && lev < ARCH_LVL && !access_rights(strs, euid) )
          return 0;

      /* Pruefen ob ein File existiert darf jeder... */
      if (fun=="file_size") return path;

      /* Fall-Through */

    case SPELLBOOKDIR:
      // Gildenpbjekte koennen hier lesen
      if (lev==0 && euid[0..4]=="GUILD") return path;

      // Mit Level <= 20 darf man hier nichts lesen
      if ( lev<=WIZARD_LVL && sizeof(regexp( ({strs[<1]}), "\\.[och]" )) )
        return 0;

      return path;

    case WIZARDDIR:
      if (s==2) return path;
      /* das eigene Verzeichniss darf man natuerlich immer lesen... */
      if (strs[1]==euid && lev>=WIZARD_LVL) return path;

      /* Pruefen ob ein File existiert darf jeder... */
      if (fun=="file_size") return path;

      /* fremde Verzeichnisse mit <= Level 20 noch nicht */
      if (lev<=WIZARD_LVL) return 0;

      /* wo man schreiben darf, darf man natuerlich auch lesen... */
      if (lev>=ELDER_LVL) return path;

      // kein Zugriff auf archivierten Code (wo z.B. secure/ drin sein
      // koennen)
      if (member(({"bz2","gz","zip"}), explode(strs[<1],".")[<1]) > -1)
        return 0;

      /* Files ohne Code sind nicht weiter interessant... */
      if ( !sizeof(regexp( ({strs[<1]}), "\\.[och]" )) )
          return path;

      /* folgende Funktionen sind natuerlich voellig uninteressant */
      if (member(({"get_dir", "restore_object"}), fun)!=-1)
          return path;

      /* Zugriff erlaubt, aber mitloggen */
      write_file( READ_FILE, sprintf("%O %s %s: %s\n", fun, ctime()[4..],
                                     euid, path) );
      return path;

    case DOMAINDIR:
      /* Mit Level 15 darf man hier _nichts_ lesen */
      if ( fun!="file_size" && lev<WIZARD_LVL &&
           sizeof(regexp( ({strs[<1]}), "\\.[och]" )) ) return 0;

      /* den Verzeichnisbaum von /d/ darf man natuerlich sonst lesen */
      if (s<=2) return path;

      /* eigenen Code darf man natuerlich auch lesen */
      if (euid==strs[2] || euid==sprintf("d.%s.%s", strs[1], strs[2]))
         return path;

      /* Deputies haben ein gemeinsames Verzeichnis unter /d/erzmagier */
      if (strs[1]=="erzmagier" && strs[2]=="polizei" && IS_DEPUTY(euid))
          return path;

      /* d/erzmagier darf man nur als Erzmagier lesen */
      if (strs[1]=="erzmagier" && lev<ARCH_LVL) return 0;

      /* einige Regionen haben std-verzeichnisse, die darf man natuerlich
         auch mit Level 20 bereits komplett lesen! */
      if (strs[2]=="std") return path;

      /* "secure"-Verzeichnisse darf nur lesen, wer da auch schreiben darf */
      if ( s > 4 && strs[<2] == "secure" && member( insecure, strs[2] ) < 0 ){
          if ( lev < ELDER_LVL && !domain_master(euid, strs[1])
               && !access_rights(strs, euid) )
              return 0;
          else
              return path;
      }

      /* Dokus, Infos und .readme darf man immer lesen... */
      if ( fun=="file_size" || !sizeof(regexp( ({strs[<1]}), "\\.[och]" )) )
          return path;

      /* Mit Level 20 darf man noch keinen fremden Code lesen! */
      if (lev<=WIZARD_LVL) return 0;

      /* Weise duerfen natuerlich alles weitere lesen */
      if (lev>=ELDER_LVL) return path;

      /* Regionsmagier in ihren Regionen natuerlich auch */
      if (lev>=LORD_LVL && domain_master(euid, strs[1])) return path;

      /* folgende Funktionen sind natuerlich voellig uninteressant */
      if (member(({"get_dir", "restore_object"}), fun)!=-1)
          return path;

      /* Zugriff erlaubt, aber mitloggen */
      write_file( READ_FILE, sprintf("%O %s %s: %s\n", fun, ctime()[4..],
                                     euid, path) );
      return path;
  }
  if (lev<ARCH_LVL) return 0;
  return path;
}


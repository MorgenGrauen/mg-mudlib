// MorgenGrauen MUDlib
//
// simul_efun.c -- simul efun's
//
// $Id: simul_efun.c 7408 2010-02-06 00:27:25Z Zesstra $
#pragma strict_types,save_types,rtt_checks
#pragma no_clone,no_shadow,no_inherit
#pragma pedantic,range_check,warn_deprecated
#pragma warn_empty_casts,warn_missing_return,warn_function_inconsistent

// Absolute Pfade erforderlich - zum Zeitpunkt, wo der Master geladen
// wird, sind noch keine Include-Pfade da ...

#define SNOOPLOGFILE "SNOOP"
#define ASNOOPLOGFILE "ARCH/SNOOP"

public int mkdirp(string dir);

#include "/secure/config.h"
#include "/secure/wizlevels.h"
#include "/sys/snooping.h"
#include "/sys/language.h"
#include "/sys/thing/properties.h"
#include "/sys/wizlist.h"
#include "/sys/erq.h"
#include "/sys/lpctypes.h"
#include "/sys/daemon.h"
#include "/sys/player/base.h"
#include "/sys/thing/description.h"
#include "/sys/container.h"
#include "/sys/defines.h"
#include "/sys/telnet.h"
#include "/sys/objectinfo.h"
#include "/sys/files.h"
#include "/sys/strings.h"
#include "/sys/time.h"
#include "/sys/lpctypes.h"
#include "/sys/notify_fail.h"
#include "/sys/tls.h"
#include "/sys/input_to.h"
#include "/sys/object_info.h"

/* function prototypes
 */
string dtime(int wann);
varargs int log_file(string file, string txt, int size_to_break);
int query_wiz_level(mixed player);
nomask varargs int snoop(object me, object you);
varargs string country(mixed ip, string num);
int query_wiz_grp(mixed wiz);
public varargs object deep_present(mixed what, object ob);
nomask int secure_level();
nomask string secure_euid();
public nomask int process_call();
nomask mixed __create_player_dummy(string name);
varargs string replace_personal(string str, mixed *obs, int caps);


//replacements for dropped efuns in LD
#if !__EFUN_DEFINED__(extract)
varargs string extract(string str, int from, int to);
#endif
#if !__EFUN_DEFINED__(slice_array)
varargs mixed slice_array(mixed array, int from, int to);
#endif
#if !__EFUN_DEFINED__(member_array)
int member_array(mixed item, mixed arraystring);
#endif

// Include the different sub 'modules' of the simul_efun
#include __DIR__"debug_info.c"
#include __DIR__"enable_commands.c"
#include __DIR__"hash.c"
#include __DIR__"object_info.c"
#include __DIR__"query_editing.c"
#include __DIR__"query_idle.c"
#include __DIR__"query_input_pending.c"
#include __DIR__"query_ip_name.c"
#include __DIR__"query_limits.c"
#include __DIR__"query_load_average.c"
#include __DIR__"query_mud_port.c"
#include __DIR__"query_once_interactive.c"
#include __DIR__"query_snoop.c"
#include __DIR__"set_heart_beat.c"
#if __BOOT_TIME__ < 1456261859
#include __DIR__"set_prompt.c"
#endif
#include __DIR__"shadow.c"
#include __DIR__"livings.c"
#include __DIR__"comm.c"
#include __DIR__"files.c"
#include __DIR__"seteuid.c"

#define TO        efun::this_object()
#define TI        efun::this_interactive()
#define TP        efun::this_player()
#define PO        efun::previous_object(0)
#define LEVEL(x) query_wiz_level(x)
#define NAME(x)  capitalize(getuid(x))

#define DEBUG(x) if (find_player("zesstra")) \
  tell_object(find_player("zesstra"),x)

mixed dtime_cache = ({-1,""});

#ifdef IOSTATS
struct iostat_s {
  string oname;
  int time;
  int size;
};
mixed saveo_stat = ({ 0,allocate(200, 0) });
mixed restoreo_stat = ({ 0,allocate(200,0) });
//mixed writefile_stat = ({ 0,allocate(100,(<iostat_s>)) });
//mixed readfile_stat = ({ 0,allocate(100,(<iostat_s>)) });
//mixed log_stat = ({ 0,allocate(100,(<iostat_s>)) });

mixed ___iostats(int type) {
  switch(type) {
    case 1:
      return saveo_stat;
    case 2:
      return restoreo_stat;
/*    case 3:
      return writefile_stat;
    case 4:
      return readfile_stat;
    case 5:
      return log_stat;
      */
  }
  return 0;
}
#endif

// Nicht jeder Magier muss die simul_efun entsorgen koennen.
string NotifyDestruct(object caller) {
    if( (caller!=this_object() && !ARCH_SECURITY) || process_call() ) {
      return "Du darfst das simul_efun Objekt nicht zerstoeren!\n";
    }
    return 0;
}

public nomask void remove_interactive( object ob )
{
    if ( objectp(ob) && previous_object()
        && object_name(previous_object())[0..7] != "/secure/"
        && ((previous_object() != ob
             && (ob != this_player() || ob != this_interactive()))
            || (previous_object() == ob
               && (this_player() && this_player() != ob
                   || this_interactive() && this_interactive() != ob)) ) )

       log_file( "PLAYERDEST",
                sprintf( "%s: %O ausgeloggt von PO %O, TI %O, TP %O\n",
                        dtime(time()), ob, previous_object(),
                        this_interactive(), this_player() ) );

    efun::remove_interactive(ob);
}


void ___updmaster()
{
    object ob;

    //sollte nicht jeder duerfen.
    if (process_call() || !ARCH_SECURITY)
      raise_error("Illegal use of ___updmaster()!");

    write("Removing old master ... ");
    foreach(string file: 
        get_dir("/secure/master/*.c",GETDIR_NAMES|GETDIR_UNSORTED|GETDIR_PATH)) {
      if (ob = find_object(file))
       efun::destruct(ob);
    }
    efun::destruct(efun::find_object("/secure/master"));
    write("done.\nLoading again ... ");
    load_object("/secure/master");

    write("done.\n");
}

varargs string country(mixed ip, string num) {
  mixed ret;

  if(ret = (string)"/p/daemon/iplookup"->country(num || ip)) {
    return ret;
  } else return "???";
}


// * Snoopen und was dazugehoert
static object find_snooped(object who)
{
  object *u;
  int i;

  for (i=sizeof(u=users())-1;i>=0;i--)
    if (who==efun::interactive_info(u[i], II_SNOOP_NEXT))
      return u[i];
  return 0;
}

private string Lcut(string str) {
  return str[5..11]+str[18..];
}

nomask varargs int snoop( object me, object you )
{
    int ret;
    object snooper0, snooper, snooper2, snooper3;

    if( !objectp(me) || me == you || !PO )
       return 0;

    snooper0 = find_snooped(me);

     if(you){
        if ( PO != me && query_wiz_grp(me) >= query_wiz_grp(geteuid(PO)) )
            return 0;

        if ( query_wiz_grp(me) <= query_wiz_grp(you) &&
             !(you->QueryAllowSnoop(me)) )
            if ( !IS_DEPUTY(me) || IS_ARCH(you) )
               return 0;

        if ( (snooper = efun::interactive_info(you, II_SNOOP_NEXT)) &&
             query_wiz_grp(snooper) >= query_wiz_grp(me) ){
            if ( (int)snooper->QueryProp(P_SNOOPFLAGS) & SF_LOCKED )
               return 0;

            tell_object( snooper, sprintf( "%s snooped jetzt %s.\n",
                                       me->name(WER), you->name(WER) ) );

            snooper2 = me;

            while ( snooper3 = interactive_info(snooper2, II_SNOOP_NEXT) ){
               tell_object( snooper,
                           sprintf( "%s wird seinerseits von %s gesnooped.\n"
                                   ,snooper2->name(WER),
                                   snooper3->name(WEM) ) );
               snooper2 = snooper3;
            }

            efun::snoop( snooper, snooper2 );

            if ( efun::interactive_info(snooper2, II_SNOOP_NEXT) != snooper )
               tell_object( snooper, sprintf( "Du kannst %s nicht snoopen.\n",
                                          snooper2->name(WEN) ) );
            else{
               tell_object( snooper, sprintf( "Du snoopst jetzt %s.\n",
                                          snooper2->name(WEN) ) );
               if ( !IS_DEPUTY(snooper) ){
                   log_file( SNOOPLOGFILE, sprintf("%s: %O %O %O\n",
                                               dtime(time()),
                                               snooper,
                                               snooper2,
                                               environment(snooper2) ),
                            100000 );
                   if (snooper0)
                      CHMASTER->send( "Snoop", snooper,
                                    sprintf( "%s *OFF* %s (%O)",
                                            capitalize(getuid(snooper)),
                                            capitalize(getuid(snooper0)),
                                            environment(snooper0) ) );

                   CHMASTER->send( "Snoop", snooper,
                                 sprintf("%s -> %s (%O)",
                                        capitalize(getuid(snooper)),
                                        capitalize(getuid(snooper2)),
                                        environment(snooper2)));
               }
               else{
                   log_file( ASNOOPLOGFILE, sprintf( "%s: %O %O %O\n",
                                                 dtime(time()),
                                                 snooper,
                                                 snooper2,
                                                 environment(snooper2) )
                            ,100000 );
               }
            }
        }
        else
            if (snooper)
               if ( !me->QueryProp(P_SNOOPFLAGS) & SF_LOCKED ){
                   printf( "%s wird bereits von %s gesnooped. Benutze das "
                          "\"f\"-Flag, wenn du dennoch snoopen willst.\n",
                          you->name(WER), snooper->name(WEM) );
                   return 0;
               }

        ret = efun::snoop( me, you );

        if ( !IS_DEPUTY(me) && efun::interactive_info(you, II_SNOOP_NEXT) == me){
            log_file( SNOOPLOGFILE, sprintf( "%s: %O %O %O\n",
                                         Lcut(dtime(time())),
                                         me, you, environment(you) ),
                     100000 );

            if (snooper0)
               CHMASTER->send( "Snoop", me,
                             sprintf( "%s *OFF* %s (%O).",
                                     capitalize(getuid(me)),
                                     capitalize(getuid(snooper0)),
                                     environment(snooper0) ) );

            CHMASTER->send( "Snoop", me, sprintf( "%s -> %s (%O).",
                                             capitalize(getuid(me)),
                                             capitalize(getuid(you)),
                                             environment(you) ) );
        }
        else{
            if ( efun::interactive_info(you, II_SNOOP_NEXT) == me ){
               log_file( ASNOOPLOGFILE, sprintf( "%s: %O %O %O\n",
                                             Lcut(dtime(time())),
                                             me, you, environment(you) ),
                        100000 );
            }
        }

        if ( ret && query_wiz_grp(me) <= query_wiz_grp(you) &&
             !IS_DEPUTY(me) )
            tell_object( you, "*** " + NAME(me) + " snoopt Dich!\n" );

        return ret;
     }
     else {
        if ( (me == PO ||
              query_wiz_grp(geteuid(PO)) > query_wiz_grp(me) ||
              (query_wiz_grp(geteuid(PO)) == query_wiz_grp(me) &&
              efun::interactive_info(PO, II_SNOOP_NEXT) == me)) && snooper0 ){
            if ( !IS_DEPUTY(me) ){
               log_file( SNOOPLOGFILE, sprintf( "%s: %O %O %O *OFF*\n",
                                            Lcut(dtime(time())), me,
                                            snooper0,
                                            environment(snooper0) ),
                        100000 );

                CHMASTER->send( "Snoop", me,
                              sprintf( "%s *OFF* %s (%O).",
                                      capitalize(getuid(me)),
                                      capitalize(getuid(snooper0)),
                                      environment(snooper0) ) );
            }
            else{
               log_file( ASNOOPLOGFILE, sprintf( "%s: %O %O %O *OFF*\n",
                                             Lcut(dtime(time())), me,
                                             snooper0,
                                             environment(snooper0) ),
                        100000 );
            }

            return efun::snoop(me);
        }
     }
     return 0;
}



// * Emulation des 'alten' explode durch das neue
string *old_explode(string str, string del) {
  int s, t;
  string *strs;

  if (!stringp(str)) {
      set_this_object(previous_object());
      raise_error(sprintf(
         "Invalid argument 1 to old_explode()! Expected <string>, got: "
         "%.30O\n",str));
  }
  if (!stringp(del)) {
      set_this_object(previous_object());
      raise_error(sprintf(
         "Invalid argument 2 to old_explode()! Expected <string>, got: "
         "%.30O\n",del));
  }
  if(del == "")
    return ({str});
  strs=efun::explode(str, del);
  t=sizeof(strs)-1;
  while(s<=t && strs[s++] == "");s--;
  while(t>=0 && strs[t--] == "");t++;
  if(s<=t)
    return strs[s..t];
  return ({});
}

int file_time(string path) {
  mixed *v;

  set_this_object(previous_object());
  if (sizeof(v=get_dir(path,GETDIR_DATES))) return v[0];
  return(0); //sonst
}

// * Magier-Level abfragen
int query_wiz_level(mixed player) {
  return (int)"/secure/master"->query_wiz_level(player);
}

#ifdef __ALISTS__
// * Element aus Alist loeschen (by key)
mixed *remove_alist(mixed key,mixed *alist)
{
  int i,j;

  if (!pointerp(alist) || !sizeof(alist))
    return alist;
  if (!pointerp(alist[0]))
  {
    if ((i=assoc(key,alist))<0)
      return alist;
    return alist[0..i-1]+alist[i+1..];
  }
  i = assoc(key,alist[0]);
  if ((i=assoc(key,alist[0]))<0)
    return alist;
  alist=alist[0..];
  for (j=sizeof(alist)-1;j>=0;j--)
    alist[j]=alist[j][0..i-1]+alist[j][i+1..];
  return alist;
}

// * Element aus Alist loeschen (by pos)
mixed *exclude_alist(int i,mixed *alist)
{
  int j;
  if (!pointerp(alist) || !sizeof(alist) || i<0)
    return alist;
  if (!pointerp(alist[0]))
    return alist[0..i-1]+alist[i+1..];
  alist=alist[0..]; /* Create PHYSICAL copy of alist */
  for (j=sizeof(alist)-1;j>=0;j--)
    alist[j]=alist[j][0..i-1]+alist[j][i+1..];
  return alist; /* order_alist is NOT necessary - see /doc/LPC/alist */
}
#endif // __ALISTS__

// * German version of ctime()
#define TAGE ({"Son","Mon","Die","Mit","Don","Fre","Sam"})
#define MONATE ({"Jan","Feb","Mar","Apr","Mai","Jun","Jul","Aug",\
                 "Sep","Okt","Nov","Dez"})
string dtime(int wann) {
  
  if (wann == dtime_cache[0])
    return(dtime_cache[1]);

  int *lt = localtime(wann);
  return sprintf("%s, %2d. %s %d, %02d:%02d:%02d",
      TAGE[lt[TM_WDAY]], lt[TM_MDAY], MONATE[lt[TM_MON]], 
      lt[TM_YEAR],lt[TM_HOUR], lt[TM_MIN], lt[TM_SEC]);
}

// wenn strftime im Driver nicht existiert, ist dies hier ein Alias auf dtime(),
// zwar stimmt das Format dann nicht, aber die Mudlib buggt nicht und schreibt
// ein ordentliches Datum/Uhrzeit.
#if !__EFUN_DEFINED__(strftime)
varargs string strftime(mixed fmt, int clock, int localized) {
  if (intp(clock) && clock >= 0)
    return dtime(clock);
  else if (intp(fmt) && fmt >= 0)
    return dtime(fmt);
  
  return dtime(time());
}
#endif //!__EFUN_DEFINED__(strftime)

// * Shutdown mit zusaetzlichem logging
nomask int shutdown(string reason)
{
  string name;
  string obname;
  string output;

  if (!reason)
    return 0;
  if ( !ARCH_SECURITY && getuid(previous_object())!=ROOTID &&
          object_name(previous_object())!="/obj/shut" )
  {
    write("You have no permission to shut down the gamedriver!\n");
    return 0;
  }
  if ((this_interactive())&&(name=getuid(this_interactive())))
  {
    name=capitalize(name);
    filter(users(),#'tell_object,//'
               capitalize(name)+" faehrt das Spiel herunter!\n");
  }
  else
    name="ANONYMOUS";
  if (previous_object()) obname=capitalize(getuid(previous_object()));
  output=name;
  if (obname && name!=obname) output=output+" ("+obname+")";
  if (previous_object()&&object_name(previous_object())=="/obj/shut"){
    output+=" faehrt das Spiel via Armageddon herunter.\n";
    output=dtime(time())+": "+output;
    log_file("GAME_LOG",output+"\n",-1);
    efun::shutdown();
    return 1;
  }
  output=ctime(time())+": "+output+" faehrt das Spiel herunter.\n";
  output+="    Grund: "+reason;
  log_file("GAME_LOG",output+"\n",-1);
  efun::shutdown();
  return 1;
}

// * lowerchar

int lowerchar(int char) {
  if (char<'A' || char>'Z') return char;
  return char+32;
}

// * upperstring

string upperstring(string s)
{
#if __EFUN_DEFINED__(upper_case)
  return(upper_case(s));
#else
  int i;
  if (!stringp(s)) return 0;
  for (i=sizeof(s)-1;i>=0;i--) s[i]=((s[i]<'a'||s[i]>'z')?s[i]:s[i]-32);
  return s;
#endif
}

// * lowerstring

string lowerstring(string s)
{
  if (!stringp(s)) return 0;
  return lower_case(s);
}


// * GD version
string version()
{
  return __VERSION__;
}

// * break_string
// stretch() -- stretch a line to fill a given width 
private string stretch(string s, int width) {
  int len=sizeof(s);
  if (len==width) return s;

  // reine Leerzeilen, direkt zurueckgeben
  string trimmed=trim(s,TRIM_LEFT," ");
  if (trimmed=="") return s; 
  int start_spaces = len - sizeof(trimmed);

  string* words = explode(trimmed, " ");
  // der letzte kriegt keine Spaces
  int word_count=sizeof(words) - 1;
  // wenn Zeile nur aus einem Wort, wird das Wort zurueckgegeben
  if (!word_count)
    return " "*start_spaces + words[0];

  int space_count = width - len;

  int space_per_word=(word_count+space_count) / word_count;
  // Anz.Woerter mit Zusatz-Space
  int rest=(word_count+space_count) % word_count; 
  // Rest-Spaces Verteilen
  foreach (int pos : rest) words[pos]+=" ";
  return (" "*start_spaces) + implode( words, " "*space_per_word );
}

// aus Geschwindigkeitsgruenden hat der Blocksatz fuer break_string eine
// eigene Funktion bekommen:
private varargs string block_string(string s, int width, int flags) {
  // wenn BS_LEAVE_MY_LFS, aber kein BS_NO_PARINDENT, dann werden Zeilen mit
  // einem Leerzeichen begonnen.
  // BTW: Wenn !BS_LEAVE_MY_LFS, hat der Aufrufer bereits alle \n durch " "
  // ersetzt.
  if ( (flags & BS_LEAVE_MY_LFS)
      && !(flags & BS_NO_PARINDENT))
  {
      s = " "+regreplace(s,"\n","\n ",1);
  }

  // sprintf fuellt die letzte Zeile auf die Feldbreite (hier also
  // Zeilenbreite) mit Fuellzeichen auf, wenn sie NICHT mit \n umgebrochen
  // ist. Es wird an die letzte Zeile aber kein Zeilenumbruch angehaengt.
  // Eigentlich ist das Auffuellen doof, aber vermutlich ist es unnoetig, es
  // wieder rueckgaengig zu machen.
  s = sprintf( "%-*=s", width, s);

  string *tmp=explode(s, "\n");
  // Nur wenn s mehrzeilig ist, Blocksatz draus machen. Die letzte Zeile wird
  // natuerlich nicht gedehnt. Sie ist dafuer schon von sprintf() aufgefuellt
  // worden. BTW: Die letzte Zeile endet u.U. noch nicht mit einem \n (bzw.
  // nur dann, wenn BS_LEAVE_MY_LFS und der uebergebene Text schon nen \n am
  // Ende der letzten Zeile hat), das macht der Aufrufer...
  if (sizeof(tmp) > 1)
    return implode( map( tmp[0..<2], #'stretch/*'*/, width ), "\n" ) 
      + "\n" + tmp[<1];

  return s;
}

public varargs string break_string(string s, int w, mixed indent, int flags)
{
    if ( !s || s == "" ) return "";

    if ( !w ) w=78;

    if( intp(indent) )
       indent = indent ? " "*indent : "";

    int indentlen=stringp(indent) ? sizeof(indent) : 0;

    if (indentlen>w) {
      set_this_object(previous_object());
      raise_error(sprintf("break_string: indent longer %d than width %d\n",
                  indentlen,w));
      // w=((indentlen/w)+1)*w;
    }

    if (!(flags & BS_LEAVE_MY_LFS)) 
      s=regreplace( s, "\n", " ", 1 );

    if ( flags & BS_SINGLE_SPACE )
       s = regreplace( s, "(^|\n| )  *", "\\1", 1 );
 
    string prefix="";
    if (indentlen && flags & BS_PREPEND_INDENT) {
      if (indentlen+sizeof(s) > w || 
         (flags & BS_LEAVE_MY_LFS) && strstr(s,"\n")>-1) {
       prefix=indent+"\n";
       indent=(flags & BS_NO_PARINDENT) ? "" : " ";
       indentlen=sizeof(indent);
      }
    }

    if ( flags & BS_BLOCK ) {
      /*
           s = implode( map( explode( s, "\n" ),
                               #'block_string, w, indentlen, flags),
                      "" );
      */
      s = block_string( s , w - indentlen, flags );
    }
    else {
      s = sprintf("%-1.*=s",w-indentlen,s);
    }
    if ( s[<1] != '\n' ) s += "\n";

    if ( !indentlen ) return prefix + s;
    
    string indent2 = ( flags & BS_INDENT_ONCE ) ? (" "*indentlen) :indent;
      
    return prefix + indent + 
      regreplace( s[0..<2], "\n", "\n"+indent2, 1 ) + "\n";
      /*
       string *buf;

       buf = explode( s, "\n" );
       return prefix + indent + implode( buf[0..<2], "\n"+indent2 ) + buf[<1] + "\n";
      */
}

// * Elemente aus mapping loeschen - mapping vorher kopieren

mapping m_copy_delete(mapping m, mixed key) {
  return m_delete(copy(m), key);
}

// * times
int last_reboot_time()
{
  return __BOOT_TIME__;
}

int first_boot_time()
{
  return 701517600;
}

int exist_days()
{
  return (((time()-first_boot_time())/8640)+5)/10;
}

// * uptime :)
string uptime()
{
  int t;
  int tmp;
  string s;

  t=time()-__BOOT_TIME__;
  s="";
  if (t>=86400)
    s+=sprintf("%d Tag%s, ",tmp=t/86400,(tmp==1?"":"e"));
  if (t>=3600)
    s+=sprintf("%d Stunde%s, ",tmp=(t=t%86400)/3600,(tmp==1?"":"n"));
  if (t>60)
    s+=sprintf("%d Minute%s und ",tmp=(t=t%3600)/60,(tmp==1?"":"n"));
  return s+sprintf("%d Sekunde%s",t=t%60,(t==1?"":"n"));
}

// * Was tun bei 'dangling' lfun-closures ?
void dangling_lfun_closure() {
  raise_error("dangling lfun closure\n");
}

// * Sperren ausser fuer master/simul_efun

#if __EFUN_DEFINED__(set_environment)
nomask void set_environment(object o1, object o2) {
  raise_error("Available only for root\n");
}
#endif

nomask void set_this_player(object pl) {
  raise_error("Available only for root\n");
}

#if __EFUN_DEFINED__(export_uid)
nomask void export_uid(object ob) {
  raise_error("Available only for root\n");
}
#endif

// * Jetzt auch closures
int process_flag;

public nomask int process_call()
{
  if (process_flag>0)
    return process_flag;
  else return(0);
}

private nomask string _process_string(string str,object po) {
              set_this_object(po);
              return(efun::process_string(str));
}

nomask string process_string( mixed str )
{
  string tmp, err;
  int flag; 

  if ( closurep(str) ) {
      set_this_object( previous_object() );
      return funcall(str);
  }
  else if (str==0)
      return((string)str);
  else if ( !stringp(str) ) {
      return to_string(str);
  }

  if ( !(flag = process_flag > time() - 60))                     
      process_flag=time();

  err = catch(tmp = funcall(#'_process_string,str,previous_object()); publish);

  if ( !flag )
    process_flag=0;

  if (err) {
    // Verarbeitung abbrechen
    set_this_object(previous_object());
    raise_error(err);
  }
  return tmp;
}

// 'mkdir -p' - erzeugt eine komplette Hierarchie von Verzeichnissen.
// wenn das Verzeichnis angelegt wurde oder schon existiert, wird 1
// zurueckgeliefert, sonst 0.
// Wirft einen Fehler, wenn das angebene Verzeichnis nicht absolut ist!
public int mkdirp(string dir) {
  // wenn es nur keinen fuehrenden / gibt, ist das ein Fehler.
  if (strstr(dir, "/") != 0)
    raise_error("mkdirp(): Pfad ist nicht absolute.\n");
  // cut off trailing /...
  if (dir[<1]=='/')
      dir = dir[0..<2];

  int fstat = file_size(dir);
  // wenn es schon existiert, tun wir einfach so, als haetten wir es angelegt.
  if (fstat == FSIZE_DIR)
    return 1;
  // wenn schon ne Datei existiert, geht es nicht.
  if (fstat != FSIZE_NOFILE)
    return 0;
  // wenn es nur einen / gibt (den fuehrenden), dann ist es ein
  // toplevel-verzeichnis, was direkt angelegt wird.
  if (strrstr(dir,"/")==0) {
    return funcall(bind_lambda(#'efun::mkdir, previous_object()), dir);
  }

  // mkdir() nicht direkt rufen, sondern vorher als closure ans aufrufende
  // Objekt binden. Sonst laeuft die Rechtepruefung in valid_write() im Master
  // unter der Annahme, dass die simul_efun.c mit ihrer root id was will.

  // jetzt rekursiv die Verzeichnishierarchie anlegen. Wenn das erfolgreich
  // ist, dann koennen wir jetzt mit mkdir das tiefste Verzeichnis anlegen
  if (mkdirp(dir[0..strrstr(dir,"/")-1]) == 1)
    return funcall(bind_lambda(#'efun::mkdir, previous_object()), dir);
}


// * Properties ggfs. mitspeichern
mixed save_object(mixed name)
{
  mapping properties;
  mapping save;
  mixed index, res;
  int i;
  string oldpath;

  // nur Strings und 0 zulassen
  if ((!stringp(name) || !sizeof(name)) && 
      (!intp(name) || name!=0)) {
      set_this_object(previous_object());
      raise_error(sprintf(
         "Only non-empty strings and 0 may be used as filename in "
         "sefun::save_object()! Argument was %O\n",name));
  }

  if (stringp(name)) {
    // abs. Pfad erzeugen. *seufz*
    if (name[0]!='/')
      name = "/" + name;
    // automatisch in LIBDATADIR speichern
    if (strstr(name,"/"LIBDATADIR"/") != 0) {
      oldpath = name;
      name = "/"LIBDATADIR + name;
      // wenn das Verzeichnis nicht existiert, ggf. anlegen
      string dir = name[0..strrstr(name,"/")-1];
      if (file_size(dir) != FSIZE_DIR) {
        if (mkdirp(dir) != 1)
          raise_error("save_object(): kann Verzeichnis " + dir
              + " nicht anlegen!");
      }
    }
  }

  save = m_allocate(0, 2);
  properties = (mapping)previous_object()->QueryProperties();

  if(mappingp(properties))
  {
    // delete all entries in mapping properties without SAVE flag!
    index = m_indices(properties);
    for(i = sizeof(index)-1; i>=0;i--)
    {
      if(properties[index[i], F_MODE] & SAVE)
      {
       save[index[i]] = properties[index[i]];
       save[index[i], F_MODE] =
       properties[index[i], F_MODE] &
                    (~(SETMNOTFOUND|QUERYMNOTFOUND|QUERYCACHED|SETCACHED));
      }
    }
  }
  else save = ([]);

  // save object!
  previous_object()->_set_save_data(save);
  // format: wie definiert in config.h
  if (stringp(name)) {
    res = funcall(bind_lambda(#'efun::save_object, previous_object()), name,
       __LIB__SAVE_FORMAT_VERSION__);
    // wenn erfolgreich und noch nen Savefile existiert, was nicht unter
    // /data/ liegt, wird das geloescht.
    if (!res && oldpath
        && file_size(oldpath+".o") >= 0)
      rm(oldpath+".o");
  }
  else
    res = funcall(bind_lambda(#'efun::save_object, previous_object()),
       __LIB__SAVE_FORMAT_VERSION__);

  previous_object()->_set_save_data(0);

#ifdef IOSTATS
  // Stats...
  struct iostat_s stat = (<iostat_s>);
  stat->oname = object_name(previous_object());
  stat->time = time();
  //stat->size = (int)object_info(previous_object(),OINFO_MEMORY,
  //    OIM_TOTAL_DATA_SIZE);
  if (stringp(name))
      stat->size = file_size(name + ".o");
  else
      stat->sizeof(res);
  //debug_message("saveo: "+saveo_stat[0]+"\n");
  saveo_stat[1][saveo_stat[0]] = stat;
  saveo_stat[0] = (saveo_stat[0] + 1) % sizeof(saveo_stat[1]);
  //debug_message("saveo 2: "+saveo_stat[0]+"\n");
#endif

  return res;
}

// * Auch Properties laden
int restore_object(string name)
{
  int result;
  mixed index;
  mixed save;
  mapping properties;
  int i;
  closure cl;

  if (sizeof(name) < 1)
  {
    set_this_object(previous_object());
    raise_error("Bad arg 1 to restore_object(): expected non-empty "
                "'string'.\n");
  }

  // Wenn name vermutlich ein Pfad (also nicht mit #x:y anfaengt)
  if (name[0] != '#')
  {
    // abs. Pfad erzeugen *seufz*
    if (name[0]!='/')
      name = "/" + name;

    // .c am Ende loeschen, sonst wird das File ggf. nicht gefunden.
    if(name[<2..]==".c")
      name=name[..<3];

    // wenn kein /data/ vorn steht, erstmal gucken, ob das Savefile unter
    // /data/ existiert. Wenn ja, wird das geladen.
    if (strstr(name,"/"LIBDATADIR"/") != 0)
    {
      string path = "/"LIBDATADIR + name;
      if (file_size(path + ".o") >= 0)
        name = path;
    }
  }

  // get actual property settings (by create())
  properties = (mapping)previous_object()->QueryProperties();

//  DEBUG(sprintf("RESTORE %O\n",name));
  // restore object
  result=funcall(bind_lambda(#'efun::restore_object, previous_object()), name);
  //'))
  //_get_save_data liefert tatsaechlich mixed zurueck, wenn das auch immer ein 
  //mapping sein sollte.
  save = (mixed)previous_object()->_get_save_data();
  if(mappingp(save))
  {
    index = m_indices(save);
    for(i = sizeof(index)-1; i>=0; i--)
    {
      properties[index[i]] = save[index[i]];
      properties[index[i], F_MODE] = save[index[i], F_MODE]
                            &~(SETCACHED|QUERYCACHED);
    }
  }
  else properties = ([]);

  // restore properties
  funcall(
          bind_lambda(
                     unbound_lambda(({'arg}), //'})
                                  ({#'call_other,({#'this_object}),
                                  "SetProperties",'arg})),//')
                     previous_object()),properties);
  previous_object()->_set_save_data(0);

#ifdef IOSTATS
  // Stats...
  //debug_message("restoreo: "+restoreo_stat[0]+"\n");
  struct iostat_s stat = (<iostat_s>);
  stat->oname = object_name(previous_object());
  stat->time = time();
  //stat->size = (int)object_info(previous_object(),OINFO_MEMORY,
  //    OIM_TOTAL_DATA_SIZE);
  stat->size = file_size(name + ".o");
  restoreo_stat[1][restoreo_stat[0]] = stat;

  restoreo_stat[0] = (restoreo_stat[0] + 1) % sizeof(restoreo_stat[1]);
#endif

  return result;
}

// * HB eines Objektes ein/ausschalten
int set_object_heart_beat(object ob, int flag)
{
  if (objectp(ob))
    return funcall(bind_lambda(#'efun::configure_object,ob), ob, OC_HEART_BEAT, flag);
}

// * Magierlevelgruppen ermitteln
int query_wiz_grp(mixed wiz)
{
  int lev;

  lev=query_wiz_level(wiz);
  if (lev<SEER_LVL) return 0;
  if (lev>=GOD_LVL) return lev;
  if (lev>=ARCH_LVL) return ARCH_GRP;
  if (lev>=ELDER_LVL) return ELDER_GRP;
  if (lev>=LORD_LVL) return LORD_GRP;
  if (lev>=SPECIAL_LVL) return SPECIAL_GRP;
  if (lev>=DOMAINMEMBER_LVL) return DOMAINMEMBER_GRP;
  if (lev>=WIZARD_LVL) return WIZARD_GRP;
  if (lev>=LEARNER_LVL) return LEARNER_GRP;
  return SEER_GRP;
}

mixed *wizlist_info()
{
  if (ARCH_SECURITY || !extern_call())
            return efun::wizlist_info();
  return 0;
}

// * wizlist ausgeben
varargs void wizlist(string name, int sortkey ) {

  if (!name)
  {
    if (this_player())
      name = getuid(this_player());
    if (!name)
      return;
  }

  // Schluessel darf nur in einem gueltigen Bereich sein
  if (sortkey<WL_NAME || sortkey>=WL_SIZE) sortkey=WL_COST;

  mixed** wl = efun::wizlist_info();
  // nach <sortkey> sortieren
  wl = sort_array(wl, function int (mixed a, mixed b)
      {return a[sortkey] < b[sortkey]; } );

  // Summe ueber alle Kommandos ermitteln.
  int total_cmd, i;
  int pos=-1;
  foreach(mixed entry : wl)
  {
    total_cmd += entry[WL_COMMANDS];
    if (entry[WL_NAME] == name)
      pos = i;
    ++i;
  }

  if (pos < 0 && name != "ALL" && name != "TOP100")
    return;

  if (name == "TOP100")
  {
    if (sizeof(wl) > 100)
      wl = wl[0..100];
    else
      wl = wl;
  }
  // um name herum schneiden
  else if (name != "ALL")
  {
    if (sizeof(wl) <= 21)
      wl = wl;
    else if (pos + 10 < sizeof(wl) && pos - 10 > 0)
      wl = wl[pos-10..pos+10];
    else if (pos <=21)
      wl = wl[0..20];
    else if (pos >= sizeof(wl) - 21)
      wl = wl[<21..];
    else
      wl = wl;
  }

  write("\nWizard top score list\n\n");
  if (total_cmd == 0)
    total_cmd = 1;
  printf("%-20s %-6s %-3s %-17s %-6s %-6s %-6s\n",
         "EUID", "cmds", "%", "Costs", "HB", "Arrays","Mapp.");
  foreach(mixed e: wl)
  {
    printf("%-:20s %:6d %:2d%% [%:6dk,%:6dG] %:6d %:6d %:6d\n",
          e[WL_NAME], e[WL_COMMANDS], e[WL_COMMANDS] * 100 / total_cmd,
          e[WL_COST] / 1000, e[WL_TOTAL_GIGACOST],
          e[WL_HEART_BEATS], e[WL_ARRAY_TOTAL], e[WL_MAPPING_TOTAL]
          );
  }
  printf("\nTotal         %7d         (%d)\n\n", total_cmd, sizeof(wl));
}


// Ab hier folgen Hilfsfunktionen fuer call_out() bzw. fuer deren Begrenzung

// ermittelt das Objekt des Callouts.
private object _call_out_obj( mixed call_out ) {
    return pointerp(call_out) ? call_out[0] : 0;
}

private void _same_object( object ob, mapping m ) {
  // ist nicht so bloed, wie es aussieht, s. nachfolgede Verwendung von m
  if ( m[ob] )
    m[ob] += ({ ob });
  else
    m[ob] = ({ ob }); 
}

// alle Objekte im Mapping m zusammenfassen, die den gleichen Loadname (Name der
// Blueprint) haben, also alle Clones der BP, die Callouts laufen haben.
// Objekte werden auch mehrfach erfasst, je nach Anzahl ihrer Callouts.
private void _same_path( object key, object *obs, mapping m ) {
  string path;
  if (!objectp(key) || !pointerp(obs)) return;

  path = load_name(key);

  if ( m[path] )
    m[path] += obs;
  else
    m[path] = obs;
}

// key kann object oder string sein.
private int _too_many( mixed key, mapping m, int i ) {
    return sizeof(m[key]) >= i;
}

// alle Objekte in obs zerstoeren und Fehlermeldung ausgeben. ACHTUNG: Die
// Objekte werden idR zu einer BP gehoeren, muessen aber nicht! In dem Fall
// wird auf der Ebene aber nur ein Objekt in der Fehlermeldung erwaehnt.
private void _destroy( mixed key, object *obs, string text, int uid ) {
    if (!pointerp(obs)) return;
    // Array mit unique Eintraege erzeugen.
    obs = m_indices( mkmapping(obs) );
    // Fehlermeldung auf der Ebene ausgeben, im catch() mit publish, damit es
    // auf der Ebene direkt scrollt, der backtrace verfuegbar ist (im
    // gegensatz zur Loesung mittels Callout), die Ausfuehrung aber weiter
    // laeuft.
    catch( efun::raise_error(           
         sprintf( text,                   
           uid ? (string)master()->creator_file(key) : key,                   
           sizeof(obs), object_name(obs[<1]) ) );publish);
    // Und weg mit dem Kram...
    filter( obs, #'efun::destruct/*'*/ );
}

// Alle Objekt einer UID im Mapping m mit UID als KEys zusammenfassen. Objekt
// sind dabei nicht unique.
private void _same_uid( string key, object *obs, mapping m, closure cf ) {
  string uid;

  if ( !pointerp(obs) || !sizeof(obs) )
    return;

  uid = funcall( cf, key );

  if ( m[uid] )
    m[uid] += obs; // obs ist nen Array
  else
    m[uid] = obs;
}

private int _log_call_out(mixed co)
{
  log_file("TOO_MANY_CALLOUTS",
      sprintf("%s::%O (%d)\n",object_name(co[0]),co[1],co[2]),
      200000);
  return 0;
}

private int last_callout_log=0;

nomask varargs void call_out( varargs mixed *args )
{
    mixed tmp, *call_outs;

    // Bei >600 Callouts alle Objekte killen, die mehr als 30 Callouts laufen
    // haben.
    if ( efun::driver_info(DI_NUM_CALLOUTS) > 600
        && geteuid(previous_object()) != ROOTID )
    {
       // Log erzeugen...
       if (last_callout_log <
           efun::driver_info(DI_NUM_HEARTBEAT_TOTAL_CYCLES) - 10
           && get_eval_cost() > 200000)
       {
         last_callout_log = efun::driver_info(DI_NUM_HEARTBEAT_TOTAL_CYCLES);
         log_file("TOO_MANY_CALLOUTS",
             sprintf("\n%s: ############ Too many callouts: %d ##############\n",
                     strftime("%y%m%d-%H%M%S"),
                     efun::driver_info(DI_NUM_CALLOUTS)));
         filter(efun::call_out_info(), #'_log_call_out);
       }
       // Objekte aller Callouts ermitteln
       call_outs = map( efun::call_out_info(), #'_call_out_obj );
       mapping objectmap = ([]);
       filter( call_outs, #'_same_object, &objectmap );
       // Master nicht grillen...
       efun::m_delete( objectmap, master(1) );
       // alle Objekte raussuchen, die zuviele haben...
       mapping res = filter_indices( objectmap, #'_too_many, objectmap, 29 );
       // und ueber alle Keys gehen, an _destroy() werden Key und Array mit
       // Objekten uebergeben (in diesem Fall sind Keys und Array mit
       // Objekten jeweils das gleiche Objekt).
       if ( sizeof(res) )       
           walk_mapping(res, #'_destroy, "CALL_OUT overflow by single "             
              "object [%O]. Destructed %d objects. [%s]\n", 0 );

       // Bei (auch nach dem ersten Aufraeumen noch) >800 Callouts alle
       // Objekte killen, die mehr als 50 Callouts laufen haben - und
       // diesmal zaehlen Clones nicht eigenstaendig! D.h. es werden alle
       // Clones einer BP gekillt, die Callouts laufen haben, falls alle
       // diese Objekte _zusammen_ mehr als 50 Callouts haben!
       if ( efun::driver_info(DI_NUM_CALLOUTS) > 800 ) {
           // zerstoerte Objekte von der letzten Aktion sind in objectmap nicht
           // mehr drin, da sie dort als Keys verwendet wurden.
           mapping pathmap=([]);
           // alle Objekt einer BP in res sortieren, BP-Name als Key, Arrays
           // von Objekten als Werte.
           walk_mapping( objectmap, #'_same_path, &pathmap);
           // alle BPs (und ihre Objekte) raussuchen, die zuviele haben...
           res = filter_indices( pathmap, #'_too_many/*'*/, pathmap, 50 );
           // und ueber alle Keys gehen, an _destroy() werden die Clones
           // uebergeben, die Callouts haben.
           if ( sizeof(res) )
              walk_mapping( res, #'_destroy/*'*/, "CALL_OUT overflow by file "
                           "'%s'. Destructed %d objects. [%s]\n", 0 );

           // Wenn beide Aufraeumarbeiten nichts gebracht haben und immer
           // noch >1000 Callouts laufen, werden diesmal alle Callouts
           // einer UID zusammengezaehlt.
           // Alle Objekte einer UID, die es in Summe aller ihrer Objekt mit
           // Callouts auf mehr als 100 Callouts bringt, werden geroestet.
           if (efun::driver_info(DI_NUM_CALLOUTS) > 1000)
           {
              // das nach BP-Namen vorgefilterte Mapping jetzt nach UIDs
              // zusammensortieren. Zerstoerte Clones filter _same_uid()
              // raus.
              mapping uidmap=([]);
              walk_mapping( pathmap, #'_same_uid, &uidmap,
                           symbol_function( "creator_file",
                                          "/secure/master" ) );
              // In res nun UIDs als Keys und Arrays von Objekten als Werte.
              // Die rausfiltern, die mehr als 100 Objekte (non-unique, d.h.
              // 100 Callouts!) haben.
              res = filter_indices( uidmap, #'_too_many, uidmap, 100 );
              // und erneut ueber die Keys laufen und jeweils die Arrays mit
              // den Objekten zur Zerstoerung an _destroy()...
              if ( sizeof(res) )
                  walk_mapping( res, #'_destroy, "CALL_OUT overflow by "
                              "UID '%s'. Destructed %d objects. [%s]\n",
                              1 );
           }
       }
    }

    // Falls das aufrufende Objekt zerstoert wurde beim Aufraeumen
    if ( !previous_object() )
       return;

    set_this_object( previous_object() );
    apply( #'efun::call_out, args );
    return;
}

mixed call_out_info() {
  
  object po = previous_object();
  mixed coi = efun::call_out_info();

  // ungefilterten Output nur fuer bestimmte Objekte, Objekte in /std oder
  // /obj haben die BackboneID.
  if (query_wiz_level(getuid(po)) >= ARCH_LVL
       || (string)master()->creator_file(load_name(po)) == BACKBONEID ) {
      return coi;
  }
  else {
      return filter(coi, function mixed (mixed arr) {
              if (pointerp(arr) && arr[0]==po)
                 return 1;
              else return 0; });
  }
}

// * Zu einer closure das Objekt, an das sie gebunden ist, suchen
// NICHT das Objekt, was ggf. die lfun definiert!
mixed query_closure_object(closure c) {
  return
    CLOSURE_IS_UNBOUND_LAMBDA(get_type_info(c, 1)) ?
      0 :
  (to_object(c) || -1);
}

// * Wir wollen nur EIN Argument ... ausserdem checks fuer den Netztotenraum
varargs void move_object(mixed what, mixed where)
{
  object po,tmp;

  po=previous_object();
  if (!where)
  {
    where=what;
    what=po;
  }
  if (((stringp(where) && where==NETDEAD_ROOM ) ||
       (objectp(where) && where==find_object(NETDEAD_ROOM))) &&
       objectp(what) && object_name(what)!="/obj/sperrer")
  {
    if (!query_once_interactive(what))
    {
      what->remove();
      if (what) destruct(what);
      return;
    }
    if (living(what) || interactive(what))
    {
      log_file("NDEAD2",sprintf("TRYED TO MOVE TO NETDEAD: %O\n",what));
      return;
    }
    set_object_heart_beat(what,0);
  }
  tmp=what;
  while (tmp=environment(tmp))
      // Ja. Man ruft die _set_xxx()-Funktionen eigentlich nicht direkt auf.
      // Aber das Lichtsystem ist schon *so* rechenintensiv und gerade der
      // P_LAST_CONTENT_CHANGE-Cache wird *so* oft benoetigt, dass es mir
      // da um jedes bisschen Rechenzeit geht.
      // Der Zweck heiligt ja bekanntlich die Mittel. ;-)
      //
      // Tiamak
    tmp->_set_last_content_change();
  funcall(bind_lambda(#'efun::move_object,po),what,where);
  if (tmp=what)
    while (tmp=environment(tmp))
      tmp->_set_last_content_change();
}


void start_simul_efun() {
  mixed *info;

  // Falls noch nicht getan, extra_wizinfo initialisieren
  if ( !pointerp(info = get_extra_wizinfo(0)) )
    set_extra_wizinfo(0, info = allocate(BACKBONE_WIZINFO_SIZE));

  InitLivingData(info);

  set_next_reset(10); // direkt mal aufraeumen
}

protected void reset() {
  set_next_reset(7200);
  CleanLivingData();
}

#if !__EFUN_DEFINED__(absolute_hb_count)
int absolute_hb_count() {
  return efun::driver_info(DI_NUM_HEARTBEAT_TOTAL_CYCLES);
}
#endif

void __set_environment(object ob, mixed target)
{
  string path;
  object obj;

  if (!objectp(ob))
    return;
  if (!IS_ARCH(geteuid(previous_object())) || !ARCH_SECURITY )
    return;
  if (objectp(target))
  {
    efun::set_environment(ob,target);
    return;
  }
  path=(string)MASTER->_get_path(target,this_interactive());
  if (stringp(path) && file_size(path+".c")>=0 &&
      !catch(load_object(path);publish) )
  {
    obj=find_object(path);
    efun::set_environment(ob,obj);
    return;
  }
}

void _dump_wizlist(string file, int sortby) {
  int i;
  mixed *a;

  if (!LORD_SECURITY)
    return;
  if (!master()->valid_write(file,geteuid(previous_object()),"write_file"))
  {
    write("NO WRITE PERMISSION\n");
    return;
  }
  a = wizlist_info();
  a = sort_array(a, lambda( ({'a,'b}),
                        ({#'<,
                          ({#'[,'a,sortby}),
                          ({#'[,'b,sortby})
                         })));
  rm(file);
  for (i=sizeof(a)-1;i>=0;i--)
    write_file(file,sprintf("%-11s: eval=%-8d cmds=%-6d HBs=%-5d array=%-5d mapping=%-7d\n",
      a[i][WL_NAME],a[i][WL_TOTAL_COST],a[i][WL_COMMANDS],a[i][WL_HEART_BEATS],
      a[i][WL_ARRAY_TOTAL],a[i][WL_CALL_OUT]));
}

public varargs object deep_present(mixed what, object ob) {

  if(!objectp(ob))
    ob=previous_object();
  // Wenn ein Objekt gesucht wird: Alle Envs dieses Objekts ermitteln und
  // schauen, ob in diesen ob vorkommt. Dann ist what in ob enthalten.
  if(objectp(what)) {
    object *envs=all_environment(what);
    // wenn ob kein Environment hat, ist es offensichtlich nicht in what
    // enthalten.
    if (!pointerp(envs)) return 0;
    if (member(envs, ob) != -1) return what;
  }
  // sonst wirds teurer, ueber alle Objekte im (deep) Inv laufen und per id()
  // testen. Dabei muss aber die gewuenschte Nr. ("flasche 3") abgeschnitten
  // werden und selber gezaehlt werden, welche das entsprechende Objekt ist.
  else if (stringp(what)) {
      int cnt;
      string newwhat;
      if(sscanf(what,"%s %d",newwhat,cnt)!=2)
       cnt=1;
      else
       what=newwhat;
      foreach(object invob: deep_inventory(ob)) {
       if (invob->id(what) && !--cnt)
           return invob;
      }
  }
  else {
    set_this_object(previous_object());
    raise_error(sprintf("Wrong argument 1 to deep_present(). "
         "Expected \"object\" or \"string\", got %.50O.\n",
         what));
  }
  return 0;
}

mapping dump_ip_mapping()
{
  return 0;
}

nomask void swap(object obj)
{
  write("Your are not allowed to swap objects by hand!\n");
  return;
}

nomask varargs void garbage_collection(string str)
{
  if(previous_object()==0 || !IS_ARCH(geteuid(previous_object())) 
      || !ARCH_SECURITY)
  {
    write("Call GC now and the mud will crash in 5-6 hours. DONT DO IT!\n");
    return;
  }
  else if (stringp(str))
  {
    return efun::garbage_collection(str);
  }
  else 
    return efun::garbage_collection();
}

varargs void notify_fail(mixed nf, int prio) {
  object po,oldo;
  int oldprio;
  
  if (!PL || !objectp(po=previous_object())) return;
  if (!stringp(nf) && !closurep(nf)) {
      set_this_object(po);
      raise_error(sprintf(
         "Only strings and closures allowed for notify_fail! "
         "Argument was: %.50O...\n",nf));
  }

  // falls ein Objekt bereits nen notify_fail() setzte, Prioritaeten abschaetzen
  // und vergleichen.
  if (objectp(oldo=query_notify_fail(1)) && po!=oldo) {
    if (!prio) {       
      //Prioritaet dieses notify_fail() 'abschaetzen'
      if (po==PL) // Spieler-interne (soul-cmds)
        prio=NF_NL_OWN;
      else if (living(po))
        prio=NF_NL_LIVING;
      else if ((int)po->IsRoom())
        prio=NF_NL_ROOM;
      else
        prio=NF_NL_THING;
    }
    //Prioritaet des alten Setzers abschaetzen
    if (oldo==PL)
      oldprio=NF_NL_OWN;
    else if (living(oldo))
      oldprio=NF_NL_LIVING;
    else if ((int)oldo->IsRoom())
      oldprio=NF_NL_ROOM;
    else
      oldprio=NF_NL_THING;
  }
  else // wenn es noch kein Notify_fail gibt:
    oldprio=NF_NL_NONE;

  //vergleichen und ggf. setzen
  if (prio >= oldprio) { 
    set_this_object(po);
    efun::notify_fail(nf);
  }

  return;
}

void _notify_fail(string str)
{
  //query_notify_fail() benutzen, um das Objekt
  //des letzten notify_fail() zu ermitteln
  object o;
  if ((o=query_notify_fail(1)) && o!=previous_object())
    return;
  //noch kein notify_fail() fuer dieses Kommando gesetzt, auf gehts.
  set_this_object(previous_object());
  efun::notify_fail(str);
  return;
}

string time2string( string format, int zeit )
{
  int i,ch,maxunit,dummy;
  string *parts, fmt;

  int secs = zeit;
  int mins = (zeit/60);
  int hours = (zeit/3600);
  int days = (zeit/86400);
  int weeks =  (zeit/604800);
  int months = (zeit/2419200);
  int abbr = 0;

  parts = regexplode( format, "\(%\(-|\)[0-9]*[nwdhmsxNWDHMSX]\)|\(%%\)" );

  for( i=1; i<sizeof(parts); i+=2 )
  {
    ch = parts[i][<1];
    switch( parts[i][<1] )
    {
    case 'x': case 'X':
       abbr = sscanf( parts[i], "%%%d", dummy ) && dummy==0;
       // NO break !
    case 'n': case 'N':
       maxunit |= 31;
       break;
    case 'w': case 'W':
       maxunit |= 15;
       break;
    case 'd': case 'D':
       maxunit |= 7;
       break;
    case 'h': case 'H':
       maxunit |= 3;
       break;
    case 'm': case 'M':
       maxunit |= 1;
       break;
    }
  }
  if( maxunit & 16 ) weeks %= 4;
  if( maxunit & 8 ) days %= 7;
  if( maxunit & 4 ) hours %= 24;
  if( maxunit & 2 ) mins %= 60;
  if( maxunit ) secs %= 60;

  for( i=1; i<sizeof(parts); i+=2 )
  {
    fmt = parts[i][0..<2];
    ch = parts[i][<1];
    if( ch=='x' )
    {
      if (months > 0) ch='n';
      else if( weeks>0 ) ch='w';
      else if( days>0 ) ch='d';
      else if( hours>0 ) ch='h'; 
      else if(mins > 0) ch = 'm';
      else ch = 's';
    }
    else if( ch=='X' )
    {
      if (months > 0) ch='N';
      else if( weeks>0 ) ch='W';
      else if( days>0 ) ch='D';
      else if( hours>0 ) ch='H'; 
      else if(mins > 0) ch = 'M';
      else ch = 'S';
    }
    
    switch( ch )
    {
      case 'n': parts[i] = sprintf( fmt+"d", months ); break;
      case 'w': parts[i] = sprintf( fmt+"d", weeks ); break;
      case 'd': parts[i] = sprintf( fmt+"d", days ); break;
      case 'h': parts[i] = sprintf( fmt+"d", hours ); break;
      case 'm': parts[i] = sprintf( fmt+"d", mins ); break;
      case 's': parts[i] = sprintf( fmt+"d", secs ); break;
      case 'N':
       if(abbr) parts[i] = "M";
       else parts[i] = sprintf( fmt+"s", (months==1) ? "Monat" : "Monate" );
       break;
      case 'W':
       if(abbr) parts[i] = "w"; else
       parts[i] = sprintf( fmt+"s", (weeks==1) ? "Woche" : "Wochen" );
       break;
      case 'D':
       if(abbr) parts[i] = "d"; else
       parts[i] = sprintf( fmt+"s", (days==1) ? "Tag" : "Tage" );
       break;
      case 'H':
       if(abbr) parts[i] = "h"; else
       parts[i] = sprintf( fmt+"s", (hours==1) ? "Stunde" : "Stunden" );
       break;
      case 'M':
       if(abbr) parts[i] = "m"; else
       parts[i] = sprintf( fmt+"s", (mins==1) ? "Minute" : "Minuten" );
       break;
      case 'S':
       if(abbr) parts[i] = "s"; else
       parts[i] = sprintf( fmt+"s", (secs==1) ? "Sekunde" : "Sekunden" );
       break;
      case '%':
       parts[i] = "%";
       break;
      }
    }
    return implode( parts, "" );
}

nomask mixed __create_player_dummy(string name)
{
  string err;
  object ob;
  mixed m;
  //hat nen Scherzkeks die Blueprint bewegt?
  if ((ob=find_object("/secure/login")) && environment(ob))
      catch(destruct(ob);publish);
  err = catch(ob = clone_object("secure/login");publish);
  if (err)
  {
    write("Fehler beim Laden von /secure/login.c\n"+err+"\n");
    return 0;
  }
  if (objectp(m=(mixed)ob->new_logon(name))) netdead[name]=m;
  return m;
}

nomask int secure_level()
{
  int *level;
  //kette der Caller durchlaufen, den niedrigsten Level in der Kette
  //zurueckgeben. Zerstoerte Objekte (Selbstzerstoerer) fuehren zur Rueckgabe
  //von 0.
  //caller_stack(1) fuegt dem Rueckgabearray this_interactive() hinzu bzw. 0,
  //wenn es keinen Interactive gibt. Die 0 fuehrt dann wie bei zerstoerten
  //Objekten zur Rueckgabe von 0, was gewuenscht ist, da es hier einen
  //INteractive geben muss.
  level=map(caller_stack(1),function int (object caller)
      {if (objectp(caller))
       return(query_wiz_level(geteuid(caller)));
       return(0); // kein Objekt da, 0.
      } );
  return(min(level)); //den kleinsten Wert im Array zurueckgeben (ggf. 0)
}

nomask string secure_euid()
{
  string euid;

  if (!this_interactive()) // Es muss einen interactive geben
     return 0;
  euid=geteuid(this_interactive());
  // ueber alle Caller iterieren. Wenn eines davon eine andere euid hat als
  // der Interactive und diese nicht die ROOTID ist, wird 0 zurueckgeben.
  // Ebenso, falls ein Selbstzerstoerer irgendwo in der Kette ist.
  foreach(object caller: caller_stack()) {
      if (!objectp(caller) ||
       (geteuid(caller)!=euid && geteuid(caller)!=ROOTID))
         return 0;
  }
  return euid; // 'sichere' euid zurueckgeben
}

// INPUT_PROMPT und nen Leerprompt hinzufuegen, wenn keins uebergeben wird.
// Das soll dazu dienen, dass alle ggf. ein EOR am Promptende kriegen...
//#if __BOOT_TIME__ < 1360017213
varargs void input_to( mixed fun, int flags, varargs mixed *args )
{
    mixed *arr;
    int i;

    if ( !this_player() || !previous_object() )
       return;

    // TODO: input_to(...,INPUT_PROMPT, "", ...), wenn kein INPUT_PROMPT
    // vorkommt...
    if ( flags&INPUT_PROMPT ) {    
        arr = ({ fun, flags }) + args;
    }
    else {
        // ggf. ein INPUT_PROMPT hinzufuegen und nen Leerstring als Prompt.
        flags |= INPUT_PROMPT;
        arr = ({ fun, flags, "" }) + args;
    }

    // Arrays gegen flatten quoten.
    for ( i = sizeof(arr) - 1; i > 1; i-- )
       if ( pointerp(arr[i]) )
           arr[i] = quote(arr[i]);

    apply( bind_lambda( unbound_lambda( ({}),
                                     ({ #'efun::input_to/*'*/ }) + arr ),
                       previous_object() ) );
}
//#endif

nomask int set_light(int i)
// erhoeht das Lichtlevel eines Objekts um i
// result: das Lichtlevel innerhalb des Objekts
{
    object ob, *inv;
    int lall, light, dark, tmp;

    if (!(ob=previous_object())) return 0; // ohne das gehts nicht.

    // aus kompatibilitaetsgruenden kann man auch den Lichtlevel noch setzen
    if (i!=0) ob->SetProp(P_LIGHT, ob->QueryProp(P_LIGHT)+i);

    // Lichtberechnung findet eigentlich in der Mudlib statt.
    return (int)ob->QueryProp(P_INT_LIGHT);
}


public string iso2ascii( string str )
{
    if ( !sizeof(str) )
       return "";
    return convert_charset(str, "ISO-8859-15", "ASCII//TRANSLIT");
}


public varargs string CountUp( string *s, string sep, string lastsep )
{
    string ret;

    if ( !pointerp(s) )
       return "";
    
    if (!sep) sep = ", ";
    if (!lastsep) lastsep = " und ";

    switch (sizeof(s))  {
       case 0: ret=""; break;
       case 1: ret=s[0]; break;
       default:
              ret = implode(s[0..<2], sep);
              ret += lastsep + s[<1];
    }
    return ret;
}

nomask varargs int query_next_reset(object ob) {

    // Typpruefung: etwas anderes als Objekte oder 0 sollen Fehler sein.
    if (ob && !objectp(ob))
      raise_error(sprintf("Bad arg 1 to query_next_reset(): got %.20O, "
           "expected object.\n",ob));

    // Defaultobjekt PO, wenn 0 uebergeben.
    if ( !objectp(ob) )
      ob = previous_object();

    return efun::object_info(ob, OI_NEXT_RESET_TIME);
}



// ### Ersatzaufloesung in Strings ###
varargs string replace_personal(string str, mixed *obs, int caps) {
  int i;
  string *parts;

  parts = regexplode(str, "@WE[A-SU]*[0-9]");
  i = sizeof(parts);

  if (i>1)
  {
    int j, t;
    closure *name_cls;

    t = j = sizeof(obs);

    name_cls  =  allocate(j);
    while (j--)
      if (objectp(obs[j]))
        name_cls[j] = symbol_function("name", obs[j]);
      else if (stringp(obs[j]))
        name_cls[j] = obs[j];

    while ((i-= 2)>0)
    {
      int ob_nr;
      // zu ersetzendes Token in Fall und Objektindex aufspalten
      ob_nr = parts[i][<1]-'1';
      if (ob_nr<0 || ob_nr>=t) {
        set_this_object(previous_object());
        raise_error(sprintf("replace_personal: using wrong object index %d\n",
                    ob_nr));
        return implode(parts, "");
      }

      // casus kann man schon hier entscheiden
      int casus;
      string part = parts[i];
      switch (part[3]) {
        case 'R': casus = WER;    break;
        case 'S': casus = WESSEN; break;
        case 'M': casus = WEM;    break;
        case 'N': casus = WEN;    break;
        default:  continue; // passt schon jetzt nicht in das Hauptmuster
      }

      // und jetzt die einzelnen Keywords ohne fuehrendes "@WE", beendende Id
      mixed tmp;
      switch (part[3..<2]) {
        case "R": case "SSEN": case "M": case "N":               // Name
          parts[i] = funcall(name_cls[ob_nr], casus, 1);  break;
        case "RU": case "SSENU": case "MU": case "NU":           // unbestimmt
          parts[i] = funcall(name_cls[ob_nr], casus);     break;
        case "RQP": case "SSENQP": case "MQP": case "NQP":       // Pronoun
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPronoun(casus);
          break;
        case "RQA": case "SSENQA": case "MQA": case "NQA":       // Article
          if (objectp(tmp = obs[ob_nr]))
            tmp = (string)tmp->QueryArticle(casus, 1, 1);
          if (stringp(tmp) && !(tmp[<1]^' ')) 
            tmp = tmp[0..<2];                // Extra-Space wieder loeschen
          break;
        case "RQPPMS": case "SSENQPPMS": case "MQPPMS": case "NQPPMS":
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPossPronoun(MALE, casus, SINGULAR);
          break;
        case "RQPPFS": case "SSENQPPFS": case "MQPPFS": case "NQPPFS":
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPossPronoun(FEMALE, casus, SINGULAR);
          break;
        case "RQPPNS": case "SSENQPPNS": case "MQPPNS": case "NQPPNS":
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPossPronoun(NEUTER, casus, SINGULAR);
          break;
        case "RQPPMP": case "SSENQPPMP": case "MQPPMP": case "NQPPMP":
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPossPronoun(MALE, casus, PLURAL);
          break;
        case "RQPPFP": case "SSENQPPFP": case "MQPPFP": case "NQPPFP":
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPossPronoun(FEMALE, casus, PLURAL);
          break;
        case "RQPPNP": case "SSENQPPNP": case "MQPPNP": case "NQPPNP":
          if (objectp(tmp = obs[ob_nr]))
            parts[i] = (string)tmp->QueryPossPronoun(NEUTER, casus, PLURAL);
          break;
        default:
          continue;
      }

      // wenn tmp ein String war, weisen wir es hier pauschal zu
      if (stringp(tmp))
        parts[i] = tmp;

      // auf Wunsch wird nach Satzenden gross geschrieben
      if (caps)
      {
        // Wenn das vorhergehende parts[i] == "" ist, sind wir am Anfang vom
        // String und dies wird wie ein Satzende vorher behandelt.
        if (parts[i-1] == "")
            parts[i] = capitalize(parts[i]);
        else
        {
          switch (parts[i-1][<2..])
          {
            case ". ":  case "! ":  case "? ":
            case ".":   case "!":   case "?":
            case ".\n": case "!\n": case "?\n":
            case "\" ": case "\"\n":
              parts[i] = capitalize(parts[i]);
              break;
          }
        }
      }
    }
    return implode(parts, "");
  }
  return str;
}

//replacements for dropped efuns in LD
#if !__EFUN_DEFINED__(extract)
deprecated varargs string extract(string str, int from, int to) {

  if(!stringp(str)) {
    set_this_object(previous_object());
    raise_error(sprintf("Bad argument 1 to extract(): %O",str));
  }
  if (intp(from) && intp(to)) {
    if (from>=0 && to>=0)
      return(str[from .. to]);
    else if (from>=0 && to<0)
      return(str[from .. <abs(to)]);
    else if (from<0 && to>=0)
      return(str[<abs(from) .. to]);
    else
      return(str[<abs(from) .. <abs(to)]);
  }
  else if (intp(from)) {
    if (from>=0)
      return(str[from .. ]);
    else
      return(str[<abs(from) .. ]);
  }
  else {
    return(str);
  }
}
#endif // !__EFUN_DEFINED__(extract)

#if !__EFUN_DEFINED__(slice_array)
deprecated varargs mixed slice_array(mixed array, int from, int to) {

  if(!pointerp(array)) {
    set_this_object(previous_object());
    raise_error(sprintf("Bad argument 1 to slice_array(): %O",array));
  }
  if (intp(from) && intp(to)) {
    if (from>=0 && to>=0)
      return(array[from .. to]);
    else if (from>=0 && to<0)
      return(array[from .. <abs(to)]);
    else if (from<0 && to>=0)
      return(array[<abs(from) .. to]);
    else
      return(array[<abs(from) .. <abs(to)]);
  }
  else if (intp(from)) {
    if (from>=0)
      return(array[from .. ]);
    else
      return(array[<abs(from) .. ]);
  }
  else {
    return(array);
  }
}
#endif // !__EFUN_DEFINED__(slice_array)

#if !__EFUN_DEFINED__(member_array)
deprecated int member_array(mixed item, mixed arraystring) {

  if (pointerp(arraystring)) {
    return(efun::member(arraystring,item));
  }
  else if (stringp(arraystring)) {
    return(efun::member(arraystring,to_int(item)));
  }
  else {
    set_this_object(previous_object());
    raise_error(sprintf("Bad argument 1 to member_array(): %O",arraystring));
  }
}
#endif // !__EFUN_DEFINED__(member_array)

// The digit at the i'th position is the number of bits set in 'i'.
string count_table =
    "0112122312232334122323342334344512232334233434452334344534454556";
int broken_count_bits( string s ) {
    int i, res;
    if( !stringp(s) || !(i=sizeof(s)) ) return 0;
    for( ; i-->0; ) {
        // We are counting 6 bits at a time using a precompiled table.
        res += count_table[(s[i]-' ')&63]-'0';
    }
    return res;
}

#if !__EFUN_DEFINED__(count_bits)
int count_bits( string s ) {
    return(broken_count_bits(s));
}
#endif


// * Teile aus einem Array entfernen *** OBSOLETE
deprecated mixed *exclude_array(mixed *arr,int from,int to)
{
  if (to<from)
    to = from;
  return arr[0..from-1]+arr[to+1..];
}


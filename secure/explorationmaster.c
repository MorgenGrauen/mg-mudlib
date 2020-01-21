// MorgenGrauen MUDlib
//
// explorationmaster.c -- Verwaltung der ExplorationPoints (FP)
//
// $Id: explorationmaster.c 9569 2016-06-05 21:28:23Z Zesstra $
//
#pragma strict_types,rtt_checks
#pragma no_clone,no_shadow,no_inherit
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#define BY_EP "/secure/ARCH/EPSTAT_BY_EP"
#define BY_PL "/secure/ARCH/EPSTAT_BY_PL"
#define BY_PN "/secure/ARCH/EPSTAT_BY_PN"
#define AVGLOG "/secure/ARCH/EPSTAT_AVG"

inherit "/std/util/pl_iterator";

#include <config.h>
#include <wizlevels.h>
#include <userinfo.h>
#include <exploration.h>
#include <properties.h>
#include <new_skills.h>

#define DOMAIN_INFO 1

#include <living/comm.h>
#define ZDEBUG(x) if (find_player("zesstra")) \
  find_player("zesstra")->ReceiveMsg(x,MT_DEBUG,0,object_name()+":",this_object())
//#define ZDEBUG(x)

// Struktur: ([ string filename : string* action, int number, int type ]) 
private mapping obs;
private int epcount;  // Anzahl FP
private int epavg;    // Durchschnittliche FP/Spieler
// Bitstrings fuer normale (alloc) und Bonus-FP (bonus)
private string alloc,bonus;

private nosave string vc_ob;
private nosave string output;

private nosave int changed;
private nosave int dumping;
private nosave mapping dumpMap, lastfound, querytime;

private nosave int stat_done;

protected void create()
{
  seteuid(getuid(this_object()));
  if (!restore_object(EPSAVEFILE)) {
    obs = ([]);
    epcount = epavg = 0;
    alloc = " ";
    bonus = " ";
    save_object(EPSAVEFILE);
  }
  if (!bonus) bonus=" ";
  lastfound=([]);
  querytime=([]);
}

public varargs int remove(int silent)
{
  save_object(EPSAVEFILE);
  destruct(this_object());
  return 1;
}

// Statistik erstellen
private int SetAverage(int newavg)
{
  if (epavg != newavg)
  {
    epavg = newavg;
    write_file(AVGLOG, sprintf("%s : %d\n",dtime(time()),newavg));
    save_object(EPSAVEFILE);
  }
  return epavg;
}

private void check_player(string pl, mixed extra)
{
  int *fp_found = extra[0];
  mapping plstat = extra[1];
  mapping pnstat = extra[2];
  object pldata = extra[3];
  if (!pldata)
  {
    pldata=clone_object("/secure/playerdata");
    extra[3]=pldata;
  }
  //ZDEBUG(sprintf("check %s...",pl));
  // Letzte Loginzeit ermitteln, wenn laenger als 90 Tage her und nicht
  // eingeloggt, wird der Spieler uebersprungen.
  pldata->ReleasePlayer();
  pldata->LoadPlayer(pl);
  // Testspieler ausnehmen, Spieler, die 90 Tage nicht da waren.
  if ( (({int})pldata->QueryProp(P_LAST_LOGIN) < time() - 7776000
        && !find_player(pl))
      || ({<int|string>})pldata->QueryProp(P_TESTPLAYER))
    return;
  // Wenn kein SPieler/Seher, sondern Magier: auch ueberspringen
  if (IS_LEARNER(pl))
    return;

  string eps=({string})master()->query_ep(pl) || "";
  int p=-1;
  int count, avgcount;
  while ((p=next_bit(eps,p)) != -1)
  {
    if (p<sizeof(fp_found))
    {
      ++count;
      ++fp_found[p];
      // es tragen nur normale EPs zum Durchschnitt bei
      if (!test_bit(bonus,p))
        ++avgcount;
    }
  }

  // Spieler mit weniger als MIN_EP ignorieren.
  if (avgcount >= MIN_EP)
  {
    extra[4] += avgcount;   // Summe der gefundenen FP
    ++extra[5];          // Anzahl beruecksichtigter Spieler
  }
  plstat += ([ pl : count ]);
  if (!member(pnstat, count))
    pnstat += ([ count : ({ pl }) ]);
  else
    pnstat[count] = ({ pl })+pnstat[count];
  //ZDEBUG(sprintf("done\n"));
}

// Mit allen Spielern fertig, aufraeumen.
#define BAR "************************************************************"
private void plcheck_finished(mixed extra)
{
  ZDEBUG("plcheck_finished entry\n");
  if (get_eval_cost() < 1000000)
  {
    call_out(#'plcheck_finished, 4+random(6), extra);
    return;
  }

  int *fp_found = extra[0];
  mapping plstat = extra[1];
  mapping pnstat = extra[2];
  object pldata = extra[3];
  int avgcount = extra[4];
  int avgspieler = extra[5];

  if (objectp(pldata))
    pldata->remove(1);

  if (file_size(BY_EP) >= 0)
    rm(BY_EP);
  if (file_size(BY_PL) >= 0)
    rm(BY_PL);
  if (file_size(BY_PN) >= 0)
    rm(BY_PN);

  // Neuen Durchschnittswert fuer gefundene FP setzen.
  if (avgspieler)
  {
    SetAverage(to_int(avgcount/avgspieler));
  }

  // Histogramm ueber alle FP schreiben: wie oft wurde jeder gefunden?
  int maxval = max(fp_found);
  int fp_index;
  foreach(int found : fp_found)
  {
    write_file(BY_EP, sprintf("%5d:%5d %s\n", fp_index, found,
               BAR[0..(60*found)/maxval]));
    ++fp_index;
  }
  // sortierte Liste der Spieler (sortiert nach gefundenen FP) erzeugen
  foreach(int fp : sort_array(m_indices(pnstat),#'<) )
  {
    foreach(string pl : pnstat[fp])
      write_file(BY_PN, sprintf("%-14s: %5d\n", pl, fp));
  }
  // alphabetisch sortierte Liste der Spieler erzeugen
  foreach(string pl : sort_array(m_indices(plstat),#'>) )
  {
    write_file(BY_PL, sprintf("%-14s: %5d\n", pl, plstat[pl]));
  }
}
#undef BAR

private void make_stat()
{
  stat_done = time();
  // Leider laesst sich epcount nicht nutzen Beim Loeschen von FP wird
  // <epcount> dekrementiert, so dass hier nicht mehr alle FPs ausgewertet
  // werden. Daher muss stattdessen bis zur Gesamtgroesse von <obs> gezaehlt
  // werden.
  int* fp_found = allocate(sizeof(obs));
  mapping plstat = m_allocate(500);
  mapping pnstat = m_allocate(500);
  start_player_check(#'check_player, #'plcheck_finished, 1200000,
                     ({fp_found,plstat,pnstat,
                       0, 0, 0}) );
}

void reset()
{
  if (changed && !dumping)
  {
    catch(rm(DUMPFILE);publish);
    dumping = 1;
    call_out("dumpEPObjects", 0, sort_array(m_indices(obs),#'> /*'*/));
    changed = 0;
  }
  // nur einmal am tag statistiken erstellen
  if (time()%86400 < 4000
      && stat_done < time()-80000)
    make_stat();
}

private string strArr(string *s)
{
  string ret;
  int i;

  ret = ("\""+s[<1]+"\"");
  for (i=sizeof(s)-2; i>=0; i--)
    ret += (", \""+s[i]+"\"");

  return ret;
}

static void dumpEPObjects(string *doit)
{
  string *toGo, id;
  int i,j;

  if (!mappingp(dumpMap))
    dumpMap = ([]);

  toGo = 0;
  if (sizeof(doit) > 200) {
    toGo = doit[200..];
    doit = doit[0..199];
  }

  j = sizeof(doit);

  for (i=0; i<j; i++) {
    id = ({string})master()->creator_file(doit[i]);
    if (member(dumpMap, id))
      dumpMap[id] += ({ doit[i] });
    else
      dumpMap += ([ id : ({ doit[i] }) ]);
  }
  if (toGo)
    call_out("dumpEPObjects", 1, toGo);
  else {
    int step;

    step = 0;
    id = "";
    toGo = sort_array(m_indices(dumpMap),#'> /*'*/ );
    for (i=0, j=sizeof(toGo); i<j; i++) {
      int k,l;
      doit = dumpMap[toGo[i]];
      id += sprintf("### %s %s\n", toGo[i], "#########################"[sizeof(toGo[i])..]);
      for (k=0, l=sizeof(doit); k<l; k++) {
        id += sprintf("%s %4d %s %s.c ({ %s })\n",
                      EP_TYPES[obs[doit[k], MPOS_TYPE]],
                      obs[doit[k], MPOS_NUM],
                      test_bit(bonus,obs[doit[k], MPOS_NUM])?"b":"n",
                      doit[k],
                      strArr(obs[doit[k]]));
        if (!(++step % 50)) {
          write_file(DUMPFILE, id);
          id = "";
        }
      }
      id += "\n";
    }
    write_file(DUMPFILE,id);
    if (dumping == 2)
      write("Fertig! Anfrage bitte wiederholen.\n");
    dumping = 0;
    changed = 0;
    dumpMap = 0;
  }
}

private string validOb(mixed ob)
{
  string fn, fpart;

  if (!objectp(ob))
    return 0;

  fn = old_explode(object_name(ob),"#")[0];
  fpart = old_explode(fn,"/")[<1];
 /*
  if (query_once_interactive(ob))
    return 0;

  if ((file_size(fn+".c") <= 0) &&
      this_player() &&
      (strstr(fpart, getuid(this_player())) >= 0))
    return 0;
 */
  return fn;
}

private int allowed()
{
  if (previous_object() && geteuid(previous_object())==ROOTID)
    return 1;
  if ( !process_call() && previous_object() && this_interactive()
        && ARCH_SECURITY )
    return 1;

  return 0;
}

nomask varargs int AddEPObject(object ob, mixed keys, int type, int bonusflag)
{
  string fn;

  if (!allowed())
    return EPERR_NOT_ARCH;

  if (!(fn = validOb(ob)))
    return EPERR_INVALID_OB;

  if (stringp(keys))
    keys = ({ keys });

  if (member(obs, fn)) {
    if (type == obs[fn, MPOS_TYPE])
      obs[fn] = keys;
    else
      obs += ([ fn : keys; obs[fn, MPOS_NUM]; type ]);
    if (bonusflag) bonus = set_bit(bonus, obs[fn, MPOS_NUM]);
       else bonus = clear_bit(bonus, obs[fn, MPOS_NUM]);
  }
  else {
    int nr, i;

    nr = 0;
    while (test_bit(alloc,nr))
      nr++;

    obs += ([ fn : keys; nr; type ]);
    ++epcount;
    alloc = set_bit(alloc,nr);
    if (bonusflag) bonus = set_bit(bonus,nr);
       else bonus = clear_bit(bonus,nr);
  }

  changed = 1;
  save_object(EPSAVEFILE);
  return epcount;
}

nomask int RemoveEPObject(string|object ob)
{
  string fn;

  if (!allowed())
    return EPERR_NOT_ARCH;
  
  if (objectp(ob))
    ob=object_name(ob);

  fn = explode(ob,"#")[0];
  if (!member(obs,fn))
    return EPERR_NO_ENTRY;

  alloc = clear_bit(alloc, obs[fn, MPOS_NUM]);
  bonus = clear_bit(bonus, obs[fn, MPOS_NUM]);

  m_delete(obs, fn);

  changed = 1;
  --epcount;
  save_object(EPSAVEFILE);
  return epcount;
}

nomask int ChangeEPObject(object ob, int what, mixed new)
{
  string fn, fn2;
  mapping entry;

  if (!allowed())
    return EPERR_NOT_ARCH;

  if (!(fn = validOb(ob)))
    return EPERR_INVALID_OB;

  if (!member(obs,fn))
    return EPERR_NO_ENTRY;

  switch(what) {
  case CHANGE_OB:
    if (!(fn2 = validOb(new)))
      return EPERR_INVALID_ARG;
    entry = ([ fn2: obs[fn]; obs[fn,MPOS_NUM]; obs[fn,MPOS_TYPE] ]);
    obs = m_copy_delete(obs, fn);
    obs += entry;
    break;
  case CHANGE_BONUS:
    if (!(intp(new)))
      return EPERR_INVALID_ARG;
    if (new) bonus=set_bit(bonus,obs[fn,MPOS_NUM]);
       else bonus=clear_bit(bonus,obs[fn,MPOS_NUM]);
    break;
  case CHANGE_KEY:
    if (!stringp(new) && !pointerp(new))
      return EPERR_INVALID_ARG;
       
    if (stringp(new))
      new = ({ new });

    obs[fn] = new;
    break;
  case CHANGE_TYPE:
    if (!intp(new) || new < 0 || new > EP_MAX)
      return EPERR_INVALID_ARG;

    obs[fn, MPOS_TYPE] = new;
    break;
  default:
    return EPERR_INVALID_ARG;
  }
  changed = 1;
  save_object(EPSAVEFILE);
  return 1;
}

nomask mixed QueryEPObject(object ob)
{
  string fn;

  if (!allowed())
    return EPERR_NOT_ARCH;

  if (!(fn = validOb(ob)))
    return EPERR_INVALID_OB;

  if (!member(obs, fn))
    return 0;

  return ({ obs[fn], obs[fn,MPOS_NUM], obs[fn, MPOS_TYPE], test_bit(bonus,obs[fn, MPOS_NUM]) });
}

private string getMatch(string m)
{
  string *res;
  int i;

  res = regexp(sort_array(m_indices(obs), #'> /*'*/), m);
  for (i=sizeof(res)-1; i>=0; i--)
    res[i] = sprintf("%s %s %s.c ({ %s })",
                     EP_TYPES[obs[res[i], MPOS_TYPE]],
                     test_bit(bonus,obs[res[i], MPOS_NUM])?"b":"n",
                     res[i],
                     strArr(obs[res[i]]));
  return implode(res, "\n");
}

private string getMatchArch(string m)
{
  string *res;
  int i;

  res = regexp(sort_array(m_indices(obs), #'> /*'*/), m);
  for (i=sizeof(res)-1; i>=0; i--)
    res[i] = sprintf("%s %4d %s %s.c ({ %s })",
                     EP_TYPES[obs[res[i], MPOS_TYPE]],
                     obs[res[i], MPOS_NUM],
                     test_bit(bonus,obs[res[i], MPOS_NUM])?"b":"n",
                     res[i],
                     strArr(obs[res[i]]));
  return implode(res, "\n");
}

/*
 *  Anleitung fuer >=EMs:
 *    ShowEPObjects() zeigt das gesamte Dumpfile an
 *    ShowEPObjects("dump") erzeugt ein neues Dumpfile
 *    ShowEPObjects("string") liefert alle EPs, die "/string/"
 *                            im Filenamen enthalten
 *    ShowEPObjects("string1","string2") liefert alle EPs, die
 *                            "/string1/string2/" im Filenamen enthalten
 *  Anleitung fuer RMs:
 *    ShowEPObjects() liefert all Deine EPs
 *    ShowEPObjects("domainname") liefert Deine EPs in Deiner Domanin
 *    ShowEPObjects("domainname","magiername") liefert die EPs des
 *                                Mitarbeiters in Deiner Region
 *
 *  Anleitung fuer <RMs:
 *    ShowEPObjects() liefert Deine EPs
 *
 *  Alle EPObjects einer ganzen Domain kann man nicht mehr auf einmal
 *  ziehen und sollte man auch nicht. Alle Zugriffe auf diese Funktion
 *  werden geloggt. Auch die der EMs!
 *
 *                                             Rikus
 */

nomask varargs void ShowEPObjects(string what, string magname)
{
  if (allowed()) {
    if (what == "dump") {
      if (!dumping) {
        dumping = 2;
        catch(rm(DUMPFILE);publish);
        call_out("dumpEPObjects", 0, sort_array(m_indices(obs),#'>/*'*/));
      }
      printf("Liste wird erstellt und in '%s' abgelegt!\n", DUMPFILE);
      return;
    }
    if (!what || what == "") {
      this_interactive()->More(DUMPFILE, 1);
      log_file("ARCH/EPZugriffe", ctime(time())+": "+
        capitalize(getuid(this_interactive()))+" schaute sich das DUMPFILE an.\n");
      return;
    }
    what="/"+what+"/";
    if (magname) what+=magname+"/";
  }
  else
#ifdef DOMAIN_INFO
    if (IS_LORD(this_interactive())) {
      if (!what || what == "")
       what = "/"+getuid(this_interactive())+"/";
      else {
       if (!master()->domain_master(getuid(this_interactive()), what)) {
         write("Sorry, Du kannst nur Objekte in Deiner eigenen Region abfragen!\n");
         return;
       }
       if (!magname || magname=="")
         magname = getuid(this_interactive());
//        if (!master()->domain_member(magname, what)) {
//         write(capitalize(magname)+" ist gar kein Mitarbeiter in Deiner Region!\n");
//          return;
//       }
       what = "/d/"+what+"/"+magname+"/";
      }
    }
    else
#endif
      {
       if (!what || what == "")
         what = getuid(this_interactive());
       else if (what != getuid(this_interactive())) {
         write("Sorry, Du kannst nur Deine eigenen Objekte abfragen!\n");
         return;
       }
       what="/"+what+"/";
      }
  if (allowed())
    this_interactive()->More(getMatchArch(what));
  else
    this_interactive()->More(getMatch(what));
  log_file("ARCH/EPZugriffe", ctime(time())+": "+
    capitalize(getuid(this_interactive()))+" schaute sich "+what+" an.\n");
  return;
}

nomask void PrepareVCQuery(string file)
{
  string path, *parts;

  vc_ob = 0;

  if (!previous_object() || !stringp(file))
    return;

  parts = explode(object_name(previous_object()),"/");

  if (parts[<1] == "virtual_compiler") {
    path = implode(parts[0..<2]+({ file }), "/");
    if (file_size(path+".c") < 0)
      vc_ob = path;
  }
}

nomask mixed *QueryExplore()
{
  string fn;

  if (!previous_object())
    return 0;

  if (!member(obs, fn = old_explode(object_name(previous_object()),"#")[0]))
    if (!vc_ob || !member(obs, fn = vc_ob)) {
      vc_ob = 0;
      return 0;
    }

  vc_ob = 0;
  return ({ obs[fn, MPOS_TYPE], obs[fn] });
}

nomask int QueryMaxEP()
{
  return (epcount||1);
}

nomask int QueryAverage()
{
  return (epavg||1);
}

static int check_arch(object u)
{
   return query_wiz_level(u)>=ARCH_LVL;
}

private int check_to_fast(string name, string fn, int gesetzt)
{
    if (gesetzt) return 1; // Rikus, sonst arger scroll :)

    log_file(FP_LOG,sprintf("%s : %s : %s\n",name,fn,dtime(time())),500000);

    if ( !member( lastfound, name ) )
        lastfound += ([ name: time(); 1; ({ fn + "#*#" + dtime(time()) + "#*#" +gesetzt }) ]);
    else if ( time() <= lastfound[name, 0] + LF_TIME ){
        lastfound[name, 1]++;
        lastfound[name, 2] = ({ fn + "#*#" + dtime(time()) + "#*#" +gesetzt })
            + lastfound[name,2];
    }
    else {
        lastfound[name, 1] = 1;
        lastfound[name, 2] = ({ fn + "#*#" + dtime(time()) + "#*#" +gesetzt });
    }

    lastfound[name, 0] = time();

    if ( lastfound[name, 1] >= LF_WARN ){
        object *u;
        int i;
        string *tmp;

//        u = filter( users(), "check_arch" );
//        map( u, #'tell_object/*'*/, "**** FP-Liste/Script " +
//                   capitalize(name) + " (" + dtime(time()) + ") ****\n" );

        for ( i = sizeof(lastfound[name, 2]); i--; ){
            tmp = explode( lastfound[name, 2][i], "#*#" );
            log_file( LF_LOG, sprintf( "%s : %s : %s : %s\n",
                                         tmp[1], name, tmp[2], tmp[0] ), 500000 );
        }

        lastfound[name, 2] = ({});
    }
    return 1;
}

nomask int GiveExplorationPoint(string key)
{
  string fn;
  string ep;
  int gesetzt;

  if (!previous_object() || !this_interactive() || !this_player() ||
       this_player() != this_interactive() ||
       this_player()->QueryProp(P_KILLS) ||
       this_player()->QueryGuest()       )
    return 0;

  fn = old_explode(object_name(previous_object()), "#")[0];

  if (!member(obs, fn))
    return 0;

  if (member(obs[fn],key) < 0)
    return 0;

  ep = (MASTER->query_ep(getuid(this_interactive())) || "");

  gesetzt=test_bit(ep,obs[fn,1]);
  check_to_fast(getuid(this_player()),fn,gesetzt);
  if (gesetzt) return 0;
  
  catch(ep = set_bit(ep, obs[fn,1]));

  return ({int})MASTER->update_ep(getuid(this_interactive()),ep);
}

nomask int GiveExplorationPointObject(string key, object ob)
{
  string fn;
  string ep;
  int gesetzt;
  
  if (!objectp(ob) || ob->QueryProp(P_KILLS ))
    return 0;

  fn = old_explode(object_name(previous_object()), "#")[0];

  if (!member(obs, fn))
    return 0;

  if (member(obs[fn],key) < 0)
    return 0;

  ep = (MASTER->query_ep(getuid(ob)) || "");

  gesetzt=test_bit(ep,obs[fn,1]);
  check_to_fast(getuid(this_player()),fn,gesetzt);
  if (gesetzt) return 0;

  catch(ep = set_bit(ep, obs[fn,1]));

  return ({int})MASTER->update_ep(getuid(ob),ep);
}


private int QueryRealExplorationPoints(string pl)
{
  return count_bits(MASTER->query_ep(pl) || " ");
}

nomask int QueryExplorationPoints(mixed pl)
{
  mixed val;
  int ep;

  if (!stringp(pl)) pl=getuid(pl);
  ep=QueryRealExplorationPoints(pl);

  if (allowed() || !ep) return ep;

  val=querytime[pl];

  if (!pointerp(val) || sizeof(val)<2)
    val=({0,time()});

  if (time()>=val[1]) {
    val = ({ ep + random(6)-3, time()+300+random(300) });
    if (val[0]<0) val[0]=0;
    querytime+=([pl:val]);
  }
  return val[0];
}

private int remove_fp(int num, string pl)
{
  int i,j,k,t,maxEP;
  string ep;
  ep = (MASTER->query_ep(pl) || "");

  maxEP = QueryMaxEP();
  for( i=0; i<num; i++)
  {
    t = random(maxEP);
    for( j=0; j<maxEP; j++ ) {
      if( test_bit(ep, k=(t+j)%maxEP ) ) break;
    }
    if( j==maxEP ) break;
    ep = clear_bit(ep, k);
  }
  MASTER->update_ep(pl,ep);
  return i;
}

/* */

// quoted from /sys/mail.h
#define MSG_FROM 0
#define MSG_SENDER 1
#define MSG_RECIPIENT 2
#define MSG_CC 3
#define MSG_BCC 4
#define MSG_SUBJECT 5
#define MSG_DATE 6
#define MSG_ID 7
#define MSG_BODY 8

nomask int RemoveFP(int num, string pl, string grund)
{
  int i;
  string text;
  mixed* mail;

  if (!allowed()) return -1;
  if( num<0 ) return -3;
  if (!grund || grund=="") grund="<unbekannt>";
  if (!pl) return -2;
  i=remove_fp(num, pl);
  log_file("ARCH/fp_strafen", ctime(time())+": "
    +this_interactive()->Name(WER)+" loescht "+pl+" "+i
    +" FPs\nGrund:"+grund+"\n");
  if( i>0 ) {
     text =
     "Hallo "+capitalize(pl)+",\n\n"+
     break_string(
      this_interactive()->Name(WER)+" hat soeben veranlasst, dass Dir "+i
      +" FPs abgezogen wurden.\nGrund:"+grund+"\n", 78 );

     mail = allocate(9);
     mail[MSG_FROM] = getuid(this_interactive());
     mail[MSG_SENDER] = MUDNAME;
     mail[MSG_RECIPIENT] = pl;
     mail[MSG_CC]=0;
     mail[MSG_BCC]=0;
     mail[MSG_SUBJECT]="FP-Reduktion";
     mail[MSG_DATE]=dtime(time());
     mail[MSG_ID]=MUDNAME":"+time();
     mail[MSG_BODY]=text;

     "/secure/mailer"->DeliverMail(mail,1);
  }
  return i;
}
/* */

private int add_fp(int num, string pl)
{
  int i,j,k,t,maxEP;
  string ep;
  ep = (MASTER->query_ep(pl) || "");

  maxEP = QueryMaxEP();
  for( i=0; i<num; i++)
  {
    t = random(maxEP);
    for( j=0; j<maxEP; j++ ) {
      if( !test_bit(ep, k=(t+j)%maxEP ) ) break;
    }
    if( j==maxEP ) break;
    ep = set_bit(ep, k);
  }
  MASTER->update_ep(pl,ep);
  return i;
}

nomask int AddFP(int num, string pl)
{
  int i;
  if (!allowed()) return -1;
  if ( num<0 ) return -3;
  if (!pl) return -2;
  i=add_fp(num, pl);
  log_file("ARCH/fp_strafen", ctime(time())+": "
    +this_interactive()->Name(WER)+" gibt "+pl+" "+i
    +" FPs\n");

  return i;
}

nomask int SetFP(int num, string pl)
{
  int maxEP;
  string ep;
  if (!allowed()) return -1;
  if ( num<0 ) return -3;
  if (!pl) return -2;
  ep = (MASTER->query_ep(pl) || "");

  maxEP = QueryMaxEP();
  if (num<0 || num>=maxEP) return -4;
  ep = set_bit(ep, num);
  MASTER->update_ep(pl,ep);
  return num;
}

nomask int ClearFP(int num, string pl)
{
  int maxEP;
  string ep;
  if (!allowed()) return -1;
  if ( num<0 ) return -3;
  if (!pl) return -2;
  ep = (MASTER->query_ep(pl) || "");

  maxEP = QueryMaxEP();
  if (num<0 || num>=maxEP) return -4;
  ep = clear_bit(ep, num);
  MASTER->update_ep(pl,ep);
  return num;
}

private void printep( int nr, string key, int kind, string* det )
{
  output+=sprintf("%4d %s %s.c %s ({ %s })\n",nr,test_bit(bonus,nr)?"b":"n",key,EP_TYPES[kind],
                strArr(det));
}

nomask varargs int ShowPlayerEPs(string pl,string pattern)
{
  string ep,teststring;
  if (!allowed()) return -1;
  ep = (MASTER->query_ep(pl) || "");

  output="";
  if (!pattern || pattern=="")
    teststring="%s";
  else teststring="%s"+pattern+"%s";
  walk_mapping( obs, function void (string fn, string* v1, int v2, int v3)
      {
        string unused;
        if ( test_bit(ep, v2) && sscanf(fn, teststring, unused, unused) )
          printep(v2, fn, v3, v1);
      });
  this_interactive()->More(output);
  return 1;
}

// Hier kommen Funktionen fuer die Levelpunkte
// Funktion fuer Levelpunkte steht im LEPMASTER!

nomask int QueryLEP(int lep) {
    raise_error("Bitte QueryLEP() im LEPMASTER abfragen, nicht im "
       "EPMASTER!");
    return(-1); //never reached
}

string QueryForschung()
{
  int my;
  string ret;

  if ((my=QueryRealExplorationPoints(getuid(previous_object()))) < MIN_EP)
    return "Du kennst Dich im "MUDNAME" so gut wie gar nicht aus.\n";

  my *= 100;
  int absolute = my/QueryMaxEP();
  int relative = my/QueryAverage();

  ret = "Verglichen mit Deinen Mitspielern kennst Du Dich im "MUDNAME" ";
  switch(relative) {
  case 0..10:
    ret += "kaum";
    break;
  case 11..40:
    ret += "aeusserst schlecht";
    break;
  case 41..56:
    ret += "sehr schlecht";
    break;
  case 57..72:
    ret += "schlecht";
    break;
  case 73..93:
    ret += "unterdurchschnittlich";
    break;
  case 94..109:
    ret += "durchschnittlich gut";
    break;
  case 110..125:
    ret += "besser als der Durchschnitt";
    break;
  case 126..145:
    ret += "recht gut";
    break;
  case 146..170:
    ret += "ziemlich gut";
    break;
  case 171..210:
    ret += "gut";
    break;
  case 211..300:
    ret += "sehr gut";
    break;
  case 301..400:
    ret += "ausserordentlich gut";
    break;
  case 401..500:
    ret += "unheimlich gut";
    break;
  default:
    ret += "einfach hervorragend";
    break;
  }
  ret += " aus.\nAbsolut gesehen ";

  switch(absolute) {
  case 0..5:
    ret += "kennst Du nur wenig vom "MUDNAME".";
    break;
  case 6..10:
    ret += "solltest Du Dich vielleicht noch genauer umsehen.";
    break;
  case 11..17:
    ret += "bist Du durchaus schon herumgekommen.";
    break;
  case 18..25:
    ret += "hast Du schon einiges gesehen.";
    break;
  case 26..35:
    ret += "bist Du schon weit herumgekommen.";
    break;
  case 36..50:
    ret += "koenntest Du eigentlich einen Reisefuehrer herausbringen.";
    break;
  case 51..75:
    ret += "hast Du schon sehr viel gesehen.";
    break;
  default:
    ret += "besitzt Du eine hervorragende Ortskenntnis.";
  }
  return break_string(ret, 78, 0, 1);
}

// Nicht jeder Magier muss den EPMASTER entsorgen koennen.
string NotifyDestruct(object caller) {
  if( (caller!=this_object() && !ARCH_SECURITY) || process_call() )
  {
    return "Du darfst den Exploration Master nicht zerstoeren!\n";
  }
  return 0;
}


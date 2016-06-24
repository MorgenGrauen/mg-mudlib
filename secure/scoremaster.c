// MorgenGrauen MUDlib
//
// scoremaster.c - Verwaltung der eindeutigen Nummernvergabe fuer NPCs und
//       MiniQuests sowie der Stufenpunkte, die sie geben  
//
// $Id: scoremaster.c 9170 2015-03-05 20:18:54Z Zesstra $
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#include "/secure/scoremaster.h"
#include "/secure/wizlevels.h"
#include <properties.h>
#include <files.h>

#define ZDEBUG(x) if (find_player("zesstra")) \
  tell_object(find_player("zesstra"),sprintf("SCM: %s\n",x))

// hoechste vergebene Nr.
private int lastNum;

// Liste alle EKs: ([obname: num; score; killcount])
private mapping npcs = m_allocate(0,3);

// Liste von Spielernamen-Wert-Paaren, die im Reset abgearbeitet wird:
// ([plname: ({wert1, wert2, wert3, ...}) ]) 
// wert > 0 bedeutet setzen des entsprechenden EKs, < 0 bedeutet loeschen.
private mapping to_change = ([]);

// Liste der EK-Tips: ([obname: Spruch])
private mapping tipList = ([]);

// Bit-Nr., die (wieder) vergeben werden duerfen.
private int *free_num = ({});

// zu entfernende EKs, Liste Bitnummern, also ints
private int *to_be_removed = ({});

// Liste von temporaeren EKs, die noch nicht bestaetigt wurden:
// ([obname: ({plname1, plname2}) ])
private mapping unconfirmed_scores = ([]);

// alle Spieler kriegen diesen 
// Nach Nr. sortierte NPC-Liste: ([num: key; score])
private nosave mapping by_num = m_allocate(0,2);

// Cache fuer EKs von Spielern: ([plname: scoresumme])
private nosave mapping users_ek = ([]);

// bitstring, der alle aktiven EKs als gesetztes Bit enthaelt.
private nosave string active_eks="";

// Prototypen
public mapping getFreeEKsForPlayer(object player);
public int addTip(mixed key,string tip);
public int changeTip(mixed key,string tip);
public int removeTip(mixed key);
private string getTipFromList(mixed key);
public string getTip(mixed key);

public void CheckNPCs(int num);
public void check_all_player(mapping allplayer);
public varargs int DumpNPCs(int sortkey);

private void make_num(string key, int num, int score) {
  by_num += ([ num : key; score ]);
  // fuer aktive EKs, die also einen Scorewert > 0 haben, wird das jeweilige
  // Bit gesetzt. Wird spaeter zum Ausfiltern inaktiver EKs aus den Bitstrings
  // in den Spieler gebraucht.
  if (score>0 && !member(unconfirmed_scores,num))
    active_eks = set_bit(active_eks, num);
}

private int allowed()
{
  if (previous_object() && geteuid(previous_object())==ROOTID)
    return 1;
  if (!process_call() && previous_object() && this_interactive() && ARCH_SECURITY)
    return 1;
  return 0;
}

protected void create()
{
  seteuid(getuid());
  if (!restore_object(SCORESAVEFILE))
  {
    lastNum=0;
    npcs=m_allocate(0,3);
    to_change=m_allocate(0,1);
    tipList=([]);
  }
  npcs-=([0]);
  walk_mapping(npcs, #'make_num);
}

public int ClearUsersEKCache()
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  users_ek = ([]);
  return 1;
}

public mixed QueryUsersEKCache()
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  return users_ek;
}

public mixed Query_free_num()
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  return free_num;
}

public mixed Add_free_num(int what)
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!what || !intp(what) || by_num[what])
    return SCORE_INVALID_ARG;
  if (member(free_num,what)==-1)
    free_num+=({what});
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("ADDFREENUM: %s %5d (%s, %O)\n",     
  strftime("%d%m%Y-%T",time()),what,
  geteuid(previous_object()), this_interactive()));

  return free_num;
}

public mixed Remove_free_num(int what)
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!what || !intp(what))
    return SCORE_INVALID_ARG;
  free_num-=({what});
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("REMOVEFREENUM: %s %5d (%s, %O)\n",     
  strftime("%d%m%Y-%T",time()),what,
  geteuid(previous_object()),this_interactive()));
  return free_num;
}

public mixed Query_to_change(string who)
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!who)
    return to_change;
  if (who=="")
    return m_indices(to_change);
  return to_change[who];
}

public mixed Add_to_change(string who, int what)
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!who || !stringp(who) || !what || !intp(what))
    return SCORE_INVALID_ARG;
  if (member(to_change,who))
  {
    to_change[who]-=({-what});
    if (member(to_change[who],what)==-1)
      to_change[who]+=({what});
  }
  else
     to_change[who]=({what});
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("ADDTOCHANGE: %s %s %5d (%s, %O)\n",
         strftime("%d%m%Y-%T",time()),who,what,
         geteuid(previous_object()), this_interactive()));
  return to_change[who];
}

public mixed Remove_to_change(string who, int what)
{
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!who || !stringp(who) || !what || !intp(what))
    return SCORE_INVALID_ARG;
  if (member(to_change,who))
  {
     to_change[who]-=({what});
     if (!sizeof(to_change[who]))
        m_delete(to_change,who);
  }
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("REMOVETOCHANGE: %s %s %5d (%s, %O)\n",
         strftime("%d%m%Y-%T",time()),who,what,
         geteuid(previous_object()), this_interactive()));
  return to_change[who];
}

void reset()
{
  string *whop,who,ek;
  mixed what;
  int i,j,value,changed;

  // falls EKs global entfernt werden sollen, schonmal den noetigen Callout
  // starten.
  if (sizeof(to_be_removed) && find_call_out(#'check_all_player) == -1)
      call_out(#'check_all_player, 10, 0);
  // EK-Mainteiner ueber unbestaetigte EKs informieren
  if (sizeof(unconfirmed_scores)) {
    foreach(string n: SCOREMAINTAINERS) {
      if (objectp(find_player(n)))
          tell_object(find_player(n),break_string(
      "Es gibt unbestaetigte EKs im Scoremaster. Schau Dir die doch "
      "mal an. ;-)",78, "Der Scoremaster teilt Dir mit: "));
    }
  }

  i=sizeof(whop=m_indices(to_change))-1;
  while (i>=0 && get_eval_cost()>100000)
  {
    ek = (string)(MASTER->query_ek(who=whop[i]) || "");
    for (j=sizeof(what=to_change[who])-1;j>=0;j--) {
      if ((value=what[j])>0) {
    // Vergabestatistik hochzaehlen.
    npcs[by_num[value,BYNUM_KEY],NPC_COUNT]++;
    ek=set_bit(ek,value);
      }
      else {
    // Vergabestatistik hochzaehlen.
    npcs[by_num[-value,BYNUM_KEY],NPC_COUNT]++;
    ek=clear_bit(ek,-value);
      }
      // if (find_player("rikus")) 
      //tell_object(find_player("rikus"),"SCOREMASTER "+who+" "+erg+"\n");

      write_file(SCOREAUTOLOG,
  sprintf("SET_CLEAR_BIT (reset): %s %4d %s\n",
    who, j, strftime("%d%m%Y-%T",time()) ));
    }
    MASTER->update_ek(who, ek);

    if (member(users_ek, who))
      m_delete(users_ek, who);
    
    m_delete(to_change,who);
    changed=1;
    i--;
  }
  if (changed) save_object(SCORESAVEFILE);
}

public varargs mixed QueryNPC(int score)
{
  string key;
  int val;

  if (!previous_object())
    return SCORE_INVALID_ARG;
  
  key = load_name(previous_object());

  // schon bekannter EK?
  if (member(npcs,key))
    return ({npcs[key,NPC_NUMBER],npcs[key,NPC_SCORE]});

  if (score<=0 || 
      member(inherit_list(previous_object()),"/std/living/life.c") < 0)
    return SCORE_INVALID_ARG;

  if (key[0..8]=="/players/") return SCORE_INVALID_ARG;

  if (score>2) score=2;

  if (sizeof(free_num)) {
      val = free_num[0];
      free_num -= ({val});
  }
  else val=++lastNum;

  npcs[key,NPC_SCORE] = score;
  npcs[key,NPC_NUMBER] = val;
  npcs[key,NPC_COUNT] = 0;
  by_num += ([val: key; score]);
  // werden noch nicht als aktive EKs gewertet, damit sie nicht als Ek-Tips
  // vergben werden.
  //active_eks = set_bit(active_eks, val);

  unconfirmed_scores += ([ val: ({}) ]);

  ClearUsersEKCache();
  save_object(SCORESAVEFILE);
  write_file(SCOREAUTOLOG,sprintf(
  "ADDNPC: %s %5d %4d %s (UID: %s, TI: %O, TP: %O)\n",
  strftime("%d%m%Y-%T",time()),val,score,key,
  getuid(previous_object()), this_interactive(), this_player()));

  while(remove_call_out("DumpNPCs") != -1) ;
  call_out("DumpNPCs",60);
  return ({val,score});
}

public varargs mixed NewNPC(string key,int score)
{
  int val;

  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!key || !stringp(key))
    return SCORE_INVALID_ARG;
  
  key = load_name(key);
  if (val=npcs[key,NPC_NUMBER])
    return ({val,npcs[key,NPC_SCORE]});
  if (score<=0)
    return SCORE_INVALID_ARG;

  if (sizeof(free_num)) {
      val=free_num[0];
      free_num=free_num[1..];
  }
  else val=++lastNum;

  npcs[key,NPC_SCORE] = score;
  npcs[key,NPC_NUMBER] = val;
  npcs[key,NPC_COUNT] = 0;
  by_num += ([val: key; score]);
  active_eks = set_bit(active_eks, val);

  ClearUsersEKCache();
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("NEWNPC: %s %5d %4d %s (%s, %O)\n",
         strftime("%d%m%Y-%T",time()),val,score,key,
         geteuid(previous_object()), this_interactive()));
 while(remove_call_out("DumpNPCs") != -1) ; 
  call_out("DumpNPCs",60);
  return ({val,score});
}

public varargs mixed AddNPC(string key,int score) { return NewNPC(key,score); }

// restauriert die Daten eines frueher geloeschten, in den Spielern noch
// enthaltenen EKs. Moeglich, wenn man Pfad, Nr. und Punkte noch kennt.
public int RestoreEK(string key, int bit, int score) {
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!stringp(key) || !sizeof(key) 
      || !intp(bit) || bit < 0
      || !intp(score) || score < 0)
      return SCORE_INVALID_ARG;

  if (member(npcs,key) || member(by_num,bit))
      return SCORE_INVALID_ARG;

  npcs += ([key: bit;score;0 ]);
  by_num += ([bit: key;score ]);

  ClearUsersEKCache();
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("RESTOREEK: %s %5d %4d %s (%s, %O)\n",
         strftime("%d%m%Y-%T",time()), bit, score, key,
         geteuid(previous_object()), this_interactive()));
  while(remove_call_out("DumpNPCs") != -1) ;
  call_out("DumpNPCs",60);
  return 1;

}

public int ConfirmScore(mixed key) {
  // Bits in den Spielern in unconfirmed_scores setzen und Statistik
  // hochzaehlen
  // Bit in active_eks setzen
  // Eintrag aus unconfirmed_scores loeschen
  int bit;

  if (!allowed()) return SCORE_NO_PERMISSION;
  if (stringp(key) && member(npcs,key)) {
      bit = npcs[key, NPC_NUMBER];
  }
  else if (intp(key) && member(by_num,key)) {
      bit = key;
  }
  else
      return SCORE_INVALID_ARG;

  if (!member(unconfirmed_scores, bit)) 
      return SCORE_INVALID_ARG;

  string obname = by_num[bit, BYNUM_KEY];
  int score = by_num[bit,BYNUM_SCORE];
  
  foreach(string pl: unconfirmed_scores[bit]) {
      string eks = (string)master()->query_ek(pl);
      eks = set_bit(eks, bit);
      master()->update_ek(pl, eks);
      write_file(SCOREAUTOLOG, sprintf(
    "SETBIT: %s %5d %s\n",
    strftime("%d%m%Y-%T",time()), bit, pl));
  }
  //Vergabestatistik hochzaehlen...
  npcs[obname,NPC_COUNT]+=sizeof(unconfirmed_scores[bit]);

  m_delete(unconfirmed_scores, bit);
  active_eks = set_bit(active_eks, bit);
  save_object(SCORESAVEFILE);

  write_file(SCORELOGFILE,sprintf(
      "CONFIRMNPC: %s %5d Score %3d %s [%s, %O]\n",
       strftime("%d%m%Y-%T",time()), bit, score, obname,
       getuid(previous_object()),this_interactive()));

  return 1;
}

public int RejectScore(mixed key) {
  // Eintrag aus unconfirmed_scores, npcs, by_num loeschen
  // Bit-Nr. in free_num eintragen
  // evtl. EK-Spruch entfernen?
  int bit;

  if (!allowed()) return SCORE_NO_PERMISSION;
  if (stringp(key) && member(npcs,key)) {
      bit = npcs[key, NPC_NUMBER];
  }
  else if (intp(key) && member(by_num,key)) {
      bit = key;
  }
  else
      return SCORE_INVALID_ARG;

  if (!member(unconfirmed_scores, bit)) 
      return SCORE_INVALID_ARG;

  string obname = by_num[bit, BYNUM_KEY];
  int score = by_num[bit,BYNUM_SCORE];

  m_delete(by_num, bit);
  m_delete(npcs, obname);
  m_delete(unconfirmed_scores,bit);
  removeTip(obname);
  free_num += ({bit});

  save_object(SCORESAVEFILE);

  write_file(SCORELOGFILE,sprintf(
      "REJECTNPC: %s %5d Score %3d %s [%s, %O]\n",
       strftime("%d%m%Y-%T",time()), bit, score, obname,
       getuid(previous_object()),this_interactive()));
  return 1;
}

// unbestaetigte NPCs in ein File ausgeben
public void DumpUnconfirmedScores() {
  if (!objectp(this_player())) return;
 
  write(sprintf("%5s  %5s  %4s   %s\n",
  "Nr.", "Cnt", "Sc", "Objekt"));
  foreach(int bit, string *pls: unconfirmed_scores) {
    write(sprintf("%5d  %5d  %4d   %s\n",
  bit, sizeof(pls), by_num[bit,BYNUM_SCORE], by_num[bit,BYNUM_KEY]));
  }
}

public mapping _query_unconfirmed() {
  if (allowed()) return unconfirmed_scores;
  return 0;
}

public varargs int SetScore(mixed key,int score)
{
  int num;
  string ob;
  int oldscore;

  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!key) return SCORE_INVALID_ARG;

  if (stringp(key) && sizeof(key)) {
    ob = load_name(key);
    if (!member(npcs, ob)) return SCORE_INVALID_ARG;
    num = npcs[ob, NPC_NUMBER];
    if (ob != by_num[num, BYNUM_KEY])
  return SCORE_INVALID_ARG;
  }
  else if (intp(key) && member(by_num,key) ) {
    num = key;
    ob = by_num[num, BYNUM_KEY];
    if (!member(npcs, ob) || (npcs[ob, NPC_NUMBER] != num))
  return SCORE_INVALID_ARG;
  }
  else
    return SCORE_INVALID_ARG;

  oldscore = by_num[num,BYNUM_SCORE];
  by_num[num,BYNUM_SCORE] = score;
  npcs[ob, NPC_SCORE] = score;

  if (score > 0)
      active_eks = set_bit(active_eks, num);
  else
      active_eks = clear_bit(active_eks, num);

  ClearUsersEKCache();
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf(
  "SETSCORE: %s %5d %.3d OSc: %.3d %s (%s, %O)\n",
         strftime("%d%m%Y-%T",time()),num,score,oldscore, ob,
         geteuid(previous_object()), this_interactive()));
 while(remove_call_out("DumpNPCs") != -1) ; 
  call_out("DumpNPCs",60);
  return 1;
}

// entfernt einen EK endgueltig und unwiderruflich und gibt die Nr. wieder
// frei.
// Technisch wird der EK erstmal in eine Liste eingetragen. Im Reset iteriert
// der Master ueber alle SPieler-Savefiles und loescht den Ek aus alle
// Spielern. Nach Abschluss wird der Eintrag in npcs geloescht und seine Nr.
// in die Liste freier Nummern eingetragen.
public int* MarkEKForLiquidation(mixed key) {
  int bit;
  if (!allowed())
      return 0;
  // nicht in to_be_removed aendern, wenn check_all_player() laeuft.
  if (find_call_out(#'check_all_player) != -1)
      return 0;

  if (stringp(key) && sizeof(key)) {
    if (!member(npcs,key)) return 0;
    bit = npcs[key,NPC_NUMBER];
  }
  else if (intp(key) && key>=0) {
    bit = key;
  }
  else
    return 0;

  if (member(to_be_removed,bit) == -1)
    to_be_removed += ({bit});
  write_file(SCORELOGFILE,sprintf("DELETEFLAG: %s %5d %s (%s, %O)\n",
  strftime("%d%m%Y-%T",time()), bit, by_num[bit,BYNUM_KEY] || "NoName",
  geteuid(previous_object()), this_interactive()));
  
  save_object(SCORESAVEFILE);
  
  return to_be_removed;
}

// geht nur, solange nach einem RemoveEK() noch kein reset() gelaufen ist!
public int* UnmarkEKForLiquidation(mixed key) {
  int bit;
  if (!allowed())
      return 0;
  // nicht in to_be_removed aendern, wenn check_all_player() laeuft.
  if (find_call_out(#'check_all_player) != -1)
      return 0;

  if (stringp(key) && sizeof(key)) {
    if (!member(npcs,key)) return 0;
    bit = npcs[key,NPC_NUMBER];
  }
  else if (intp(key) && key>=0) {
    bit = key;
  }
  else
    return 0;
 
  to_be_removed -= ({bit});
  write_file(SCORELOGFILE,sprintf("UNDELETEFLAG: %s %5d %s (%s, %O\n",
  strftime("%d%m%Y-%T",time()),bit, by_num[bit, BYNUM_KEY] || "NoName",
  geteuid(previous_object()), this_interactive()));
  
  save_object(SCORESAVEFILE);

  return to_be_removed;
}

public int* QueryLiquidationMarks() {
  if (allowed())
      return to_be_removed;
  else
      return 0;;
}

// setzt nur den Scorewert auf 0, sonst nix. Solche EKs koennen dann spaeter
// durch Angabe eines neues Scorewertes reaktiviert werden.
public int RemoveScore(mixed key) {
  int changed;
  int oldscore;

  if (!allowed())
    return SCORE_NO_PERMISSION;

  if (stringp(key) && member(npcs,key)) {
    int num = npcs[key, NPC_NUMBER];
    if ( key == by_num[num, BYNUM_KEY]) {
      oldscore = by_num[num, BYNUM_SCORE];
      npcs[key, NPC_SCORE] = 0;
      by_num[num, BYNUM_SCORE] = 0;
      active_eks = clear_bit(active_eks,num); 
      write_file(SCORELOGFILE,sprintf(
      "REMOVESCORE: %s %5d OSc: %.3d %s (%s, %O)\n",
        strftime("%d%m%Y-%T",time()), num, oldscore, key, 
        geteuid(previous_object()), this_interactive()));
      changed = 1;
    }
  }
  else if (intp(key) && member(by_num, key)) {
    string obname = by_num[key, BYNUM_KEY];
    if (key == npcs[obname, NPC_NUMBER]) {
      oldscore = by_num[key, BYNUM_SCORE];
      npcs[obname, NPC_SCORE] = 0;
      by_num[key, BYNUM_SCORE] = 0;
      active_eks = clear_bit(active_eks,key); 
      write_file(SCORELOGFILE,sprintf(
      "REMOVESCORE: %s %5d OSc: %.3d %s (%s, %O)\n",
        strftime("%d%m%Y-%T",time()),key, oldscore, obname,
        geteuid(previous_object()), this_interactive()));
      changed = 1;
    }
  }

  if (changed) {
    ClearUsersEKCache();
    save_object(SCORESAVEFILE);
    while(remove_call_out("DumpNPCs") != -1) ;
    call_out("DumpNPCs",60);
    return 1;
  }
  return SCORE_INVALID_ARG;
}

public varargs int MoveScore(mixed oldkey, string newpath)
{
  int num,score;
  string oldpath;
  string tip;
  
  if (!allowed())
    return SCORE_NO_PERMISSION;
  if (!stringp(newpath))
    return SCORE_INVALID_ARG;

  if (stringp(oldkey)) {
    oldkey = load_name(oldkey); 
    num=npcs[oldkey,NPC_NUMBER];
  }
  else if (intp(oldkey)) num=oldkey;
  else return SCORE_INVALID_ARG;

  if (!member(by_num,num)) return SCORE_INVALID_ARG;
  
  tip=getTipFromList(oldkey);
  oldpath = by_num[num, BYNUM_KEY];
  score = by_num[num, BYNUM_SCORE];

  if (member(npcs, oldpath)) {
    m_delete(npcs, oldpath);
    removeTip(oldkey);
    if(tip!="") addTip(newpath,tip);
    npcs[newpath, NPC_SCORE] = score;
    npcs[newpath, NPC_NUMBER] = num;
  }
  else return SCORE_INVALID_ARG;

  by_num += ([num: newpath; score]);

  ClearUsersEKCache();
  save_object(SCORESAVEFILE);
  write_file(SCORELOGFILE,sprintf("MOVESCORE: %s %s %s (%s, %O)\n",
  strftime("%d%m%Y-%T",time()),oldpath,newpath,
  geteuid(previous_object()),this_interactive()));

  while(remove_call_out("DumpNPCs") != -1) ;
  call_out("DumpNPCs",60);
  return 1;
}

// liefert alle Kills des Spielers zurueck, auch solche, die momentan
// ausgetragen/deaktiviet sind.
public string QueryAllKills(string pl)
{
  return (MASTER->query_ek(pl) || "");
}

// filtert alle Eintraege aus dem Bitstring heraus, die fuer
// ausgetragene/inaktive EKs stehen.
public string QueryKills(string pl) {
  string res = (string)MASTER->query_ek(pl) || "";
  // vergleichen mit den aktiven EKs aus active_eks und nur jene Bits
  // zurueckliefern, die in beiden Strings gesetzt sind.
  return and_bits(res,active_eks);
}

public int QueryKillPoints(mixed pl) {
  
  if (!allowed() &&
      (!previous_object() 
       || strstr(object_name(previous_object()), "/gilden/") != 0) )
     return 0;

  if (!stringp(pl)) pl=getuid(pl);

  if (member(users_ek,pl)) return users_ek[pl];

  string s = (MASTER->query_ek(pl) || "");
  
  int p=-1;
  int summe;
  while ((p=next_bit(s,p)) != -1) {
      summe+=by_num[p,BYNUM_SCORE];
  }

  users_ek += ([pl:summe]);
  return summe;
}

public mixed *QueryNPCbyNumber(int num)
{
  if (!allowed())
    return 0;

  if (member(by_num, num))
    return ({num, by_num[num, BYNUM_SCORE], by_num[num, BYNUM_KEY]});

  return 0;
}

protected mixed *StaticQueryNPCbyNumber(int num)
{
  if (member(by_num, num))
    return ({num, by_num[num, BYNUM_SCORE], by_num[num, BYNUM_KEY]});

  return 0;
}

public mixed *QueryNPCbyObject(object o)
{
  string key;
  int val;

  key=load_name(o);
  if (member(npcs,key)) {
    val = npcs[key,NPC_NUMBER];
    return ({val, by_num[val, BYNUM_SCORE], by_num[val, BYNUM_KEY]});
  }
  return 0;
}

public int GiveKill(object pl, int bit)
{
  mixed info;
  object po;
  int drin;
  string pls, ek;


  if (!pointerp(info = StaticQueryNPCbyNumber(bit)))
    return -1;

  if ((!po=previous_object()) 
      || load_name(po) != info[SCORE_KEY])
    return -2;

  pls=getuid(pl);

  // wenn unbestaetigt, Spieler fuer spaeter merken
  if (member(unconfirmed_scores, bit)) {
    if (member(unconfirmed_scores[bit], pls) == -1)
  unconfirmed_scores[bit] += ({pls});
  }
  else {
    // sonst wird das Bit direkt im Spieler gesetzt.
    ek = (MASTER->query_ek(pls) || "");
    if (test_bit(ek, bit))
      return -3;
    ek = set_bit(ek, bit);
    MASTER->update_ek(pls, ek);
    // Vergabestatistik hochzaehlen.
    npcs[by_num[bit,BYNUM_KEY],NPC_COUNT]++;
  }

  if (member(users_ek, pls))
    m_delete(users_ek, pls);

  EK_GIVENLOG(sprintf("%s: %s", info[SCORE_KEY], pls)); 

  return info[SCORE_SCORE];
}

public int HasKill(mixed pl, mixed npc)
{
  string fn, *pls;

  if (!objectp(pl) && !stringp(pl) && 
      !objectp(npc) && !stringp(npc) && !intp(npc))
    return 0;
  if (!stringp(pl)) 
    pl=getuid(pl);

  if (intp(npc))
    npc=by_num[npc,BYNUM_KEY];
  fn=load_name(npc);

  if (!member(npcs, fn)) return 0;
  
  int bit = npcs[fn, NPC_NUMBER];
  
  if (pointerp(pls=unconfirmed_scores[bit]) &&
      member(pls,pl) != -1)
    return 1;

  string eks = (MASTER->query_ek(pl) || "");

  return test_bit(eks, bit);
}

private void WriteDumpFile(string *keys) {
  int maxn;

  if (!pointerp(keys)) return;

  rm(SCOREDUMPFILE);

  write_file(SCOREDUMPFILE,sprintf("%5s  %5s  %4s   %s\n",
  "Nr.", "Cnt", "Sc", "Objekt"));
  foreach(string key: keys) {
    write_file(SCOREDUMPFILE,sprintf("%5d  %5d  %4d   %O\n",
    npcs[key,NPC_NUMBER], npcs[key,NPC_COUNT],
    npcs[key,NPC_SCORE], key));
    maxn += npcs[key,NPC_SCORE];
  }
  write_file(SCOREDUMPFILE,sprintf(
  "========================================================\n"
  "NPCs gesamt: %d Punkte\n\n",maxn));
}

public varargs int DumpNPCs(int sortkey) {

  if (extern_call() && !allowed()) return SCORE_NO_PERMISSION;
  if (!intp(sortkey)) return SCORE_INVALID_ARG;

  rm(SCOREDUMPFILE);

  // sortieren
  string *keys=sort_array(m_indices(npcs), function int (string a, string b) {
        return(npcs[a,sortkey] < npcs[b,sortkey]); } );
  call_out(#'WriteDumpFile, 2, keys);

  return 1;
}

public int SetScoreBit(string pl, int bit)
{
  string ek;

  if (!allowed())
    return SCORE_NO_PERMISSION;

  ek = (MASTER->query_ek(pl) || "");
  ek = set_bit(ek, bit);
  MASTER->update_ek(pl, ek);

  // Vergabestatistik hochzaehlen.
  npcs[by_num[bit,BYNUM_KEY],NPC_COUNT]++;

  if (member(users_ek, pl))
    m_delete(users_ek, pl);

  write_file(SCORELOGFILE,sprintf("SETBIT: %s %s %5d Sc: %.3d %s (%s, %O)\n",
         strftime("%d%m%Y-%T",time()),pl, bit,
         by_num[bit,BYNUM_SCORE], by_num[bit,BYNUM_KEY],
         geteuid(previous_object()), this_interactive()));
  return 1;
}

public int ClearScoreBit(string pl, int bit)
{
  string ek;

  if (!allowed())
    return SCORE_NO_PERMISSION;

  ek = (MASTER->query_ek(pl) || "");
  ek = clear_bit(ek, bit);
  MASTER->update_ek(pl, ek);

  // Vergabestatistik runterzaehlen.
  npcs[by_num[bit,BYNUM_KEY],NPC_COUNT]--;

  if (member(users_ek, pl))
    m_delete(users_ek, pl);

  write_file(SCORELOGFILE,sprintf(
  "CLEARBIT: %s %s %5d Sc: %.3d %s (%s, %O)\n",       
  strftime("%d%m%Y-%T",time()),pl,bit,
  by_num[bit,BYNUM_SCORE],by_num[bit,BYNUM_KEY],
  geteuid(previous_object()), this_interactive()));
  return 1;
}

private status ektipAllowed()
{ 
  status poOK;
  string poName;
  status ret;
                
  poName=load_name(previous_object());        
  poOK=previous_object() &&     
    ((previous_object()==find_object(EKTIPGIVER)) || (poName==EKTIPLIST) );

  ret=allowed() || 
    (this_player() && this_interactive() && previous_object() && 
     this_interactive()==this_player() && poOK);
  return ret;
}

// liefert alle EKs, die aktiv sind und die der Spieler noch nicht hat in
// einem Mapping entsprechend npcs zurueck.
public mapping getFreeEKsForPlayer(object player)
{
  if(!ektipAllowed() || !objectp(player) || !query_once_interactive(player)){
      return ([]);
  }
  // alle EKs, die der Spieler hat
  string eks = (string)master()->query_ek(getuid(player));
  // als Tips kommen alle in Frage, die er nicht hat, vor dem Invertieren muss
  // aber sichergestellt werden, dass eks min. so lang ist wie active_eks, da
  // die Invertierung ja z.B. nur EKs 0-1700 beruecksichtigt, wenn 1700 der
  // hoechste EK im Spieler ist und dann als Tips alle EKs ueber
  // 1700 verlorengingen.
  // hier wird das letzte Bit von active_eks ermittelt und das darauf folgende
  // Bit im Spieler-EK-String gesetzt und wieder geloescht, woraufhin der
  // EK-String min. so lang wie active_eks ist. (es ist egal, wenn er
  // laenger ist, auch egal, wenn man ein Bit ueberschreibt, das faellt alles
  // gleich beim and_bits() raus.
  int lb = last_bit(active_eks) + 1;
  eks = clear_bit(set_bit(eks, lb), lb);
  // jetzt invertieren
  string non_eks = invert_bits(eks);
  // jetzt vorhande EK-Tips ausfiltern. Im Prinzip gleiches Spiel wie oben.
  string ektips = (string)master()->query_ektips(getuid(player));
  // jetzt alle nicht als Tip vergebenen NPC ermitteln, vor dem Invertieren
  // wieder Laenge angleichen...
  ektips = invert_bits(clear_bit(set_bit(ektips, lb), lb));
  // verunden liefert EKs, die der Spieler nicht hat und nicht als Tip hat
  ektips = and_bits(ektips, non_eks);

  // und nun die inaktive EKs ausfiltern, nochmal verunden
  ektips = and_bits(ektips, active_eks);

  // mal Platz reservieren, entsprechend der Menge an Bits
  mapping freeKills = m_allocate( count_bits(ektips), 2);
  // durch alle jetzt gesetzten Bits laufen, d.h. alle EKs, die der Spieler
  // nicht hat und ein Mapping a la npcs erstellen.
  int p=-1;
  while ( (p=next_bit(ektips, p)) != -1) {
    freeKills += ([ by_num[p,0]: p; by_num[p,1] ]);
  }

  return freeKills;
}

public int addTip(mixed key,string tip)
{
  string fn;
  
  if (!allowed())
    return SCORE_NO_PERMISSION;

  if (!tip || (!objectp(key) && !stringp(key)))
    return SCORE_INVALID_ARG;

  fn=load_name(key);

  if (!member(npcs, fn)) return SCORE_INVALID_ARG;
  tipList+=([fn:tip]);
  save_object(SCORESAVEFILE);
    
  return 1;
}

public int changeTip(mixed key,string tip)
{
    return addTip(key,tip);
}

public int removeTip(mixed key)
{
  string fn;
  
  if (!allowed())
    return SCORE_NO_PERMISSION;
  
  if ((!objectp(key) && !stringp(key)))
    return SCORE_INVALID_ARG;

  fn=load_name(key);
  
  if (!member(tipList, fn)) return SCORE_INVALID_ARG;
    
  m_delete(tipList,fn);
  save_object(SCORESAVEFILE);
    
  return 1;  
}

private string getTipFromList(mixed key)
{
  string fn;
  
  if (!ektipAllowed())
    return "";
  
  if ((!objectp(key) && !stringp(key)))
    return "";

  fn=load_name(key);
  
  if (!member(tipList, fn)) return "";
        
  return tipList[fn];  
}

private string _getTip(mixed key)
{
  string fn;
  string tip;
  string* path;
    
  if ((!objectp(key) && !stringp(key)))
    return "";

  fn=load_name(key);
  
  if(!member(npcs,fn)) return "";
  
  tip=getTipFromList(fn);
  if(!tip || tip==""){
    path=explode(fn,"/")-({""});
    if(sizeof(path)<3) return "";
    if(path[0]=="players") {
      string tiptext;
      if ( path[1] == "ketos" )
        tiptext = "Ketos im Gebirge";
      else if ( path[1] == "boing" && path[2] == "friedhof" )
        tiptext = "Boing im eisigen Polar";
      else
        tiptext = capitalize(path[1]);
      return "Schau Dich doch mal bei "+tiptext+" um.";
    }
    
    if(path[0]=="d")
    {
      tip+="Schau Dich doch mal ";
    
      if(file_size("/players/"+path[2])==-2)
      {
        tip+="bei "+capitalize(path[2]+" ");
      }
      if ( path[1]=="polar" && file_size("/players/"+path[3])==-2 )
      {
        tip+="bei "+capitalize(path[3])+" ";
      }

      if(path[1]=="anfaenger")
        tip+="in den Anfaengergebieten ";
      if(path[1]=="fernwest")
        tip+="in Fernwest ";
      if(path[1]=="dschungel")
        tip+="im Dschungel ";
      if(path[1]=="schattenwelt")
        tip+="in der Welt der Schatten ";
      if(path[1]=="unterwelt")
        tip+="in der Unterwelt ";
      if(path[1]=="gebirge")
        tip+="im Gebirge ";
      if(path[1]=="seher")
        tip+="bei den Sehergebieten ";
      if(path[1]=="vland")
        tip+="auf dem Verlorenen Land ";
      if(path[1]=="ebene")
        tip+="in der Ebene ";
      if(path[1]=="inseln")
        tip+="auf den Inseln ";
      if(path[1]=="wald")
        tip+="im Wald ";
      if(path[1]=="erzmagier")
        tip+="bei den Erzmagiern ";
      if(path[1]=="polar")
      {
        if (path[2]=="files.chaos")
          tip+="in den Raeumen der Chaosgilde ";
        tip+="im eisigen Polar ";
      }
      if(path[1]=="wueste")
        tip+="in der Wueste ";
      tip+="um.";
    }
    else if ( path[0]=="gilden" )
    {
      tip+="Schau Dich doch mal";
      switch( path[1] )
      {
        case "mon.elementar": 
          tip+=" unter den Anfuehrern der Elementargilde"; 
          break;
        case "files.dunkelelfen":
          tip+=" unter den Anfuehrern der Dunkelelfengilde";
          break;
        case "files.klerus":
          tip+=" beim Klerus"; 
          break;
        case "files.werwoelfe": 
          tip+=" unter den Anfuehrern der Werwoelfe";
          break;
        case "files.chaos": 
          tip+=" unter den Anfuehrern der Chaosgilde";
          break;
        default: 
          tip+=" in einer der Gilden"; 
          break;
      }
      tip+=" um.";
    }
    else if ( path[0] == "p" ) 
    {
      tip+="Schau Dich doch mal ";
      switch( path[1] ) 
      {
        case "zauberer":
          tip+="in der Zauberergilde zu Taramis";
          break;
        case "kaempfer":
          tip+="bei den Angehoerigen des Koru-Tschakar-Struvs";
          break;
        case "katzenkrieger":
          tip+="bei der Gilde der Katzenkrieger";
          break;
        case "tanjian":
          tip+="unter den Meistern der Tanjiangilde";
          break;
      }
      tip+=" um.";
    }
  }
  return tip;
}

// return valid tips from database or existing 
public string getTip(mixed key)
{
  string fn;
  string tip;
  string* path;
  
  if (!ektipAllowed())
    return "";
  
  return _getTip(key);
}

// liefert ein Array mit allen Objekten zurueck, auf die bitstr verweist, also
// eine Liste aller Objekte, die als Tip vergeben wurden.
private string* makeTiplistFromBitString(string bitstr)
{ 
  string * ret= allocate(count_bits(bitstr));
  // ueber alle gesetzten bits laufen und Array zusammensammeln
  int i;
  int p=-1;
  while ((p=next_bit(bitstr,p)) != -1) {
    ret[i] = by_num[p, 0];
    i++;
  }
  // zur Sicherheit
  ret -= ({0});

  return ret;
}

// gibt die Objektnamen der EK-Tips vom jeweiligen Spieler zurueck.
public string *QueryTipObjects(mixed player) {
  
  if (extern_call() && !allowed())
      return 0;
  if (objectp(player) && query_once_interactive(player))
      player=getuid(player);
  if (!stringp(player))
    return 0;

  string tipstr=(string)master()->query_ektips(player);
  // jetzt EK-Tips ausfiltern, die erledigt sind. Dazu EK-Liste holen...
  string eks=(string)master()->query_ek(player);
  // als Tips kommen alle in Frage, die er nicht hat, vor dem Invertieren muss
  // aber sichergestellt werden, dass eks min. so lang ist wie tipstr, da
  // die Invertierung ja z.B. nur EKs 0-1700 beruecksichtigt, wenn 1700 der
  // hoechste EK im Spieler ist und dann alle Tips ueber
  // 1700 verlorengingen.
  // hier wird das letzte Bit von tipstr ermittelt und das darauf folgende
  // Bit im Spieler-EK-String gesetzt und wieder geloescht, woraufhin der
  // EK-String min. so lang wie der Tipstring ist. (es ist egal, wenn er
  // laenger ist, auch egal, wenn man ein Bit ueberschreibt, das faellt alles
  // gleich beim and_bits() raus.
  int lb = last_bit(tipstr) + 1;
  eks = clear_bit(set_bit(eks, lb), lb);
  // jetzt invertieren
  string non_eks = invert_bits(eks);
  // jetzt verunden und man hat die Tips, die noch nicht gehauen wurden.
  tipstr = and_bits(tipstr, non_eks);
  // noch inaktive EKs ausfiltern...
  tipstr = and_bits(tipstr, active_eks);

  return makeTiplistFromBitString(tipstr);
}

public string allTipsForPlayer(object player)
{

  if(!player || !this_interactive() || 
      (this_interactive()!=player && !IS_ARCH(this_interactive())) )
    return "";    
 
  string *tips = QueryTipObjects(player);

  tips = map(tips, #'_getTip);
  tips -= ({0,""});

  return implode(tips, "\n");
}

public status playerMayGetTip(object player)
{
  int numElegible;
  int numReceived;
  int lvl;
  int i;
  string tips;
  
  if(!ektipAllowed() || !player || !query_once_interactive(player))      
      return 0;

  if(!player || !query_once_interactive(player))    
      return 0;
  
  lvl=(int)player->QueryProp(P_LEVEL);
  numElegible=0;
  i=sizeof(EKTIPS_LEVEL_LIMITS)-1;

  if(lvl>EKTIPS_LEVEL_LIMITS[i])    
      numElegible+=(lvl-EKTIPS_LEVEL_LIMITS[i]);

  for(i;i>=0;i--){
      if(lvl>=EKTIPS_LEVEL_LIMITS[i]) numElegible++;
  }

  tips=(string)MASTER->query_ektips(getuid(player)) || "";
  // inaktive EKs ausfiltern.
  tips = and_bits(tips, active_eks);
  // und Gesamtzahl an Tips zaehlen. Hier werden erledigte Tips explizit nicht
  // ausgefiltert!
  numReceived=count_bits(tips);

  return numElegible>numReceived;
}

public string giveTipForPlayer(object player)
{
  string* tmp;
  mapping free;
  string tip,pl,ektip;
  int index;
  
  if(!ektipAllowed() || !player || 
      !query_once_interactive(player) || !playerMayGetTip(player))  
    return "";
  
  pl=getuid(player);
  free=getFreeEKsForPlayer(player);

  if(!mappingp(free) || sizeof(free)==0)
    return "";

  tmp=m_indices(free);

  ektip=(string)MASTER->query_ektips(pl) || "";
 
  foreach(int i: EKTIPS_MAX_RETRY) {
      index=random(sizeof(tmp));
      tip=getTip(tmp[index]);
      if (stringp(tip) && sizeof(tip)) {
    ektip=set_bit(ektip,npcs[tmp[index],NPC_NUMBER]);
    MASTER->update_ektips(pl,ektip);
    break; //fertig
      }
  }

  return tip;  
}

// checkt NPCs auf Existenz und Ladbarkeit
public void CheckNPCs(int num) {
  string fn;
  object ekob;
  if (!num) rm(SCORECHECKFILE);
  while (num <= lastNum && get_eval_cost() > 1480000) {
    if (!by_num[num,1] || !by_num[num,0]) {
      num++;
      continue;
    }
    fn = by_num[num,0] + ".c";
    if (file_size(fn) <= 0) {
      // File nicht existent
      write_file(SCORECHECKFILE, sprintf(
    "EK %.4d ist nicht existent (%s)\n",num,fn));
    }
    else if (catch(ekob=load_object(fn)) || !objectp(ekob) ) {
      // NPC offenbar nicht ladbar.
      write_file(SCORECHECKFILE, sprintf(
    "EK %.4d ist nicht ladbar (%s)\n",num,fn));
    }
    num++;
  }
  ZDEBUG(sprintf("%d NPC checked",num));
  if (num <= lastNum)
    call_out(#'CheckNPCs,4,num);
  else
    ZDEBUG("Finished!");
}

// liquidiert einen EK endgueltig. An diesem Punkt wird davon augegangen, dass
// kein Spieler den EK mehr gesetzt hat!
private void LiquidateEK(int bit) {

  if (!intp(bit) || bit < 0) return;

  string obname = by_num[bit, BYNUM_KEY];
  int score = by_num[bit, BYNUM_SCORE];

  if (member(npcs, obname) && (npcs[obname, NPC_NUMBER] == bit)) {
    m_delete(by_num, bit);
    m_delete(npcs, obname);
    if (member(unconfirmed_scores,bit))
      m_delete(unconfirmed_scores,bit);
    active_eks = clear_bit(active_eks,bit);
    removeTip(obname);
    free_num += ({bit});
    write_file(SCOREAUTOLOG,sprintf(
    "LIQUIDATEEK: %s %5d Score %3d %s\n",
    strftime("%d%m%Y-%T",time()), bit, score, obname));
  }
}

private void check_player(string pl) {
  int changed, changed2; 
  
  // EKs pruefen
  string eks=(string)master()->query_ek(pl) || "";
  string *opfer=allocate( (sizeof(eks)*6)+1, "");
  int p=-1;
  while ((p=next_bit(eks,p)) != -1) {
    if (!member(by_num, p)) {
  write_file(SCORECHECKFILE, sprintf(
   "UNKNOWNEK %s %5d in %s gefunden.\n", 
    strftime("%d%m%Y-%T",time()), p, pl));
    }
    // wenn das aktuelle Bit geloescht werden soll, also in to_be_removed
    // steht...
    if (member(to_be_removed,p) != -1) {
  eks = clear_bit(eks,p);
  changed=1;
  write_file(EKCLEANLOG,sprintf(
        "CLEARBIT: %s %O %5d %s\n",
        strftime("%d%m%Y-%T",time()), pl, p, 
        by_num[p,BYNUM_KEY] || "NoName"));
    }
    else {
      // sonst statistikpflege
      npcs[by_num[p,BYNUM_KEY],NPC_COUNT]++;
      // loggen, welche NPC der Spieler hat
      opfer[p]=to_string(p);
    }
  }
  // und noch die Ek-Tips...
  string ektips = (string)master()->query_ektips(pl) || "";
  p = -1;
  while ((p=next_bit(ektips,p)) != -1) {
    if (!member(by_num, p)) {
  write_file(EKCLEANLOG, sprintf(
    "UNKNOWNEK %s %5d in %s (EKTips) gefunden - clearing.\n", 
    strftime("%d%m%Y-%T",time()), p, pl));
  ektips = clear_bit(ektips, p); // hier direkt loeschen.
  changed2 = 1;
    }
    // wenn das aktuelle Bit geloescht werden soll, also in to_be_removed
    // steht...
    else if (member(to_be_removed,p) != -1) {
  ektips = clear_bit(ektips,p);
  changed2=1;
  write_file(EKCLEANLOG,sprintf(
        "CLEAREKTIP: %s %O %5d %s\n",
        strftime("%d%m%Y-%T",time()), pl, p, 
        by_num[p,BYNUM_KEY] || "NoName"));
    }
  }

  if (changed) {
      master()->update_ek(pl, eks);
  }
  if (changed2) {
      master()->update_ektips(pl, ektips);
  }
  opfer-=({""});
  write_file(WERKILLTWEN,sprintf("%s\n%=-78s\n\n",pl,CountUp(opfer)||""));
}

public void check_all_player(mapping allplayer) {
 
  if (extern_call() && !allowed())
      return;

  if (!mappingp(allplayer)) {
      foreach(string key: npcs) {
  npcs[key,NPC_COUNT]=0;
      }
      allplayer=(mapping)master()->get_all_players();
      rm(WERKILLTWEN);
      call_out(#'check_all_player,2,allplayer);
      return;
  }

  // offenbar fertig mit allen Spielern, jetzt noch den Rest erledigen.
  if (!sizeof(allplayer)) {
    foreach(int bit: to_be_removed) {
      LiquidateEK(bit);
    }
    to_be_removed=({});
    save_object(SCORESAVEFILE); 
    ZDEBUG("Spielerchecks und EK-Liquidation fertig.\n");
    return;
  }

  string dir=m_indices(allplayer)[0];
  string *pls=allplayer[dir];
  foreach(string pl: pls) {
    if (get_eval_cost() < 1250000)
      break; // spaeter weitermachen.
    catch(check_player(pl) ; publish);
    pls-=({pl});
  }
  allplayer[dir] = pls; 
 
  if (!sizeof(allplayer[dir]))
    m_delete(allplayer,dir);
 
  call_out(#'check_all_player,2,allplayer);
}


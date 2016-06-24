// MorgenGrauen MUDlib
//
// svn2news.c -- Setzt die Subversion-Eintraege in magier.mudlib
//
// $Id$

// Erstellt auf Basis von rcs2news von Zook.

#pragma strict_types,save_types
#pragma no_clone,no_shadow

#include <daemon.h>

#define NEWSFILE "/p/daemon/save/svn2news.txt"
#define NEWSFILEOLD "/p/daemon/save/svn2news.old"

#define DEBUG(str) if (find_player("zook"))  \
  tell_object(find_player("zook"), sprintf("svn2news: DEBUG: %O\n",str))

static void checkrcs()
{
  string str;
  mixed art;
  // Letzte Eintrag sollte ein wenig her sein, damit es keine
  // Ueberschneidungen gibt.
  if (file_time(NEWSFILE)+1800 > time()) return;
  if (!(str=read_file(NEWSFILE,0,10000)) || (str=="")) return;
  art=({"magier.mudlib","SVN2NEWS",0,0,
        "SVN-Eintraege vom "+dtime(time()),str});
  if (("/secure/news"->WriteNote(art,1)>=1) ||
      (("/secure/news"->RemoveNote("magier.mudlib",0)>=1) &&
       ("/secure/news"->WriteNote(art,1)>=1)))
    catch(rename(NEWSFILE,NEWSFILEOLD);publish);
  else log_file("SVN2NEWS",dtime(time())+
		" : Kann RCS-Eintraege nicht nach magier.mudlib posten!\n");
}

void reset()
{
  // call_out() noetig, da sonst kein weiteres reset() gerufen wird
  // reset() wird nur gerufen bei einem call einer anderen Funktion
  // nach einem Reset
  call_out("checkrcs",1);
}

void create()
{
  seteuid(getuid());
  reset();
}

void clean_up(int refc)
{
  return 0;
}

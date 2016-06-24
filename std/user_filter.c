// MorgenGrauen MUDlib
//
// user_filter.c -- Hilfsmodul fuer die wer-Liste
//
// $Id: user_filter.c 8755 2014-04-26 13:13:40Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

#include <properties.h>
#include <wizlevels.h>
#include <language.h>

#define ME this_object()

static int is_in ( object ob, mixed x )
{
  return (strstr( lower_case(country(ob)), x ) != -1 ||
          strstr( lower_case(ob->Query(P_LOCATION) || ""), x ) != -1);
}

static int is_at( object ob, mixed x )
{
  return environment(ob) == x;
}

static int is_wiz_level_lt( object ob, mixed x )
{
  return query_wiz_level(ob) < x;
}

static int is_wiz_level_ge( object ob, mixed x )
{
  return query_wiz_level(ob) >= x;
}

static int is_prop_set( object ob, mixed x )
{
  return (mixed)ob->QueryProp(x) != 0;
}

static int is_second( object ob, mixed x )
{
  return ob->QueryProp(P_SECOND) && ob->QueryProp(P_SECOND_MARK) > x;
}

static int is_mage_second( object ob )
{
  return ob->QueryProp(P_SECOND) && IS_LEARNER(ob->QueryProp(P_SECOND));
}

static int is_in_gilde( object ob, mixed x )
{
  string str;

  // nur Magier koennen eine andere Gilde per P_VISIBLE_GUILD in werlisten
  // etc. vortaeuschen.
  if (IS_LEARNER(ob)) {
    // Querymethode von P_VISIBLE_GUILD fragt ggf. auch P_GUILD ab.
    if ( !stringp(str = (string)ob->QueryProp(P_VISIBLE_GUILD)) )
      return 0;
  }
  else {
      str = (string)ob->QueryProp(P_GUILD);
  }
  if (!stringp(str)) return 0; // login hat z.B. keine Gilde.
  return strstr(lower_case(str),x) == 0;
}

static int is_in_team( object ob, mixed x )
{
  object team;
  
  if ( !objectp(team = (object)ob->QueryProp(P_TEAM)) )
    return x == "";
  
  return lower_case((string)team->Name()) == x;
}

static int is_name_in( object ob, mixed x )
{
  return member( x, capitalize(geteuid(ob)) ) >= 0;
}

static int is_in_region( object ob, mixed x )
{
  return environment(ob) &&
    (object_name(environment(ob))[0..(sizeof(x)-1)] == x);
}

static int is_region_member( object ob, mixed x )
{
  return (int)master()->domain_member( geteuid(ob), x );
}

static int is_region_master( object ob, mixed x )
{
  return (int)master()->domain_master( geteuid(ob), x );
}

static int is_guild_master(object ob, mixed x)
{
  return (int)master()->guild_master(geteuid(ob), x);
}

static int is_gender( object ob, mixed x )
{
  return ob->QueryProp(P_GENDER) == x;
}

static int is_race( object ob, mixed x )
{
  return ob->QueryProp(P_RACE) == x;
}

static int is_unmarried( object ob )
{
  return ob->QueryProp(P_MARRIED) == 0;
}

static int is_idle( object ob, mixed x )
{
  return query_idle(ob) >= x;
}

static int is_hc( object ob)
{
  return ob->query_hc_play()>0;
}

static int is_ghost( object ob)
{
  return (int)ob->QueryProp(P_GHOST);
}

static int uses_ssl(object ob)
{
  if(!this_interactive() || !IS_LEARNER(this_interactive()))
    return 0;
#if __EFUN_DEFINED__(tls_query_connection_info)
  return tls_query_connection_info(ob) != 0;
#else
  return 0;
#endif
}

protected int is_active_guide(object ob) {
    int cic=(int)ob->QueryProp(P_NEWBIE_GUIDE);
    if (!intp(cic) || cic <= 0)
      return 0;
    else if (cic < 60)
      return 1; // kleiner 60s in der Prop -> immer aktiv.
    else if (cic <= 86400) {
      // wenn laenger idle als cic -> inaktiv
      if (query_idle(ob) > cic)
	return 0;
      return 1;
    }
    // ungueltiger Wertebereich -> inaktiv.
    return 0;
}

object *filter_users( string str )
{
  object *res, *orig, *zwi;
  string *words;
  int i, sz, f, l,t;
  mixed x;
  
  orig = users();
  
  if (!str)
    return orig;
  
  res = ({});
  words = old_explode( lower_case(str), " " );
  sz = sizeof(words);
  
  for ( i = 0; i < sz; i++ ){
    zwi = ({});
    
    switch (words[i]) {
    case "ausser":
    case "ohne":
    case "nicht":
      f=-1;
      if (!i) res=orig;
      continue;
      
    case "oder":
      f=0;
      continue;
      
    case "und":
      f=1;
      continue;
      
    case "alle":
      zwi = orig;
      break;

    case "in":
    case "aus":
      if ( ++i >= sz )
        break;
      zwi = filter( orig, "is_in", ME, words[i] );
      break;
      
    case "region":
      if ( ++i >= sz )
        break;
      zwi = filter( orig, "is_in_region", ME, "/d/" + words[i] );
      break;
      
    case "mitarbeiter":
    case "regionsmitarbeiter":
      if ( ++i >= sz )
        break;
      zwi = filter( orig, "is_region_member", ME, words[i] );
      break;
      
    case "regionsmagier":
      if ( ++i >= sz )
        break;
      zwi = filter( orig, "is_region_master", ME, words[i] );
      break;
      
    case "gildenmagier":
      if ( ++i >= sz )
	break;
      zwi = filter (orig, "is_guild_master", ME, words[i] );
      break;
      
    case "gilde":
      if ( ++i >= sz )
        break;
      zwi = filter( orig, "is_in_gilde", ME, words[i] );
      break;
      
    case "team":
      if ( ++i >= sz )
        break;
      zwi = filter( orig, "is_in_team", ME,
                          lower_case("team " + words[i]) );
      break;
      
    case "bei":
      if ( ++i >= sz )
        break;
      if ( !objectp(x = find_player(words[i]) ) &&
           !objectp(x = find_living(words[i])) )
        break;
      if ( x->QueryProp(P_INVIS) )
        break;
      zwi = filter( orig, "is_at", ME, environment(x) );
      break;

    case "goetter":
      zwi = filter( orig, "is_wiz_level_ge", ME, GOD_LVL );
      break;
      
    case "erzmagier":
    case "erzi":
    case "erzis":
      zwi = filter( orig, "is_wiz_level_ge", ME, ARCH_LVL );
      break;
      
    case "magier":
      zwi = filter( orig, "is_wiz_level_ge", ME, LEARNER_LVL );
      break;
      
    case "spieler":
      zwi = filter( orig, "is_wiz_level_lt", ME, SEER_LVL );
      break;
      
    case "seher":
      zwi = filter( filter( orig, "is_wiz_level_lt", ME,
                                        LEARNER_LVL),
                          "is_wiz_level_ge", ME, SEER_LVL );
      break;

    case "maennlich":
      zwi = filter( orig, "is_gender", ME, MALE );
      break;
      
    case "weiblich":
      zwi = filter( orig, "is_gender", ME, FEMALE );
      break;
      
    case "ledig":
      zwi = filter( orig, "is_unmarried" );
      break;

    case "ssl":
    case "stunnel":
      zwi = filter(orig,"uses_ssl");
      break;
      
    case "hardcore":
      zwi = filter( orig, "is_hc" );
      break;
      
    case "geist":
    case "tot":
      zwi = filter( orig, "is_ghost" );
      break;
      
    case "mensch":
      zwi = filter( orig, "is_race", ME, "Mensch" );
      break;
      
    case "zwerg":
      zwi = filter( orig, "is_race", ME, "Zwerg" );
      break;
      
    case "elf":
      zwi = filter( orig, "is_race", ME, "Elf" );
      break;
      
    case "feline" :
      zwi = filter( orig, "is_race", ME, "Feline" );
      break;
      
    case "hobbit":
      zwi = filter( orig, "is_race", ME, "Hobbit" );
      break;
      
    case "dunkelelf":
      zwi = filter( orig, "is_race", ME, "Dunkelelf" );
      break;

    case "goblin":
      zwi = filter( orig, "is_race", ME, "Goblin" );
      break;

    case "ork":
      zwi = filter( orig, "is_race", ME, "Ork" );
      break;

    case "frosch":
      zwi = filter( orig, "is_prop_set", ME, P_FROG );
      break;

    case "weg":
      zwi = filter( orig, "is_prop_set", ME, P_AWAY );
      break;

    case "idle":
      zwi = filter( orig, "is_idle", ME, 120 );
      break;

    case "idlezeit":
      if ( ++i >= sz )
        break;
      sscanf(words[i],"%d",t);
      zwi = filter( orig, "is_idle", ME, 60*t );
      break;

    case "cicerone":
    case "cicerones":
      // is_active_guide() ist in /std/player/guide.c
      zwi = filter( orig, #'is_active_guide);
      break;

    case "testie":
    case "testies":
    case "testspieler":
      zwi = filter( orig, "is_prop_set", ME, P_TESTPLAYER );
      break;
      
    case "zweitie":
    case "zweities":
    case "zweitspieler":
        if (IS_LEARNER(this_player()))
          zwi = filter( orig, "is_second", ME, -2 );
        else
          zwi = filter( orig, "is_second", ME, -1 );
      break;

    case "magierzweitie":
    case "magierzweities":
    case "magierzweitspieler":
        if (IS_ARCH(this_player()))
          zwi = filter( orig, "is_mage_second");
      break;

    case "erwartete":
      if ( i < (sz-1) && words[i+1] == "wegen" ) {
        i++;

        if ( !mappingp(x = (mixed)this_player()->QueryProp(P_WAITFOR_REASON)) ||
             !pointerp(x = m_indices(x)) )
          break;
        
        zwi = filter( orig, "is_name_in", ME, x );
        break;
      }
      
      if ( !pointerp(x = (mixed)this_player()->QueryProp(P_WAITFOR)) )
        break;
      zwi = filter( orig, "is_name_in", ME, x );
      break;

    case "icq":
      zwi = filter( orig, "is_prop_set", ME, P_ICQ );
      break;

    case "url":
    case "www":
      zwi = filter( orig, "is_prop_set", ME, P_HOMEPAGE );
      break;
          
    default:
      continue;
    }
    
    switch (f) {
    case -1:
      res -= zwi;
      break;
      
    case 1:
      res &= zwi;
      break;

    default:
      res += zwi;
    }
  }
  
  return res;
}


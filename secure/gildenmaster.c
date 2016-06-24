#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <properties.h>
#include <wizlevels.h>
#include <new_skills.h>
#include <events.h>

string *valid_guilds = ({});

void create() {
  seteuid(getuid(this_object()));
  restore_object(GUILD_SAVEFILE);
  if (!pointerp(valid_guilds)) 
    valid_guilds=({});
}

//noetig, da nun valid_guilds eine Variable ist
public mixed QueryProp( string name )
{
  if (name==P_VALID_GUILDS) 
      return copy(valid_guilds);
  return 0;
}

nomask int beitreten() {
  object pl,gilde;
  string gname,ogname;

  if (!(pl=this_player()) || !(gilde=previous_object()))
      return 0;
  // Gilden sind Blueprints.
  gname=object_name(gilde);

  if ((GUILD_DIR+ogname)==gname) {
    write("Du bist schon in dieser Gilde.\n");
    return -4;
  }

  if ((ogname=(string)pl->QueryProp(P_GUILD)) && 
      (ogname!=(((string)pl->QueryProp(P_DEFAULT_GUILD)) || DEFAULT_GUILD))) {
    write("Du bist noch in einer anderen Gilde.\n");
    return -1;
  }
  if (gname[0..7]!=GUILD_DIR ||
        member(valid_guilds,(gname=gname[8..]))<0) {
      write("Diese Gilde ist leider nicht zugelassen.\n"+
              "Bitte verstaendige einen Erzmagier.\n");
      return -2;
  }
  pl->SetProp(P_GUILD,gname);
  // Event Gildenwechsel triggern
  EVENTD->TriggerEvent(EVT_GUILD_CHANGE, ([
      E_OBJECT: pl, E_PLNAME: getuid(pl),
      E_ENVIRONMENT: environment(pl),
      E_GUILDNAME: gname,
      E_LAST_GUILDNAME: ogname ]) );

  return 1;
}

static nomask void loose_ability(mixed key, mixed dat, int loss) {
  mixed val;

  val=mappingp(dat)?dat[SI_SKILLABILITY]:dat;
  if (!intp(val) || val<=0) return;
  val-=(val*loss)/100;if (val<=0) val=1;
  if (mappingp(dat))
      dat[SI_SKILLABILITY]=val;
  else
      dat=val;
}

varargs nomask int austreten(int loss) {
  object pl,gilde;
  string gname;
  mapping skills;

  if (!(pl=this_player()) || !(gilde=previous_object()))
      return 0;
  // Gilden muessen Blueprints sein, so. ;-)
  gname=object_name(gilde);
  if (gname[0..7]!=GUILD_DIR ||
      ((string)pl->QueryProp(P_GUILD))!=gname[8..]) {
      write("Du kannst hier nicht aus einer anderen Gilde austreten.\n");
      return -1;
  }
  if (gname[8..]==(pl->QueryProp(P_DEFAULT_GUILD)||DEFAULT_GUILD))
  {
        write("Aus dieser Gilde kannst Du nicht austreten.\n");
        return -1;
  }
  if (loss<=0) loss=20;
  skills=(mapping)pl->QueryProp(P_NEWSKILLS);
  walk_mapping(skills,"loose_ability",this_object(),loss);
  pl->SetProp(P_NEWSKILLS,skills);
  pl->SetProp(P_GUILD,0);
  // Event Gildenwechsel triggern
  EVENTD->TriggerEvent(EVT_GUILD_CHANGE, ([
      E_OBJECT: pl, E_PLNAME: getuid(pl),
      E_ENVIRONMENT: environment(pl),
      E_GUILDNAME: pl->QueryProp(P_GUILD),
      E_LAST_GUILDNAME: gname ]) );

  // Defaultgilde ggf. neuen Titel setzen lassen.
  gname = (string)pl->QueryProp(P_GUILD);
  (GUILD_DIR+"/"+gname)->adjust_title(pl);

  return 1;
}

nomask static int security_check()
{
  if (previous_object() && geteuid(previous_object())==ROOTID)
    return 1;
  if (!process_call() && previous_object() && ARCH_SECURITY)
    return 1;
  return 0;
}

nomask int AddGuild(string gildob) {
  object tp;

  if (!stringp(gildob) || !sizeof(gildob) || !security_check() 
      || file_size(GUILD_DIR+gildob+".c")<0)
      return 0;
  if (member(valid_guilds, gildob) != -1)
      return 0;
  valid_guilds+=({gildob});
  save_object(GUILD_SAVEFILE);
  return 1;
}

nomask int RemoveGuild(string gildob) {
  object tp;

  if (!stringp(gildob) || !sizeof(gildob) 
      || !security_check())
      return 0;
  if (member(valid_guilds, gildob) == -1)
      return 0;
  valid_guilds-=({gildob});
  
  save_object(GUILD_SAVEFILE);
  return 1;
}

nomask int ValidGuild(string gildenob) {
  if (!stringp(gildenob)) 
      return 0;

  return (member(valid_guilds,gildenob)>=0);
}

nomask string *_query_valid_guilds() {return copy(valid_guilds); }


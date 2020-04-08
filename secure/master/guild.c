// MorgenGrauen MUDlib
//
// master/guild.c -- Gilden und Gildenmagier
//
// $Id: guild.c 6142 2007-01-31 20:34:39Z Zesstra $

/*
 * Dies kann irgendwann auch hinsichtlich einer automatischen 
 * Rechtevergabe auf Gilden-Verzeichnisse erweitert werden.
 * Bisher werden nur Gildenmagier verwaltet.
 */

int guild_master(string user, string guild)
{
  string *guilds;
  int i;

  if (!find_userinfo(user)||
      !pointerp(guilds=query_userlist(user, USER_GUILD)))
    return 0;
  
  return (member(guilds,guild) != -1);
}

int add_guild_master(string user, string guild)
{
  string *guilds;
  
  if ((({int})call_other(SIMUL_EFUN_FILE, "process_call") ||
       ({int})call_other(SIMUL_EFUN_FILE, "secure_level") < GOD_LVL) ||
      !find_userinfo(user))
    return 0;

  guilds=query_userlist(user, USER_GUILD);
  if (!guilds)
    set_guilds(user, ({ guild }) );
  else {
    guilds = guilds - ({guild}) + ({guild});
    set_guilds(user, guilds);
  }
  return 1;
}

int remove_guild_master(string user, string guild)
{
  string *guilds;

  if (!IS_GOD(geteuid(previous_object()))
      ||!find_userinfo(user)
      ||!(guilds=query_userlist(user, USER_GUILD))
      || member(guilds,guild)==-1)
    return 0;
  guilds -= ({ guild });
  set_guilds(user, guilds);
  return 1;
}


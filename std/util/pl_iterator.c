// MorgenGrauen MUDlib
/** \file /file.c
* Kurzbeschreibung.
* Langbeschreibung...
* \author <Autor>
* \date <date>
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types, save_types, rtt_checks
#pragma no_clone, no_shadow
#pragma pedantic, range_check
#pragma warn_deprecated

private void check_all_player(mapping allplayer, closure check_cl,
                             closure finish_cl, int res,
                             varargs mixed extra)
{
  // offenbar fertig mit allen Spielern, Aufrufer informieren.
  if (!sizeof(allplayer))
  {
    funcall(finish_cl, extra...);
    return;
  }

  string dir=m_indices(allplayer)[0];
  string *pls=allplayer[dir];
  foreach(string pl: &pls)
  {
    if (get_eval_cost() < res)
      break; // spaeter weitermachen
    catch(funcall(check_cl, pl, extra...) ; publish);
    pl = 0;
  }
  pls-=({0});
  allplayer[dir] = pls;

  if (!sizeof(allplayer[dir]))
    m_delete(allplayer,dir);

  call_out(#'check_all_player,2, allplayer, check_cl, finish_cl, res, extra...);
}

protected int start_player_check(closure check_cl, closure finish_cl, int res,
                             varargs mixed extra)
{
  res ||= 1250000;
  mapping allplayer=(mapping)master()->get_all_players();
  call_out(#'check_all_player,2, allplayer, check_cl, finish_cl, res, extra...);
  return 1;
}


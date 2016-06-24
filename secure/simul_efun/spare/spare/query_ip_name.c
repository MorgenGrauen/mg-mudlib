/* This sefun is to provide a replacement for the efuns query_ip_name() and
 * query_ip_number().
 * Feel free to add it to your mudlibs, if you have much code relying on that.
 */

#if ! __EFUN_DEFINED__(query_ip_name)

#include <interactive_info.h>

private varargs string _query_ip_name(object player)
{
    object ob = player;
    ob ||= efun::this_player();

    player = efun::interactive_info(ob, II_IP_ADDRESS);
    return efun::interactive_info(ob, II_IP_NAME);
}

private varargs string _query_ip_number(object player)
{
    object ob = player;
    ob ||= efun::this_player();

    player = efun::interactive_info(ob, II_IP_ADDRESS);
    return efun::interactive_info(ob, II_IP_NUMBER);
}

// * Herkunfts-Ermittlung
string query_ip_number(object ob)
{
  ob= ob || this_player();
  if (!objectp(ob) || !interactive(ob)) return 0;
  if(ob->query_realip() && (string)ob->query_realip()!="")
  {
    return (string)ob->query_realip();
  }
  return _query_ip_number(ob);
}

string query_ip_name(mixed ob)
{
  if ( !ob || objectp(ob) )
      ob=_query_ip_number(ob);
  return (string)"/p/daemon/iplookup"->host(ob);
}

#endif

